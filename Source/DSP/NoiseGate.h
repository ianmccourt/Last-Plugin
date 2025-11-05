#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Noise gate for clean signal processing
 * Includes creative gating effects for rhythmic and abstract sounds
 */
class NoiseGate
{
public:
    NoiseGate();
    ~NoiseGate();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setThreshold(float thresholdDb);   // -90 to 0 dB
    void setRelease(float releaseMs);       // 5 to 500 ms
    
    // Advanced parameters for creative gating
    void setAttack(float attackMs);         // 0.1 to 50 ms
    void setHold(float holdMs);             // 0 to 100 ms
    void setLookahead(float lookaheadMs);   // 0 to 5 ms
    void setHysteresis(float hysteresisDb); // 0 to 10 dB (prevents chattering)
    
    // Creative gating modes
    void setGateMode(int mode);             // 0=normal, 1=ducking, 2=rhythmic
    void setRhythmicRate(float rateHz);     // 0.1 to 10 Hz (for rhythmic gating)
    void setRhythmicDepth(float depth);     // 0 to 1
    
    // Apply parameters from preset
    void applyParameters(const NoiseGateParams& params);
    
    // Metering
    float getCurrentGateState() const { return currentGateState; }
    float getInputLevel() const { return inputLevel; }
    
private:
    // Parameters
    bool enabled = true;
    float thresholdDb = -55.0f;
    float releaseMs = 100.0f;
    float attackMs = 1.0f;
    float holdMs = 10.0f;
    float lookaheadMs = 1.0f;
    float hysteresisDb = 3.0f;
    
    // Creative parameters
    int gateMode = 0; // 0=normal, 1=ducking, 2=rhythmic
    float rhythmicRate = 2.0f;
    float rhythmicDepth = 0.8f;
    
    // DSP components
    juce::dsp::DelayLine<float> lookaheadDelayL, lookaheadDelayR;
    juce::dsp::Oscillator<float> rhythmicLFO;
    
    // Envelope detection
    juce::SmoothedValue<float> gateEnvelope;
    
    // Processing state
    double currentSampleRate = 44100.0;
    bool isPrepared = false;
    bool gateOpen = false;
    
    // Hold timer
    int holdSamples = 0;
    int holdCounter = 0;
    
    // Hysteresis thresholds
    float openThreshold = 0.0f;
    float closeThreshold = 0.0f;
    
    // Metering
    std::atomic<float> currentGateState{0.0f};
    std::atomic<float> inputLevel{0.0f};
    
    // Gate state calculation
    bool shouldGateOpen(float inputLevel);
    bool shouldGateClose(float inputLevel);
    
    // Level detection
    float calculateLevel(const float* samples, int numSamples);
    
    // Creative gating
    float calculateRhythmicGating();
    float calculateDuckingGating(float inputLevel);
    
    // Parameter updates
    void updateThresholds();
    void updateEnvelope();
    void updateRhythmicLFO();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGate)
};
