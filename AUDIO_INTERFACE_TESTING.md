# 🎸 Testing AI Guitar Plugin with PreSonus AudioBox

## 🎯 Current Status
- ✅ **AI Server**: Running on `http://localhost:8787`
- ✅ **Plugin Built**: AU, VST3, Standalone formats ready
- ✅ **Plugin Installed**: Available in system directories
- ✅ **Preset Generation**: Working with OpenAI API

## 🔌 Hardware Setup

### 1. Connect Your PreSonus AudioBox
1. **Connect AudioBox to Mac**: USB cable to your Mac
2. **Connect Guitar**: Plug guitar into Input 1 (Hi-Z/Instrument input)
3. **Connect Monitors/Headphones**: To AudioBox outputs
4. **Power On**: AudioBox should show power LED

### 2. Configure macOS Audio
1. **System Preferences** → **Sound** → **Output**
2. Select **"PreSonus AudioBox"** as output device
3. **Input** tab → Select **"PreSonus AudioBox"** as input device

## 🎵 Testing Options

### Option 1: Standalone Plugin (Recommended First)
```bash
# Open the standalone app
open "/Users/ianmccourt/AI/Last-Plugin/build/AIGuitarPlugin_artefacts/Release/Standalone/AI Guitar Plugin.app"
```

**Standalone Benefits:**
- Direct audio interface access
- No DAW complexity
- Immediate testing
- Built-in audio device selection

### Option 2: Waveform Free (DAW Testing)
1. **Launch Waveform Free**
2. **Audio Settings**:
   - Input: PreSonus AudioBox
   - Output: PreSonus AudioBox
   - Sample Rate: 44.1kHz or 48kHz
   - Buffer Size: 128 or 256 samples (low latency)
3. **Load Plugin**:
   - Create audio track
   - Add plugin: "AI Guitar Plugin" (VST3 or AU)

## 🎛️ Plugin Testing Steps

### 1. Basic Audio Test
1. **Play guitar** → Should see input signal
2. **Adjust Gain** → Test volume control
3. **Adjust Tone** → Test filter sweep
4. **Bypass** → Compare processed vs. clean signal

### 2. AI Preset Testing
Since the UI doesn't have AI integration yet, test presets via server:

```bash
# Generate a clean preset
curl -X POST http://localhost:8787/preset \
  -H "Content-Type: application/json" \
  -d '{"prompt": "warm clean jazz guitar"}' | jq '.'

# Generate an ambient preset  
curl -X POST http://localhost:8787/preset \
  -H "Content-Type: application/json" \
  -d '{"prompt": "ethereal ambient wash"}' | jq '.'

# Generate a drive preset
curl -X POST http://localhost:8787/preset \
  -H "Content-Type: application/json" \
  -d '{"prompt": "vintage tube overdrive"}' | jq '.'
```

### 3. Performance Testing
- **Latency**: Should feel responsive while playing
- **CPU Usage**: Monitor Activity Monitor
- **Audio Quality**: No clicks, pops, or dropouts
- **Stability**: Plugin shouldn't crash or freeze

## 🔧 Troubleshooting

### Audio Interface Issues
```bash
# Check if AudioBox is recognized
system_profiler SPAudioDataType | grep -i presonus

# List available audio devices
ls /System/Library/Extensions/AppleHDA.kext/Contents/PlugIns/
```

### Plugin Issues
```bash
# Verify plugin installation
ls -la "/Users/ianmccourt/Library/Audio/Plug-Ins/VST3/AI Guitar Plugin.vst3"
ls -la "/Users/ianmccourt/Library/Audio/Plug-Ins/Components/AI Guitar Plugin.component"

# Check plugin validation (AU)
auval -v aufx Aigp Aigc
```

### Waveform Free Setup
1. **Preferences** → **Audio**
2. **Device Type**: Core Audio
3. **Input Device**: PreSonus AudioBox
4. **Output Device**: PreSonus AudioBox
5. **Sample Rate**: Match your interface settings
6. **Buffer Size**: Start with 256, lower if needed

## 🎸 Test Scenarios

### Scenario 1: Clean Playing
- **Goal**: Test basic audio processing
- **Settings**: Gain ~0.5, Tone ~0.7
- **Play**: Clean chords and single notes
- **Listen**: Should sound clear with slight tone coloring

### Scenario 2: Gain Staging
- **Goal**: Test parameter response
- **Action**: Slowly increase gain while playing
- **Listen**: Smooth parameter changes, no clicks

### Scenario 3: Tone Sweep
- **Goal**: Test filter response
- **Action**: Sweep tone from 0 to 1 while playing
- **Listen**: Gradual high-frequency roll-off

## 📊 Expected Results

### ✅ Success Indicators
- Guitar signal passes through plugin
- Controls respond smoothly
- No audio dropouts or glitches
- Low latency feel while playing
- AI server generates presets successfully

### ⚠️ Potential Issues
- **High Latency**: Increase buffer size in DAW
- **No Audio**: Check audio device selection
- **Clicks/Pops**: Increase buffer size or check CPU usage
- **Plugin Not Found**: Verify installation paths

## 🚀 Next Steps After Testing

1. **If Basic Audio Works**: Great! The core system is solid
2. **If AI Integration Needed**: We can add UI controls for preset loading
3. **If Performance Issues**: We can optimize DSP or threading
4. **If All Good**: Ready for advanced features and more effects!

## 📝 Test Results Template

```
Date: ___________
Hardware: PreSonus AudioBox + Guitar
Software: Waveform Free / Standalone

✅ Audio Input: Working / Issues: ___________
✅ Audio Output: Working / Issues: ___________  
✅ Gain Control: Working / Issues: ___________
✅ Tone Control: Working / Issues: ___________
✅ Latency: Good / High / Issues: ___________
✅ Stability: Stable / Crashes / Issues: ___________
✅ AI Server: Working / Issues: ___________

Overall: Success / Needs Work
Notes: ___________
```

Ready to rock! 🎸🤖
