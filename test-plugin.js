// Simple test script to verify the plugin is working
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = 3001;

app.use(cors());
app.use(express.json());

// Test endpoint that returns a mock preset
app.post('/generate-preset', (req, res) => {
    console.log('Received request:', req.body);
    
    // Return a simple mock preset
    const mockPreset = {
        name: `Test Preset - ${req.body.description || 'Default'}`,
        description: req.body.description || 'Test preset',
        chain: [
            {
                type: "noise_gate",
                enabled: true,
                threshold: -40.0,
                ratio: 10.0,
                attack: 0.5,
                release: 100.0
            },
            {
                type: "drive",
                enabled: true,
                type: "soft",
                gain: 0.3,
                tone: 0.6,
                oversample: 2
            },
            {
                type: "reverb",
                enabled: true,
                type: "hall",
                size: 0.7,
                decay: 0.5,
                damping: 0.3,
                width: 1.0,
                wet: 0.3,
                dry: 0.7
            }
        ]
    };
    
    res.json(mockPreset);
});

app.get('/health', (req, res) => {
    res.json({ status: 'ok', message: 'Test server running' });
});

app.listen(PORT, () => {
    console.log(`🎸 Test AI Guitar Plugin Server running on http://localhost:${PORT}`);
    console.log('Ready to generate mock presets!');
});
