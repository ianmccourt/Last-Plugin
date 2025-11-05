#include "LevelMeterComponent.h"
#include "ModernLookAndFeel.h"

LevelMeterComponent::LevelMeterComponent()
{
    updateTimerInterval();
}

LevelMeterComponent::~LevelMeterComponent()
{
    stopTimer();
}

void LevelMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw background
    g.setColour(ModernLookAndFeel::Colors::backgroundDark);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Draw border
    g.setColour(ModernLookAndFeel::Colors::border);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
    
    if (vertical)
    {
        // Vertical layout: Input | Output | GR (if enabled)
        auto meterWidth = bounds.getWidth() / (showGainReduction ? 3.0f : 2.0f);
        
        auto inputBounds = bounds.removeFromLeft(meterWidth).reduced(4.0f);
        drawMeter(g, inputBounds, smoothedInputLevel, inputPeakHold, inputClipping, "IN");
        
        auto outputBounds = bounds.removeFromLeft(meterWidth).reduced(4.0f);
        drawMeter(g, outputBounds, smoothedOutputLevel, outputPeakHold, outputClipping, "OUT");
        
        if (showGainReduction && bounds.getWidth() > 10)
        {
            auto grBounds = bounds.reduced(4.0f);
            drawGainReductionMeter(g, grBounds, smoothedGainReduction);
        }
    }
    else
    {
        // Horizontal layout: stacked meters
        auto meterHeight = bounds.getHeight() / (showGainReduction ? 3.0f : 2.0f);
        
        auto inputBounds = bounds.removeFromTop(meterHeight).reduced(4.0f);
        drawMeter(g, inputBounds, smoothedInputLevel, inputPeakHold, inputClipping, "IN");
        
        auto outputBounds = bounds.removeFromTop(meterHeight).reduced(4.0f);
        drawMeter(g, outputBounds, smoothedOutputLevel, outputPeakHold, outputClipping, "OUT");
        
        if (showGainReduction && bounds.getHeight() > 10)
        {
            auto grBounds = bounds.reduced(4.0f);
            drawGainReductionMeter(g, grBounds, smoothedGainReduction);
        }
    }
}

void LevelMeterComponent::resized()
{
    // Nothing specific needed for resize
}

void LevelMeterComponent::timerCallback()
{
    // Update smoothed values
    auto currentInput = inputLevel.load();
    auto currentOutput = outputLevel.load();
    auto currentGR = gainReduction.load();
    
    // Smooth the levels
    smoothedInputLevel = smoothedInputLevel * smoothingFactor + currentInput * (1.0f - smoothingFactor);
    smoothedOutputLevel = smoothedOutputLevel * smoothingFactor + currentOutput * (1.0f - smoothingFactor);
    smoothedGainReduction = smoothedGainReduction * smoothingFactor + currentGR * (1.0f - smoothingFactor);
    
    // Update peak hold
    if (currentInput > inputPeakHold)
    {
        inputPeakHold = currentInput;
        inputPeakHoldTime = peakHoldDuration;
    }
    else if (inputPeakHoldTime > 0)
    {
        inputPeakHoldTime -= 1000 / refreshRateHz;
        if (inputPeakHoldTime <= 0)
            inputPeakHold = smoothedInputLevel;
    }
    
    if (currentOutput > outputPeakHold)
    {
        outputPeakHold = currentOutput;
        outputPeakHoldTime = peakHoldDuration;
    }
    else if (outputPeakHoldTime > 0)
    {
        outputPeakHoldTime -= 1000 / refreshRateHz;
        if (outputPeakHoldTime <= 0)
            outputPeakHold = smoothedOutputLevel;
    }
    
    // Update clipping indicators
    inputClipping = currentInput > 0.95f;
    outputClipping = currentOutput > 0.95f;
    
    if (inputClipping)
        inputClipTime = clipIndicatorDuration;
    else if (inputClipTime > 0)
        inputClipTime -= 1000 / refreshRateHz;
    
    if (outputClipping)
        outputClipTime = clipIndicatorDuration;
    else if (outputClipTime > 0)
        outputClipTime -= 1000 / refreshRateHz;
    
    inputClipping = inputClipTime > 0;
    outputClipping = outputClipTime > 0;
    
    repaint();
}

void LevelMeterComponent::setInputLevel(float level)
{
    inputLevel.store(juce::jlimit(0.0f, 1.0f, level));
}

void LevelMeterComponent::setOutputLevel(float level)
{
    outputLevel.store(juce::jlimit(0.0f, 1.0f, level));
}

