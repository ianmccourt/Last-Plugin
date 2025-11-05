#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Advanced AI prompt input component with suggestions and history
 * Features intelligent text completion and visual feedback
 */
class AIPromptComponent : public juce::Component,
                        public juce::TextEditor::Listener,
                        public juce::Timer
{
public:
    AIPromptComponent();
    ~AIPromptComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    // TextEditor listener
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor& editor) override;
    void textEditorFocusLost(juce::TextEditor& editor) override;
    
    // Public interface
    juce::String getCurrentPrompt() const;
    void setPrompt(const juce::String& text);
    void clearPrompt();
    void setGenerating(bool isGenerating);
    
    // Callbacks
    std::function<void(const juce::String&)> onPromptSubmitted;
    std::function<void()> onVariationsRequested;
    std::function<void()> onSurpriseMeRequested;
    
    // Configuration
    void setShowSuggestions(bool show) { showSuggestions = show; }
    void setMaxHistorySize(int size) { maxHistorySize = size; }
    
private:
    // UI Components
    juce::TextEditor promptEditor;
    juce::TextButton generateButton;
    juce::TextButton variationsButton;
    juce::TextButton surpriseMeButton;
    juce::TextButton historyButton;
    
    // Suggestion system
    juce::ListBox suggestionsList;
    juce::Component suggestionsContainer;
    bool showSuggestions = true;
    bool suggestionsVisible = false;
    
    // History system
    juce::StringArray promptHistory;
    int maxHistorySize = 50;
    int currentHistoryIndex = -1;
    
    // Visual state
    bool isGenerating = false;
    float typingAnimationPhase = 0.0f;
    float generateButtonPulse = 0.0f;
    
    // Predefined suggestions
    juce::StringArray defaultSuggestions;
    juce::StringArray contextualSuggestions;
    
    // Suggestion categories
    struct SuggestionCategory
    {
        juce::String name;
        juce::StringArray suggestions;
        juce::Colour colour;
    };
    
    std::vector<SuggestionCategory> suggestionCategories;
    
    // Layout and interaction
    void updateLayout();
    void updateSuggestions();
    void showSuggestionsPopup();
    void hideSuggestionsPopup();
    void applySuggestion(const juce::String& suggestion);
    
    // History management
    void addToHistory(const juce::String& prompt);
    void navigateHistory(int direction);
    void showHistoryPopup();
    
    // Button callbacks
    void onGenerateClicked();
    void onVariationsClicked();
    void onSurpriseMeClicked();
    void onHistoryClicked();
    
    // Suggestion generation
    juce::StringArray generateContextualSuggestions(const juce::String& currentText);
    juce::StringArray filterSuggestions(const juce::StringArray& suggestions, 
                                       const juce::String& filter);
    
    // Animation and visual effects
    void updateAnimations();
    void drawTypingIndicator(juce::Graphics& g, const juce::Rectangle<int>& area);
    void drawGeneratingEffect(juce::Graphics& g, const juce::Rectangle<int>& area);
    
    // Initialization
    void setupComponents();
    void setupSuggestions();
    void setupKeyboardShortcuts();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIPromptComponent)
};
