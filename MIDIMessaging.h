/**
	Classes for interfacing with the Windows MIDI API. 
	@file MIDIMessaging.h
	@author Cole McKinney
	@version 2022-01-24
*/
#ifndef WINDMIDIMDG_MIDIMESSAGING_H_
#define WINDMIDIMDG_MIDIMESSAGING_H_

#include <string>
#include <ostream>

// WIN32 MIDI API ///
#include <Windows.h>
#include <mmeapi.h>
#pragma comment(lib, "winmm.lib")
/////////////////////

#include "MIDIMsgDefs.h"

/**
	@param errorCode the return value of a Windows MIDI function
	@return a wide-string of the error associated with the errorCode
*/
std::wstring midiInParseError(MMRESULT errorCode);

/**
	@param errorCode the return value of a Windows MIDI function
	@return a wide-string of the error associated with the errorCode
*/
std::wstring midiOutParseError(MMRESULT errorCode);

/**
	@param index 0-based index of an input MIDI device
	@return a wide string of the format "Input Dev # - [name of input device]"
*/
std::wstring PrintMIDIInputDeviceInfo(int index);

/**
	@param index 0-based index of an output MIDI device
	@return a wide string of the format "Output Dev # - [name of output device]"
*/
std::wstring PrintMIDIOutputDeviceInfo(int index);

/**
	@return a string listing all of the MIDI devices visible to Windows
*/
std::wstring PrintMIDIDeviceCount();

/**
    Overloaded operator for printing the ShortMsg type in the format:
      Ch## __MSG_NAME__; K### V###
      Channel number, the MIDI code, the associated piano key (Middle C = 60), the velocity (0-127)
    @param os the output stream to write to
    @param msg the MIDI message to be printed
    @return return the input stream; for chaining the operator multiple times
*/
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

#endif // WINDMIDIMDG_MIDIMESSAGING_H_