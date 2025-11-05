# AI Guitar Plugin

An innovative guitar effects plugin that uses AI to generate custom tones from natural language descriptions. Simply describe the sound you want (e.g., "ambient, grimy, twangy") and the AI will create a complete effects chain to match your vision.

## Features

- **Natural Language Tone Generation**: Describe your desired sound in plain English
- **Real-time Audio Processing**: Zero-latency DSP chain built with JUCE
- **Professional Effects Chain**: Noise gate, compressor, drive, amp simulation, cabinet IR, chorus, delay, reverb, and EQ
- **OpenAI Integration**: Uses GPT with Structured Outputs for reliable preset generation
- **Cross-Platform**: macOS Universal Binary (Intel + Apple Silicon)
- **Plugin Formats**: AU, VST3, and Standalone application

## Architecture

### Plugin (C++/JUCE)
- **Audio Thread**: Real-time DSP processing with oversampling, convolution, and modulation effects
- **Background Thread**: Handles AI API calls without blocking audio
- **Parameter Smoothing**: Seamless transitions between presets
- **Universal Binary**: Supports both Intel and Apple Silicon Macs

### AI Server (Node.js)
- **OpenAI Integration**: Uses Structured Outputs for reliable JSON generation
- **Security**: API keys stored server-side, not in plugin binary
- **Validation**: Ensures all generated parameters are within safe ranges
- **CORS Support**: Configured for local plugin communication

## Quick Start

### 1. Set Up the AI Server

```bash
cd preset-server
npm install
cp env.example .env
```

Edit `.env` and add your OpenAI API key:
```
OPENAI_API_KEY=your_api_key_here
```

Start the server:
```bash
npm start
```

The server will run on `http://localhost:8787`

### 2. Build the Plugin

Requirements:
- Xcode (macOS)
- CMake 3.22+
- JUCE (automatically downloaded)

```bash
mkdir build
cd build
cmake .. -G Xcode
open AIGuitarPlugin.xcodeproj
```

Build the project in Xcode for Universal Binary (arm64 + x86_64).

### 3. Install and Use

1. Copy the built AU/VST3 to your plugin directories
2. Launch your DAW (Logic Pro, Reaper, etc.)
3. Create an audio track with your guitar input
4. Insert the AI Guitar Plugin
5. Type a description like "warm blues tone with vintage delay"
6. Click "Generate" and play!

## Usage Examples

### Tone Descriptions That Work Well

- **"ambient, grimy, twangy"** → Generates reverb-heavy tone with drive and bright amp settings
- **"heavy metal, tight, aggressive"** → High-gain amp, noise gate, hard clipping, closed cabinet
- **"clean jazz, warm, spacious"** → Clean amp, light compression, plate reverb
- **"shoegaze, dreamy, wall of sound"** → Heavy reverb/delay, chorus, moderate drive
- **"blues, vintage, creamy overdrive"** → Tube screamer drive, clean amp, light delay
- **"ambient post-rock, ethereal"** → Shimmer reverb, chorus, delay, clean tones

### UI Controls

- **Text Input**: Describe your desired tone
- **Generate**: Create new preset from description
- **Variations**: Generate variations of the current preset
- **A/B Compare**: Switch between two presets
- **Save/Load**: Manage your favorite presets
- **Bypass**: Quick on/off toggle
- **Input/Output Gain**: Level control
- **Dry/Wet**: Blend processed and clean signal

## DSP Chain

The plugin processes audio through this signal chain:

1. **Input Gain** → Level adjustment
2. **Noise Gate** → Removes unwanted noise
3. **Compressor** → Dynamic control
4. **Drive** → Overdrive/distortion with oversampling
5. **Amp Simulator** → Tube amp modeling with tone stack
6. **Cabinet Simulator** → Impulse response convolution
7. **Chorus** → Modulation effect
8. **Delay** → Echo/slapback
9. **Reverb** → Spatial effects (room, plate, hall, shimmer)
10. **Equalizer** → Final tone shaping
11. **Output Gain** → Final level

## Technical Details

### AI Integration
- Uses OpenAI's Structured Outputs for reliable JSON generation
- Validates all parameters within safe audio ranges
- Runs on background thread to maintain real-time performance
- Fallback system for network issues

### Audio Processing
- 44.1kHz to 192kHz sample rate support
- Oversampling (2x/4x) for distortion algorithms
- Convolution-based cabinet simulation
- Lock-free parameter updates
- SIMD-optimized where possible

### Security
- API keys never stored in plugin binary
- Local server handles all external communication
- CORS protection for API endpoints
- Input validation and sanitization

## Development

### Project Structure
```
Source/
├── PluginProcessor.cpp/h     # Main plugin class
├── PluginEditor.cpp/h        # User interface
├── DSP/                      # Audio processing
│   ├── DSPChain.cpp/h       # Main processing chain
│   ├── Drive.cpp/h          # Overdrive/distortion
│   ├── AmpSimulator.cpp/h   # Amp modeling
│   └── ...                  # Other effects
├── Preset/                   # Preset management
│   ├── PresetSchema.cpp/h   # Data structures
│   └── PresetManager.cpp/h  # Save/load system
├── Network/                  # API communication
│   └── APIClient.cpp/h      # HTTP client
└── Utils/                    # Utilities
    └── ParameterSmoother.h  # Smooth parameter changes
```

### Adding New Effects
1. Create new DSP class in `Source/DSP/`
2. Add parameters to `PresetSchema.h`
3. Update JSON schema in server
4. Integrate into `DSPChain`
5. Add UI controls if needed

### Customizing AI Behavior
Edit the system prompt in `preset-server/server.js` to:
- Add new effect types
- Modify tone interpretation
- Adjust parameter ranges
- Add style-specific presets

## Troubleshooting

### Common Issues

**Plugin not loading:**
- Ensure Universal Binary is built (arm64 + x86_64)
- Check plugin is in correct directory (`~/Library/Audio/Plug-Ins/`)
- Verify code signing if distributing

**AI generation not working:**
- Check server is running on port 8787
- Verify OpenAI API key is valid
- Check network connectivity
- Look at server logs for errors

**Audio issues:**
- Use small buffer sizes (64-128 samples) for low latency
- Check sample rate compatibility
- Ensure audio interface is properly configured

**High CPU usage:**
- Reduce oversampling factor
- Use smaller cabinet IR files
- Disable unused effects

### Logs and Debugging

Server logs:
```bash
cd preset-server
npm start
# Watch console output for API calls and errors
```

Plugin debugging:
- Use Xcode debugger for development
- Check JUCE console output
- Monitor CPU usage in DAW

## License

MIT License - see LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## Roadmap

- [ ] Neural amp modeling integration
- [ ] User preset sharing
- [ ] MIDI control mapping
- [ ] Additional plugin formats (AAX)
- [ ] Mobile version (AUv3)
- [ ] Real-time collaboration features

## Support

For issues and questions:
- Check the troubleshooting section above
- Open an issue on GitHub
- Join our Discord community (link coming soon)

---

*Built with ❤️ using JUCE, OpenAI, and modern C++*
