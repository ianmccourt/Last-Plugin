#include "DSPChain.h"

DSPChain::DSPChain()
{
    // Initialize with default bypass state
    bypassed = false;
}

DSPChain::~DSPChain()
{
}

void DSPChain::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;
    
    // Initialize gain processor
    gainProcessor.reset(sampleRate, 0.05); // 50ms ramp time
    gainProcessor.setCurrentAndTargetValue(1.0f);
    
    // Initialize tone filters
    updateToneFilter();
    
    // Initialize drive/distortion
    driveGain.prepare({sampleRate, (juce::uint32)samplesPerBlock, 2});
    driveShaper.prepare({sampleRate, (juce::uint32)samplesPerBlock, 2});
    driveShaper.functionToUse = [](float input) {
        // Soft clipping
        return juce::jlimit(-0.95f, 0.95f, input * (1.0f + input * input * 0.5f));
    };
    
    // Initialize delay
    delayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 2.0)); // 2 second max delay
    delayLine.setDelay(static_cast<float>(sampleRate * 0.25)); // 250ms default
    delayMixer.prepare({sampleRate, (juce::uint32)samplesPerBlock, 2});
    delayMixer.setWetMixProportion(0.0f);
    
    // Initialize chorus
    chorusProcessor.prepare({sampleRate, (juce::uint32)samplesPerBlock, 2});
    chorusProcessor.setRate(0.5f);
    chorusProcessor.setDepth(0.5f);
    chorusProcessor.setCentreDelay(7.0f);
    chorusProcessor.setFeedback(0.0f);
    chorusProcessor.setMix(0.0f);
    
    // Initialize EQ filters
    highPassFilterL.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    highPassFilterR.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    lowPassFilterL.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    lowPassFilterR.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    midFilterL.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    midFilterR.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    
    // Set initial EQ parameters
    updateEQFilters();
}

