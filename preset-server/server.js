import express from "express";
import cors from "cors";
import helmet from "helmet";
import dotenv from "dotenv";
import OpenAI from "openai";

dotenv.config();

const app = express();
const PORT = process.env.PORT || 8787;

// Security middleware
app.use(helmet());
app.use(cors({
  origin: ['http://localhost:3000', 'http://127.0.0.1:3000'], // Add your plugin's origins
  credentials: true
}));
app.use(express.json({ limit: '1mb' }));

// Initialize OpenAI client
const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY
});

// JSON Schema for OpenAI Structured Outputs - simplified
const presetSchema = {
  name: "TonePreset",
  strict: true,
  schema: {
    type: "object",
    required: ["name", "notes", "chain"],
    additionalProperties: false,
    properties: {
      name: { 
        type: "string"
      },
      notes: { 
        type: "string"
      },
      chain: {
        type: "array",
        items: {
          type: "object",
          required: ["block", "enabled"],
          additionalProperties: false,
          properties: {
            block: { 
              type: "string"
            },
            enabled: { 
              type: "boolean" 
            }
          }
        }
      }
    }
  }
};

// System prompt for the AI
const systemPrompt = `You are an expert guitar tone designer specializing in both traditional and experimental sounds. Convert user descriptions into JSON guitar presets that can range from classic tones to abstract soundscapes.

RULES:
- Output MUST follow the JSON schema exactly
- For traditional sounds: prefer musical, live-playable settings
- For abstract sounds: embrace extreme parameters and creative combinations
- Keep levels sane to avoid DC offset or aliasing
- Use oversample=2 or 4 for drive blocks when using high gain/distortion
- enabled: true/false for each block
- All parameter values must be within valid ranges

BLOCK TYPES & PARAMETERS:
noise_gate: threshold_db (-90 to 0), release_ms (5 to 500)
compressor: ratio (1 to 10), threshold_db (-60 to 0), attack_ms (0.1 to 50), release_ms (10 to 500), makeup_db (-12 to 12)
drive: type ("softclip"|"hardclip"|"tubescreamer"|"fuzz"), drive (0 to 1), tone (0 to 1), oversample (1|2|4)
amp: model ("clean_blackface"|"jangly_vox"|"brit_crunch"|"hi_gain"), gain (0 to 1), bass (0 to 1), mid (0 to 1), treble (0 to 1), presence (0 to 1), master (0 to 1)
cab: ir_name ("1x12_open"|"2x12_open"|"4x12_closed"), lo_cut_hz (20 to 200), hi_cut_hz (3000 to 12000)
chorus: rate_hz (0.05 to 5), depth (0 to 1), mix (0 to 1)
delay: time_ms (40 to 1200), feedback (0 to 0.95), mix (0 to 1)
reverb: algo ("room"|"plate"|"hall"|"shimmer"), pre_delay_ms (0 to 60), decay_s (0.2 to 12), damping (0 to 1), mix (0 to 1)
eq: low_shelf_hz (60 to 200), low_gain_db (-12 to 12), mid_hz (300 to 3000), mid_q (0.3 to 4), mid_gain_db (-12 to 12), high_shelf_hz (4000 to 10000), high_gain_db (-12 to 12)

TRADITIONAL TONE EXAMPLES:
- "ambient": Enable reverb (hall/shimmer), chorus, delay. Moderate drive, clean amp
- "grimy": Enable drive (fuzz/tubescreamer), brit_crunch amp, reduce hi_cut_hz
- "twangy": Enable clean_blackface amp, boost treble/presence, light compression
- "heavy": Enable hi_gain amp, drive (hardclip), 4x12_closed cab, gate
- "clean": Disable drive, use clean_blackface, light compression, plate reverb
- "shoegaze": Heavy reverb/delay, chorus, moderate drive, shimmer reverb
- "blues": tubescreamer drive, clean_blackface amp, light delay
- "metal": hi_gain amp, hardclip drive, noise_gate, 4x12_closed cab

ABSTRACT/EXPERIMENTAL TONE EXAMPLES:
- "ethereal": shimmer reverb with long decay (8-12s), high feedback delay (0.7-0.9), chorus, minimal drive
- "industrial": fuzz drive with oversample=4, hi_gain amp, short aggressive delay, harsh EQ cuts
- "underwater": heavy chorus (depth 0.8+), long delay with high feedback, muffled cab (hi_cut 4000-6000)
- "crystalline": clean signal, shimmer reverb, fast chorus (rate 2-5Hz), bright EQ, minimal compression
- "broken radio": fuzz drive, extreme EQ cuts/boosts, short delay with high feedback, compressed
- "cosmic": long shimmer reverb (decay 10-12s), slow deep chorus, clean amp, wide stereo delay
- "glitchy": fast chorus (3-5Hz), short delay with max feedback (0.9+), aggressive compression
- "haunting": hall reverb with long decay, slow chorus, clean/soft drive, dark EQ (treble cuts)
- "alien": fuzz + hi_gain amp, shimmer reverb, fast modulated delay, extreme EQ shaping
- "dreamy": soft drive, long plate/shimmer reverb, gentle chorus, warm EQ (mid boost, slight treble cut)

CREATIVE PARAMETER COMBINATIONS:
- High feedback delays (0.8-0.95) for infinite sustain effects
- Shimmer reverb + long decay (8-12s) for ambient washes  
- Fast chorus rates (3-5Hz) + high depth (0.7-1.0) for warbling effects
- Extreme EQ cuts/boosts for telephone/radio effects
- Fuzz + oversample=4 for maximum saturation
- Multiple time-based effects for complex textures
- Aggressive compression (ratio 8-10, fast attack) for pumping effects`;

