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

/* 
no options just lists the available MIDI devices
'i #' for input device test
'o #' for output device test
'm' for a manual testing mode
*/
int main(int argc, char *argv[]) {
	wcout << "Test program for the WinMIDIMsg module" << endl << endl;
	
	// poll MIDI devices
	wcout << PrintMIDIDevices() << endl;

	// pass arguments of the form '[i|o] [#]'
	if (argc > 2) {
		// open output device and send some messages
		if (argv[1][0] == 'o') {
			int outputDeviceNumber = argv[2][0] - '0';
			if (outputDeviceNumber >= GetMIDIOutputDeviceCount()) {
				wcout << outputDeviceNumber << " is not a valid output device index." << endl;
				return 0;
			}
			OutputMidiDevice outMidi(outputDeviceNumber);
			outMidi.debug = true;
			wcout << outMidi.SelfIdentify() << endl;
			outMidi.beep();
		}
		// open input device and listen a little bit	
		if (argv[1][0] == 'i') {
			int inputDeviceNumber = argv[2][0] - '0';
			if (inputDeviceNumber >= GetMIDIInputDeviceCount()) {
				wcout << inputDeviceNumber << " is not a valid input device index." << endl;
				return 0;
			}
			InputMidiDevice inMidi(inputDeviceNumber);
			inMidi.debug = true;
			wcout << inMidi.SelfIdentify() << endl;
		}
	}
}