void DSPChain::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;
    
    // Update parameters
    auto gainValue = gainParameter.load();
    auto driveValue = driveParameter.load();
    auto reverbMixValue = reverbMixParameter.load();
    auto reverbDecayValue = reverbDecayParameter.load();
    
    gainProcessor.setTargetValue(gainValue);
    
    // Check if tone parameter changed
    static float lastToneValue = -1.0f;
    auto toneValue = toneParameter.load();
    if (std::abs(toneValue - lastToneValue) > 0.001f)
    {
        updateToneFilter();
        lastToneValue = toneValue;
    }
    
    // Update drive gain
    driveGain.setGainLinear(1.0f + driveValue * 10.0f); // 1x to 11x gain
    
    // Update delay parameters
    auto delayTimeValue = delayTimeParameter.load();
    auto delayMixValue = delayMixParameter.load();
    delayLine.setDelay(static_cast<float>(currentSampleRate * delayTimeValue));
    delayMixer.setWetMixProportion(delayMixValue);
    
    // Update chorus parameters
    auto chorusRateValue = chorusRateParameter.load();
    auto chorusMixValue = chorusMixParameter.load();
    chorusProcessor.setRate(chorusRateValue);
    chorusProcessor.setMix(chorusMixValue);
    
    // Update EQ parameters
    auto eqHighValue = eqHighParameter.load();
    auto eqMidValue = eqMidParameter.load();
    auto eqLowValue = eqLowParameter.load();
    
    // Check if EQ parameters changed
    static float lastEQHigh = -1.0f, lastEQMid = -1.0f, lastEQLow = -1.0f;
    if (std::abs(eqHighValue - lastEQHigh) > 0.001f || 
        std::abs(eqMidValue - lastEQMid) > 0.001f || 
        std::abs(eqLowValue - lastEQLow) > 0.001f)
    {
        updateEQFilters();
        lastEQHigh = eqHighValue;
        lastEQMid = eqMidValue;
        lastEQLow = eqLowValue;
    }
    
    // Create DSP context
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Process each channel with the full chain
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto& toneFilter = (channel == 0) ? toneFilterL : toneFilterR;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Apply gain
            float gainedSample = channelData[sample] * gainProcessor.getNextValue();
            
            // Apply tone filter
            float tonedSample = toneFilter.processSample(gainedSample);
            
            // Apply drive/distortion if enabled
            if (driveValue > 0.001f)
            {
                tonedSample = driveShaper.processSample(tonedSample);
            }
            
            channelData[sample] = tonedSample;
        }
    }
    
    // Apply simple reverb if enabled
    if (reverbMixValue > 0.001f)
    {
        // Store dry signal for mixing
        juce::AudioBuffer<float> dryBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            dryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        }
        
        // Simple delay-based reverb effect
        static const int delaySamples = 4410; // ~100ms at 44.1kHz
        static juce::AudioBuffer<float> delayBuffer(2, delaySamples);
        static int delayIndex = 0;
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            auto* delayData = delayBuffer.getWritePointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                // Get delayed sample
                float delayedSample = delayData[delayIndex];
                
                // Mix current sample with delayed sample
                float reverbSample = channelData[sample] + delayedSample * reverbDecayValue * 0.3f;
                
                // Store in delay buffer
                delayData[delayIndex] = reverbSample;
                
                // Update delay index
                delayIndex = (delayIndex + 1) % delaySamples;
                
                // Apply reverb mix
                float dry = dryBuffer.getSample(channel, sample);
                channelData[sample] = dry * (1.0f - reverbMixValue) + reverbSample * reverbMixValue;
            }
        }
    }
    
    // Apply delay if enabled
    if (delayMixValue > 0.001f)
    {
        // Store dry signal for delay mixing
        juce::AudioBuffer<float> dryBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            dryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        }
        
        // Process delay
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float delayedSample = delayLine.popSample(channel, 0.0f);
                float delayedWithFeedback = channelData[sample] + delayedSample * 0.3f; // 30% feedback
                delayLine.pushSample(channel, delayedWithFeedback);
                channelData[sample] = delayedWithFeedback;
            }
        }
        
        // Mix dry and wet signals
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            auto* dryData = dryBuffer.getWritePointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float dry = dryData[sample];
                float wet = channelData[sample];
                channelData[sample] = dry * (1.0f - delayMixValue) + wet * delayMixValue;
            }
        }
    }
    
    // Apply chorus if enabled
    if (chorusMixValue > 0.001f)
    {
        chorusProcessor.process(context);
    }
    
    // Apply EQ filters (simplified implementation)
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto& highPass = (channel == 0) ? highPassFilterL : highPassFilterR;
        auto& lowPass = (channel == 0) ? lowPassFilterL : lowPassFilterR;
        auto& mid = (channel == 0) ? midFilterL : midFilterR;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float sampleValue = channelData[sample];
            
            // Apply EQ chain
            sampleValue = highPass.processSample(sampleValue);
            sampleValue = mid.processSample(sampleValue);
            sampleValue = lowPass.processSample(sampleValue);
            
            channelData[sample] = sampleValue;
        }
    }
}

void DSPChain::reset()
{
    gainProcessor.reset(currentSampleRate, 0.05);
    toneFilterL.reset();
    toneFilterR.reset();
    // Note: Reverb uses static buffers, no reset needed
}

void DSPChain::setBypassed(bool shouldBeBypassed)
{
    bypassed = shouldBeBypassed;
}

bool DSPChain::isBypassed() const
{
    return bypassed;
}

void DSPChain::setGain(float newGain)
{
    gainParameter.store(juce::jlimit(0.0f, 2.0f, newGain));
}

void DSPChain::setTone(float newTone)
{
    toneParameter.store(juce::jlimit(0.0f, 1.0f, newTone));
}

void DSPChain::setDrive(float newDrive)
{
    driveParameter.store(juce::jlimit(0.0f, 1.0f, newDrive));
}

