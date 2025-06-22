cmake_minimum_required(VERSION 3.15)
project(MultiThreadGraphRender VERSION 0.0.1)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
include(cmake/deps.cmake)
include_directories(${JUCE_SOURCE_DIR}/modules ${json_SOURCE_DIR}/include src/deps/dart ${vst24_SOURCE_DIR})

set(SOURCES
        src/multi_thread_graph_render.h
        src/multi_thread_render_graph.cpp
)

add_library(multi_thread_graph_render ${SOURCES})

add_executable(example example/main.cpp)

target_link_libraries(multi_thread_graph_render
        PRIVATE
        juce::juce_core
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_graphics
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
        juce::juce_audio_devices
        juce::juce_dsp
        juce::juce_opengl
        juce::juce_audio_utils)

target_link_libraries(example
        PUBLIC
        multi_thread_graph_render
        PRIVATE

        juce::juce_core
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_graphics
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
        juce::juce_audio_devices
        juce::juce_dsp
        juce::juce_opengl
        juce::juce_audio_utils)