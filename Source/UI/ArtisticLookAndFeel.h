#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>

/**
 * Artistic look and feel for the AI Guitar Plugin
 * Features vibrant colors, gradients, and modern design elements
 */
class ArtisticLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ArtisticLookAndFeel();
    ~ArtisticLookAndFeel() override;
    
    // Override component drawing methods
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
    
    void drawTextEditorOutline(juce::Graphics& g, int width, int height,
                               juce::TextEditor& textEditor) override;
    
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    // Custom drawing methods
    void drawEffectMeter(juce::Graphics& g, juce::Rectangle<int> bounds, float level, 
                        const juce::Colour& color, const juce::String& label);
    void drawGradientBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawGlowEffect(juce::Graphics& g, juce::Rectangle<int> bounds, 
                       const juce::Colour& glowColor, float intensity);
    
    // Color scheme
    struct ColorScheme
    {
        juce::Colour background = juce::Colour(0xFF1a1a2e);
        juce::Colour surface = juce::Colour(0xFF16213e);
        juce::Colour primary = juce::Colour(0xFF0f3460);
        juce::Colour accent = juce::Colour(0xFFe94560);
        juce::Colour highlight = juce::Colour(0xFF0f4c75);
        juce::Colour text = juce::Colour(0xFFeeeeee);
        juce::Colour textSecondary = juce::Colour(0xFFb8b8b8);
        
        // Effect colors
        juce::Colour gainColor = juce::Colour(0xFF4CAF50);
        juce::Colour driveColor = juce::Colour(0xFFff5722);
        juce::Colour reverbColor = juce::Colour(0xFF2196F3);
        juce::Colour delayColor = juce::Colour(0xFF9C27B0);
        juce::Colour chorusColor = juce::Colour(0xFF00BCD4);
        juce::Colour eqColor = juce::Colour(0xFFFFC107);
        juce::Colour toneColor = juce::Colour(0xFF607D8B);
    };
    
    ColorScheme colors;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArtisticLookAndFeel)
};
