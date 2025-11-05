#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Modern, dark-themed LookAndFeel for the AI Guitar Plugin
 * Features smooth gradients, rounded corners, and subtle animations
 */
class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel();
    ~ModernLookAndFeel() override;
    
    // Color scheme
    struct Colors
    {
        static const juce::Colour background;
        static const juce::Colour backgroundLight;
        static const juce::Colour backgroundDark;
        static const juce::Colour surface;
        static const juce::Colour surfaceLight;
        static const juce::Colour primary;
        static const juce::Colour primaryLight;
        static const juce::Colour primaryDark;
        static const juce::Colour accent;
        static const juce::Colour accentLight;
        static const juce::Colour text;
        static const juce::Colour textSecondary;
        static const juce::Colour textDisabled;
        static const juce::Colour border;
        static const juce::Colour borderLight;
        static const juce::Colour success;
        static const juce::Colour warning;
        static const juce::Colour error;
        static const juce::Colour transparent;
    };
    
    // Button overrides
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, 
                            bool shouldDrawButtonAsDown) override;
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted, 
                       bool shouldDrawButtonAsDown) override;
    
    // Slider overrides
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    // Text editor overrides
    void fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                juce::TextEditor& textEditor) override;
    
    void drawTextEditorOutline(juce::Graphics& g, int width, int height,
                              juce::TextEditor& textEditor) override;
    
    // ComboBox overrides
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& box) override;
    
    // Progress bar overrides
    void drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                        int width, int height, double progress, 
                        const juce::String& textToShow) override;
    
    // Toggle button overrides
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, 
                         bool shouldDrawButtonAsDown) override;
    
    // Group component overrides
    void drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                  const juce::String& text, const juce::Justification& position,
                                  juce::GroupComponent& group) override;
    
    // Custom drawing methods
    void drawGlassPanel(juce::Graphics& g, const juce::Rectangle<int>& area, 
                       const juce::Colour& baseColour, float cornerRadius = 8.0f);
    
    void drawGradientBackground(juce::Graphics& g, const juce::Rectangle<int>& area,
                               const juce::Colour& topColour, const juce::Colour& bottomColour);
    
    void drawNeonGlow(juce::Graphics& g, const juce::Rectangle<float>& area,
                     const juce::Colour& glowColour, float intensity = 1.0f);
    
    void drawVoluMeter(juce::Graphics& g, const juce::Rectangle<int>& area,
                      float level, bool isClipping = false);
    
    // Fonts
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getLabelFont(juce::Label&) override;
    
private:
    // Custom fonts
    juce::Font primaryFont;
    juce::Font boldFont;
    juce::Font lightFont;
    
    // Animation helpers
    float getAnimationProgress(juce::Component* component, const juce::String& property);
    void startAnimation(juce::Component* component, const juce::String& property, 
                       float targetValue, int durationMs = 200);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernLookAndFeel)
};
