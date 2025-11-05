#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Dynamic range compressor
 * Supports traditional compression and creative pumping effects
 */
class Compressor
{
public:
    Compressor();
    ~Compressor();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setRatio(float ratio);             // 1 to 10
    void setThreshold(float thresholdDb);   // -60 to 0 dB
    void setAttack(float attackMs);         // 0.1 to 50 ms
    void setRelease(float releaseMs);       // 10 to 500 ms
    void setMakeupGain(float makeupDb);     // -12 to 12 dB
    
    // Advanced parameters for creative compression
    void setKnee(float knee);               // 0 to 1 (hard to soft knee)
    void setLookahead(float lookaheadMs);   // 0 to 10 ms
    void setSidechain(bool enabled);        // External sidechain input
    void setMix(float mix);                 // 0 to 1 (parallel compression)
    
    // Apply parameters from preset
    void applyParameters(const CompressorParams& params);
    
    // Metering
    float getCurrentGainReduction() const { return currentGainReduction; }
    float getInputLevel() const { return inputLevel; }
    float getOutputLevel() const { return outputLevel; }
    
private:
    // Parameters
    bool enabled = true;
    float ratio = 3.0f;
    float thresholdDb = -18.0f;
    float attackMs = 10.0f;
    float releaseMs = 60.0f;
    float makeupDb = 2.0f;
    float knee = 0.5f;
    float lookaheadMs = 2.0f;
    bool sidechainEnabled = false;
    float mix = 1.0f;
    
    // DSP components
    juce::dsp::Gain<float> inputGain, outputGain, makeupGain;
    juce::dsp::DelayLine<float> lookaheadDelayL, lookaheadDelayR;
    
    // Envelope followers
    juce::SmoothedValue<float> envelopeFollower;
    juce::SmoothedValue<float> gainSmoother;
    
    // Peak detection
    float peakHoldL = 0.0f;
    float peakHoldR = 0.0f;
    int peakHoldCounter = 0;
    
    // Processing state
    double currentSampleRate = 44100.0;
    bool isPrepared = false;
    
    // Metering
    std::atomic<float> currentGainReduction{0.0f};
    std::atomic<float> inputLevel{0.0f};
    std::atomic<float> outputLevel{0.0f};
    
    // Compression calculation
    float calculateGainReduction(float inputLevel);
    float applyKnee(float inputLevel, float threshold, float knee);
    
    // RMS/Peak detection
    float calculateRMS(const float* samples, int numSamples);
    float calculatePeak(const float* samples, int numSamples);
    
    // Parameter updates
    void updateEnvelopeFollower();
    void updateLookahead();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};
