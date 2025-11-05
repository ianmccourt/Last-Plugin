#pragma once

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include "../Network/APIClient.h"
#include "../Utils/ThreadSafeQueue.h"
#include "PresetSchema.h"
#include <atomic>
#include <queue>
#include <mutex>

/**
 * Manages AI preset generation with proper threading and queue management
 * Handles multiple requests, cancellation, and thread-safe communication
 */
class PresetGenerationManager : public juce::Thread,
                               public juce::Timer
{
public:
    PresetGenerationManager();
    ~PresetGenerationManager() override;
    
    // Public interface (called from UI thread)
    void generatePreset(const juce::String& description, int requestId = -1);
    void generateVariations(const juce::String& baseDescription, int count = 3);
    void generateSurpriseMe();
    void cancelAllRequests();
    void cancelRequest(int requestId);
    
    // Configuration
    void setServerURL(const juce::String& url);
    void setTimeout(int timeoutMs);
    void setMaxConcurrentRequests(int maxRequests) { maxConcurrentRequests = maxRequests; }
    void setRetryCount(int retries) { maxRetries = retries; }
    
    // Status queries
    bool isGenerating() const { return activeRequests.load() > 0; }
    int getQueueSize() const;
    int getActiveRequestCount() const { return activeRequests.load(); }
    
    // Message handling (called from message thread)
    void processMessages();
    bool hasMessages() const { return !messageQueue.empty(); }
    
    // Callbacks (set these to receive notifications)
    std::function<void(const PresetData&, const juce::String&, int)> onPresetGenerated;
    std::function<void(const juce::String&, const juce::String&)> onGenerationFailed;
    std::function<void(const juce::String&)> onGenerationStarted;
    std::function<void(const juce::String&, float)> onStatusUpdate;

private:
    // Thread implementation
    void run() override;
    void timerCallback() override;
    
    // Request management
    struct GenerationRequest
    {
        int requestId;
        juce::String description;
        juce::Time requestTime;
        int retryCount = 0;
        bool isVariation = false;
        juce::String basePrompt;
        
        GenerationRequest(int id, const juce::String& desc)
            : requestId(id), description(desc), requestTime(juce::Time::getCurrentTime())
        {}
    };
    
    // Thread-safe request queue
    std::queue<GenerationRequest> requestQueue;
    std::mutex queueMutex;
    juce::WaitableEvent newRequestEvent;
    
    // Active request tracking
    std::atomic<int> activeRequests{0};
    std::atomic<int> nextRequestId{1};
    std::set<int> cancelledRequests;
    std::mutex cancelMutex;
    
    // Configuration
    int maxConcurrentRequests = 2;
    int maxRetries = 2;
    juce::String serverURL = "http://localhost:8787";
    int timeoutMs = 30000;
    
    // Communication
    MessageQueue messageQueue;
    APIClient apiClient;
    
    // Request processing
    void processRequest(const GenerationRequest& request);
    void retryRequest(GenerationRequest request);
    bool isRequestCancelled(int requestId);
    void sendMessage(std::unique_ptr<ThreadMessages::BaseMessage> message);
    
    // Variation generation
    juce::StringArray generateVariationPrompts(const juce::String& basePrompt, int count);
    juce::String addVariationSuffix(const juce::String& basePrompt);
    
    // Surprise me generation
    juce::StringArray surpriseMePrompts;
    void initializeSurpriseMePrompts();
    juce::String getRandomSurpriseMePrompt();
    
    // Utility
    int generateRequestId() { return nextRequestId.fetch_add(1); }
    void logRequest(const GenerationRequest& request, const juce::String& status);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetGenerationManager)
};
