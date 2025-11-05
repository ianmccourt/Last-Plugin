#include "LevelMeter.h"

LevelMeter::LevelMeter()
{
    artisticLookAndFeel = std::make_unique<ArtisticLookAndFeel>();
    setLookAndFeel(artisticLookAndFeel.get());
    
    levelSmoother.reset(60.0, 0.1); // 60Hz, 100ms ramp
    levelSmoother.setCurrentAndTargetValue(0.3f); // Set a default level so meters show something
    
    // Disabled timer to prevent crashes in audio plugins
    // startTimerHz(30); // Update at 30fps
}

LevelMeter::~LevelMeter()
{
    stopTimer(); // Stop any running timer
    setLookAndFeel(nullptr);
}

void LevelMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    switch (meterType)
    {
        case Vertical:
        {
            auto meterWidth = bounds.getWidth() * 0.8f;
            auto meterHeight = bounds.getHeight() * 0.9f;
            auto meterX = bounds.getX() + (bounds.getWidth() - meterWidth) / 2.0f;
            auto meterY = bounds.getY() + bounds.getHeight() * 0.05f;
            
            auto meterBounds = juce::Rectangle<float>(meterX, meterY, meterWidth, meterHeight);
            artisticLookAndFeel->drawEffectMeter(g, meterBounds.toNearestInt(), smoothedLevel, meterColor, label);
            break;
        }
        
        case Horizontal:
        {
            auto meterWidth = bounds.getWidth() * 0.9f;
            auto meterHeight = bounds.getHeight() * 0.3f;
            auto meterX = bounds.getX() + bounds.getWidth() * 0.05f;
            auto meterY = bounds.getY() + (bounds.getHeight() - meterHeight) / 2.0f;
            
            auto meterBounds = juce::Rectangle<float>(meterX, meterY, meterWidth, meterHeight);
            
            // Rotate for horizontal display
            g.saveState();
            g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi / 2.0f, 
                                                         meterBounds.getCentreX(), meterBounds.getCentreY()));
            auto rotatedBounds = juce::Rectangle<float>(meterY, meterX, meterHeight, meterWidth);
            artisticLookAndFeel->drawEffectMeter(g, rotatedBounds.toNearestInt(), smoothedLevel, meterColor, label);
            g.restoreState();
            break;
        }
        
        case Circular:
        {
            auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.8f;
            auto x = bounds.getX() + (bounds.getWidth() - diameter) / 2.0f;
            auto y = bounds.getY() + (bounds.getHeight() - diameter) / 2.0f;
            
            auto circleBounds = juce::Rectangle<float>(x, y, diameter, diameter);
            
            // Background circle
            g.setColour(artisticLookAndFeel->colors.surface);
            g.fillEllipse(circleBounds);
            
            // Level arc
            auto centre = circleBounds.getCentre();
            auto radius = diameter / 2.0f - 10.0f;
            auto angle = smoothedLevel * juce::MathConstants<float>::twoPi * 0.75f; // 3/4 circle
            
            juce::Path levelArc;
            levelArc.addCentredArc(centre.x, centre.y, radius, radius, 
                                  -juce::MathConstants<float>::pi / 2.0f, 0.0f, angle, true);
            
            juce::ColourGradient gradient(meterColor, centre.x, centre.y - radius,
                                         meterColor.darker(0.3f), centre.x, centre.y + radius,
                                         false);
            g.setGradientFill(gradient);
            g.strokePath(levelArc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            
            // Border
            g.setColour(artisticLookAndFeel->colors.highlight);
            g.drawEllipse(circleBounds, 2.0f);
            
            // Label
            g.setColour(artisticLookAndFeel->colors.textSecondary);
            g.setFont(10.0f);
            g.drawText(label, circleBounds, juce::Justification::centred);
            break;
        }
    }
}

void LevelMeter::resized()
{
    // Component layout handled in paint()
}

void LevelMeter::timerCallback()
{
    smoothedLevel = levelSmoother.getNextValue();
    repaint();
}

void LevelMeter::setLevel(float newLevel)
{
    level = juce::jlimit(0.0f, 1.0f, newLevel);
    levelSmoother.setTargetValue(level);
}

void LevelMeter::setColor(const juce::Colour& newColor)
{
    meterColor = newColor;
    repaint();
}

void LevelMeter::setLabel(const juce::String& newLabel)
{
    label = newLabel;
    repaint();
}

void LevelMeter::setMeterType(MeterType type)
{
    meterType = type;
    repaint();
}
