#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Preset/PresetSchema.h"

/**
 * Visual representation of the effect chain
 * Shows active effects with animated connections and parameter visualization
 */
class EffectChainVisualizer : public juce::Component,
                            public juce::Timer
{
public:
    EffectChainVisualizer();
    ~EffectChainVisualizer() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
    // Preset updates
    void updateFromPreset(const PresetData& preset);
    void setEffectEnabled(EffectBlockType type, bool enabled);
    
    // Visual configuration
    void setCompactMode(bool compact) { compactMode = compact; repaint(); }
    void setShowParameters(bool show) { showParameters = show; repaint(); }
    void setAnimationSpeed(float speed) { animationSpeed = speed; }
    
    // Callbacks
    std::function<void(EffectBlockType)> onEffectClicked;
    std::function<void(EffectBlockType)> onEffectDoubleClicked;
    
private:
    struct EffectBlock
    {
        EffectBlockType type;
        juce::String name;
        juce::String shortName;
        bool enabled = false;
        juce::Colour colour;
        juce::Rectangle<float> bounds;
        float animationProgress = 0.0f;
        bool isHovered = false;
        
        // Parameter visualization
        std::vector<std::pair<juce::String, float>> parameters;
    };
    
    std::vector<EffectBlock> effectBlocks;
    
    // Visual state
    bool compactMode = false;
    bool showParameters = true;
    float animationSpeed = 1.0f;
    int hoveredBlockIndex = -1;
    
    // Animation properties
    float connectionFlow = 0.0f;
    float pulsePhase = 0.0f;
    
    // Layout
    void calculateLayout();
    void layoutHorizontal();
    void layoutVertical();
    void layoutGrid();
    
    // Drawing
    void drawEffectBlock(juce::Graphics& g, const EffectBlock& block);
    void drawConnections(juce::Graphics& g);
    void drawConnection(juce::Graphics& g, const juce::Point<float>& start, 
                       const juce::Point<float>& end, bool isActive);
    void drawParameterVisualization(juce::Graphics& g, const EffectBlock& block);
    void drawTooltip(juce::Graphics& g, const EffectBlock& block);
    
    // Interaction
    int getBlockAtPosition(const juce::Point<int>& position);
    void updateHoverState(int blockIndex);
    
    // Initialization
    void initializeEffectBlocks();
    juce::Colour getEffectColour(EffectBlockType type);
    juce::String getEffectName(EffectBlockType type);
    juce::String getEffectShortName(EffectBlockType type);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectChainVisualizer)
};
