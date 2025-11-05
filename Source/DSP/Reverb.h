#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Preset/PresetSchema.h"

/**
 * Multi-algorithm reverb processor
 * Supports Room, Plate, Hall, and Shimmer algorithms for wide sonic range
 */
class Reverb
{
public:
    Reverb();
    ~Reverb();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setAlgorithm(ReverbAlgorithm algorithm);
    void setPreDelay(float preDelayMs);     // 0 to 60 ms
    void setDecayTime(float decaySeconds);  // 0.2 to 12 seconds
    void setDamping(float damping);         // 0 to 1 (high frequency damping)
    void setMix(float mix);                 // 0 to 1 (dry/wet)
    
    // Advanced parameters for abstract sounds
    void setModulationRate(float rateHz);   // 0.1 to 5 Hz (for shimmer/chorus)
    void setModulationDepth(float depth);   // 0 to 1
    void setShimmerPitch(float semitones);  // -12 to +12 semitones
    void setShimmerMix(float mix);          // 0 to 1
    
    // Apply parameters from preset
    void applyParameters(const ReverbParams& params);
    
private:
    // Parameters
    bool enabled = true;
    ReverbAlgorithm algorithm = ReverbAlgorithm::Plate;
    float preDelayMs = 12.0f;
    float decaySeconds = 4.5f;
    float damping = 0.35f;
    float mix = 0.28f;
    
    // Advanced parameters
    float modulationRate = 0.5f;
    float modulationDepth = 0.3f;
    float shimmerPitch = 12.0f;  // One octave up
    float shimmerMix = 0.3f;
    
    // DSP components
    juce::dsp::Reverb juceReverb;
    juce::dsp::DelayLine<float> preDelayLine;
    
    // Shimmer effect components
    juce::dsp::Oversampling<float> shimmerOversampler;
    std::unique_ptr<juce::dsp::FFT> fft;
    std::vector<std::complex<float>> fftBuffer;
    juce::AudioBuffer<float> shimmerBuffer;
    
    // Modulation
    juce::dsp::Oscillator<float> lfoL, lfoR;
    juce::dsp::DelayLine<float> modulationDelayL, modulationDelayR;
    
    // Filtering for damping
    juce::dsp::IIR::Filter<float> dampingFilterL, dampingFilterR;
    
    // Processing state
    double currentSampleRate = 44100.0;
    int currentNumChannels = 2;
    bool isPrepared = false;
    
    // Algorithm-specific processing
    void processRoom(juce::AudioBuffer<float>& buffer);
    void processPlate(juce::AudioBuffer<float>& buffer);
    void processHall(juce::AudioBuffer<float>& buffer);
    void processShimmer(juce::AudioBuffer<float>& buffer);
    
    // Shimmer effect helpers
    void processShimmerEffect(juce::AudioBuffer<float>& buffer);
    void pitchShift(juce::AudioBuffer<float>& buffer, float semitones);
    
    // Parameter updates
    void updateReverbParameters();
    void updateDampingFilter();
    void updateModulation();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};
