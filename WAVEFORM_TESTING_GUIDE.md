# Testing AI Guitar Plugin with Waveform Free

## 🎯 **Complete Testing Workflow**

This guide will walk you through building and testing your AI guitar plugin with Waveform Free and your PreSonus AudioBox interface.

## 📋 **Prerequisites Checklist**

### Required Software
- [ ] **Xcode** (latest version from App Store)
- [ ] **CMake** 3.22+ (`brew install cmake`)
- [ ] **Node.js** 18+ (`brew install node`)
- [ ] **Waveform Free** (download from Tracktion)
- [ ] **PreSonus AudioBox drivers** (latest from PreSonus website)

### Required Accounts
- [ ] **OpenAI API account** with credits
- [ ] **OpenAI API key** ready

## 🔧 **Step 1: Set Up the AI Server**

### 1.1 Install Server Dependencies
```bash
cd /Users/ianmccourt/AI/Last-Plugin/preset-server
npm install
```

### 1.2 Configure API Key
```bash
# Copy the example environment file
cp env.example .env

# Edit the .env file and add your OpenAI API key
nano .env
```

Add this line to `.env`:
```
OPENAI_API_KEY=your_actual_openai_api_key_here
```

### 1.3 Test the Server
```bash
# Start the server
npm start
```

You should see:
```
🎸 AI Guitar Preset Server running on http://localhost:8787
📡 Health check: http://localhost:8787/health
🎵 Preset endpoint: POST http://localhost:8787/preset
```

### 1.4 Verify Server is Working
Open a new terminal and test:
```bash
# Test health endpoint
curl http://localhost:8787/health

# Should return: {"status":"ok","timestamp":"...","version":"1.0.0"}
```

### 1.5 Test Preset Generation
```bash
cd /Users/ianmccourt/AI/Last-Plugin/preset-server
node test-preset.js "ambient clean guitar"
```

If successful, you'll see a generated preset with effect chain details.

## 🏗️ **Step 2: Build the Plugin**

### 2.1 Generate Xcode Project
```bash
cd /Users/ianmccourt/AI/Last-Plugin
./build.sh --generate-only
```

This creates `build/AIGuitarPlugin.xcodeproj`

### 2.2 Open in Xcode
```bash
open build/AIGuitarPlugin.xcodeproj
```

### 2.3 Configure Build Settings
In Xcode:
1. Select **AIGuitarPlugin_All** scheme
2. Set build configuration to **Release**
3. Ensure target is **"My Mac"**

### 2.4 Build the Plugin
Press **Cmd+B** or click **Product → Build**

**Expected build time**: 2-5 minutes for first build

### 2.5 Verify Build Success
Check for these files in `build/AIGuitarPlugin_artefacts/Release/`:
- `AIGuitarPlugin.component` (AU plugin)
- `AIGuitarPlugin.vst3` (VST3 plugin)
- `AIGuitarPlugin.app` (Standalone app)

## 🎵 **Step 3: Install Plugin for Waveform**

