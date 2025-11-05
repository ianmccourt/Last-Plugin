#include "ParameterSmoother.h"

ParameterSmoother::ParameterSmoother()
{
}

ParameterSmoother::~ParameterSmoother()
{
}

void ParameterSmoother::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Update all smoothers with new sample rate
    for (auto& [paramId, param] : parameters)
    {
        param.smoother.reset(sampleRate, 0.05); // 50ms default ramp
    }
}

void ParameterSmoother::reset()
{
    for (auto& [paramId, param] : parameters)
    {
        param.smoother.reset(currentSampleRate, 0.05);
        param.isActive = false;
    }
    isPresetTransition = false;
}

void ParameterSmoother::addParameter(const juce::String& paramId, float initialValue)
{
    parameters[paramId] = SmoothedParameter();
    parameters[paramId].smoother.setCurrentAndTargetValue(initialValue);
    parameters[paramId].targetValue = initialValue;
}

void ParameterSmoother::setTargetValue(const juce::String& paramId, float targetValue, float rampTimeSeconds)
{
    auto it = parameters.find(paramId);
    if (it != parameters.end())
    {
        it->second.smoother.reset(currentSampleRate, rampTimeSeconds);
        it->second.smoother.setTargetValue(targetValue);
        it->second.targetValue = targetValue;
        it->second.isActive = true;
    }
}

void ParameterSmoother::setCurrentValue(const juce::String& paramId, float currentValue)
{
    auto it = parameters.find(paramId);
    if (it != parameters.end())
    {
        it->second.smoother.setCurrentAndTargetValue(currentValue);
        it->second.targetValue = currentValue;
        it->second.isActive = false;
    }
}

void ParameterSmoother::process(int numSamples)
{
    for (auto& [paramId, param] : parameters)
    {
        if (param.isActive)
        {
            param.smoother.skip(numSamples);
            
            // Check if we've reached the target
            if (std::abs(param.smoother.getCurrentValue() - param.targetValue) < 0.001f)
            {
                param.smoother.setCurrentAndTargetValue(param.targetValue);
                param.isActive = false;
            }
        }
    }
}

float ParameterSmoother::getCurrentValue(const juce::String& paramId) const
{
    auto it = parameters.find(paramId);
    if (it != parameters.end())
    {
        return it->second.smoother.getCurrentValue();
    }
    return 0.0f;
}

bool ParameterSmoother::isSmoothing(const juce::String& paramId) const
{
    auto it = parameters.find(paramId);
    if (it != parameters.end())
    {
        return it->second.isActive;
    }
    return false;
}

bool ParameterSmoother::isAnyParameterSmoothing() const
{
    for (const auto& [paramId, param] : parameters)
    {
        if (param.isActive)
            return true;
    }
    return false;
}

void ParameterSmoother::beginPresetTransition(float rampTimeSeconds)
{
    isPresetTransition = true;
    
    // Reset all smoothers with the transition time
    for (auto& [paramId, param] : parameters)
    {
        param.smoother.reset(currentSampleRate, rampTimeSeconds);
    }
}

void ParameterSmoother::setAllTargetValues(const std::map<juce::String, float>& targetValues)
{
    for (const auto& [paramId, targetValue] : targetValues)
    {
        auto it = parameters.find(paramId);
        if (it != parameters.end())
        {
            it->second.smoother.setTargetValue(targetValue);
            it->second.targetValue = targetValue;
            it->second.isActive = true;
        }
    }
}

void ParameterSmoother::endPresetTransition()
{
    isPresetTransition = false;
}
