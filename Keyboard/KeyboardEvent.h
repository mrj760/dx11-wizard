#pragma once
class KeyboardEvent
{
public:
	enum EventType
	{
		Press,
		Release,
		Invalid
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool isPress() const;
	bool isRelease() const;
	bool isValid() const;
	unsigned char getKeyCode() const;

private:
	EventType type;
	unsigned char key;
};