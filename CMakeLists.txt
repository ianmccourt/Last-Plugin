cmake_minimum_required(VERSION 3.22)

project(AIGuitarPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
)
FetchContent_MakeAvailable(JUCE)

# Plugin target
juce_add_plugin(AIGuitarPlugin
    COMPANY_NAME "AIGuitarCo"
    BUNDLE_ID "com.aiguitarco.aiguitarplugin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Aigc
    PLUGIN_CODE Aigp
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "AI Guitar Plugin"
)

# Source files (minimal working version)
target_sources(AIGuitarPlugin
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/DSPChain.cpp
        Source/Preset/PresetSchema.cpp
        Source/Preset/PresetManager.cpp
        Source/Utils/ParameterSmoother.cpp
)

# Headers
target_include_directories(AIGuitarPlugin
    PRIVATE
        Source
)

# JUCE modules
target_link_libraries(AIGuitarPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compile definitions
target_compile_definitions(AIGuitarPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
    PRIVATE
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

# Compiler flags for macOS compatibility
if(APPLE)
    target_compile_options(AIGuitarPlugin PRIVATE
        -Wno-deprecated-declarations
        -Wno-unused-parameter
        -Wno-unused-variable
    )
endif()

# Universal binary for macOS
if(APPLE)
    set_target_properties(AIGuitarPlugin PROPERTIES
        OSX_ARCHITECTURES "arm64;x86_64"
    )
endif()
