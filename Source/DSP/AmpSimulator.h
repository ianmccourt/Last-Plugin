#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Guitar amplifier simulator with multiple models
 * Includes traditional and experimental amp models for wide tonal range
 */
class AmpSimulator
{
public:
    AmpSimulator();
    ~AmpSimulator();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setAmpModel(AmpModel model);
    void setGain(float gain);           // 0 to 1
    void setBass(float bass);           // 0 to 1
    void setMid(float mid);             // 0 to 1
    void setTreble(float treble);       // 0 to 1
    void setPresence(float presence);   // 0 to 1
    void setMaster(float master);       // 0 to 1
    
    // Apply parameters from preset
    void applyParameters(const AmpParams& params);
    
private:
    // Parameters
    bool enabled = true;
    AmpModel ampModel = AmpModel::CleanBlackface;
    float gain = 0.45f;
    float bass = 0.48f;
    float mid = 0.35f;
    float treble = 0.62f;
    float presence = 0.52f;
    float master = 0.7f;
    
    // DSP components
    juce::dsp::Gain<float> inputGain, outputGain;
    
    // Tone stack filters (based on classic amp circuits)
    juce::dsp::IIR::Filter<float> bassFilterL, bassFilterR;
    juce::dsp::IIR::Filter<float> midFilterL, midFilterR;
    juce::dsp::IIR::Filter<float> trebleFilterL, trebleFilterR;
    juce::dsp::IIR::Filter<float> presenceFilterL, presenceFilterR;
    
    // Tube saturation simulation
    juce::dsp::WaveShaper<float> preampStage;
    juce::dsp::WaveShaper<float> powerAmpStage;
    
    // Oversampling for nonlinear processing
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    
    // Processing state
    double currentSampleRate = 44100.0;
    bool isPrepared = false;
    
    // Amp model implementations
    void setupCleanBlackface();
    void setupJanglyVox();
    void setupBritCrunch();
    void setupHiGain();
    
    // Tone stack calculations (based on real amp circuits)
    void updateToneStack();
    void calculateBlackfaceToneStack();
    void calculateVoxToneStack();
    void calculateMarshallToneStack();
    void calculateModernToneStack();
    
    // Tube saturation functions
    static float tubeWarmSaturation(float sample);
    static float tubeCrunchSaturation(float sample);
    static float tubeHiGainSaturation(float sample);
    static float solidStateClipping(float sample);
    
    // Utility functions
    void updateFilters();
    void updateSaturation();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpSimulator)
};
