#include "KeyboardClass.h"

KeyboardClass::KeyboardClass()
{
	for (int i = 0; i < 256; i++)
	{
		this->keyStates[i] = false; // Init all key states to off ( false )
	}
}

bool KeyboardClass::isPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

bool KeyboardClass::isKeyBufferEmpty()
{
	return this->keyBuffer.empty();
}

KeyboardEvent KeyboardClass::readKey()
{
	if (this->keyBuffer.empty())
	{
		return KeyboardEvent(); // empty keyboard event
	}
	// otherwise get and remove first key from the queue
	KeyboardEvent e = this->keyBuffer.front();
	this->keyBuffer.pop();
	return e;
}

void KeyboardClass::onKeyPress(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::onKeyRelease(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

bool KeyboardClass::isCharBufferEmpty()
{
	return this->charBuffer.empty();
}

unsigned char KeyboardClass::readChar()
{
	if (this->charBuffer.empty())
		return 0u;

	unsigned char e = this->charBuffer.front();
	this->charBuffer.pop();
	return e;
}

void KeyboardClass::onCharPress(const unsigned char ch)
{
	this->charBuffer.push(ch);
}

void KeyboardClass::setAutoRepeatKeys(bool repeat)
{
	autoRepeatKeys = repeat;
}

void KeyboardClass::setAutoRepeatChars(bool repeat)
{
	autoRepeatChars = repeat;
}

bool KeyboardClass::isAutoRepeatKeys()
{
	return autoRepeatKeys;
}

bool KeyboardClass::isAutoRepeatChars()
{
	return autoRepeatChars;
}
