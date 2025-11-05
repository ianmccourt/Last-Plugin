#!/usr/bin/env node

// Simple test script to verify the preset server is working
// Usage: node test-preset.js "ambient, grimy, twangy"

import fetch from 'node-fetch';

const SERVER_URL = 'http://localhost:8787';

async function testPresetGeneration(description) {
    console.log(`🎸 Testing preset generation for: "${description}"`);
    console.log('=' .repeat(50));
    
    try {
        // Test health endpoint first
        console.log('🏥 Checking server health...');
        const healthResponse = await fetch(`${SERVER_URL}/health`);
        
        if (!healthResponse.ok) {
            throw new Error(`Health check failed: ${healthResponse.status}`);
        }
        
        const healthData = await healthResponse.json();
        console.log('✅ Server is healthy:', healthData.status);
        
        // Test preset generation
        console.log('\n🎵 Generating preset...');
        const startTime = Date.now();
        
        const response = await fetch(`${SERVER_URL}/preset`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                prompt: description
            })
        });
        
        const endTime = Date.now();
        const duration = endTime - startTime;
        
        if (!response.ok) {
            const errorData = await response.json();
            throw new Error(`Preset generation failed: ${errorData.error}`);
        }
        
        const preset = await response.json();
        
        console.log(`✅ Preset generated in ${duration}ms`);
        console.log('\n📋 Generated Preset:');
        console.log(`   Name: ${preset.name}`);
        console.log(`   Notes: ${preset.notes}`);
        console.log(`   Effects Chain: ${preset.chain.length} blocks`);
        
        console.log('\n🔧 Effect Blocks:');
        preset.chain.forEach((block, index) => {
            const status = block.enabled ? '🟢' : '🔴';
            console.log(`   ${index + 1}. ${status} ${block.block}`);
        });
        
        // Validate structure
        console.log('\n🔍 Validation:');
        const requiredFields = ['name', 'notes', 'chain'];
        const missingFields = requiredFields.filter(field => !preset[field]);
        
        if (missingFields.length > 0) {
            console.log(`❌ Missing fields: ${missingFields.join(', ')}`);
        } else {
            console.log('✅ All required fields present');
        }
        
        if (!Array.isArray(preset.chain)) {
            console.log('❌ Chain is not an array');
        } else if (preset.chain.length === 0) {
            console.log('❌ Chain is empty');
        } else {
            console.log(`✅ Chain has ${preset.chain.length} blocks`);
        }
        
        // Check for valid block types
        const validBlocks = ['noise_gate', 'compressor', 'drive', 'amp', 'cab', 'chorus', 'delay', 'reverb', 'eq'];
        const invalidBlocks = preset.chain.filter(block => !validBlocks.includes(block.block));
        
        if (invalidBlocks.length > 0) {
            console.log(`❌ Invalid block types: ${invalidBlocks.map(b => b.block).join(', ')}`);
        } else {
            console.log('✅ All block types are valid');
        }
        
        console.log('\n🎉 Test completed successfully!');
        
    } catch (error) {
        console.error('\n❌ Test failed:', error.message);
        
        if (error.code === 'ECONNREFUSED') {
            console.log('\n💡 Make sure the server is running:');
            console.log('   cd preset-server');
            console.log('   npm install');
            console.log('   npm start');
        }
        
        process.exit(1);
    }
}

// Get description from command line or use default
const description = process.argv[2] || 'ambient, grimy, twangy';

// Add node-fetch to package.json if not present
console.log('📦 Note: This test requires node-fetch. Install it with:');
console.log('   npm install node-fetch\n');

testPresetGeneration(description);
