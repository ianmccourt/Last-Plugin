#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class AIGuitarPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AIGuitarPluginAudioProcessorEditor (AIGuitarPluginAudioProcessor&);
    ~AIGuitarPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Reference to processor
    AIGuitarPluginAudioProcessor& audioProcessor;
    
    // Control components
    juce::Slider gainSlider, toneSlider;
    juce::TextEditor promptEditor;
    juce::TextButton generateButton;
    juce::Label gainLabel, toneLabel, titleLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    
    // Event handlers
    void generateButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AIGuitarPluginAudioProcessorEditor)
};