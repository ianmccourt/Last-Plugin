#!/bin/bash

# AI Guitar Plugin - Build Verification Script
# Checks that all components are ready for testing

set -e

echo "🎸 AI Guitar Plugin - Build Verification"
echo "========================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✅ $2${NC}"
    else
        echo -e "${RED}❌ $2${NC}"
        return 1
    fi
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

# Check prerequisites
echo -e "\n${BLUE}🔍 Checking Prerequisites...${NC}"

# Check Xcode
if command -v xcodebuild &> /dev/null; then
    print_status 0 "Xcode is installed"
else
    print_status 1 "Xcode is not installed"
    echo "   Install Xcode from the App Store"
    exit 1
fi

# Check CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    print_status 0 "CMake is installed (version $CMAKE_VERSION)"
else
    print_status 1 "CMake is not installed"
    echo "   Install with: brew install cmake"
    exit 1
fi

# Check Node.js
if command -v node &> /dev/null; then
    NODE_VERSION=$(node --version)
    print_status 0 "Node.js is installed ($NODE_VERSION)"
else
    print_status 1 "Node.js is not installed"
    echo "   Install with: brew install node"
    exit 1
fi

# Check source files
echo -e "\n${BLUE}📁 Checking Source Files...${NC}"

REQUIRED_FILES=(
    "CMakeLists.txt"
    "Source/PluginProcessor.h"
    "Source/PluginProcessor.cpp"
    "Source/PluginEditor.h"
    "Source/PluginEditor.cpp"
    "Source/DSP/DSPChain.h"
    "Source/DSP/Drive.h"
    "Source/DSP/Drive.cpp"
    "Source/DSP/Reverb.h"
    "Source/DSP/Reverb.cpp"
    "Source/Preset/PresetSchema.h"
    "Source/Preset/PresetGenerationManager.h"
    "Source/Preset/PresetGenerationManager.cpp"
    "Source/Network/APIClient.h"
    "Source/Network/APIClient.cpp"
    "Source/Utils/ParameterSmoother.h"
    "Source/Utils/ParameterSmoother.cpp"
    "Source/Utils/ThreadSafeQueue.h"
    "Source/UI/ModernLookAndFeel.h"
    "Source/UI/ModernLookAndFeel.cpp"
    "Source/UI/LevelMeterComponent.h"
    "Source/UI/LevelMeterComponent.cpp"
)

MISSING_FILES=()
for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$file" ]; then
        print_status 0 "$file exists"
    else
        print_status 1 "$file is missing"
        MISSING_FILES+=("$file")
    fi
done

if [ ${#MISSING_FILES[@]} -gt 0 ]; then
    echo -e "\n${RED}❌ Missing required files. Cannot proceed with build.${NC}"
    exit 1
fi

# Check server files
echo -e "\n${BLUE}🌐 Checking Server Files...${NC}"

SERVER_FILES=(
    "preset-server/package.json"
    "preset-server/server.js"
    "preset-server/env.example"
)

for file in "${SERVER_FILES[@]}"; do
    if [ -f "$file" ]; then
        print_status 0 "$file exists"
    else
        print_status 1 "$file is missing"
        exit 1
    fi
done

# Check if .env exists
if [ -f "preset-server/.env" ]; then
    print_status 0 "Server .env file exists"
    
    # Check if API key is set
    if grep -q "OPENAI_API_KEY=your_" preset-server/.env; then
        print_warning "OpenAI API key needs to be configured in preset-server/.env"
    elif grep -q "OPENAI_API_KEY=" preset-server/.env; then
        print_status 0 "OpenAI API key is configured"
    else
        print_warning "OpenAI API key not found in .env file"
    fi
else
    print_warning "Server .env file not found. Copy from env.example and add your API key."
fi

# Test server dependencies
echo -e "\n${BLUE}📦 Checking Server Dependencies...${NC}"

if [ -d "preset-server/node_modules" ]; then
    print_status 0 "Server dependencies are installed"
else
    print_warning "Server dependencies not installed. Run: cd preset-server && npm install"
fi

# Generate build files
echo -e "\n${BLUE}🔧 Generating Build Files...${NC}"

if [ ! -d "build" ]; then
    mkdir build
    print_info "Created build directory"
fi

cd build

print_info "Running CMake..."
if cmake .. -G Xcode > cmake_output.log 2>&1; then
    print_status 0 "CMake generation successful"
else
    print_status 1 "CMake generation failed"
    echo "Check build/cmake_output.log for details"
    exit 1
fi

# Check if Xcode project was created
if [ -f "AIGuitarPlugin.xcodeproj/project.pbxproj" ]; then
    print_status 0 "Xcode project created successfully"
else
    print_status 1 "Xcode project creation failed"
    exit 1
fi

cd ..

# Test build (optional - can be slow)
echo -e "\n${BLUE}🏗️  Testing Build (Optional)...${NC}"
read -p "Do you want to test build the plugin now? This may take 2-5 minutes. (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_info "Starting test build..."
    
    cd build
    if xcodebuild -project AIGuitarPlugin.xcodeproj -scheme AIGuitarPlugin_All -configuration Release > build_output.log 2>&1; then
        print_status 0 "Test build successful!"
        
        # Check for output files
        if [ -f "AIGuitarPlugin_artefacts/Release/AIGuitarPlugin.component/Contents/MacOS/AIGuitarPlugin" ]; then
            print_status 0 "AU plugin built successfully"
        fi
        
        if [ -f "AIGuitarPlugin_artefacts/Release/AIGuitarPlugin.vst3/Contents/MacOS/AIGuitarPlugin" ]; then
            print_status 0 "VST3 plugin built successfully"
        fi
        
        if [ -f "AIGuitarPlugin_artefacts/Release/AIGuitarPlugin.app/Contents/MacOS/AIGuitarPlugin" ]; then
            print_status 0 "Standalone app built successfully"
        fi
        
    else
        print_status 1 "Test build failed"
        echo "Check build/build_output.log for details"
        cd ..
        exit 1
    fi
    cd ..
else
    print_info "Skipping test build. You can build later with: ./build.sh"
fi

# Final summary
echo -e "\n${GREEN}🎉 Build Verification Complete!${NC}"
echo -e "\n${BLUE}Next Steps:${NC}"
echo "1. Configure OpenAI API key in preset-server/.env"
echo "2. Install server dependencies: cd preset-server && npm install"
echo "3. Start the server: cd preset-server && npm start"
echo "4. Build the plugin: ./build.sh"
echo "5. Follow the Waveform testing guide: WAVEFORM_TESTING_GUIDE.md"

echo -e "\n${BLUE}Quick Start Commands:${NC}"
echo "# Set up server:"
echo "cd preset-server"
echo "npm install"
echo "cp env.example .env"
echo "# Edit .env and add your OpenAI API key"
echo "npm start"
echo ""
echo "# In another terminal, build plugin:"
echo "./build.sh"

echo -e "\n${GREEN}✅ Ready for testing with Waveform!${NC}"
