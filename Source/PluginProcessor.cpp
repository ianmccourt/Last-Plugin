#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AIGuitarPluginAudioProcessor::AIGuitarPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      parameters(*this, nullptr, juce::Identifier("AIGuitarPlugin"), createParameterLayout())
{
}

AIGuitarPluginAudioProcessor::~AIGuitarPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AIGuitarPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AIGuitarPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AIGuitarPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AIGuitarPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AIGuitarPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AIGuitarPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AIGuitarPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AIGuitarPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String AIGuitarPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AIGuitarPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AIGuitarPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize DSP chain
    dspChain.prepareToPlay(sampleRate, samplesPerBlock);
    
    // Initialize parameter smoother
    parameterSmoother.prepareToPlay(sampleRate, samplesPerBlock);
}

void AIGuitarPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AIGuitarPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AIGuitarPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Update DSP parameters from UI
    updateDSPFromParameters();
    
    // Update smoothed parameters
    parameterSmoother.process(buffer.getNumSamples());
    
    // Process audio through DSP chain
    dspChain.processBlock(buffer);
}

//==============================================================================
bool AIGuitarPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AIGuitarPluginAudioProcessor::createEditor()
{
    return new AIGuitarPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AIGuitarPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save parameters to memory block
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AIGuitarPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore parameters from memory block
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AIGuitarPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Add basic parameters for testing
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "gain", "Gain", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
        
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "tone", "Tone", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    
    return layout;
}

void AIGuitarPluginAudioProcessor::updateDSPFromParameters()
{
    // Update DSP chain from current parameter values
    auto* gainParam = parameters.getRawParameterValue("gain");
    auto* toneParam = parameters.getRawParameterValue("tone");
    
    if (gainParam != nullptr)
        dspChain.setGain(*gainParam);
        
    if (toneParam != nullptr)
        dspChain.setTone(*toneParam);
}

// AI Preset Generation methods
void AIGuitarPluginAudioProcessor::generatePresetFromText(const juce::String& description)
{
    // For now, create a simple HTTP request to the AI server
    // Use a lambda without complex captures to avoid compilation issues
    auto task = [this, description]() -> void {
        try {
            DBG("Starting AI request for: " + description);
            
            // Create the JSON request - use simple string construction
            auto postData = R"({"prompt":")" + description + R"("})";
            
            DBG("POST data: " + postData);
            
            // Create URL with POST data - try different approaches
            juce::URL url("http://127.0.0.1:8787/preset");  // Use 127.0.0.1 instead of localhost
            auto postUrl = url.withPOSTData(postData);
            
            DBG("Request URL: " + postUrl.toString(true));
            
            // Try the request with timeout and headers
            juce::String headers = "Content-Type: application/json\r\nAccept: application/json";
            
            auto inputStream = postUrl.createInputStream(
                juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
                    .withConnectionTimeoutMs(10000)
                    .withExtraHeaders(headers)
                    .withResponseHeaders(nullptr)
            );
            
            if (inputStream != nullptr)
            {
                DBG("Successfully connected to AI server");
                auto response = inputStream->readEntireStreamAsString();
                
                if (response.isNotEmpty())
                {
                    DBG("Received response: " + response);
                    
                    // Parse and apply the preset
                    auto presetJson = juce::JSON::parse(response);
                    if (presetJson.isObject())
                    {
                        DBG("Successfully parsed preset JSON");
                        
                        // Apply the preset parameters - use a simpler approach
                        juce::MessageManager::callAsync([this, presetJson, description]() -> void {
                            this->applyPresetFromJson(presetJson, description);
                        });
                    }
                    else
                    {
                        DBG("Failed to parse preset JSON");
                        juce::MessageManager::callAsync([description]() -> void {
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::WarningIcon,
                                "AI Generation Error",
                                "Failed to parse AI response for: " + description,
                                "OK");
                        });
                    }
                }
                else
                {
                    DBG("Empty response from AI server");
                    // Handle empty response
                    juce::MessageManager::callAsync([description]() -> void {
                        DBG("Empty response from AI server for: " + description);
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "AI Generation Error",
                            "Empty response from AI server for: " + description,
                            "OK");
                    });
                }
            }
            else
            {
                DBG("Failed to create input stream");
                // Handle connection error
                juce::MessageManager::callAsync([description]() -> void {
                    DBG("Failed to connect to AI server for: " + description);
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "AI Generation Error",
                        "Failed to connect to AI server for: " + description + "\n\nMake sure the server is running on 127.0.0.1:8787\n\nThis might be a plugin sandboxing issue.",
                        "OK");
                });
            }
        }
        catch (const std::exception& e)
        {
            DBG("Exception: " + juce::String(e.what()));
            // Handle any other errors
            juce::MessageManager::callAsync([description, e]() -> void {
                DBG("Exception during AI request for " + description + ": " + e.what());
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "AI Generation Error",
                    "Exception during AI request for: " + description + "\n\nError: " + e.what(),
                    "OK");
            });
        }
    };
    
    // Launch the task
    juce::Thread::launch(task);
}