### 3.1 Copy Plugin Files
```bash
# Create plugin directories if they don't exist
mkdir -p ~/Library/Audio/Plug-Ins/Components
mkdir -p ~/Library/Audio/Plug-Ins/VST3

# Copy the plugins
cp -R build/AIGuitarPlugin_artefacts/Release/AIGuitarPlugin.component ~/Library/Audio/Plug-Ins/Components/
cp -R build/AIGuitarPlugin_artefacts/Release/AIGuitarPlugin.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

### 3.2 Verify Installation
```bash
# Check if files were copied correctly
ls -la ~/Library/Audio/Plug-Ins/Components/AIGuitarPlugin.component
ls -la ~/Library/Audio/Plug-Ins/VST3/AIGuitarPlugin.vst3
```

## 🎛️ **Step 4: Set Up PreSonus AudioBox**

### 4.1 Connect Hardware
1. **Connect guitar** to AudioBox Input 1 (instrument input)
2. **Connect headphones/monitors** to AudioBox outputs
3. **Connect AudioBox** to Mac via USB

### 4.2 Configure AudioBox
1. **Set input gain** so signal peaks around -12dB to -6dB
2. **Turn OFF direct monitoring** (we want to hear the plugin)
3. **Set sample rate** to 44.1kHz or 48kHz

### 4.3 Test AudioBox
Open **Audio MIDI Setup** (Applications → Utilities):
1. Find your AudioBox device
2. Verify it shows **2 inputs, 2 outputs**
3. Set sample rate to **44.1kHz**

## 🎚️ **Step 5: Configure Waveform Free**

### 5.1 Launch Waveform
Open **Waveform Free** from Applications

### 5.2 Set Audio Device
1. Go to **Settings → Audio**
2. Select **PreSonus AudioBox** as audio device
3. Set **Sample Rate**: 44.1kHz
4. Set **Buffer Size**: 128 samples (start here, adjust if needed)
5. Click **Apply**

### 5.3 Scan for Plugins
1. Go to **Settings → Plugins**
2. Click **Scan for new plugins**
3. Wait for scan to complete
4. Verify **AI Guitar Plugin** appears in the list

## 🎸 **Step 6: Test the Complete System**

### 6.1 Create New Project
1. **File → New Project**
2. Choose **Empty Project** or **Basic Template**
3. Set project sample rate to **44.1kHz**

### 6.2 Add Audio Track
1. **Insert → Audio Track**
2. Set track input to **Input 1** (your guitar)
3. **Arm the track** for recording (red button)
4. **Enable input monitoring** (speaker icon)

### 6.3 Insert AI Guitar Plugin
1. Click the track's **FX** button
2. **Add Plugin → AI Guitar Plugin**
3. The plugin UI should open

### 6.4 First Test - Basic Functionality
1. **Play your guitar** - you should hear clean signal
2. **Click the Bypass button** - signal should cut out
3. **Unbypass** - signal returns
4. **Adjust Input/Output gains** - levels should change smoothly

## 🤖 **Step 7: Test AI Generation**

### 7.1 Ensure Server is Running
In terminal:
```bash
cd /Users/ianmccourt/AI/Last-Plugin/preset-server
npm start
```

Keep this running during testing.

### 7.2 Test Basic AI Generation
1. In the plugin, type: **"clean jazz tone"**
2. Click **Generate**
3. Watch for:
   - Status changes to "Generating tone..."
   - Progress bar appears
   - Generation time displays
   - Status changes to "Ready" when complete

### 7.3 Test Abstract Sounds
Try these descriptions:
- **"ethereal floating"** → Should generate shimmer reverb + delay
- **"underwater dreams"** → Should generate heavy chorus + muffled EQ
- **"broken radio"** → Should generate extreme EQ + compression
- **"cosmic voyage"** → Should generate long reverb + slow modulation

### 7.4 Test Advanced Features
1. **Variations**: Generate a preset, then click "Variations"
2. **Surprise Me**: Click for random experimental sounds
3. **A/B Compare**: Generate two presets and compare them

## 🔧 **Step 8: Optimize Performance**

### 8.1 Adjust Buffer Size
If you experience:
- **Audio glitches/dropouts**: Increase buffer size (256 or 512)
- **Too much latency**: Decrease buffer size (64 samples)

### 8.2 Monitor CPU Usage
In Waveform:
1. Check **CPU meter** in top bar
2. Keep usage below **50%** for stable performance
3. If high CPU, try:
   - Increase buffer size
   - Disable unused effects in generated presets
   - Close other applications

### 8.3 Test Latency
1. Play guitar and listen for delay between input and output
2. **Good latency**: <10ms (imperceptible)
3. **Acceptable**: 10-20ms (slight delay but playable)
4. **Too high**: >20ms (noticeable delay)

## 🎵 **Step 9: Creative Testing**

### 9.1 Record Test Clips
1. **Record clean guitar** (bypass plugin)
2. **Generate AI preset** with description
3. **Record processed guitar** with same performance
4. **Compare results** - A/B the recordings

### 9.2 Test Different Guitar Types
- **Single coil pickups** (Stratocaster-style)
- **Humbuckers** (Les Paul-style)
- **Acoustic guitar** (if you have one)

### 9.3 Test Musical Styles
- **Clean jazz chords** → "warm clean jazz tone"
- **Blues licks** → "vintage blues overdrive"
- **Ambient picking** → "ethereal floating soundscape"
- **Heavy riffs** → "industrial chaos"

## 🚨 **Troubleshooting Common Issues**

### Plugin Won't Load in Waveform
```bash
# Check if plugin files exist
ls -la ~/Library/Audio/Plug-Ins/Components/AIGuitarPlugin.component
ls -la ~/Library/Audio/Plug-Ins/VST3/AIGuitarPlugin.vst3

# If missing, rebuild and reinstall
cd /Users/ianmccourt/AI/Last-Plugin
./build.sh
# Then copy files again
```

### AI Generation Not Working
1. **Check server status**:
   ```bash
   curl http://localhost:8787/health
   ```

2. **Check server logs** for errors

3. **Verify API key** in `.env` file

4. **Test with curl**:
   ```bash
   curl -X POST http://localhost:8787/preset \
     -H "Content-Type: application/json" \
     -d '{"prompt": "clean guitar"}'
   ```

### Audio Issues
- **No sound**: Check input routing and plugin bypass
- **Distorted sound**: Lower input gain on AudioBox
- **Crackling**: Increase buffer size in Waveform
- **High latency**: Decrease buffer size

### High CPU Usage
- **Disable oversampling** in drive effects
- **Use shorter reverb decay** times
- **Close other applications**
- **Increase buffer size**

## ✅ **Success Criteria**

Your system is working correctly if:
- [ ] Plugin loads in Waveform without errors
- [ ] Clean guitar signal passes through
- [ ] AI generation completes in 5-15 seconds
- [ ] Generated presets sound musical and appropriate
- [ ] Abstract descriptions create experimental sounds
- [ ] No audio glitches during preset changes
- [ ] CPU usage stays reasonable (<50%)
- [ ] Latency is acceptable for playing

## 🎯 **Next Steps After Testing**

Once basic testing works:
1. **Experiment with creative descriptions**
2. **Build a library of favorite presets**
3. **Test with different musical styles**
4. **Record demo tracks** showcasing the AI capabilities
5. **Share results** and get feedback

## 📞 **Getting Help**

If you encounter issues:
1. **Check server logs** in terminal
2. **Look at Waveform's plugin scanner** for error messages
3. **Try the standalone app** first (simpler testing)
4. **Test with built-in example presets** before AI generation

The key is to test **step by step** - get the basic audio working first, then add AI generation, then optimize performance!