void DSPChain::setDriveType(const juce::String& type)
{
    // Convert string to int for atomic storage
    int typeIndex = 0; // default to softclip
    if (type == "hardclip")
        typeIndex = 1;
    else if (type == "fuzz")
        typeIndex = 2;
    
    driveTypeParameter.store(typeIndex);
    
    // Update the waveshaper function based on type
    if (type == "hardclip")
    {
        driveShaper.functionToUse = [](float input) {
            return juce::jlimit(-0.95f, 0.95f, input);
        };
    }
    else if (type == "fuzz")
    {
        driveShaper.functionToUse = [](float input) {
            // Aggressive fuzz
            return juce::jlimit(-0.95f, 0.95f, input * (1.0f + input * input * 2.0f));
        };
    }
    else // default to softclip
    {
        driveShaper.functionToUse = [](float input) {
            return juce::jlimit(-0.95f, 0.95f, input * (1.0f + input * input * 0.5f));
        };
    }
}

void DSPChain::setReverbMix(float newMix)
{
    reverbMixParameter.store(juce::jlimit(0.0f, 1.0f, newMix));
}

void DSPChain::setReverbDecay(float newDecay)
{
    reverbDecayParameter.store(juce::jlimit(0.0f, 1.0f, newDecay));
}

void DSPChain::setDelayMix(float newMix)
{
    delayMixParameter.store(juce::jlimit(0.0f, 1.0f, newMix));
}

void DSPChain::setDelayTime(float newTime)
{
    delayTimeParameter.store(juce::jlimit(0.0f, 2.0f, newTime));
}

void DSPChain::setChorusMix(float newMix)
{
    chorusMixParameter.store(juce::jlimit(0.0f, 1.0f, newMix));
}

void DSPChain::setChorusRate(float newRate)
{
    chorusRateParameter.store(juce::jlimit(0.1f, 5.0f, newRate));
}

void DSPChain::setEQHigh(float newHigh)
{
    eqHighParameter.store(juce::jlimit(0.0f, 1.0f, newHigh));
}

void DSPChain::setEQMid(float newMid)
{
    eqMidParameter.store(juce::jlimit(0.0f, 1.0f, newMid));
}

void DSPChain::setEQLow(float newLow)
{
    eqLowParameter.store(juce::jlimit(0.0f, 1.0f, newLow));
}

void DSPChain::updateFromPreset(const PresetData& preset)
{
    // This is a simplified version - in the full implementation,
    // we would parse the preset and update all effect parameters
    
    // For now, just set some basic parameters
    setGain(1.0f);
    setTone(0.5f);
    
    // TODO: Parse preset.chain and configure all effects
    juce::ignoreUnused(preset);
}

void DSPChain::updateToneFilter()
{
    // Simple tone control using a low-pass filter
    // toneValue: 0.0 = dark, 1.0 = bright
    auto toneValue = toneParameter.load();
    
    // Calculate cutoff frequency (500Hz to 8kHz range)
    float cutoffFreq = 500.0f + (toneValue * 7500.0f);
    
    // Create filter coefficients
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, cutoffFreq, 0.707f);
    
    // Update both channel filters
    toneFilterL.coefficients = coeffs;
    toneFilterR.coefficients = coeffs;
}

void DSPChain::updateEQFilters()
{
    auto eqHighValue = eqHighParameter.load();
    auto eqMidValue = eqMidParameter.load();
    auto eqLowValue = eqLowParameter.load();
    
    // High-pass filter (removes low frequencies)
    float highPassFreq = 20.0f + eqLowValue * 2000.0f; // 20Hz to 2kHz
    auto highPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        currentSampleRate, highPassFreq, 0.707f);
    highPassFilterL.coefficients = highPassCoeffs;
    highPassFilterR.coefficients = highPassCoeffs;
    
    // Low-pass filter (removes high frequencies)
    float lowPassFreq = 2000.0f + eqHighValue * 18000.0f; // 2kHz to 20kHz
    auto lowPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, lowPassFreq, 0.707f);
    lowPassFilterL.coefficients = lowPassCoeffs;
    lowPassFilterR.coefficients = lowPassCoeffs;
    
    // Mid filter (parametric boost/cut around 1kHz)
    float midFreq = 1000.0f;
    float midGain = (eqMidValue - 0.5f) * 12.0f; // -6dB to +6dB
    auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate, midFreq, 0.7f, juce::Decibels::decibelsToGain(midGain));
    midFilterL.coefficients = midCoeffs;
    midFilterR.coefficients = midCoeffs;
}