cmake_minimum_required(VERSION 3.12)

project(WinMIDIMsg)

# a lot of the Windows MIDI interface needs unicode support for retrieving device names and descriptions
add_compile_options( "SHELL:/D _UNICODE /D UNICODE" /W4 /Zc:wchar_t)

# add build target for the MIDIMessaging library
add_library(winMIDIMsg MIDIMessaging.cpp)
target_include_directories(winMIDIMsg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")


# adds build target for demo/testing purposes
add_executable(WMM_Demo wmmDemo.cpp)
target_link_libraries(WMM_Demo PUBLIC winMIDIMsg)