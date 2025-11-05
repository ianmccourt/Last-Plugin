#include "ModernLookAndFeel.h"

// Color definitions
const juce::Colour ModernLookAndFeel::Colors::background = juce::Colour(0xff1a1a1a);
const juce::Colour ModernLookAndFeel::Colors::backgroundLight = juce::Colour(0xff2a2a2a);
const juce::Colour ModernLookAndFeel::Colors::backgroundDark = juce::Colour(0xff0f0f0f);
const juce::Colour ModernLookAndFeel::Colors::surface = juce::Colour(0xff2d2d2d);
const juce::Colour ModernLookAndFeel::Colors::surfaceLight = juce::Colour(0xff3d3d3d);
const juce::Colour ModernLookAndFeel::Colors::primary = juce::Colour(0xff4a9eff);
const juce::Colour ModernLookAndFeel::Colors::primaryLight = juce::Colour(0xff6bb6ff);
const juce::Colour ModernLookAndFeel::Colors::primaryDark = juce::Colour(0xff2986e6);
const juce::Colour ModernLookAndFeel::Colors::accent = juce::Colour(0xffff6b4a);
const juce::Colour ModernLookAndFeel::Colors::accentLight = juce::Colour(0xffff8a6b);
const juce::Colour ModernLookAndFeel::Colors::text = juce::Colour(0xfff0f0f0);
const juce::Colour ModernLookAndFeel::Colors::textSecondary = juce::Colour(0xffb0b0b0);
const juce::Colour ModernLookAndFeel::Colors::textDisabled = juce::Colour(0xff606060);
const juce::Colour ModernLookAndFeel::Colors::border = juce::Colour(0xff404040);
const juce::Colour ModernLookAndFeel::Colors::borderLight = juce::Colour(0xff606060);
const juce::Colour ModernLookAndFeel::Colors::success = juce::Colour(0xff4ade80);
const juce::Colour ModernLookAndFeel::Colors::warning = juce::Colour(0xfffbbf24);
const juce::Colour ModernLookAndFeel::Colors::error = juce::Colour(0xfff87171);
const juce::Colour ModernLookAndFeel::Colors::transparent = juce::Colour(0x00000000);

ModernLookAndFeel::ModernLookAndFeel()
{
    // Set up fonts
    primaryFont = juce::Font("Arial", 14.0f, juce::Font::plain);
    boldFont = juce::Font("Arial", 14.0f, juce::Font::bold);
    lightFont = juce::Font("Arial", 14.0f, juce::Font::plain);
    
    // Set default colors
    setColour(juce::ResizableWindow::backgroundColourId, Colors::background);
    setColour(juce::TextButton::buttonColourId, Colors::surface);
    setColour(juce::TextButton::textColourOffId, Colors::text);
    setColour(juce::TextButton::textColourOnId, Colors::text);
    setColour(juce::Slider::backgroundColourId, Colors::surface);
    setColour(juce::Slider::thumbColourId, Colors::primary);
    setColour(juce::Slider::trackColourId, Colors::primaryLight);
    setColour(juce::TextEditor::backgroundColourId, Colors::surface);
    setColour(juce::TextEditor::textColourId, Colors::text);
    setColour(juce::TextEditor::outlineColourId, Colors::border);
    setColour(juce::TextEditor::focusedOutlineColourId, Colors::primary);
    setColour(juce::ComboBox::backgroundColourId, Colors::surface);
    setColour(juce::ComboBox::textColourId, Colors::text);
    setColour(juce::ComboBox::outlineColourId, Colors::border);
    setColour(juce::ProgressBar::backgroundColourId, Colors::surface);
    setColour(juce::ProgressBar::foregroundColourId, Colors::primary);
}

ModernLookAndFeel::~ModernLookAndFeel()
{
}

void ModernLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                           const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = 8.0f;
    
    // Determine button state colors
    juce::Colour baseColour = backgroundColour;
    
    if (button.getToggleState())
    {
        baseColour = Colors::primary;
    }
    else if (shouldDrawButtonAsDown)
    {
        baseColour = baseColour.darker(0.3f);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        baseColour = baseColour.brighter(0.2f);
    }
    
    // Draw glass panel effect
    drawGlassPanel(g, bounds.toNearestInt(), baseColour, cornerRadius);
    
    // Add subtle glow for primary buttons
    if (button.getToggleState() || button.getName() == "generate" || button.getName() == "primary")
    {
        drawNeonGlow(g, bounds.reduced(2.0f), Colors::primary, 0.3f);
    }
}

void ModernLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                     bool shouldDrawButtonAsHighlighted,
                                     bool shouldDrawButtonAsDown)
{
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    
    juce::Colour textColour = Colors::text;
    
    if (!button.isEnabled())
        textColour = Colors::textDisabled;
    else if (button.getToggleState())
        textColour = Colors::text;
    
    g.setColour(textColour);
    
    auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;
    
    auto fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    auto leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    auto rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    auto textWidth = button.getWidth() - leftIndent - rightIndent;
    
    if (textWidth > 0)
    {
        g.drawFittedText(button.getButtonText(),
                        leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                        juce::Justification::centred, 2);
    }
}

void ModernLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 8.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // Draw outer ring
    g.setColour(Colors::surface);
    g.fillEllipse(rx, ry, rw, rw);
    
    // Draw inner gradient
    juce::ColourGradient gradient(Colors::backgroundDark, centreX, centreY - radius * 0.5f,
                                 Colors::surfaceLight, centreX, centreY + radius * 0.5f, false);
    g.setGradientFill(gradient);
    g.fillEllipse(rx + 4, ry + 4, rw - 8, rw - 8);
    
    // Draw track
    juce::Path track;
    track.addCentredArc(centreX, centreY, radius - 6, radius - 6, 0.0f, 
                       rotaryStartAngle, rotaryEndAngle, true);
    
    g.setColour(Colors::border);
    g.strokePath(track, juce::PathStrokeType(3.0f));
    
    // Draw active track
    juce::Path activeTrack;
    activeTrack.addCentredArc(centreX, centreY, radius - 6, radius - 6, 0.0f,
                             rotaryStartAngle, angle, true);
    
    g.setColour(Colors::primary);
    g.strokePath(activeTrack, juce::PathStrokeType(3.0f));
    
    // Draw thumb
    juce::Path thumb;
    auto thumbRadius = 6.0f;
    auto thumbX = centreX + (radius - 12) * std::cos(angle - juce::MathConstants<float>::halfPi);
    auto thumbY = centreY + (radius - 12) * std::sin(angle - juce::MathConstants<float>::halfPi);
    
    thumb.addEllipse(thumbX - thumbRadius, thumbY - thumbRadius, thumbRadius * 2, thumbRadius * 2);
    
    g.setColour(Colors::primary);
    g.fillPath(thumb);
    
    // Add glow to thumb
    drawNeonGlow(g, thumb.getBounds(), Colors::primary, 0.5f);
}

void ModernLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float minSliderPos, float maxSliderPos,
                                       const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto trackWidth = juce::jmin(6.0f, (float)juce::jmin(width, height) * 0.25f);
    
    juce::Point<float> startPoint, endPoint;
    
    if (slider.isHorizontal())
    {
        startPoint = {(float)x, (float)y + (float)height * 0.5f};
        endPoint = {(float)(x + width), startPoint.y};
    }
    else
    {
        startPoint = {(float)x + (float)width * 0.5f, (float)(y + height)};
        endPoint = {startPoint.x, (float)y};
    }
    
    // Draw track
    juce::Path track;
    track.startNewSubPath(startPoint);
    track.lineTo(endPoint);
    g.setColour(Colors::surface);
    g.strokePath(track, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw active track
    juce::Path activeTrack;
    juce::Point<float> thumbPoint;
    
    if (slider.isHorizontal())
    {
        thumbPoint = {sliderPos, startPoint.y};
        activeTrack.startNewSubPath(startPoint);
        activeTrack.lineTo(thumbPoint);
    }
    else
    {
        thumbPoint = {startPoint.x, sliderPos};
        activeTrack.startNewSubPath(startPoint);
        activeTrack.lineTo(thumbPoint);
    }
    
    g.setColour(Colors::primary);
    g.strokePath(activeTrack, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw thumb
    auto thumbRadius = trackWidth * 1.5f;
    g.setColour(Colors::primary);
    g.fillEllipse(thumbPoint.x - thumbRadius, thumbPoint.y - thumbRadius, thumbRadius * 2, thumbRadius * 2);
    
    // Add glow to thumb
    drawNeonGlow(g, juce::Rectangle<float>(thumbPoint.x - thumbRadius, thumbPoint.y - thumbRadius, 
                                          thumbRadius * 2, thumbRadius * 2), Colors::primary, 0.4f);
}

void ModernLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                               juce::TextEditor& textEditor)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    drawGlassPanel(g, bounds, Colors::surface, 6.0f);
}

void ModernLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height,
                                            juce::TextEditor& textEditor)
{
    auto bounds = juce::Rectangle<float>(0.5f, 0.5f, width - 1.0f, height - 1.0f);
    
    juce::Colour outlineColour = Colors::border;
    if (textEditor.hasKeyboardFocus(true))
        outlineColour = Colors::primary;
    
    g.setColour(outlineColour);
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
    
    if (textEditor.hasKeyboardFocus(true))
    {
        drawNeonGlow(g, bounds, Colors::primary, 0.3f);
    }
}

void ModernLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                   int buttonX, int buttonY, int buttonW, int buttonH,
                                   juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    
    juce::Colour backgroundColour = Colors::surface;
    if (isButtonDown)
        backgroundColour = backgroundColour.darker(0.2f);
    
    drawGlassPanel(g, bounds, backgroundColour, 6.0f);
    
    // Draw border
    g.setColour(Colors::border);
    g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f), 6.0f, 1.0f);
    
    // Draw arrow
    auto arrowBounds = juce::Rectangle<float>(buttonX, buttonY, buttonW, buttonH).reduced(4.0f);
    juce::Path arrow;
    arrow.addTriangle(arrowBounds.getX(), arrowBounds.getY(),
                     arrowBounds.getRight(), arrowBounds.getY(),
                     arrowBounds.getCentreX(), arrowBounds.getBottom());
    
    g.setColour(Colors::textSecondary);
    g.fillPath(arrow);
}

void ModernLookAndFeel::drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                                      int width, int height, double progress,
                                      const juce::String& textToShow)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    
    // Draw background
    drawGlassPanel(g, bounds, Colors::surface, 4.0f);
    
    // Draw progress
    if (progress > 0.0)
    {
        auto progressBounds = bounds.reduced(2);
        progressBounds.setWidth(static_cast<int>(progressBounds.getWidth() * progress));
        
        juce::ColourGradient gradient(Colors::primary, 0, 0, Colors::primaryLight, 0, height, false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(progressBounds.toFloat(), 2.0f);
        
        // Add glow
        drawNeonGlow(g, progressBounds.toFloat(), Colors::primary, 0.4f);
    }
    
    // Draw text
    if (textToShow.isNotEmpty())
    {
        g.setColour(Colors::text);
        g.setFont(primaryFont);
        g.drawText(textToShow, bounds, juce::Justification::centred, false);
    }
}

void ModernLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                       bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto toggleBounds = bounds.removeFromLeft(bounds.getHeight()).reduced(4.0f);
    
    // Draw toggle background
    juce::Colour backgroundColour = button.getToggleState() ? Colors::primary : Colors::surface;
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(toggleBounds, toggleBounds.getHeight() * 0.5f);
    
    // Draw toggle thumb
    auto thumbBounds = toggleBounds.reduced(2.0f);
    thumbBounds.setWidth(thumbBounds.getHeight());
    
    if (button.getToggleState())
        thumbBounds.setX(toggleBounds.getRight() - thumbBounds.getWidth() - 2.0f);
    
    g.setColour(Colors::text);
    g.fillEllipse(thumbBounds);
    
    // Draw text
    if (button.getButtonText().isNotEmpty())
    {
        auto textBounds = bounds.withTrimmedLeft(toggleBounds.getRight() + 8);
        g.setColour(Colors::text);
        g.setFont(primaryFont);
        g.drawText(button.getButtonText(), textBounds.toNearestInt(), juce::Justification::centredLeft, false);
    }
}

void ModernLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                                const juce::String& text, const juce::Justification& position,
                                                juce::GroupComponent& group)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    
    // Draw background panel
    drawGlassPanel(g, bounds, Colors::surface.withAlpha(0.3f), 8.0f);
    
    // Draw border
    g.setColour(Colors::border);
    g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f), 8.0f, 1.0f);
    
    // Draw title
    if (text.isNotEmpty())
    {
        auto textBounds = juce::Rectangle<int>(12, 0, width - 24, 24);
        
        // Draw title background
        g.setColour(Colors::background);
        g.fillRoundedRectangle(textBounds.toFloat().reduced(2.0f), 4.0f);
        
        g.setColour(Colors::text);
        g.setFont(boldFont);
        g.drawText(text, textBounds, juce::Justification::centred, false);
    }
}

void ModernLookAndFeel::drawGlassPanel(juce::Graphics& g, const juce::Rectangle<int>& area,
                                     const juce::Colour& baseColour, float cornerRadius)
{
    auto bounds = area.toFloat();
    
    // Draw main background
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Add subtle gradient overlay
    juce::ColourGradient gradient(baseColour.brighter(0.1f), bounds.getX(), bounds.getY(),
                                 baseColour.darker(0.1f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Add highlight on top edge
    g.setColour(baseColour.brighter(0.3f).withAlpha(0.3f));
    g.fillRoundedRectangle(bounds.removeFromTop(2.0f), cornerRadius);
}

void ModernLookAndFeel::drawGradientBackground(juce::Graphics& g, const juce::Rectangle<int>& area,
                                             const juce::Colour& topColour, const juce::Colour& bottomColour)
{
    juce::ColourGradient gradient(topColour, 0, area.getY(), bottomColour, 0, area.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRect(area);
}

void ModernLookAndFeel::drawNeonGlow(juce::Graphics& g, const juce::Rectangle<float>& area,
                                   const juce::Colour& glowColour, float intensity)
{
    auto glowRadius = 8.0f * intensity;
    auto glowBounds = area.expanded(glowRadius);
    
    juce::ColourGradient glow(glowColour.withAlpha(0.6f * intensity), area.getCentreX(), area.getCentreY(),
                             glowColour.withAlpha(0.0f), glowBounds.getCentreX(), glowBounds.getY(), true);
    
    g.setGradientFill(glow);
    g.fillEllipse(glowBounds);
}

void ModernLookAndFeel::drawVoluMeter(juce::Graphics& g, const juce::Rectangle<int>& area,
                                    float level, bool isClipping)
{
    auto bounds = area.toFloat();
    
    // Draw background
    g.setColour(Colors::backgroundDark);
    g.fillRoundedRectangle(bounds, 2.0f);
    
    // Draw level
    if (level > 0.0f)
    {
        auto levelBounds = bounds.reduced(1.0f);
        levelBounds.setHeight(levelBounds.getHeight() * level);
        levelBounds.setY(bounds.getBottom() - levelBounds.getHeight() - 1.0f);
        
        juce::Colour levelColour = Colors::success;
        if (level > 0.8f)
            levelColour = Colors::warning;
        if (isClipping)
            levelColour = Colors::error;
        
        g.setColour(levelColour);
        g.fillRoundedRectangle(levelBounds, 1.0f);
    }
}

juce::Font ModernLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return juce::Font(juce::jmin(16.0f, buttonHeight * 0.6f), juce::Font::bold);
}

juce::Font ModernLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return primaryFont;
}

juce::Font ModernLookAndFeel::getLabelFont(juce::Label&)
{
    return primaryFont;
}
