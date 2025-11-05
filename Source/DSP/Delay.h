#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Delay effect with feedback and modulation
 * Supports everything from slapback to infinite ambient delays
 */
class Delay
{
public:
    Delay();
    ~Delay();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setDelayTime(float timeMs);        // 40 to 1200 ms
    void setFeedback(float feedback);       // 0 to 0.95
    void setMix(float mix);                 // 0 to 1 (dry/wet)
    
    // Advanced parameters for creative delays
    void setModulationRate(float rateHz);   // 0 to 2 Hz (tape wow/flutter)
    void setModulationDepth(float depth);   // 0 to 1
    void setHighCut(float cutoffHz);        // 1000 to 20000 Hz (analog simulation)
    void setLowCut(float cutoffHz);         // 20 to 500 Hz
    void setStereoSpread(float spread);     // 0 to 1 (ping-pong effect)
    void setDiffusion(float diffusion);     // 0 to 1 (smear the repeats)
    
    // Apply parameters from preset
    void applyParameters(const DelayParams& params);
    
    // Tempo sync (for future implementation)
    void setTempoSync(bool sync) { tempoSync = sync; }
    void setBPM(double bpm) { currentBPM = bpm; }
    
private:
    // Parameters
    bool enabled = true;
    float delayTimeMs = 420.0f;
    float feedback = 0.35f;
    float mix = 0.2f;
    float modulationRate = 0.1f;
    float modulationDepth = 0.05f;
    float highCutHz = 8000.0f;
    float lowCutHz = 80.0f;
    float stereoSpread = 0.3f;
    float diffusion = 0.1f;
    
    // Tempo sync
    bool tempoSync = false;
    double currentBPM = 120.0;
    
    // DSP components
    juce::dsp::DelayLine<float> delayLineL, delayLineR;
    juce::dsp::Oscillator<float> modulationLFO;
    
    // Filtering for analog character
    juce::dsp::IIR::Filter<float> highCutFilterL, highCutFilterR;
    juce::dsp::IIR::Filter<float> lowCutFilterL, lowCutFilterR;
    
    // Diffusion (all-pass filters for smearing)
    juce::dsp::IIR::Filter<float> diffusionFilter1L, diffusionFilter1R;
    juce::dsp::IIR::Filter<float> diffusionFilter2L, diffusionFilter2R;
    
    // Processing state
    double currentSampleRate = 44100.0;
    int maxDelayInSamples = 0;
    bool isPrepared = false;
    
    // Ping-pong delay state
    float crossfeedL = 0.0f;
    float crossfeedR = 0.0f;
    
    // Parameter updates
    void updateDelayTime();
    void updateFilters();
    void updateModulation();
    void updateDiffusion();
    
    // Utility functions
    int msToSamples(float ms) const;
    float samplesToMs(int samples) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};