// Health check endpoint
app.get('/health', (req, res) => {
  res.json({ 
    status: 'ok', 
    timestamp: new Date().toISOString(),
    version: '1.0.0'
  });
});

// Main preset generation endpoint
app.post('/preset', async (req, res) => {
  try {
    const { prompt } = req.body;
    
    if (!prompt || typeof prompt !== 'string') {
      return res.status(400).json({ 
        error: 'Missing or invalid prompt field' 
      });
    }
    
    // Sanitize and limit prompt length
    const sanitizedPrompt = prompt.slice(0, 280).trim();
    
    if (sanitizedPrompt.length === 0) {
      return res.status(400).json({ 
        error: 'Empty prompt' 
      });
    }
    
    console.log(`Generating preset for: "${sanitizedPrompt}"`);
    
    const userPrompt = `Description: "${sanitizedPrompt}"\n\nReturn only the JSON preset.`;
    
    // Call OpenAI with Structured Outputs
    const completion = await openai.chat.completions.create({
      model: "gpt-4o-mini", // Use the latest available model
      messages: [
        { role: "system", content: systemPrompt },
        { role: "user", content: userPrompt }
      ],
      response_format: { 
        type: "json_schema", 
        json_schema: presetSchema 
      },
      temperature: 0.7,
      max_tokens: 2000
    });
    
    const content = completion.choices[0]?.message?.content;
    
    if (!content) {
      throw new Error('No content received from OpenAI');
    }
    
    // Parse and validate the JSON
    let preset;
    try {
      preset = JSON.parse(content);
    } catch (parseError) {
      console.error('JSON parse error:', parseError);
      throw new Error('Invalid JSON response from AI');
    }
    
    // Basic validation
    if (!preset.name || !preset.chain || !Array.isArray(preset.chain)) {
      throw new Error('Invalid preset structure');
    }
    
    console.log(`Generated preset: "${preset.name}"`);
    
    res.json(preset);
    
  } catch (error) {
    console.error('Preset generation error:', error);
    
    let statusCode = 500;
    let errorMessage = 'Internal server error';
    
    if (error.code === 'insufficient_quota') {
      statusCode = 429;
      errorMessage = 'API quota exceeded';
    } else if (error.code === 'invalid_api_key') {
      statusCode = 401;
      errorMessage = 'Invalid API key';
    } else if (error.message) {
      errorMessage = error.message;
    }
    
    res.status(statusCode).json({ 
      error: errorMessage,
      timestamp: new Date().toISOString()
    });
  }
});

// Error handling middleware
app.use((error, req, res, next) => {
  console.error('Unhandled error:', error);
  res.status(500).json({ 
    error: 'Internal server error',
    timestamp: new Date().toISOString()
  });
});

// 404 handler
app.use('*', (req, res) => {
  res.status(404).json({ 
    error: 'Endpoint not found' 
  });
});

// Start server
app.listen(PORT, () => {
  console.log(`🎸 AI Guitar Preset Server running on http://localhost:${PORT}`);
  console.log(`📡 Health check: http://localhost:${PORT}/health`);
  console.log(`🎵 Preset endpoint: POST http://localhost:${PORT}/preset`);
  
  if (!process.env.OPENAI_API_KEY) {
    console.warn('⚠️  WARNING: OPENAI_API_KEY not set in environment variables');
  }
});

// Graceful shutdown
process.on('SIGINT', () => {
  console.log('\n🛑 Shutting down server...');
  process.exit(0);
});

process.on('SIGTERM', () => {
  console.log('\n🛑 Shutting down server...');
  process.exit(0);
});