void LevelMeterComponent::setGainReduction(float gr)
{
    gainReduction.store(juce::jlimit(0.0f, 1.0f, gr));
}

void LevelMeterComponent::drawMeter(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                                  float level, float peakHold, bool isClipping, 
                                  const juce::String& label)
{
    // Draw meter background
    g.setColour(ModernLookAndFeel::Colors::backgroundDark);
    g.fillRoundedRectangle(bounds, 2.0f);
    
    // Calculate level bounds
    juce::Rectangle<float> levelBounds;
    juce::Rectangle<float> peakBounds;
    
    if (vertical)
    {
        // Vertical meter
        auto levelHeight = bounds.getHeight() * level;
        levelBounds = bounds.withTop(bounds.getBottom() - levelHeight);
        
        auto peakY = bounds.getBottom() - bounds.getHeight() * peakHold;
        peakBounds = juce::Rectangle<float>(bounds.getX(), peakY - 1, bounds.getWidth(), 2);
    }
    else
    {
        // Horizontal meter
        auto levelWidth = bounds.getWidth() * level;
        levelBounds = bounds.withWidth(levelWidth);
        
        auto peakX = bounds.getX() + bounds.getWidth() * peakHold;
        peakBounds = juce::Rectangle<float>(peakX - 1, bounds.getY(), 2, bounds.getHeight());
    }
    
    // Draw level bar
    if (level > 0.0f)
    {
        auto levelColour = getLevelColour(level);
        if (isClipping)
            levelColour = ModernLookAndFeel::Colors::error;
        
        g.setColour(levelColour);
        g.fillRoundedRectangle(levelBounds, 1.0f);
        
        // Add subtle glow for high levels
        if (level > 0.7f)
        {
            g.setColour(levelColour.withAlpha(0.3f));
            g.fillRoundedRectangle(levelBounds.expanded(1.0f), 2.0f);
        }
    }
    
    // Draw peak hold line
    if (peakHold > 0.0f)
    {
        auto peakColour = getLevelColour(peakHold);
        if (isClipping)
            peakColour = ModernLookAndFeel::Colors::error;
        
        g.setColour(peakColour.brighter(0.3f));
        g.fillRoundedRectangle(peakBounds, 1.0f);
    }
    
    // Draw label
    if (label.isNotEmpty())
    {
        g.setColour(ModernLookAndFeel::Colors::textSecondary);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        
        if (vertical)
        {
            auto labelBounds = bounds.removeFromBottom(12);
            g.drawText(label, labelBounds.toNearestInt(), juce::Justification::centred, false);
        }
        else
        {
            auto labelBounds = bounds.removeFromLeft(20);
            g.drawText(label, labelBounds.toNearestInt(), juce::Justification::centred, false);
        }
    }
}

void LevelMeterComponent::drawGainReductionMeter(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                                               float gainReduction)
{
    // Draw background
    g.setColour(ModernLookAndFeel::Colors::backgroundDark);
    g.fillRoundedRectangle(bounds, 2.0f);
    
    // Draw gain reduction (inverted - more GR = more bar)
    if (gainReduction > 0.0f)
    {
        juce::Rectangle<float> grBounds;
        
        if (vertical)
        {
            auto grHeight = bounds.getHeight() * gainReduction;
            grBounds = bounds.withTop(bounds.getBottom() - grHeight);
        }
        else
        {
            auto grWidth = bounds.getWidth() * gainReduction;
            grBounds = bounds.withWidth(grWidth);
        }
        
        // Use orange/yellow color for gain reduction
        g.setColour(ModernLookAndFeel::Colors::warning);
        g.fillRoundedRectangle(grBounds, 1.0f);
    }
    
    // Draw "GR" label
    g.setColour(ModernLookAndFeel::Colors::textSecondary);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    
    if (vertical)
    {
        auto labelBounds = bounds.removeFromBottom(12);
        g.drawText("GR", labelBounds.toNearestInt(), juce::Justification::centred, false);
    }
    else
    {
        auto labelBounds = bounds.removeFromLeft(18);
        g.drawText("GR", labelBounds.toNearestInt(), juce::Justification::centred, false);
    }
}

juce::Colour LevelMeterComponent::getLevelColour(float level)
{
    if (level < 0.6f)
        return ModernLookAndFeel::Colors::success;
    else if (level < 0.8f)
        return ModernLookAndFeel::Colors::warning;
    else
        return ModernLookAndFeel::Colors::error;
}

void LevelMeterComponent::updateTimerInterval()
{
    if (refreshRateHz > 0)
    {
        startTimerHz(refreshRateHz);
    }
}
