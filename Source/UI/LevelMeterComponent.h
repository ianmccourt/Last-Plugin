#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * Animated level meter component with peak hold and clipping detection
 * Shows input and output levels with smooth animations
 */
class LevelMeterComponent : public juce::Component,
                          public juce::Timer
{
public:
    LevelMeterComponent();
    ~LevelMeterComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    // Level updates
    void setInputLevel(float level);
    void setOutputLevel(float level);
    void setGainReduction(float gainReduction);
    
    // Configuration
    void setOrientation(bool isVertical) { vertical = isVertical; }
    void setShowGainReduction(bool show) { showGainReduction = show; }
    void setRefreshRate(int hz) { refreshRateHz = hz; updateTimerInterval(); }
    
private:
    // Level data
    std::atomic<float> inputLevel{0.0f};
    std::atomic<float> outputLevel{0.0f};
    std::atomic<float> gainReduction{0.0f};
    
    // Smoothed display values
    float smoothedInputLevel = 0.0f;
    float smoothedOutputLevel = 0.0f;
    float smoothedGainReduction = 0.0f;
    
    // Peak hold
    float inputPeakHold = 0.0f;
    float outputPeakHold = 0.0f;
    int inputPeakHoldTime = 0;
    int outputPeakHoldTime = 0;
    
    // Clipping detection
    bool inputClipping = false;
    bool outputClipping = false;
    int inputClipTime = 0;
    int outputClipTime = 0;
    
    // Configuration
    bool vertical = true;
    bool showGainReduction = true;
    int refreshRateHz = 30;
    
    // Visual properties
    float smoothingFactor = 0.8f;
    int peakHoldDuration = 1000; // ms
    int clipIndicatorDuration = 500; // ms
    
    // Drawing helpers
    void drawMeter(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                  float level, float peakHold, bool isClipping, 
                  const juce::String& label);
    
    void drawGainReductionMeter(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                               float gainReduction);
    
    juce::Colour getLevelColour(float level);
    void updateTimerInterval();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterComponent)
};
