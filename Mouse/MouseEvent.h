#pragma once



class MouseEvent
{
public: 
	struct MousePoint
	{
		int x, y;
	};
	enum class EventType
	{
		LPress,
		LRealease,
		RPress,
		RRelease,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RAW_MOVE,
		Invalid
	};

	MouseEvent();
	MouseEvent(const EventType type, const int x, const int y);

	bool isValid() const;
	EventType getType() const;
	MousePoint getPos() const;
	int getPosX() const;
	int getPosY() const;

private:
	EventType type;
	int x, y;
};

