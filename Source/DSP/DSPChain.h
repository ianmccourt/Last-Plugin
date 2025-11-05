#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../Preset/PresetSchema.h"

/**
 * Simplified DSP processing chain for basic audio processing
 * Handles gain and tone controls with smooth parameter changes
 */
class DSPChain
{
public:
    DSPChain();
    ~DSPChain();
    
    // Audio processing
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();
    
    // Bypass control
    void setBypassed(bool shouldBeBypassed);
    bool isBypassed() const;
    
    // Parameter control
    void setGain(float newGain);
    void setTone(float newTone);
    void setDrive(float newDrive);
    void setDriveType(const juce::String& type);
    void setReverbMix(float newMix);
    void setReverbDecay(float newDecay);
    void setDelayMix(float newMix);
    void setDelayTime(float newTime);
    void setChorusMix(float newMix);
    void setChorusRate(float newRate);
    void setEQHigh(float newHigh);
    void setEQMid(float newMid);
    void setEQLow(float newLow);
    
    // Preset management (simplified)
    void updateFromPreset(const PresetData& preset);
    
private:
    // Audio processing state
    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    bool bypassed = false;
    
    // Basic processing
    juce::SmoothedValue<float> gainProcessor;
    juce::dsp::IIR::Filter<float> toneFilterL, toneFilterR;
    
    // Drive/distortion
    juce::dsp::Gain<float> driveGain;
    juce::dsp::WaveShaper<float> driveShaper;
    
    // Delay effect
    juce::dsp::DelayLine<float> delayLine;
    juce::dsp::DryWetMixer<float> delayMixer;
    
    // Chorus effect
    juce::dsp::Chorus<float> chorusProcessor;
    
    // EQ effect (simplified implementation)
    juce::dsp::IIR::Filter<float> highPassFilterL, highPassFilterR;
    juce::dsp::IIR::Filter<float> lowPassFilterL, lowPassFilterR;
    juce::dsp::IIR::Filter<float> midFilterL, midFilterR;
    
    // Atomic parameters for thread safety
    std::atomic<float> gainParameter{1.0f};
    std::atomic<float> toneParameter{0.5f};
    std::atomic<float> driveParameter{0.0f};
    std::atomic<int> driveTypeParameter{0}; // 0=softclip, 1=hardclip, 2=fuzz
    std::atomic<float> reverbMixParameter{0.0f};
    std::atomic<float> reverbDecayParameter{0.5f};
    std::atomic<float> delayMixParameter{0.0f};
    std::atomic<float> delayTimeParameter{0.25f};
    std::atomic<float> chorusMixParameter{0.0f};
    std::atomic<float> chorusRateParameter{0.5f};
    std::atomic<float> eqHighParameter{0.5f};
    std::atomic<float> eqMidParameter{0.5f};
    std::atomic<float> eqLowParameter{0.5f};
    
    // Helper methods
    void updateToneFilter();
    void updateEQFilters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DSPChain)
};