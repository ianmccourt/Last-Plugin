#include "Reverb.h"

Reverb::Reverb()
    : shimmerOversampler(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
}

Reverb::~Reverb()
{
}

void Reverb::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;
    currentNumChannels = numChannels;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);
    
    // Initialize main reverb
    juceReverb.prepare(spec);
    updateReverbParameters();
    
    // Initialize pre-delay
    preDelayLine.prepare(spec);
    preDelayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.06)); // 60ms max
    
    // Initialize shimmer components
    shimmerOversampler.reset();
    shimmerOversampler.initProcessing(static_cast<size_t>(samplesPerBlock));
    
    // Initialize FFT for pitch shifting (shimmer effect)
    int fftOrder = 11; // 2048 samples
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);
    fftBuffer.resize(static_cast<size_t>(fft->getSize() * 2));
    
    shimmerBuffer.setSize(numChannels, samplesPerBlock);
    
    // Initialize modulation LFOs
    lfoL.prepare(spec);
    lfoR.prepare(spec);
    lfoL.initialise([](float x) { return std::sin(x); });
    lfoR.initialise([](float x) { return std::sin(x); });
    
    // Initialize modulation delay lines
    modulationDelayL.prepare(spec);
    modulationDelayR.prepare(spec);
    modulationDelayL.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.02)); // 20ms max
    modulationDelayR.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.02));
    
    // Initialize damping filters
    dampingFilterL.prepare(spec);
    dampingFilterR.prepare(spec);
    updateDampingFilter();
    
    updateModulation();
    
    isPrepared = true;
}

void Reverb::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (!isPrepared || !enabled)
        return;
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Store dry signal for mixing
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    // Apply pre-delay
    if (preDelayMs > 0.1f)
    {
        int delaySamples = static_cast<int>(preDelayMs * currentSampleRate / 1000.0f);
        preDelayLine.setDelay(static_cast<float>(delaySamples));
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float delayedSample = preDelayLine.popSample(channel);
                preDelayLine.pushSample(channel, channelData[sample]);
                channelData[sample] = delayedSample;
            }
        }
    }
    
    // Process based on algorithm
    switch (algorithm)
    {
        case ReverbAlgorithm::Room:
            processRoom(buffer);
            break;
        case ReverbAlgorithm::Plate:
            processPlate(buffer);
            break;
        case ReverbAlgorithm::Hall:
            processHall(buffer);
            break;
        case ReverbAlgorithm::Shimmer:
            processShimmer(buffer);
            break;
    }
    
    // Apply damping filter
    juce::dsp::AudioBlock<float> dampingBlock(buffer);
    if (buffer.getNumChannels() >= 1)
    {
        juce::dsp::AudioBlock<float> monoBlockL(buffer.getArrayOfWritePointers(), 1, buffer.getNumSamples());
        dampingFilterL.process(juce::dsp::ProcessContextReplacing<float>(monoBlockL));
    }
    if (buffer.getNumChannels() >= 2)
    {
        juce::dsp::AudioBlock<float> monoBlockR(buffer.getArrayOfWritePointers() + 1, 1, buffer.getNumSamples());
        dampingFilterR.process(juce::dsp::ProcessContextReplacing<float>(monoBlockR));
    }
    
    // Mix dry and wet signals
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* wetData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            wetData[sample] = dryData[sample] * (1.0f - mix) + wetData[sample] * mix;
        }
    }
}

void Reverb::releaseResources()
{
    juceReverb.reset();
    isPrepared = false;
}

void Reverb::setAlgorithm(ReverbAlgorithm newAlgorithm)
{
    algorithm = newAlgorithm;
    if (isPrepared)
        updateReverbParameters();
}

void Reverb::setPreDelay(float newPreDelayMs)
{
    preDelayMs = juce::jlimit(0.0f, 60.0f, newPreDelayMs);
}

void Reverb::setDecayTime(float newDecaySeconds)
{
    decaySeconds = juce::jlimit(0.2f, 12.0f, newDecaySeconds);
    if (isPrepared)
        updateReverbParameters();
}

void Reverb::setDamping(float newDamping)
{
    damping = juce::jlimit(0.0f, 1.0f, newDamping);
    if (isPrepared)
        updateDampingFilter();
}

void Reverb::setMix(float newMix)
{
    mix = juce::jlimit(0.0f, 1.0f, newMix);
}

void Reverb::setModulationRate(float rateHz)
{
    modulationRate = juce::jlimit(0.1f, 5.0f, rateHz);
    if (isPrepared)
        updateModulation();
}

void Reverb::setModulationDepth(float depth)
{
    modulationDepth = juce::jlimit(0.0f, 1.0f, depth);
}

void Reverb::setShimmerPitch(float semitones)
{
    shimmerPitch = juce::jlimit(-12.0f, 12.0f, semitones);
}

