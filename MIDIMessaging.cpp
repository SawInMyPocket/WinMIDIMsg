#include "MIDIMessaging.h"

// Win32 MIDI documentation
// https://docs.microsoft.com/en-us/windows/win32/multimedia/musical-instrument-digital-interface--midi

#include <iostream>
#include <sstream>
using std::wstring;
using std::wcout;
using std::wcin;
using std::endl;

//TODO: add declaration in macro that works if the macro is called more than once
#define MMSYSERRCHECK(call) err = ##call; if (err != MMSYSERR_NOERROR) throw err;

wstring midiInParseError(MMRESULT errorCode) {
	WCHAR textBuffer[MAXERRORLENGTH];
	MMRESULT nestedError = midiInGetErrorText(errorCode, textBuffer, MAXERRORLENGTH);
	if (nestedError != MMSYSERR_NOERROR)
		midiInGetErrorText(errorCode, textBuffer, MAXERRORLENGTH);

	return wstring(textBuffer);
}

wstring midiOutParseError(MMRESULT errorCode) {
	WCHAR textBuffer[MAXERRORLENGTH];
	MMRESULT nestedError = midiOutGetErrorText(errorCode, textBuffer, MAXERRORLENGTH);
	if (nestedError != MMSYSERR_NOERROR)
		midiOutGetErrorText(errorCode, textBuffer, MAXERRORLENGTH);

	return wstring(textBuffer);
}

wstring PrintMIDIInputDeviceInfo(int index) {
	MIDIINCAPSA* inDeviceProperties = new MIDIINCAPSA();

	midiInGetDevCapsA(index, inDeviceProperties, sizeof(*inDeviceProperties));

	std::wstringstream printString;
	printString << "Input Dev " << index << " - " << inDeviceProperties->szPname;

	delete inDeviceProperties;
	return printString.str();
}

wstring PrintMIDIOutputDeviceInfo(int index) {
	MIDIOUTCAPS* outDeviceProperties = new MIDIOUTCAPS();

	midiOutGetDevCaps(index, outDeviceProperties, sizeof(*outDeviceProperties));

	std::wstringstream printString;
	printString << "Output Dev " << index << " - " << outDeviceProperties->szPname;

	delete outDeviceProperties;
	return printString.str();
}

wstring PrintMIDIDeviceCount() {
	UINT midiInCount = midiInGetNumDevs();
	UINT midiOutCount = midiOutGetNumDevs();

	std::wstringstream printString;

	printString << "Found " << midiInCount << " input devices\n";
	for (UINT i = 0; i < midiInCount; i++)
		printString << PrintMIDIInputDeviceInfo(i) << endl;
	printString << endl;
	printString << "Found " << midiOutCount << " output devices\n";
	for (UINT i = 0; i < midiOutCount; i++) 
		printString << PrintMIDIOutputDeviceInfo(i) << endl;

	return printString.str();
}

std::wostream& operator<<(std::wostream& os, const Midi::ShortMsg& msg) {
	//Ch## __MSG_NAME__; K### V###\0
	os << "Ch" << MIDI_MSG_CHNL(msg);

	const char* name;
	switch (MIDI_MSG_CODE(msg)) {
		using namespace Midi;
	case NOTE_OFF:
		name = "NOTE_OFF";
		break;
	case NOTE_ON:
		name = "NOTE_ON";
		break;
	case AFTERTOUCH:
		name = "AFTERTOUCH";
		break;
	case CTRL_CHANGE:
		name = "CTRL_CHANGE";
		break;
	case PROG_CHANGE:
		name = "PROG_CHANGE";
		break;
	case PITCH_BEND:
		name = "PITCH_BEND";
		break;
	case SYSTEM:
		name = "SYSTEM_MSG";
		break;
	default:
		name = "invalid";
	}

	os << " " << name << "; K" << msg.bData[1] << " V" << msg.bData[2]+'0';
	return os;
}

/////////////////////////////////////////////////////////////
// InputMidiDevice-specific definitions /////////////////////
void CALLBACK InputMidiDevice::InputMidiCallback(HMIDIIN deviceHandle, UINT msgCode, 
	DWORD_PTR userInstData, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	InputMidiDevice* instObj = (InputMidiDevice*)(userInstData);

	Midi::ShortMsg msg;
	msg.dwData = (DWORD)dwParam1;
	
	switch (msgCode) {
	case MIM_OPEN:
		instObj->OnDeviceOpen();
		break;
	case MIM_CLOSE:
		instObj->OnDeviceClose();
		break;
	case MIM_DATA:
	case MIM_LONGDATA:
		instObj->HandleMidiMessage(msg, dwParam2);
		break;
	case MIM_ERROR:
	case MIM_LONGERROR:
		instObj->HandleMidiError(msg, dwParam2);
		break;
	default:
		wcout << "Other message\n";
	}
}

