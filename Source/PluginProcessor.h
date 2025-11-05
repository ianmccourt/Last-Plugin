#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/DSPChain.h"
#include "Preset/PresetManager.h"
#include "Utils/ParameterSmoother.h"

class AIGuitarPluginAudioProcessor : public juce::AudioProcessor
{
public:
    AIGuitarPluginAudioProcessor();
    ~AIGuitarPluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

            // AI Preset Generation
            void generatePresetFromText(const juce::String& description);
            void generatePresetVariations(const juce::String& baseDescription, int count = 3);
            void generateSurpriseMe();
            void cancelPresetGeneration();
            void applyPresetFromJson(const juce::var& presetJson, const juce::String& description);
    
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    
    // Status queries
    bool isGeneratingPreset() const;
    int getGenerationQueueSize() const;
    
    // Listeners (called on message thread) - TODO: Implement when threading is added
    // std::function<void(const juce::String&)> onPresetGenerationStarted;
    // std::function<void(const PresetData&, const juce::String&, int)> onPresetGenerationCompleted;
    // std::function<void(const juce::String&, const juce::String&)> onPresetGenerationFailed;
    // std::function<void(const juce::String&, float)> onStatusUpdate;

private:
    // Parameter system
    juce::AudioProcessorValueTreeState parameters;
    
    // DSP components
    DSPChain dspChain;
    ParameterSmoother parameterSmoother;
    
    // Preset management
    PresetManager presetManager;
    
    // Parameter creation
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateDSPFromParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AIGuitarPluginAudioProcessor)
};
