#pragma once

#include <string>

// WIN32 MIDI API ///
#include <Windows.h>
#include <mmeapi.h>
#pragma comment(lib, "winmm.lib")
/////////////////////

#include "MIDIMsgDefs.h"

// functions for interpreting information provided by the Windows MIDI API
std::wstring midiInParseError(MMRESULT errorCode);
std::wstring midiOutParseError(MMRESULT errorCode);
std::wstring PrintMIDIInputDeviceInfo(int index);
std::wstring PrintMIDIDeviceCount(); 
std::wostream& operator<<(std::wostream& os, const Midi::ShortMsg& msg);

// base class for an object that can connect to a MIDI input device and handle the messages
// the default behaviour is to print the messages to the console
// a derived class should overwrite the 4 virtual functions below
// Programs that use this interface should pass a valid 0-indexed MIDI device ID
// The constructor will throw an MMRESULT code if it fails, use [midiInParseError] to read the associated message
class InputMidiDevice {
private:
	InputMidiDevice();
	HMIDIIN deviceHandle = 0;
	UINT deviceId;

	static void CALLBACK InputMidiCallback(HMIDIIN deviceHandle, UINT msgCode, 
		DWORD_PTR userInstData, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
protected:
	virtual void HandleMidiMessage(Midi::ShortMsg msg, DWORD t_ms);
	virtual void HandleMidiError(Midi::ShortMsg msg, DWORD t_ms);
	virtual void OnDeviceOpen();
	virtual void OnDeviceClose();
public:
	InputMidiDevice(UINT uDeviceID);
	~InputMidiDevice();

	bool debug = false;
	std::wstring SelfIdentify();

	void StartMsgListening();
	void StopMsgListening();
};

// base class for an object that can connect to a MIDI output device, pass messages, and handle the few messages it passes back
// an example of constructing a MIDI message is in the [beep] function
// Programs that use this class should pass a valid 0-indexed MIDI device ID
// The constructor will throw an MMRESULT code if it fails, use [midiOutParseError] to read the associated message
class OutputMidiDevice {
private:
	OutputMidiDevice();
	HMIDIOUT deviceHandle = 0;
	UINT deviceId;
	LPMIDIOUTCAPSA pDeviceCapabilities;

	static void CALLBACK OutputMidiCallback(HMIDIOUT deviceHandle, UINT msgCode, 
		DWORD_PTR userInstData, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
public:
	OutputMidiDevice(UINT uDeviceID);
	~OutputMidiDevice();

	bool debug = false;
	std::wstring SelfIdentify();

	void beep();
	void SendShortMsg(Midi::ShortMsg msg);
};

// an example class for deriving an InputMidiDevice
class KeyboardDisplay : public InputMidiDevice {

	bool boardState[100];

	void HandleMidiMessage(Midi::ShortMsg msg, DWORD t_ms);

public:
	KeyboardDisplay(int index);
};