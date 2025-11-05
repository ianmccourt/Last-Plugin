#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Preset/PresetSchema.h"

class Drive
{
public:
    Drive();
    ~Drive();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setDriveType(DriveType type);
    void setDrive(float drive);      // 0.0 to 1.0
    void setTone(float tone);        // 0.0 to 1.0 (low-pass filter)
    void setOversample(int factor);  // 1, 2, or 4
    
    // Apply parameters from preset
    void applyParameters(const DriveParams& params);
    
private:
    // Parameters
    bool enabled = true;
    DriveType driveType = DriveType::SoftClip;
    float drive = 0.5f;
    float tone = 0.5f;
    int oversampleFactor = 2;
    
    // DSP components
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    juce::dsp::IIR::Filter<float> toneFilterL, toneFilterR;
    juce::dsp::Gain<float> inputGain, outputGain;
    
    // Processing state
    double currentSampleRate = 44100.0;
    bool isPrepared = false;
    
    // Waveshaping functions
    float applySoftClip(float sample);
    float applyHardClip(float sample);
    float applyTubeScreamer(float sample);
    float applyFuzz(float sample);
    
    // Tone filter update
    void updateToneFilter();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Drive)
};
