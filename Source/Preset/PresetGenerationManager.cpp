#include "PresetGenerationManager.h"
#include <random>

PresetGenerationManager::PresetGenerationManager()
    : juce::Thread("PresetGenerationManager")
{
    initializeSurpriseMePrompts();
    
    // Start the processing thread
    startThread(juce::Thread::Priority::normal);
    
    // Start timer for message processing (60 Hz for responsive UI)
    startTimerHz(60);
}

PresetGenerationManager::~PresetGenerationManager()
{
    // Stop timer first
    stopTimer();
    
    // Cancel all requests and stop thread
    cancelAllRequests();
    signalThreadShouldExit();
    newRequestEvent.signal();
    
    // Wait for thread to finish (with timeout)
    if (!waitForThreadToExit(5000))
    {
        // Force kill if it doesn't exit gracefully
        juce::Logger::writeToLog("Warning: PresetGenerationManager thread did not exit gracefully");
    }
}

void PresetGenerationManager::generatePreset(const juce::String& description, int requestId)
{
    if (description.isEmpty())
        return;
    
    if (requestId == -1)
        requestId = generateRequestId();
    
    GenerationRequest request(requestId, description.trim());
    
    // Add to queue
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.push(request);
    }
    
    // Signal worker thread
    newRequestEvent.signal();
    
    // Notify UI that generation started
    sendMessage(std::make_unique<ThreadMessages::PresetGenerationStartedMessage>(description));
    
    logRequest(request, "Queued");
}

void PresetGenerationManager::generateVariations(const juce::String& baseDescription, int count)
{
    if (baseDescription.isEmpty())
        return;
    
    auto variationPrompts = generateVariationPrompts(baseDescription, count);
    
    for (const auto& prompt : variationPrompts)
    {
        int requestId = generateRequestId();
        GenerationRequest request(requestId, prompt);
        request.isVariation = true;
        request.basePrompt = baseDescription;
        
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            requestQueue.push(request);
        }
    }
    
    newRequestEvent.signal();
    
    sendMessage(std::make_unique<ThreadMessages::StatusUpdateMessage>(
        "Generating " + juce::String(count) + " variations...", 0.0f));
}

void PresetGenerationManager::generateSurpriseMe()
{
    auto surprisePrompt = getRandomSurpriseMePrompt();
    generatePreset(surprisePrompt);
}

void PresetGenerationManager::cancelAllRequests()
{
    {
        std::lock_guard<std::mutex> lock(cancelMutex);
        std::lock_guard<std::mutex> queueLock(queueMutex);
        
        // Mark all queued requests as cancelled
        std::queue<GenerationRequest> emptyQueue;
        while (!requestQueue.empty())
        {
            cancelledRequests.insert(requestQueue.front().requestId);
            requestQueue.pop();
        }
        requestQueue = emptyQueue;
    }
    
    sendMessage(std::make_unique<ThreadMessages::StatusUpdateMessage>("Cancelled all requests", 0.0f));
}

void PresetGenerationManager::cancelRequest(int requestId)
{
    std::lock_guard<std::mutex> lock(cancelMutex);
    cancelledRequests.insert(requestId);
}

void PresetGenerationManager::setServerURL(const juce::String& url)
{
    serverURL = url;
    apiClient.setServerURL(url);
}

void PresetGenerationManager::setTimeout(int timeout)
{
    timeoutMs = timeout;
    apiClient.setTimeout(timeout);
}

int PresetGenerationManager::getQueueSize() const
{
    std::lock_guard<std::mutex> lock(queueMutex);
    return static_cast<int>(requestQueue.size());
}

