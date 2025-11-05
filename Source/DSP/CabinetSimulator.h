#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Cabinet simulator using impulse response convolution
 * Supports multiple cabinet types and dynamic IR loading
 */
class CabinetSimulator
{
public:
    CabinetSimulator();
    ~CabinetSimulator();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setCabinetIR(CabinetIR irType);
    void setLowCut(float cutoffHz);     // 20 to 200 Hz
    void setHighCut(float cutoffHz);    // 3000 to 12000 Hz
    
    // Advanced parameters
    void setMicPosition(float position); // 0 to 1 (on-axis to off-axis simulation)
    void setRoomAmbience(float amount);  // 0 to 1 (room tone blend)
    void setIRLength(float lengthMs);    // 10 to 500 ms (for creative effects)
    
    // Apply parameters from preset
    void applyParameters(const CabinetParams& params);
    
    // IR management
    bool loadCustomIR(const juce::File& irFile);
    void generateBuiltInIR(CabinetIR irType);
    
private:
    // Parameters
    bool enabled = true;
    CabinetIR currentIR = CabinetIR::TwoByTwelveOpen;
    float lowCutHz = 70.0f;
    float highCutHz = 8000.0f;
    float micPosition = 0.5f;
    float roomAmbience = 0.1f;
    float irLengthMs = 100.0f;
    
    // DSP components
    juce::dsp::Convolution convolutionL, convolutionR;
    
    // Filtering
    juce::dsp::IIR::Filter<float> lowCutFilterL, lowCutFilterR;
    juce::dsp::IIR::Filter<float> highCutFilterL, highCutFilterR;
    
    // Room ambience simulation
    juce::dsp::Reverb roomReverb;
    juce::AudioBuffer<float> ambienceBuffer;
    
    // Processing state
    double currentSampleRate = 44100.0;
    bool isPrepared = false;
    bool irLoaded = false;
    
    // IR data
    juce::AudioBuffer<float> currentIRBuffer;
    std::atomic<bool> irUpdatePending{false};
    
    // Built-in IR generation (simplified cabinet responses)
    void generateOneTwelveOpenIR();
    void generateTwoTwelveOpenIR();
    void generateFourTwelveClosedIR();
    
    // IR processing utilities
    void processIRWithMicPosition(juce::AudioBuffer<float>& ir, float position);
    void applyIRLengthLimit(juce::AudioBuffer<float>& ir, float lengthMs);
    
    // Parameter updates
    void updateFilters();
    void updateConvolution();
    void updateRoomAmbience();
    
    // Thread-safe IR loading
    void loadIRSafely(const juce::AudioBuffer<float>& newIR);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabinetSimulator)
};
