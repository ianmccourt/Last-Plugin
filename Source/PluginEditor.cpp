#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AIGuitarPluginAudioProcessorEditor::AIGuitarPluginAudioProcessorEditor (AIGuitarPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set up gain slider
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainSlider.setRange(0.0, 2.0, 0.01);
    gainSlider.setValue(1.0);
    addAndMakeVisible(gainSlider);
    
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);
    
    // Set up tone slider
    toneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    toneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    toneSlider.setRange(0.0, 1.0, 0.01);
    toneSlider.setValue(0.5);
    addAndMakeVisible(toneSlider);
    
    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.attachToComponent(&toneSlider, false);
    toneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(toneLabel);
    
    // Set up title label
    titleLabel.setText("AI Guitar Plugin", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    // Set up AI prompt text editor - simple and reliable
    promptEditor.setMultiLine(false);
    promptEditor.setReturnKeyStartsNewLine(false);
    promptEditor.setText("Enter tone description...");
    promptEditor.setFont(juce::FontOptions(16.0f));
    addAndMakeVisible(promptEditor);
    
    // Set up generate button
    generateButton.setButtonText("Generate Tone");
    generateButton.onClick = [this] { generateButtonClicked(); };
    addAndMakeVisible(generateButton);
    
    // Set up parameter attachments
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "gain", gainSlider);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "tone", toneSlider);
    
    // Set size - smaller and simpler
    setSize (500, 300);
}

AIGuitarPluginAudioProcessorEditor::~AIGuitarPluginAudioProcessorEditor()
{
    // Simple destructor - no custom look and feel to clean up
}


//==============================================================================
void AIGuitarPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Simple background
    g.fillAll(juce::Colours::darkgrey);
}

void AIGuitarPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Title area
    auto titleArea = bounds.removeFromTop(40);
    titleLabel.setBounds(titleArea);
    
    // AI input area
    auto aiArea = bounds.removeFromTop(80);
    auto promptArea = aiArea.removeFromTop(30).reduced(10, 5);
    promptEditor.setBounds(promptArea);
    
    auto buttonArea = aiArea.removeFromTop(30).reduced(10, 5);
    generateButton.setBounds(buttonArea);
    
    // Sliders area
    auto sliderArea = bounds.reduced(20);
    auto sliderWidth = sliderArea.getWidth() / 2;
    
    gainSlider.setBounds(sliderArea.removeFromLeft(sliderWidth).reduced(5));
    toneSlider.setBounds(sliderArea.reduced(5));
}

void AIGuitarPluginAudioProcessorEditor::generateButtonClicked()
{
    auto description = promptEditor.getText();
    if (description.isNotEmpty() && description != "Enter tone description...")
    {
        // Disable button during generation
        generateButton.setEnabled(false);
        generateButton.setButtonText("Generating...");
        
        // Call the AI generation
        audioProcessor.generatePresetFromText(description);
        
        // Re-enable button after a delay
        juce::Timer::callAfterDelay(2000, [this]() {
            generateButton.setEnabled(true);
            generateButton.setButtonText("Generate Tone");
        });
    }
}
