#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

// Forward declarations
struct NoiseGateParams;
struct CompressorParams;
struct DriveParams;
struct AmpParams;
struct CabinetParams;
struct ChorusParams;
struct DelayParams;
struct ReverbParams;
struct EqualizerParams;

// Effect block types
enum class EffectBlockType
{
    NoiseGate,
    Compressor,
    Drive,
    Amp,
    Cabinet,
    Chorus,
    Delay,
    Reverb,
    Equalizer
};

// Base effect block
struct EffectBlock
{
    EffectBlockType type;
    bool enabled = true;
    
    virtual ~EffectBlock() = default;
    virtual std::unique_ptr<EffectBlock> clone() const = 0;
    virtual juce::var toVar() const = 0;
    virtual void fromVar(const juce::var& var) = 0;
};

// Noise Gate Parameters
struct NoiseGateParams : public EffectBlock
{
    float thresholdDb = -55.0f;  // -90 to 0 dB
    float releaseMs = 100.0f;    // 5 to 500 ms
    
    NoiseGateParams() { type = EffectBlockType::NoiseGate; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<NoiseGateParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Compressor Parameters
struct CompressorParams : public EffectBlock
{
    float ratio = 3.0f;          // 1 to 10
    float thresholdDb = -18.0f;  // -60 to 0 dB
    float attackMs = 10.0f;      // 0.1 to 50 ms
    float releaseMs = 60.0f;     // 10 to 500 ms
    float makeupDb = 2.0f;       // -12 to 12 dB
    
    CompressorParams() { type = EffectBlockType::Compressor; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<CompressorParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Drive Parameters
enum class DriveType { SoftClip, HardClip, TubeScreamer, Fuzz };

struct DriveParams : public EffectBlock
{
    DriveType driveType = DriveType::SoftClip;
    float drive = 0.6f;          // 0 to 1
    float tone = 0.55f;          // 0 to 1
    int oversample = 2;          // 1, 2, or 4
    
    DriveParams() { type = EffectBlockType::Drive; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<DriveParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Amp Parameters
enum class AmpModel { CleanBlackface, JanglyVox, BritCrunch, HiGain };

struct AmpParams : public EffectBlock
{
    AmpModel model = AmpModel::CleanBlackface;
    float gain = 0.45f;          // 0 to 1
    float bass = 0.48f;          // 0 to 1
    float mid = 0.35f;           // 0 to 1
    float treble = 0.62f;        // 0 to 1
    float presence = 0.52f;      // 0 to 1
    float master = 0.7f;         // 0 to 1
    
    AmpParams() { type = EffectBlockType::Amp; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<AmpParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Cabinet Parameters
enum class CabinetIR { OneByTwelveOpen, TwoByTwelveOpen, FourByTwelveClosed };

struct CabinetParams : public EffectBlock
{
    CabinetIR irName = CabinetIR::TwoByTwelveOpen;
    float loCutHz = 70.0f;       // 20 to 200 Hz
    float hiCutHz = 8000.0f;     // 3000 to 12000 Hz
    
    CabinetParams() { type = EffectBlockType::Cabinet; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<CabinetParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Chorus Parameters
struct ChorusParams : public EffectBlock
{
    float rateHz = 0.3f;         // 0.05 to 5 Hz
    float depth = 0.35f;         // 0 to 1
    float mix = 0.25f;           // 0 to 1
    
    ChorusParams() { type = EffectBlockType::Chorus; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<ChorusParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Delay Parameters
struct DelayParams : public EffectBlock
{
    float timeMs = 420.0f;       // 40 to 1200 ms
    float feedback = 0.35f;      // 0 to 0.95
    float mix = 0.2f;            // 0 to 1
    
    DelayParams() { type = EffectBlockType::Delay; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<DelayParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Reverb Parameters
enum class ReverbAlgorithm { Room, Plate, Hall, Shimmer };

struct ReverbParams : public EffectBlock
{
    ReverbAlgorithm algorithm = ReverbAlgorithm::Plate;
    float preDelayMs = 12.0f;    // 0 to 60 ms
    float decayS = 4.5f;         // 0.2 to 12 s
    float damping = 0.35f;       // 0 to 1
    float mix = 0.28f;           // 0 to 1
    
    ReverbParams() { type = EffectBlockType::Reverb; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<ReverbParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Equalizer Parameters
struct EqualizerParams : public EffectBlock
{
    float lowShelfHz = 120.0f;   // 60 to 200 Hz
    float lowGainDb = 1.5f;      // -12 to 12 dB
    float midHz = 1200.0f;       // 300 to 3000 Hz
    float midQ = 0.9f;           // 0.3 to 4
    float midGainDb = -1.0f;     // -12 to 12 dB
    float highShelfHz = 6000.0f; // 4000 to 10000 Hz
    float highGainDb = 1.0f;     // -12 to 12 dB
    
    EqualizerParams() { type = EffectBlockType::Equalizer; }
    
    std::unique_ptr<EffectBlock> clone() const override
    {
        return std::make_unique<EqualizerParams>(*this);
    }
    
    juce::var toVar() const override;
    void fromVar(const juce::var& var) override;
};

// Complete preset data
struct PresetData
{
    juce::String name;
    juce::String notes;
    std::vector<std::unique_ptr<EffectBlock>> chain;
    
    // Copy constructor
    PresetData(const PresetData& other);
    PresetData& operator=(const PresetData& other);
    
    // Move constructor
    PresetData(PresetData&& other) noexcept = default;
    PresetData& operator=(PresetData&& other) noexcept = default;
    
    PresetData() = default;
    
    // Serialization
    juce::var toVar() const;
    void fromVar(const juce::var& var);
    
    // JSON Schema for OpenAI Structured Outputs
    static juce::String getJSONSchema();
    
    // Validation
    bool isValid() const;
    void clampToValidRanges();
};

// Utility functions
juce::String effectBlockTypeToString(EffectBlockType type);
EffectBlockType stringToEffectBlockType(const juce::String& str);

juce::String driveTypeToString(DriveType type);
DriveType stringToDriveType(const juce::String& str);

juce::String ampModelToString(AmpModel model);
AmpModel stringToAmpModel(const juce::String& str);

juce::String cabinetIRToString(CabinetIR ir);
CabinetIR stringToCabinetIR(const juce::String& str);

juce::String reverbAlgorithmToString(ReverbAlgorithm algo);
ReverbAlgorithm stringToReverbAlgorithm(const juce::String& str);