void PresetGenerationManager::processMessages()
{
    std::unique_ptr<ThreadMessages::BaseMessage> message;
    
    // Process all pending messages
    while (messageQueue.tryPop(message))
    {
        switch (message->type)
        {
            case ThreadMessages::MessageType::PresetGenerated:
            {
                auto* presetMsg = static_cast<ThreadMessages::PresetGeneratedMessage*>(message.get());
                if (onPresetGenerated)
                    onPresetGenerated(presetMsg->preset, presetMsg->originalPrompt, presetMsg->generationTimeMs);
                break;
            }
            
            case ThreadMessages::MessageType::PresetGenerationFailed:
            {
                auto* errorMsg = static_cast<ThreadMessages::PresetGenerationFailedMessage*>(message.get());
                if (onGenerationFailed)
                    onGenerationFailed(errorMsg->errorMessage, errorMsg->originalPrompt);
                break;
            }
            
            case ThreadMessages::MessageType::PresetGenerationStarted:
            {
                auto* startMsg = static_cast<ThreadMessages::PresetGenerationStartedMessage*>(message.get());
                if (onGenerationStarted)
                    onGenerationStarted(startMsg->prompt);
                break;
            }
            
            case ThreadMessages::MessageType::StatusUpdate:
            {
                auto* statusMsg = static_cast<ThreadMessages::StatusUpdateMessage*>(message.get());
                if (onStatusUpdate)
                    onStatusUpdate(statusMsg->statusText, statusMsg->progress);
                break;
            }
            
            default:
                break;
        }
    }
}

void PresetGenerationManager::run()
{
    juce::Logger::writeToLog("PresetGenerationManager thread started");
    
    while (!threadShouldExit())
    {
        // Wait for new requests or exit signal
        if (getQueueSize() == 0)
        {
            newRequestEvent.wait(1000); // 1 second timeout
            continue;
        }
        
        // Check if we can process more requests
        if (activeRequests.load() >= maxConcurrentRequests)
        {
            juce::Thread::sleep(100);
            continue;
        }
        
        // Get next request
        GenerationRequest request(0, "");
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!requestQueue.empty())
            {
                request = requestQueue.front();
                requestQueue.pop();
            }
            else
            {
                continue;
            }
        }
        
        // Check if request was cancelled
        if (isRequestCancelled(request.requestId))
        {
            logRequest(request, "Cancelled");
            continue;
        }
        
        // Process the request
        processRequest(request);
    }
    
    juce::Logger::writeToLog("PresetGenerationManager thread exiting");
}

void PresetGenerationManager::timerCallback()
{
    processMessages();
}

void PresetGenerationManager::processRequest(const GenerationRequest& request)
{
    activeRequests.fetch_add(1);
    
    auto startTime = juce::Time::getCurrentTime();
    logRequest(request, "Processing");
    
    try
    {
        // Check for cancellation before making API call
        if (isRequestCancelled(request.requestId))
        {
            logRequest(request, "Cancelled before processing");
            activeRequests.fetch_sub(1);
            return;
        }
        
        // Make synchronous API call (this will block the worker thread, not the UI)
        auto result = apiClient.generatePreset(request.description);
        
        if (result.has_value())
        {
            auto endTime = juce::Time::getCurrentTime();
            int generationTimeMs = static_cast<int>((endTime - startTime).inMilliseconds());
            
            // Send success message
            sendMessage(std::make_unique<ThreadMessages::PresetGeneratedMessage>(
                std::move(result.value()), request.description, generationTimeMs));
            
            logRequest(request, "Completed in " + juce::String(generationTimeMs) + "ms");
        }
        else
        {
            // API call failed, retry if possible
            if (request.retryCount < maxRetries)
            {
                GenerationRequest retryRequest = request;
                retryRequest.retryCount++;
                retryRequest(retryRequest);
                logRequest(request, "Retrying (attempt " + juce::String(retryRequest.retryCount) + ")");
            }
            else
            {
                sendMessage(std::make_unique<ThreadMessages::PresetGenerationFailedMessage>(
                    "Failed to generate preset after " + juce::String(maxRetries + 1) + " attempts",
                    request.description));
                logRequest(request, "Failed after retries");
            }
        }
    }
    catch (const std::exception& e)
    {
        sendMessage(std::make_unique<ThreadMessages::PresetGenerationFailedMessage>(
            juce::String("Generation error: ") + e.what(), request.description));
        logRequest(request, "Exception: " + juce::String(e.what()));
    }
    
    activeRequests.fetch_sub(1);
}

