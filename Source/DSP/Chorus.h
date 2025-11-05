#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Chorus/Flanger modulation effect
 * Capable of traditional chorus, flanger, and abstract modulation sounds
 */
class Chorus
{
public:
    Chorus();
    ~Chorus();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setRate(float rateHz);         // 0.05 to 5 Hz
    void setDepth(float depth);         // 0 to 1
    void setMix(float mix);             // 0 to 1 (dry/wet)
    
    // Advanced parameters for creative sounds
    void setFeedback(float feedback);   // -0.95 to 0.95 (negative for flanger)
    void setDelay(float delayMs);       // 1 to 50 ms (base delay time)
    void setSpread(float spread);       // 0 to 1 (stereo width)
    void setWaveform(int waveform);     // 0=sine, 1=triangle, 2=saw, 3=square
    
    // Apply parameters from preset
    void applyParameters(const ChorusParams& params);
    
private:
    // Parameters
    bool enabled = true;
    float rate = 0.3f;
    float depth = 0.35f;
    float mix = 0.25f;
    float feedback = 0.0f;
    float baseDelayMs = 7.0f;
    float spread = 0.7f;
    int waveform = 0; // sine
    
    // DSP components
    juce::dsp::DelayLine<float> delayLineL, delayLineR;
    juce::dsp::Oscillator<float> lfoL, lfoR;
    
    // Filtering for feedback
    juce::dsp::IIR::Filter<float> feedbackFilterL, feedbackFilterR;
    
    // Processing state
    double currentSampleRate = 44100.0;
    int maxDelayInSamples = 0;
    bool isPrepared = false;
    
    // LFO phase offset for stereo spread
    float phaseOffset = 0.0f;
    
    // Waveform generation
    float generateLFOSample(float phase, int waveformType);
    
    // Parameter updates
    void updateLFOs();
    void updateDelayLines();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chorus)
};
