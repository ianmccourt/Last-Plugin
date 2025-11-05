#pragma once

#include <juce_core/juce_core.h>
#include "PresetSchema.h"

/**
 * Simple preset manager for saving/loading presets
 * This is a minimal implementation for testing
 */
class PresetManager
{
public:
    PresetManager();
    ~PresetManager();
    
    // Preset operations
    void savePreset(const PresetData& preset, const juce::String& name);
    PresetData loadPreset(const juce::String& name);
    juce::StringArray getPresetNames() const;
    
    // Current preset
    void setCurrentPreset(const PresetData& preset);
    PresetData getCurrentPreset() const;
    
private:
    PresetData currentPreset;
    juce::File presetDirectory;
    
    juce::File getPresetFile(const juce::String& name) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