void AIGuitarPluginAudioProcessor::generatePresetVariations(const juce::String& baseDescription, int count)
{
    juce::ignoreUnused(baseDescription, count);
    // TODO: Implement preset variations
}

void AIGuitarPluginAudioProcessor::generateSurpriseMe()
{
    // TODO: Implement surprise me feature
}

void AIGuitarPluginAudioProcessor::cancelPresetGeneration()
{
    // TODO: Implement cancellation
}

bool AIGuitarPluginAudioProcessor::isGeneratingPreset() const
{
    return false; // TODO: Return actual status
}

int AIGuitarPluginAudioProcessor::getGenerationQueueSize() const
{
    return 0; // TODO: Return actual queue size
}

void AIGuitarPluginAudioProcessor::applyPresetFromJson(const juce::var& presetJson, const juce::String& description)
{
    DBG("Applying preset from JSON for: " + description);
    
    try
    {
        // Extract the chain array from the preset
        auto chain = presetJson["chain"];
        if (chain.isArray())
        {
            DBG("Found chain with " + juce::String(chain.size()) + " blocks");
            
            // Apply preset based on description and enabled blocks
            // This is a simplified implementation that maps presets to gain/tone values
            
            float gainValue = 1.0f;
            float toneValue = 0.5f;
            
            // Extract preset name for mapping
            auto presetName = presetJson["name"];
            juce::String name = presetName.toString().toLowerCase();
            
            DBG("Applying preset: " + name);
            
            // Map preset types to gain/tone values
            if (name.contains("heavy") || name.contains("metal") || name.contains("distorted"))
            {
                gainValue = 1.8f;  // High gain
                toneValue = 0.3f;  // Darker tone
                DBG("Applied heavy/metal preset mapping");
            }
            else if (name.contains("clean") || name.contains("ambient"))
            {
                gainValue = 0.3f;  // Low gain
                toneValue = 0.7f;  // Bright tone
                DBG("Applied clean/ambient preset mapping");
            }
            else if (name.contains("alien") || name.contains("ethereal") || name.contains("weird"))
            {
                gainValue = 1.5f;  // Medium-high gain
                toneValue = 0.8f;  // Very bright tone
                DBG("Applied alien/ethereal preset mapping");
            }
            else if (name.contains("blues") || name.contains("crunch"))
            {
                gainValue = 1.2f;  // Medium gain
                toneValue = 0.4f;  // Warm tone
                DBG("Applied blues/crunch preset mapping");
            }
            else
            {
                // Default mapping based on enabled blocks
                bool hasDrive = false;
                bool hasReverb = false;
                bool hasDelay = false;
                
                for (int i = 0; i < chain.size(); ++i)
                {
                    auto block = chain[i];
                    if (block.isObject())
                    {
                        auto blockType = block["block"].toString();
                        auto enabled = block["enabled"];
                        
                        if (enabled.isBool() && static_cast<bool>(enabled))
                        {
                            if (blockType == "drive") hasDrive = true;
                            if (blockType == "reverb") hasReverb = true;
                            if (blockType == "delay") hasDelay = true;
                        }
                    }
                }
                
                // Apply logic based on enabled effects
                if (hasDrive)
                {
                    gainValue = 1.4f;  // Higher gain when drive is enabled
                    toneValue = 0.4f;  // Slightly darker
                }
                if (hasReverb || hasDelay)
                {
                    toneValue = juce::jmax(toneValue, 0.6f);  // Brighter when reverb/delay
                }
                
                DBG("Applied block-based preset mapping - Drive: " + juce::String(hasDrive) + 
                    ", Reverb: " + juce::String(hasReverb) + ", Delay: " + juce::String(hasDelay));
            }
            
            // Apply the parameters with smoothing
            parameterSmoother.setTargetValue("gain", gainValue);
            parameterSmoother.setTargetValue("tone", toneValue);
            
            // Apply additional DSP parameters directly to the DSP chain
            // (These don't use the parameter system yet, so we set them directly)
            
            // Set drive based on preset type
            float driveValue = 0.0f;
            juce::String driveType = "softclip";
            
            if (name.contains("heavy") || name.contains("metal") || name.contains("distorted"))
            {
                driveValue = 0.8f;  // High drive
                driveType = "hardclip";
            }
            else if (name.contains("alien") || name.contains("ethereal") || name.contains("weird"))
            {
                driveValue = 0.6f;  // Medium drive
                driveType = "fuzz";
            }
            else if (name.contains("blues") || name.contains("crunch"))
            {
                driveValue = 0.4f;  // Light drive
                driveType = "softclip";
            }
            else
            {
                // Check if drive is enabled in the preset
                bool presetHasDrive = false;
                for (int i = 0; i < chain.size(); ++i)
                {
                    auto block = chain[i];
                    if (block.isObject())
                    {
                        auto blockType = block["block"].toString();
                        auto enabled = block["enabled"];
                        if (blockType == "drive" && enabled.isBool() && static_cast<bool>(enabled))
                        {
                            presetHasDrive = true;
                            break;
                        }
                    }
                }
                
                if (presetHasDrive)
                {
                    driveValue = 0.5f;  // Default drive
                    driveType = "softclip";
                }
            }
            
            // Set reverb based on preset type
            float reverbMix = 0.0f;
            float reverbDecay = 0.5f;
            
            if (name.contains("ambient") || name.contains("ethereal") || name.contains("space"))
            {
                reverbMix = 0.4f;    // High reverb
                reverbDecay = 0.8f;  // Long decay
            }
            else if (name.contains("alien") || name.contains("weird"))
            {
                reverbMix = 0.3f;    // Medium reverb
                reverbDecay = 0.9f;  // Very long decay
            }
            else
            {
                // Check if reverb is enabled in the preset
                bool presetHasReverb = false;
                for (int i = 0; i < chain.size(); ++i)
                {
                    auto block = chain[i];
                    if (block.isObject())
                    {
                        auto blockType = block["block"].toString();
                        auto enabled = block["enabled"];
                        if (blockType == "reverb" && enabled.isBool() && static_cast<bool>(enabled))
                        {
                            presetHasReverb = true;
                            break;
                        }
                    }
                }
                
                if (presetHasReverb)
                {
                    reverbMix = 0.2f;    // Light reverb
                    reverbDecay = 0.6f;  // Medium decay
                }
            }
            
            // Set delay and chorus based on preset type
            float delayMix = 0.0f;
            float delayTime = 0.25f;
            float chorusMix = 0.0f;
            float chorusRate = 0.5f;
            
            if (name.contains("space") || name.contains("echo") || name.contains("delay"))
            {
                delayMix = 0.3f;
                delayTime = 0.5f;
            }
            else if (name.contains("alien") || name.contains("weird"))
            {
                delayMix = 0.2f;
                delayTime = 0.75f;
                chorusMix = 0.4f;
                chorusRate = 1.5f;
            }
            else if (name.contains("ambient") || name.contains("ethereal"))
            {
                delayMix = 0.25f;
                delayTime = 0.6f;
                chorusMix = 0.3f;
                chorusRate = 0.8f;
            }
            
            // Set EQ based on preset type
            float eqHigh = 0.5f;
            float eqMid = 0.5f;
            float eqLow = 0.5f;
            
            if (name.contains("bright") || name.contains("crisp") || name.contains("shimmer"))
            {
                eqHigh = 0.8f;
                eqMid = 0.6f;
            }
            else if (name.contains("warm") || name.contains("dark") || name.contains("mellow"))
            {
                eqHigh = 0.3f;
                eqLow = 0.8f;
            }
            else if (name.contains("alien") || name.contains("weird"))
            {
                eqHigh = 0.9f;
                eqMid = 0.3f;
                eqLow = 0.7f;
            }
            
            // Apply all the new parameters
            dspChain.setDrive(driveValue);
            dspChain.setDriveType(driveType);
            dspChain.setReverbMix(reverbMix);
            dspChain.setReverbDecay(reverbDecay);
            dspChain.setDelayMix(delayMix);
            dspChain.setDelayTime(delayTime);
            dspChain.setChorusMix(chorusMix);
            dspChain.setChorusRate(chorusRate);
            dspChain.setEQHigh(eqHigh);
            dspChain.setEQMid(eqMid);
            dspChain.setEQLow(eqLow);
            
                    // Show success message (simplified to prevent crashes)
                    DBG("AI Preset Applied Successfully: " + description);
                    DBG("Gain: " + juce::String(gainValue, 2) + ", Tone: " + juce::String(toneValue, 2));
                    DBG("Drive: " + juce::String(driveValue, 2) + " (" + driveType + ")");
                    DBG("Reverb: " + juce::String(reverbMix, 2) + " mix, " + juce::String(reverbDecay, 2) + " decay");
                    DBG("Delay: " + juce::String(delayMix, 2) + " mix, " + juce::String(delayTime, 2) + " time");
                    DBG("Chorus: " + juce::String(chorusMix, 2) + " mix, " + juce::String(chorusRate, 2) + " rate");
                    DBG("EQ: H=" + juce::String(eqHigh, 2) + " M=" + juce::String(eqMid, 2) + " L=" + juce::String(eqLow, 2));
                
            DBG("Successfully applied preset - Gain: " + juce::String(gainValue) + ", Tone: " + juce::String(toneValue));
        }
                else
                {
                    DBG("No chain found in preset JSON");
                    DBG("AI Generation Error: No effect chain found in AI response for: " + description);
                }
    }
            catch (const std::exception& e)
            {
                DBG("Exception applying preset: " + juce::String(e.what()));
                DBG("AI Generation Error: Exception applying preset for: " + description + " - Error: " + e.what());
            }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AIGuitarPluginAudioProcessor();
}