void PresetGenerationManager::retryRequest(GenerationRequest request)
{
    // Add small delay before retry
    juce::Thread::sleep(1000 * request.retryCount);
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.push(request);
    }
    
    newRequestEvent.signal();
}

bool PresetGenerationManager::isRequestCancelled(int requestId)
{
    std::lock_guard<std::mutex> lock(cancelMutex);
    return cancelledRequests.find(requestId) != cancelledRequests.end();
}

void PresetGenerationManager::sendMessage(std::unique_ptr<ThreadMessages::BaseMessage> message)
{
    messageQueue.push(std::move(message));
}

juce::StringArray PresetGenerationManager::generateVariationPrompts(const juce::String& basePrompt, int count)
{
    juce::StringArray variations;
    
    // Define variation modifiers
    juce::StringArray modifiers = {
        "more aggressive", "softer", "brighter", "darker", "warmer", "cooler",
        "with more reverb", "drier", "with delay", "compressed", "open",
        "tighter", "looser", "vintage", "modern", "experimental"
    };
    
    // Define style additions
    juce::StringArray styles = {
        "shoegaze", "post-rock", "ambient", "industrial", "dreamy",
        "ethereal", "gritty", "clean", "saturated", "crystalline"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (int i = 0; i < count; ++i)
    {
        juce::String variation = basePrompt;
        
        // Add random modifier
        if (gen() % 3 == 0) // 33% chance
        {
            auto modifier = modifiers[gen() % modifiers.size()];
            variation += ", " + modifier;
        }
        
        // Add random style
        if (gen() % 4 == 0) // 25% chance
        {
            auto style = styles[gen() % styles.size()];
            variation += ", " + style;
        }
        
        // Add variation suffix to make it unique
        variation += addVariationSuffix(variation);
        
        variations.add(variation);
    }
    
    return variations;
}

juce::String PresetGenerationManager::addVariationSuffix(const juce::String& basePrompt)
{
    juce::StringArray suffixes = {
        " with subtle changes",
        " alternative version",
        " different approach",
        " creative twist",
        " unique interpretation"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    return suffixes[gen() % suffixes.size()];
}

void PresetGenerationManager::initializeSurpriseMePrompts()
{
    surpriseMePrompts = {
        // Traditional sounds
        "warm blues overdrive with vintage character",
        "clean jazz tone with subtle compression",
        "heavy metal with tight gate and aggressive distortion",
        "classic rock crunch with mid boost",
        "country twang with slapback delay",
        
        // Abstract/experimental sounds
        "ethereal floating soundscape",
        "underwater dreams with heavy modulation",
        "broken radio transmission from space",
        "crystalline sparkles in a cathedral",
        "industrial machinery grinding",
        "cosmic voyage through nebula",
        "glitchy digital decay",
        "haunting whispers in fog",
        "alien communication signals",
        "melting synthesizer textures",
        "frozen time crystals",
        "electric storm in desert",
        "digital ghosts in machine",
        "liquid mercury flowing",
        "neon lights reflecting in rain",
        
        // Hybrid concepts
        "vintage meets futuristic",
        "organic meets synthetic",
        "chaos meets order",
        "darkness meets light",
        "ancient meets modern",
        "earth meets space",
        "fire meets ice",
        "analog meets digital"
    };
}

juce::String PresetGenerationManager::getRandomSurpriseMePrompt()
{
    if (surpriseMePrompts.isEmpty())
        return "surprise me with something creative";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    return surpriseMePrompts[gen() % surpriseMePrompts.size()];
}

void PresetGenerationManager::logRequest(const GenerationRequest& request, const juce::String& status)
{
    juce::String logMessage = "Request " + juce::String(request.requestId) + 
                             " (\"" + request.description.substring(0, 30) + "...\"): " + status;
    juce::Logger::writeToLog(logMessage);
}
