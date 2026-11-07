cmake_minimum_required(VERSION 3.22)

project(VocalEpicPro VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(VocalEpicPro
    COMPANY_NAME "Louis Butron"
    COMPANY_COPYRIGHT "Copyright (c) 2026 Louis Butron. Todos los derechos reservados."
    DESCRIPTION "Vocal Epic Pro - Cadena vocal profesional"
    VERSION 2.0.0
    PLUGIN_MANUFACTURER_CODE Vzpa
    PLUGIN_CODE Vepr
    FORMATS VST3 Standalone
    PRODUCT_NAME "Vocal Epic Pro"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(VocalEpicPro PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_options(VocalEpicPro PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

target_compile_definitions(VocalEpicPro PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(VocalEpicPro
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
