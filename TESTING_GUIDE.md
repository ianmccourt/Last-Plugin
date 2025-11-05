# AI Guitar Plugin - Testing Guide

## Testing Setup for PreSonus AudioBox & Waveform Free

This guide will help you test the AI Guitar Plugin with your specific hardware and software setup.

### Hardware Setup

#### PreSonus AudioBox Configuration
1. **Connect your guitar** to the AudioBox instrument input (usually Input 1)
2. **Set input gain** appropriately - aim for signal peaks around -12dB to -6dB
3. **Enable direct monitoring** if you want to hear your dry signal while recording
4. **Connect headphones/monitors** to the AudioBox outputs

#### Recommended AudioBox Settings
- **Sample Rate**: 44.1kHz or 48kHz (start with 44.1kHz)
- **Buffer Size**: 128 samples (good balance of latency and stability)
- **Input Gain**: Adjust so your guitar signal peaks around -12dB
- **Direct Monitor**: OFF (so you only hear the processed plugin signal)

### Software Setup

#### Waveform Free Configuration

1. **Audio Device Setup**:
   - Go to `Settings > Audio`
   - Select PreSonus AudioBox as your audio device
   - Set sample rate to match your AudioBox (44.1kHz recommended)
   - Set buffer size to 128 samples initially

2. **Create New Project**:
   - File > New Project
   - Choose appropriate template or start blank

3. **Add Audio Track**:
   - Insert > Audio Track
   - Set input to your guitar input (usually Input 1)
   - Arm the track for recording

4. **Insert AI Guitar Plugin**:
   - Click the track's FX button
   - Add the AI Guitar Plugin (AU or VST3)
   - Enable input monitoring to hear the effect in real-time

### Testing Workflow

#### Phase 1: Basic Functionality Test

1. **Start the AI Server**:
   ```bash
   cd preset-server
   npm install
   cp env.example .env
   # Edit .env and add your OpenAI API key
   npm start
   ```
   
2. **Verify Server is Running**:
   - Open http://localhost:8787/health in browser
   - Should show: `{"status":"ok",...}`

3. **Test Plugin Loading**:
   - Load plugin in Waveform
   - Verify UI appears correctly
   - Check that audio passes through (bypass mode)

#### Phase 2: AI Generation Tests

Test these descriptions to verify the AI is generating appropriate presets:

**Traditional Sounds**:
- "clean jazz tone" → Should generate clean amp, light compression, plate reverb
- "blues overdrive" → Should generate tube screamer, clean amp, light delay
- "heavy metal" → Should generate high gain amp, noise gate, hard clipping
- "ambient clean" → Should generate reverb, delay, chorus, clean signal

**Abstract Sounds** (your specialty):
- "ethereal floating" → Should generate shimmer reverb, long delays, minimal drive
- "underwater dreams" → Should generate heavy chorus, muffled EQ, long reverb
- "broken radio transmission" → Should generate extreme EQ, compression, fuzz
- "crystalline sparkle" → Should generate bright EQ, fast chorus, shimmer reverb
- "industrial chaos" → Should generate maximum fuzz, aggressive compression
- "cosmic voyage" → Should generate infinite reverb, slow modulation

#### Phase 3: Audio Quality Tests

1. **Latency Test**:
   - Play guitar and listen for delay between input and output
   - If too much latency, reduce buffer size to 64 samples
   - If audio glitches, increase buffer size to 256 samples

2. **CPU Usage Test**:
   - Monitor CPU usage in Waveform
   - Generate complex presets (shimmer reverb + delay + chorus)
   - Ensure CPU stays below 50% for stable performance

3. **Audio Quality Test**:
   - Test with different guitar types (single coil, humbucker, acoustic)
   - Verify no digital artifacts or unwanted noise
   - Check that extreme presets don't cause clipping

#### Phase 4: Creative Testing

**Abstract Sound Exploration**:
1. Try these experimental descriptions:
   - "glitchy robotic voice"
   - "haunted cathedral"
   - "alien communication"
   - "melting synthesizer"
   - "digital decay"
   - "frozen in time"
   - "underwater cave"
   - "space transmission"

2. **Parameter Extremes**:
   - Generate presets with maximum reverb decay (12 seconds)
   - Test high feedback delays (0.9+)
   - Try fast chorus rates (3-5Hz) with high depth
   - Experiment with extreme EQ cuts and boosts

3. **Layering Test**:
   - Record a guitar part with one AI preset
   - Duplicate the track and generate a complementary preset
   - Test how different AI-generated sounds layer together

### Troubleshooting

#### Common Issues & Solutions

**Plugin Won't Load**:
- Ensure you built Universal Binary (arm64 + x86_64)
- Check plugin is in correct directory
- Verify Waveform is scanning the right plugin folders

**No AI Generation**:
- Check server is running on port 8787
- Verify OpenAI API key is valid and has credits
- Check network connectivity
- Look at server console for error messages

**Audio Issues**:
- **Crackling/Glitches**: Increase buffer size
- **Too Much Latency**: Decrease buffer size
- **No Sound**: Check input routing and plugin bypass
- **Distorted Output**: Check input gain levels

**High CPU Usage**:
- Disable oversampling in drive blocks
- Use shorter reverb decay times
- Reduce number of active effects

#### PreSonus AudioBox Specific Tips

1. **Driver Issues**:
   - Ensure latest AudioBox drivers are installed
   - Use USB 2.0 port (USB 3.0 can cause issues)
   - Avoid USB hubs if possible

2. **Monitoring Setup**:
   - Turn off direct monitoring when using the plugin
   - Use headphones to avoid feedback
   - Set appropriate output levels

3. **Sample Rate Matching**:
   - Keep AudioBox and Waveform at same sample rate
   - 44.1kHz is most compatible
   - Higher rates (96kHz) may cause issues with some plugins

### Performance Optimization

#### For Best Results:

1. **Audio Settings**:
   - Buffer Size: 128 samples (adjust as needed)
   - Sample Rate: 44.1kHz
   - Bit Depth: 24-bit

2. **System Optimization**:
   - Close unnecessary applications
   - Disable WiFi if not needed for AI calls
   - Use wired internet connection for AI server

3. **Plugin Usage**:
   - Start with simpler presets, build complexity gradually
   - Use bypass to A/B compare processed vs. dry signal
   - Save favorite AI-generated presets for later use

### Testing Checklist

- [ ] AudioBox drivers installed and working
- [ ] Waveform recognizes AudioBox as audio device
- [ ] AI server starts and responds to health check
- [ ] Plugin loads in Waveform without errors
- [ ] Audio passes through plugin (bypass mode)
- [ ] AI generation works with simple descriptions
- [ ] Complex abstract descriptions generate appropriate presets
- [ ] No audio artifacts or excessive CPU usage
- [ ] Latency is acceptable for real-time playing
- [ ] Presets can be saved and recalled

### Next Steps

Once basic testing is complete:

1. **Record Test Tracks**: Create recordings with various AI-generated presets
2. **Document Favorite Descriptions**: Keep a list of text descriptions that generate great sounds
3. **Experiment with Combinations**: Try layering multiple AI-generated tracks
4. **Share Results**: Consider sharing interesting presets with the community

### Support

If you encounter issues:

1. Check the main README.md troubleshooting section
2. Look at server console logs for API errors
3. Monitor Waveform's plugin scanner for loading issues
4. Test with the standalone plugin version to isolate DAW issues

Remember: The goal is to create a wide range of sounds from traditional to highly abstract. Don't be afraid to try unusual descriptions - the AI is designed to handle experimental requests!
