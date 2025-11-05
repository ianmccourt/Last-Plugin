#pragma once

#include <JuceHeader.h>
#include "../Preset/PresetSchema.h"

/**
 * Multi-band parametric equalizer
 * Supports traditional tone shaping and creative frequency manipulation
 */
class Equalizer
{
public:
    Equalizer();
    ~Equalizer();
    
    // Audio processing lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();
    
    // Parameter control
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    // Standard 3-band EQ parameters
    void setLowShelfFreq(float freqHz);     // 60 to 200 Hz
    void setLowShelfGain(float gainDb);     // -12 to 12 dB
    void setMidFreq(float freqHz);          // 300 to 3000 Hz
    void setMidQ(float q);                  // 0.3 to 4
    void setMidGain(float gainDb);          // -12 to 12 dB
    void setHighShelfFreq(float freqHz);    // 4000 to 10000 Hz
    void setHighShelfGain(float gainDb);    // -12 to 12 dB
    
    // Advanced parameters for creative EQ
    void setLowShelfQ(float q);             // 0.3 to 2 (shelf slope)
    void setHighShelfQ(float q);            // 0.3 to 2 (shelf slope)
    void setTiltBalance(float tilt);        // -1 to 1 (tilt EQ)
    void setAnalogModeling(bool enabled);   // Analog saturation simulation
    
    // Apply parameters from preset
    void applyParameters(const EqualizerParams& params);
    
    // Frequency response analysis (for UI)
    void getFrequencyResponse(std::vector<float>& frequencies, 
                            std::vector<float>& magnitudes);
    
private:
    // Parameters
    bool enabled = true;
    float lowShelfFreq = 120.0f;
    float lowShelfGain = 1.5f;
    float lowShelfQ = 0.707f;
    float midFreq = 1200.0f;
    float midQ = 0.9f;
    float midGain = -1.0f;
    float highShelfFreq = 6000.0f;
    float highShelfGain = 1.0f;
    float highShelfQ = 0.707f;
    float tiltBalance = 0.0f;
    bool analogModeling = true;
    
    // DSP components
    juce::dsp::IIR::Filter<float> lowShelfL, lowShelfR;
    juce::dsp::IIR::Filter<float> midBandL, midBandR;
    juce::dsp::IIR::Filter<float> highShelfL, highShelfR;
    juce::dsp::IIR::Filter<float> tiltFilterL, tiltFilterR;
    
    // Analog modeling
    juce::dsp::WaveShaper<float> analogSaturation;
    juce::dsp::IIR::Filter<float> analogFilterL, analogFilterR;
    
    // Processing state
    double currentSampleRate = 44100.0;
    bool isPrepared = false;
    
    // Filter coefficient calculation
    void updateLowShelf();
    void updateMidBand();
    void updateHighShelf();
    void updateTiltFilter();
    void updateAnalogModeling();
    
    // Analog modeling functions
    static float analogSaturationFunction(float sample);
    void applyAnalogCharacter(juce::AudioBuffer<float>& buffer);
    
    // Utility functions
    juce::dsp::IIR::Coefficients<float>::Ptr makeLowShelf(double sampleRate, 
                                                          float freq, float q, float gain);
    juce::dsp::IIR::Coefficients<float>::Ptr makeHighShelf(double sampleRate, 
                                                           float freq, float q, float gain);
    juce::dsp::IIR::Coefficients<float>::Ptr makePeakingFilter(double sampleRate, 
                                                               float freq, float q, float gain);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Equalizer)
};
