#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * Multi-parameter smoother for glitch-free parameter changes
 * Handles smooth transitions when AI generates new presets
 */
class ParameterSmoother
{
public:
    ParameterSmoother();
    ~ParameterSmoother();
    
    // Setup
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    
    // Parameter management
    void addParameter(const juce::String& paramId, float initialValue = 0.0f);
    void setTargetValue(const juce::String& paramId, float targetValue, float rampTimeSeconds = 0.05f);
    void setCurrentValue(const juce::String& paramId, float currentValue);
    
    // Processing
    void process(int numSamples);
    float getCurrentValue(const juce::String& paramId) const;
    bool isSmoothing(const juce::String& paramId) const;
    bool isAnyParameterSmoothing() const;
    
    // Bulk operations for preset changes
    void beginPresetTransition(float rampTimeSeconds = 0.1f);
    void setAllTargetValues(const std::map<juce::String, float>& targetValues);
    void endPresetTransition();
    
private:
    struct SmoothedParameter
    {
        juce::SmoothedValue<float> smoother;
        float targetValue = 0.0f;
        bool isActive = false;
        
        SmoothedParameter()
        {
            smoother.reset(44100.0, 0.05); // Default: 44.1kHz, 50ms ramp
        }
    };
    
    std::map<juce::String, SmoothedParameter> parameters;
    double currentSampleRate = 44100.0;
    bool isPresetTransition = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSmoother)
};
