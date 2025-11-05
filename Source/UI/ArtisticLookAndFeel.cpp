#include "ArtisticLookAndFeel.h"

ArtisticLookAndFeel::ArtisticLookAndFeel()
{
    // Set default fonts
    setDefaultSansSerifTypefaceName("Helvetica Neue");
    
    // Customize colors
    setColour(juce::Slider::thumbColourId, colors.accent);
    setColour(juce::Slider::rotarySliderFillColourId, colors.primary);
    setColour(juce::Slider::rotarySliderOutlineColourId, colors.highlight);
    setColour(juce::TextButton::buttonColourId, colors.primary);
    setColour(juce::TextButton::textColourOnId, colors.text);
    setColour(juce::TextButton::textColourOffId, colors.text);
    setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    setColour(juce::TextEditor::textColourId, juce::Colours::black);
    setColour(juce::TextEditor::outlineColourId, colors.primary);
    setColour(juce::TextEditor::highlightColourId, colors.accent.withAlpha(0.3f));
    setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::black);
    setColour(juce::Label::textColourId, colors.text);
}

ArtisticLookAndFeel::~ArtisticLookAndFeel()
{
}

void ArtisticLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;
    
    // Background circle with gradient
    juce::ColourGradient gradient(colors.surface, bounds.getCentreX(), bounds.getCentreY() - radius,
                                  colors.highlight, bounds.getCentreX(), bounds.getCentreY() + radius,
                                  false);
    g.setGradientFill(gradient);
    g.fillEllipse(bounds.reduced(2));
    
    // Track arc
    juce::Path trackArc;
    trackArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
                          0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(colors.primary);
    g.strokePath(trackArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Value arc
    if (sliderPos > 0.0f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
                              0.0f, rotaryStartAngle, toAngle, true);
        g.setColour(colors.accent);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
    // Thumb
    juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                 bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
    g.setColour(colors.accent);
    g.fillEllipse(juce::Rectangle<float>(lineW * 1.5f, lineW * 1.5f).withCentre(thumbPoint));
    
    // Glow effect
    drawGlowEffect(g, bounds.toNearestInt(), colors.accent, 0.3f);
}

void ArtisticLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    
    // Create gradient based on button state
    juce::ColourGradient gradient;
    if (shouldDrawButtonAsDown)
    {
        gradient = juce::ColourGradient(colors.accent.darker(0.2f), bounds.getX(), bounds.getY(),
                                       colors.accent, bounds.getX(), bounds.getBottom(),
                                       false);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        gradient = juce::ColourGradient(colors.accent, bounds.getX(), bounds.getY(),
                                       colors.accent.darker(0.1f), bounds.getX(), bounds.getBottom(),
                                       false);
    }
    else
    {
        gradient = juce::ColourGradient(colors.primary, bounds.getX(), bounds.getY(),
                                       colors.highlight, bounds.getX(), bounds.getBottom(),
                                       false);
    }
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Border
    g.setColour(colors.highlight);
    g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
    
    // Glow effect
    if (shouldDrawButtonAsHighlighted)
    {
        drawGlowEffect(g, bounds.toNearestInt(), colors.accent, 0.5f);
    }
}

void ArtisticLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                        bool shouldDrawButtonAsHighlighted,
                                        bool shouldDrawButtonAsDown)
{
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    g.setColour(colors.text);
    
    auto textArea = button.getLocalBounds();
    g.drawFittedText(button.getButtonText(), textArea, juce::Justification::centred, 1);
}

void ArtisticLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height,
                                               juce::TextEditor& textEditor)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
    
    // Background - use white for maximum text visibility
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Border
    g.setColour(colors.primary);
    g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
    
    // Glow effect when focused
    if (textEditor.hasKeyboardFocus(true))
    {
        drawGlowEffect(g, bounds.toNearestInt(), colors.accent, 0.3f);
    }
}


void ArtisticLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(colors.text);
    g.setFont(getLabelFont(label));
    g.drawFittedText(label.getText(), label.getLocalBounds(), label.getJustificationType(), 1);
}

void ArtisticLookAndFeel::drawEffectMeter(juce::Graphics& g, juce::Rectangle<int> bounds, float level,
                                         const juce::Colour& color, const juce::String& label)
{
    auto meterBounds = bounds.toFloat();
    auto barHeight = meterBounds.getHeight() * juce::jlimit(0.0f, 1.0f, level);
    auto barY = meterBounds.getBottom() - barHeight;
    
    // Background
    g.setColour(colors.surface);
    g.fillRoundedRectangle(meterBounds, 4.0f);
    
    // Meter bar with gradient
    auto barBounds = juce::Rectangle<float>(meterBounds.getX(), barY, meterBounds.getWidth(), barHeight);
    juce::ColourGradient gradient(color, barBounds.getX(), barBounds.getY(),
                                  color.darker(0.3f), barBounds.getX(), barBounds.getBottom(),
                                  false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(barBounds, 4.0f);
    
    // Border
    g.setColour(color.withAlpha(0.5f));
    g.drawRoundedRectangle(meterBounds, 4.0f, 1.0f);
    
    // Label
    g.setColour(colors.textSecondary);
    g.setFont(12.0f);
    g.drawText(label, meterBounds, juce::Justification::centredBottom);
}

void ArtisticLookAndFeel::drawGradientBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::ColourGradient gradient(colors.background, bounds.getX(), bounds.getY(),
                                  colors.surface, bounds.getX(), bounds.getBottom(),
                                  false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Add some subtle texture
    g.setColour(colors.highlight.withAlpha(0.1f));
    for (int i = 0; i < bounds.getHeight(); i += 20)
    {
        g.drawHorizontalLine(i, bounds.getX(), bounds.getRight());
    }
}

void ArtisticLookAndFeel::drawGlowEffect(juce::Graphics& g, juce::Rectangle<int> bounds,
                                        const juce::Colour& glowColor, float intensity)
{
    auto glowBounds = bounds.expanded(10);
    auto glowColorWithAlpha = glowColor.withAlpha(intensity);
    
    // Multiple glow layers for smooth effect
    for (int i = 0; i < 5; ++i)
    {
        auto alpha = intensity * (1.0f - i * 0.2f);
        g.setColour(glowColorWithAlpha.withMultipliedAlpha(alpha));
        g.drawRoundedRectangle(glowBounds.toFloat(), 8.0f + i * 2, 2.0f + i);
    }
}
