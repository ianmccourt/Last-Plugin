#!/bin/bash

# AI Guitar Plugin Build Script
# This script sets up and builds the plugin for macOS

set -e  # Exit on any error

echo "🎸 AI Guitar Plugin Build Script"
echo "================================="

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ This script is designed for macOS only"
    exit 1
fi

# Check for required tools
echo "🔍 Checking build requirements..."

if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found. Please install CMake 3.22 or later"
    echo "   You can install it via: brew install cmake"
    exit 1
fi

if ! command -v xcodebuild &> /dev/null; then
    echo "❌ Xcode not found. Please install Xcode from the App Store"
    exit 1
fi

echo "✅ Build requirements satisfied"

# Create build directory
echo "📁 Setting up build directory..."
mkdir -p build
cd build

# Generate build files
echo "🔧 Generating build files..."
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Check if user wants to build or just generate
if [[ "$1" == "--generate-only" ]]; then
    echo "✅ Build files generated successfully!"
    echo "📂 Use 'ninja' command in build directory to build manually"
    exit 0
fi

# Build the project
echo "🔨 Building plugin (this may take a few minutes)..."
ninja

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "✅ Build completed successfully!"
    echo ""
    echo "📦 Plugin files created:"
    find . -name "*.component" -o -name "*.vst3" -o -name "*.app" 2>/dev/null | while read file; do
        echo "   📄 $file"
    done
    echo ""
    echo "🚀 Next steps:"
    echo "   1. Start the preset server: cd preset-server && npm install && npm start"
    echo "   2. Install plugins to your system directories (or use JUCE's auto-copy)"
    echo "   3. Launch your DAW and enjoy AI-generated guitar tones!"
else
    echo "❌ Build failed. Check the output above for errors."
    exit 1
fi
