#include "PresetManager.h"

PresetManager::PresetManager()
{
    // Set up preset directory
    presetDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                     .getChildFile("AI Guitar Plugin")
                     .getChildFile("Presets");
    
    if (!presetDirectory.exists())
        presetDirectory.createDirectory();
}

PresetManager::~PresetManager()
{
}

void PresetManager::savePreset(const PresetData& preset, const juce::String& name)
{
    auto file = getPresetFile(name);
    auto json = preset.toVar();
    auto jsonString = juce::JSON::toString(json);
    
    file.replaceWithText(jsonString);
}

PresetData PresetManager::loadPreset(const juce::String& name)
{
    auto file = getPresetFile(name);
    
    if (file.exists())
    {
        auto jsonString = file.loadFileAsString();
        auto json = juce::JSON::parse(jsonString);
        
        PresetData preset;
        preset.fromVar(json);
        return preset;
    }
    
    return PresetData(); // Return empty preset if file doesn't exist
}

juce::StringArray PresetManager::getPresetNames() const
{
    juce::StringArray names;
    
    for (auto& file : presetDirectory.findChildFiles(juce::File::findFiles, false, "*.json"))
    {
        names.add(file.getFileNameWithoutExtension());
    }
    
    return names;
}

void PresetManager::setCurrentPreset(const PresetData& preset)
{
    currentPreset = preset;
}

PresetData PresetManager::getCurrentPreset() const
{
    return currentPreset;
}

juce::File PresetManager::getPresetFile(const juce::String& name) const
{
    return presetDirectory.getChildFile(name + ".json");
}
