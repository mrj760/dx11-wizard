

#pragma once
#include "KeyboardEvent.h"
#include <queue>

class KeyboardClass
{
public:
	
	KeyboardClass();

	bool isPressed(const unsigned char keycode);

	// if the key buffer(s) is/are not empty then we will read the keys
	bool isKeyBufferEmpty();
	KeyboardEvent readKey();

	// manages addition of keys to the key queue(s)
	void onKeyPress(const unsigned char key);
	void onKeyRelease(const unsigned char key);

	// if the char buffer(s) is/are not empty then we will read the chars
	bool isCharBufferEmpty();
	unsigned char readChar();

	// manages addition of chars to the char queue(s)
	void onCharPress(const unsigned char key);

	// dealing with whether we want auto repetition
	void setAutoRepeatKeys(bool repeat);
	void setAutoRepeatChars(bool repeat);
	bool isAutoRepeatKeys();
	bool isAutoRepeatChars();

private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];

	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

