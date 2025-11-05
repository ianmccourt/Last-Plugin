# AI Guitar Plugin - Setup Guide

## Quick Start

This plugin has been improved and fixed for stability. Follow these steps to get it running.

## Prerequisites

### For Building the Plugin (macOS)
- macOS 10.15 or later
- Xcode 12 or later (with Command Line Tools)
- CMake 3.22 or later

### For the AI Server
- Node.js 16 or later
- OpenAI API key

## Installation Steps

### 1. Set Up the AI Server

```bash
# Navigate to the preset server directory
cd preset-server

# Install dependencies
npm install

# Create environment file
cp env.example .env

# Edit .env and add your OpenAI API key
# OPENAI_API_KEY=your_api_key_here
nano .env  # or use any text editor
```

### 2. Start the AI Server

```bash
# From the preset-server directory
npm start
```

The server will run on `http://localhost:8787`

Leave this terminal window open and running.

### 3. Build the Plugin (Optional - for development)

```bash
# From the project root
mkdir build
cd build

# Configure with CMake
cmake .. -G "Xcode"

# Build the project
cmake --build . --config Release

# Or open in Xcode
open AIGuitarPlugin.xcodeproj
```

The plugin will be built as:
- VST3: `build/AIGuitarPlugin_artefacts/Release/VST3/`
- AU: `build/AIGuitarPlugin_artefacts/Release/AU/`
- Standalone: `build/AIGuitarPlugin_artefacts/Release/Standalone/`

### 4. Install the Plugin

**Option A: Automatic (when building)**
CMake is configured to automatically copy plugins to the correct directories.

**Option B: Manual**
```bash
# For AU (Audio Unit)
cp -r build/AIGuitarPlugin_artefacts/Release/AU/AIGuitarPlugin.component ~/Library/Audio/Plug-Ins/Components/

# For VST3
cp -r build/AIGuitarPlugin_artefacts/Release/VST3/AIGuitarPlugin.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

### 5. Use the Plugin

1. Open your DAW (Logic Pro, Reaper, Ableton, etc.)
2. Create an audio track with your guitar input
3. Insert "AI Guitar Plugin"
4. Make sure the AI server is running (step 2)
5. Type a tone description (e.g., "heavy metal distortion")
6. Click "Generate Tone"
7. Adjust gain and tone controls as needed

## Troubleshooting

### Server Issues

**Problem:** "Failed to connect to AI server"
- Make sure the server is running: `cd preset-server && npm start`
- Check that it's listening on port 8787
- Verify no firewall is blocking localhost:8787

**Problem:** "Invalid API key" or quota errors
- Check your .env file has a valid OpenAI API key
- Verify you have API credits available

### Build Issues

**Problem:** CMake not found
```bash
# Install CMake via Homebrew
brew install cmake
```

**Problem:** "No CMAKE_CXX_COMPILER could be found"
```bash
# Install Xcode Command Line Tools
xcode-select --install
```

**Problem:** JUCE download fails
- Check your internet connection
- CMake will automatically download JUCE 8.0.4

### Plugin Issues

**Problem:** Plugin doesn't appear in DAW
- Rescan plugins in your DAW
- Check plugin was copied to the correct directory
- For AU plugins on macOS, validate with `auval -a` command

**Problem:** No sound output
- Check input/output gain settings
- Verify your audio interface is properly configured
- Try bypassing and re-enabling the plugin

## Key Improvements Made

This version includes critical fixes:

1. **Fixed crash with multiple instances** - Static variables in audio processing removed
2. **Fixed memory leaks** - Proper thread pool management for AI requests
3. **Added safety checks** - Null pointer checks and parameter validation
4. **Improved stability** - Proper cleanup on plugin destruction
5. **Fixed package dependencies** - Corrected npm package versions

## Testing

To verify the plugin is working:

1. Start the server: `cd preset-server && npm start`
2. Test the server: `curl http://localhost:8787/health`
   - Should return: `{"status":"ok","timestamp":"...","version":"1.0.0"}`
3. Load plugin in your DAW
4. Generate a preset and verify parameters change

## Development

To contribute or modify:

```bash
# Install server dependencies
cd preset-server && npm install

# Make your changes to C++ code in Source/

# Rebuild
cd build
cmake --build . --config Release

# Test in your DAW
```

## License

MIT License - see LICENSE file for details.

## Support

For issues:
1. Check this guide's troubleshooting section
2. Verify server is running and accessible
3. Check JUCE framework documentation for audio issues
4. Review the main README.md for architecture details
