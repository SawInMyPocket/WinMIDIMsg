cmake_minimum_required(VERSION 3.12)

project(WinMIDIMsg)

add_compile_options( "SHELL:/D _UNICODE /D UNICODE" /W4 /Zc:wchar_t)
add_library(winMIDIMsg MIDIMessaging.cpp)