void InputMidiDevice::HandleMidiMessage(Midi::ShortMsg msg, DWORD t_ms) {
	wcout << "T" << t_ms << "ms - " << msg << endl;
}

void InputMidiDevice::HandleMidiError(Midi::ShortMsg msg, DWORD t_ms) {
	wcout << "T" << t_ms << "ERR - " << msg << endl;
}
void InputMidiDevice::OnDeviceOpen() {
	wcout << "Input Device Opened" << endl;
}
void InputMidiDevice::OnDeviceClose() {
	wcout << "Input Device Closed" << endl;
}

wstring InputMidiDevice::SelfIdentify() {
	std::wstringstream str;
	str << "This is an Input MIDI device on input number [" << char(deviceId) + '0' << "]";
	return str.str();
}

void InputMidiDevice::StartMsgListening()
{
	midiInStart(deviceHandle);
}

void InputMidiDevice::StopMsgListening()
{
	midiInStop(deviceHandle);
}

InputMidiDevice::InputMidiDevice(UINT uDeviceID) {
	HMIDIIN localHandle;

	MMRESULT err = midiInOpen(&localHandle, uDeviceID, 
		(DWORD_PTR)InputMidiCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (err != MMSYSERR_NOERROR)
		throw err;

	deviceHandle = localHandle;
}
InputMidiDevice::~InputMidiDevice() {
	if (deviceHandle != NULL) {
		midiInClose(deviceHandle);
		midiInReset(deviceHandle);
		midiInClose(deviceHandle);
	}
}
/////////////////////////////////////////////////////////////
// OutputMidiDevice-specific definitions /////////////////////
void CALLBACK OutputMidiDevice::OutputMidiCallback(HMIDIOUT deviceHandle, UINT msgCode, DWORD_PTR userInstData, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	switch (msgCode) {
	case MOM_OPEN:
		wcout << "MOM_OPEN\n";
		break;
	case MOM_CLOSE:
		wcout << "MOM_CLOSE\n";
		break;
	case MOM_DONE:
		wcout << "MOM_ONE\n";
	}
}

void OutputMidiDevice::beep() {
	Midi::ShortMsg newMsg;

	newMsg.bData[0] = 0b10010000; // Note on, channel 1
	newMsg.bData[1] = 60; // middle C
	newMsg.bData[2] = 0b01111111; // maximum volume
	newMsg.bData[3] = 0; // always set to 0

	MMRESULT err;
	MMSYSERRCHECK(midiOutShortMsg(deviceHandle, newMsg.dwData));
}

void OutputMidiDevice::SendShortMsg(Midi::ShortMsg msg) {
	if (debug)
		wcout << "Sending - " << msg << endl;
	MMRESULT err;
	MMSYSERRCHECK(midiOutShortMsg(deviceHandle, msg.dwData));
}

wstring OutputMidiDevice::SelfIdentify() {
	std::wstringstream str; 
	str << "Output MIDI device [" << char(deviceId) + '0' << "]";
	str << "\t" << pDeviceCapabilities->szPname;
	return str.str();
}

OutputMidiDevice::OutputMidiDevice(UINT uDeviceID) {
	MMRESULT err;
	MMSYSERRCHECK( midiOutOpen(&deviceHandle, uDeviceID, (DWORD_PTR)OutputMidiCallback, reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION));

	deviceId = uDeviceID;
	pDeviceCapabilities = new MIDIOUTCAPSA();
	MMSYSERRCHECK( midiOutGetDevCapsA(deviceId, pDeviceCapabilities, sizeof(*pDeviceCapabilities)) );
};
OutputMidiDevice::~OutputMidiDevice() {
	if (deviceHandle != NULL)
	{
		midiOutReset(deviceHandle);
		midiOutClose(deviceHandle);
	}
	
	delete pDeviceCapabilities;
};


/////////////////////////////////////////////////////////////
// KeyboardDisplay-specific definitions /////////////////////
void KeyboardDisplay::HandleMidiMessage(Midi::ShortMsg msg, DWORD t_ms) {
	if (MIDI_MSG_CODE(msg) == Midi::NOTE_ON) {
		boardState[msg.bData[1]] = true;
	}
	else if (MIDI_MSG_CODE(msg) == Midi::NOTE_OFF) {
		boardState[msg.bData[1]] = false;
	}

	if (MIDI_MSG_CODE(msg) == Midi::NOTE_OFF || MIDI_MSG_CODE(msg) == Midi::NOTE_ON) {
		// there's a better way to do this looping
		system("cls");
		for (int l = 0; l < 4; l++) {
			for (int i = 0; i < 100; i++) {
				wcout.put(boardState[i] ? 'H' : '|');
			}
			wcout.put('\n');
		}
		wcout.flush();
	}
}

KeyboardDisplay::KeyboardDisplay(int index) : InputMidiDevice(index) {
	for (int i = 0; i < 100; i++)
		boardState[i] = false;
}