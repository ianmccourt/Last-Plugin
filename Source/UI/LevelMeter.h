#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "ArtisticLookAndFeel.h"

/**
 * Level meter component for displaying effect levels and input/output levels
 */
class LevelMeter : public juce::Component, public juce::Timer
{
public:
    LevelMeter();
    ~LevelMeter() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    // Level control
    void setLevel(float newLevel);
    void setColor(const juce::Colour& newColor);
    void setLabel(const juce::String& newLabel);
    
    // Meter type
    enum MeterType
    {
        Horizontal,
        Vertical,
        Circular
    };
    
    void setMeterType(MeterType type);
    
private:
    float level = 0.0f;
    float smoothedLevel = 0.0f;
    juce::Colour meterColor = juce::Colours::green;
    juce::String label = "Level";
    MeterType meterType = Vertical;
    
    juce::SmoothedValue<float> levelSmoother;
    std::unique_ptr<ArtisticLookAndFeel> artisticLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
