#include "PresetSchema.h"

// Minimal implementations for PresetSchema classes

// PresetData implementation
PresetData::PresetData(const PresetData& other)
    : name(other.name), notes(other.notes)
{
    for (const auto& block : other.chain)
    {
        if (block)
            chain.push_back(block->clone());
    }
}

PresetData& PresetData::operator=(const PresetData& other)
{
    if (this != &other)
    {
        name = other.name;
        notes = other.notes;
        chain.clear();
        for (const auto& block : other.chain)
        {
            if (block)
                chain.push_back(block->clone());
        }
    }
    return *this;
}

juce::var PresetData::toVar() const
{
    auto obj = new juce::DynamicObject();
    obj->setProperty("name", name);
    obj->setProperty("notes", notes);
    
    juce::Array<juce::var> chainArray;
    for (const auto& block : chain)
    {
        if (block)
            chainArray.add(block->toVar());
    }
    obj->setProperty("chain", chainArray);
    
    return juce::var(obj);
}

void PresetData::fromVar(const juce::var& var)
{
    if (var.hasProperty("name"))
        name = var["name"].toString();
    if (var.hasProperty("notes"))
        notes = var["notes"].toString();
    
    // Chain parsing would be implemented here
    // For now, just clear the chain
    chain.clear();
}

juce::String PresetData::getJSONSchema()
{
    return R"({
        "type": "object",
        "required": ["name", "chain"],
        "properties": {
            "name": {"type": "string"},
            "notes": {"type": "string"},
            "chain": {
                "type": "array",
                "items": {
                    "type": "object",
                    "required": ["block", "enabled", "params"],
                    "properties": {
                        "block": {"type": "string"},
                        "enabled": {"type": "boolean"},
                        "params": {"type": "object"}
                    }
                }
            }
        }
    })";
}

bool PresetData::isValid() const
{
    return !name.isEmpty() && !chain.empty();
}

void PresetData::clampToValidRanges()
{
    // Implementation would clamp all parameter values to valid ranges
}

// Stub implementations for parameter classes
juce::var NoiseGateParams::toVar() const
{
    auto obj = new juce::DynamicObject();
    obj->setProperty("block", "noise_gate");
    obj->setProperty("enabled", enabled);
    
    auto params = new juce::DynamicObject();
    params->setProperty("threshold_db", thresholdDb);
    params->setProperty("release_ms", releaseMs);
    obj->setProperty("params", juce::var(params));
    
    return juce::var(obj);
}

void NoiseGateParams::fromVar(const juce::var& var)
{
    enabled = var.getProperty("enabled", true);
    auto params = var["params"];
    if (params.isObject())
    {
        thresholdDb = params.getProperty("threshold_db", -55.0f);
        releaseMs = params.getProperty("release_ms", 100.0f);
    }
}

// Similar stub implementations for other parameter classes
juce::var CompressorParams::toVar() const { return juce::var(); }
void CompressorParams::fromVar(const juce::var& var) {}

juce::var DriveParams::toVar() const { return juce::var(); }
void DriveParams::fromVar(const juce::var& var) {}

juce::var AmpParams::toVar() const { return juce::var(); }
void AmpParams::fromVar(const juce::var& var) {}

juce::var CabinetParams::toVar() const { return juce::var(); }
void CabinetParams::fromVar(const juce::var& var) {}

juce::var ChorusParams::toVar() const { return juce::var(); }
void ChorusParams::fromVar(const juce::var& var) {}

juce::var DelayParams::toVar() const { return juce::var(); }
void DelayParams::fromVar(const juce::var& var) {}

juce::var ReverbParams::toVar() const { return juce::var(); }
void ReverbParams::fromVar(const juce::var& var) {}

juce::var EqualizerParams::toVar() const { return juce::var(); }
void EqualizerParams::fromVar(const juce::var& var) {}

