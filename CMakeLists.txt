cmake_minimum_required(VERSION 3.16)
project(SynthesizerCPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(PortAudio REQUIRED)

set(SOURCES
    src/main.cpp
    src/synth.cpp
)

add_executable(synthesizer_cpp ${SOURCES})

target_include_directories(synthesizer_cpp PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(synthesizer_cpp PRIVATE PortAudio::PortAudio)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(synthesizer_cpp PRIVATE Threads::Threads m)
elseif(APPLE)
    find_library(COREAUDIO_FRAMEWORK CoreAudio)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    find_library(AUDIOUNIT_FRAMEWORK AudioUnit)
    find_library(CORESERVICES_FRAMEWORK CoreServices)
    target_link_libraries(cli_synth PRIVATE 
        ${COREAUDIO_FRAMEWORK} 
        ${AUDIOTOOLBOX_FRAMEWORK} 
        ${AUDIOUNIT_FRAMEWORK} 
        ${CORESERVICES_FRAMEWORK}
    )
endif()