void Reverb::setShimmerMix(float newShimmerMix)
{
    shimmerMix = juce::jlimit(0.0f, 1.0f, newShimmerMix);
}

void Reverb::applyParameters(const ReverbParams& params)
{
    setEnabled(params.enabled);
    setAlgorithm(params.algorithm);
    setPreDelay(params.preDelayMs);
    setDecayTime(params.decayS);
    setDamping(params.damping);
    setMix(params.mix);
}

void Reverb::processRoom(juce::AudioBuffer<float>& buffer)
{
    // Room algorithm: shorter, more intimate reverb
    juce::dsp::AudioBlock<float> block(buffer);
    juceReverb.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void Reverb::processPlate(juce::AudioBuffer<float>& buffer)
{
    // Plate algorithm: classic studio plate reverb sound
    juce::dsp::AudioBlock<float> block(buffer);
    juceReverb.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void Reverb::processHall(juce::AudioBuffer<float>& buffer)
{
    // Hall algorithm: large, spacious reverb
    juce::dsp::AudioBlock<float> block(buffer);
    juceReverb.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void Reverb::processShimmer(juce::AudioBuffer<float>& buffer)
{
    // Shimmer algorithm: reverb + pitch shifting for ethereal effects
    
    // First apply regular reverb
    juce::dsp::AudioBlock<float> block(buffer);
    juceReverb.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    // Then add shimmer effect
    processShimmerEffect(buffer);
}

void Reverb::processShimmerEffect(juce::AudioBuffer<float>& buffer)
{
    // Create a copy for pitch shifting
    shimmerBuffer.makeCopyOf(buffer);
    
    // Apply pitch shifting to the shimmer buffer
    pitchShift(shimmerBuffer, shimmerPitch);
    
    // Mix the pitch-shifted signal back into the main buffer
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* mainData = buffer.getWritePointer(channel);
        auto* shimmerData = shimmerBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            mainData[sample] += shimmerData[sample] * shimmerMix;
        }
    }
}

void Reverb::pitchShift(juce::AudioBuffer<float>& buffer, float semitones)
{
    // Simplified pitch shifting using time-domain stretching
    // In a production implementation, you'd use more sophisticated algorithms
    
    float pitchRatio = std::pow(2.0f, semitones / 12.0f);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        // Simple pitch shifting by resampling (crude but effective for shimmer)
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float sourceIndex = sample * pitchRatio;
            int index1 = static_cast<int>(sourceIndex);
            int index2 = index1 + 1;
            
            if (index2 < buffer.getNumSamples())
            {
                float fraction = sourceIndex - index1;
                float interpolated = channelData[index1] * (1.0f - fraction) + 
                                   channelData[index2] * fraction;
                channelData[sample] = interpolated * 0.5f; // Reduce level
            }
            else
            {
                channelData[sample] = 0.0f;
            }
        }
    }
}

void Reverb::updateReverbParameters()
{
    juce::Reverb::Parameters params;
    
    // Set parameters based on algorithm
    switch (algorithm)
    {
        case ReverbAlgorithm::Room:
            params.roomSize = 0.3f + (decaySeconds / 12.0f) * 0.4f;
            params.damping = 0.5f + damping * 0.4f;
            params.wetLevel = 1.0f;
            params.dryLevel = 0.0f;
            params.width = 0.8f;
            break;
            
        case ReverbAlgorithm::Plate:
            params.roomSize = 0.5f + (decaySeconds / 12.0f) * 0.3f;
            params.damping = 0.3f + damping * 0.5f;
            params.wetLevel = 1.0f;
            params.dryLevel = 0.0f;
            params.width = 1.0f;
            break;
            
        case ReverbAlgorithm::Hall:
            params.roomSize = 0.7f + (decaySeconds / 12.0f) * 0.3f;
            params.damping = 0.2f + damping * 0.6f;
            params.wetLevel = 1.0f;
            params.dryLevel = 0.0f;
            params.width = 1.0f;
            break;
            
        case ReverbAlgorithm::Shimmer:
            params.roomSize = 0.8f + (decaySeconds / 12.0f) * 0.2f;
            params.damping = 0.1f + damping * 0.4f;
            params.wetLevel = 1.0f;
            params.dryLevel = 0.0f;
            params.width = 1.0f;
            break;
    }
    
    juceReverb.setParameters(params);
}

void Reverb::updateDampingFilter()
{
    // High frequency damping filter
    float cutoffHz = 20000.0f * (1.0f - damping);
    cutoffHz = juce::jlimit(1000.0f, 20000.0f, cutoffHz);
    
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, cutoffHz, 0.707f);
    
    *dampingFilterL.state = *coefficients;
    *dampingFilterR.state = *coefficients;
}

void Reverb::updateModulation()
{
    lfoL.setFrequency(static_cast<float>(modulationRate));
    lfoR.setFrequency(static_cast<float>(modulationRate * 1.1f)); // Slight detuning for stereo width
}
