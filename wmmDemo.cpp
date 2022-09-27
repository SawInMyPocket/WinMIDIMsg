/**
	Classes for interfacing with the Windows MIDI API.
	@file MIDIMessaging.cpp
	@author Cole McKinney
	@version 2022-01-24
*/
#include "MIDIMessaging.h"

#include <iostream>
using std::wcout;
using std::wcin;
using std::endl;

int main(/*int argc, char *argv[]*/) {
	wcout << "Test program for the WinMIDIMsg module" << endl << endl;
	
	// poll MIDI devices
	wcout << PrintMIDIDevices() << endl;
	// open output device and send some messages?
	// open input device and listen a little bit?
}