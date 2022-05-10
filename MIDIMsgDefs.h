/**
	Data types to use whenn interfacing with the Windows MIDI subsystem
	@file MIDIMsgDefs.h
	@author Cole McKinney
	@version 2022-01-05
*/

#pragma once

#define MIDI_MSG_CODE(m) (m.bData[0] & 0xF0)
#define MIDI_MSG_CHNL(m) (m.bData[0] & 0x0F)

namespace Midi {
	// for note messages, the 4 bytes are used like so -> [name code, key, velocity, 0]
	// the Windows MIDI interface passes it's messages around as DWORDs
	union ShortMsg {
		BYTE bData[4];
		DWORD dwData;
	};

	// a format for storing MIDI messages with their timestamps
	struct ShortMsgTime {
		ShortMsg msg;
		DWORD time;
	};

	// Shortcuts for the name codes needed in MIDI messages
	enum MsgName {
		NOTE_OFF = 0x80,
		NOTE_ON = 0x90,
		AFTERTOUCH = 0xA0,
		CTRL_CHANGE = 0xB0,
		PROG_CHANGE = 0xC0,
		PITCH_BEND = 0xE0,
		SYSTEM = 0xF0
	};

	// message for resetting the state of a MIDI output device
	const ShortMsg ALL_NOTES_OFF = {CTRL_CHANGE, 0x7B, 0, 0};
}

