#include "Drive.h"

Drive::Drive()
{
}

Drive::~Drive()
{
}

void Drive::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;
    
    // Initialize oversampler
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        numChannels, oversampleFactor, 
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);
    
    oversampler->reset();
    oversampler->initProcessing(static_cast<size_t>(samplesPerBlock));
    
    // Initialize tone filters (low-pass for tone control)
    updateToneFilter();
    
    // Initialize gain stages
    inputGain.prepare(spec);
    outputGain.prepare(spec);
    
    isPrepared = true;
}

void Drive::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (!isPrepared || !enabled)
        return;
    
    juce::dsp::AudioBlock<float> block(buffer);
    
    // Apply input gain
    inputGain.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    // Upsample for distortion processing
    auto oversampledBlock = oversampler->processSamplesUp(block);
    
    // Apply drive/distortion
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = oversampledBlock.getChannelPointer(channel);
        
        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float outputSample = 0.0f;
            
            // Apply drive gain
            float drivenSample = inputSample * (1.0f + drive * 20.0f);
            
            // Apply waveshaping based on drive type
            switch (driveType)
            {
                case DriveType::SoftClip:
                    outputSample = applySoftClip(drivenSample);
                    break;
                case DriveType::HardClip:
                    outputSample = applyHardClip(drivenSample);
                    break;
                case DriveType::TubeScreamer:
                    outputSample = applyTubeScreamer(drivenSample);
                    break;
                case DriveType::Fuzz:
                    outputSample = applyFuzz(drivenSample);
                    break;
            }
            
            channelData[sample] = outputSample;
        }
    }
    
    // Downsample back to original rate
    oversampler->processSamplesDown(block);
    
    // Apply tone filter (per channel)
    if (buffer.getNumChannels() >= 1)
    {
        juce::dsp::AudioBlock<float> monoBlockL(buffer.getArrayOfWritePointers(), 1, buffer.getNumSamples());
        toneFilterL.process(juce::dsp::ProcessContextReplacing<float>(monoBlockL));
    }
    
    if (buffer.getNumChannels() >= 2)
    {
        juce::dsp::AudioBlock<float> monoBlockR(buffer.getArrayOfWritePointers() + 1, 1, buffer.getNumSamples());
        toneFilterR.process(juce::dsp::ProcessContextReplacing<float>(monoBlockR));
    }
    
    // Apply output gain
    outputGain.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void Drive::releaseResources()
{
    if (oversampler)
        oversampler.reset();
    
    isPrepared = false;
}

void Drive::setDriveType(DriveType type)
{
    driveType = type;
}

void Drive::setDrive(float newDrive)
{
    drive = juce::jlimit(0.0f, 1.0f, newDrive);
    
    // Update input gain based on drive amount
    float inputGainValue = 1.0f + drive * 3.0f; // Up to 4x gain
    inputGain.setGainLinear(inputGainValue);
}

void Drive::setTone(float newTone)
{
    tone = juce::jlimit(0.0f, 1.0f, newTone);
    updateToneFilter();
}

void Drive::setOversample(int factor)
{
    if (factor == 1 || factor == 2 || factor == 4)
    {
        oversampleFactor = factor;
        
        // Recreate oversampler if already prepared
        if (isPrepared)
        {
            // Note: In a real implementation, you'd want to do this on a background thread
            // and swap atomically to avoid audio glitches
            oversampler.reset();
            // Would need to call prepareToPlay again or handle this more gracefully
        }
    }
}

void Drive::applyParameters(const DriveParams& params)
{
    setEnabled(params.enabled);
    setDriveType(params.driveType);
    setDrive(params.drive);
    setTone(params.tone);
    setOversample(params.oversample);
}

float Drive::applySoftClip(float sample)
{
    // Hyperbolic tangent soft clipping
    return std::tanh(sample);
}

float Drive::applyHardClip(float sample)
{
    // Hard clipping with slight smoothing
    return juce::jlimit(-0.95f, 0.95f, sample);
}

float Drive::applyTubeScreamer(float sample)
{
    // Asymmetric soft clipping (simulates tube screamer)
    if (sample > 0.0f)
        return std::tanh(sample * 1.5f) * 0.7f;
    else
        return std::tanh(sample * 0.8f) * 0.9f;
}

float Drive::applyFuzz(float sample)
{
    // Square wave-like fuzz distortion
    float sign = (sample >= 0.0f) ? 1.0f : -1.0f;
    float abs_sample = std::abs(sample);
    
    if (abs_sample < 0.1f)
        return sample * 5.0f; // Boost quiet signals
    else
        return sign * (0.5f + 0.5f * std::tanh(abs_sample * 10.0f));
}

void Drive::updateToneFilter()
{
    if (!isPrepared)
        return;
    
    // Tone control: 0.0 = dark (low-pass at ~1kHz), 1.0 = bright (no filtering)
    float cutoffHz = 1000.0f + tone * 9000.0f; // 1kHz to 10kHz
    
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, cutoffHz, 0.707f);
    
    *toneFilterL.state = *coefficients;
    *toneFilterR.state = *coefficients;
}