// Utility functions
juce::String effectBlockTypeToString(EffectBlockType type)
{
    switch (type)
    {
        case EffectBlockType::NoiseGate: return "noise_gate";
        case EffectBlockType::Compressor: return "compressor";
        case EffectBlockType::Drive: return "drive";
        case EffectBlockType::Amp: return "amp";
        case EffectBlockType::Cabinet: return "cab";
        case EffectBlockType::Chorus: return "chorus";
        case EffectBlockType::Delay: return "delay";
        case EffectBlockType::Reverb: return "reverb";
        case EffectBlockType::Equalizer: return "eq";
        default: return "unknown";
    }
}

EffectBlockType stringToEffectBlockType(const juce::String& str)
{
    if (str == "noise_gate") return EffectBlockType::NoiseGate;
    if (str == "compressor") return EffectBlockType::Compressor;
    if (str == "drive") return EffectBlockType::Drive;
    if (str == "amp") return EffectBlockType::Amp;
    if (str == "cab") return EffectBlockType::Cabinet;
    if (str == "chorus") return EffectBlockType::Chorus;
    if (str == "delay") return EffectBlockType::Delay;
    if (str == "reverb") return EffectBlockType::Reverb;
    if (str == "eq") return EffectBlockType::Equalizer;
    return EffectBlockType::NoiseGate; // default
}

juce::String driveTypeToString(DriveType type)
{
    switch (type)
    {
        case DriveType::SoftClip: return "softclip";
        case DriveType::HardClip: return "hardclip";
        case DriveType::TubeScreamer: return "tubescreamer";
        case DriveType::Fuzz: return "fuzz";
        default: return "softclip";
    }
}

DriveType stringToDriveType(const juce::String& str)
{
    if (str == "softclip") return DriveType::SoftClip;
    if (str == "hardclip") return DriveType::HardClip;
    if (str == "tubescreamer") return DriveType::TubeScreamer;
    if (str == "fuzz") return DriveType::Fuzz;
    return DriveType::SoftClip; // default
}

juce::String ampModelToString(AmpModel model)
{
    switch (model)
    {
        case AmpModel::CleanBlackface: return "clean_blackface";
        case AmpModel::JanglyVox: return "jangly_vox";
        case AmpModel::BritCrunch: return "brit_crunch";
        case AmpModel::HiGain: return "hi_gain";
        default: return "clean_blackface";
    }
}

AmpModel stringToAmpModel(const juce::String& str)
{
    if (str == "clean_blackface") return AmpModel::CleanBlackface;
    if (str == "jangly_vox") return AmpModel::JanglyVox;
    if (str == "brit_crunch") return AmpModel::BritCrunch;
    if (str == "hi_gain") return AmpModel::HiGain;
    return AmpModel::CleanBlackface; // default
}

juce::String cabinetIRToString(CabinetIR ir)
{
    switch (ir)
    {
        case CabinetIR::OneByTwelveOpen: return "1x12_open";
        case CabinetIR::TwoByTwelveOpen: return "2x12_open";
        case CabinetIR::FourByTwelveClosed: return "4x12_closed";
        default: return "2x12_open";
    }
}

CabinetIR stringToCabinetIR(const juce::String& str)
{
    if (str == "1x12_open") return CabinetIR::OneByTwelveOpen;
    if (str == "2x12_open") return CabinetIR::TwoByTwelveOpen;
    if (str == "4x12_closed") return CabinetIR::FourByTwelveClosed;
    return CabinetIR::TwoByTwelveOpen; // default
}

juce::String reverbAlgorithmToString(ReverbAlgorithm algo)
{
    switch (algo)
    {
        case ReverbAlgorithm::Room: return "room";
        case ReverbAlgorithm::Plate: return "plate";
        case ReverbAlgorithm::Hall: return "hall";
        case ReverbAlgorithm::Shimmer: return "shimmer";
        default: return "plate";
    }
}

ReverbAlgorithm stringToReverbAlgorithm(const juce::String& str)
{
    if (str == "room") return ReverbAlgorithm::Room;
    if (str == "plate") return ReverbAlgorithm::Plate;
    if (str == "hall") return ReverbAlgorithm::Hall;
    if (str == "shimmer") return ReverbAlgorithm::Shimmer;
    return ReverbAlgorithm::Plate; // default
}
