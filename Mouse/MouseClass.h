#pragma once
#include "MouseEvent.h"
#include <queue>

// Will store a queue of Mouse Events
class MouseClass
{

public:
	// Window Proc will receive these functions
	void leftPressed(const int &x, const int &y);
	void leftReleased(const int &x, const int &y);
	void rightPressed(const int &x, const int &y);
	void rightReleased(const int &x, const int &y);
	void middlePressed(const int &x, const int &y);
	void middleReleased(const int &x, const int &y);
	void wheelUp(const int &x, const int &y);
	void wheelDown(const int &x, const int &y);
	void moved(const int &x, const int &y);

	bool isLeftDown();
	bool isMiddleDown();
	bool isRightDown();

	int getPosX();
	int getPosY();
	MouseEvent::MousePoint getPos();

	bool isEventBufferEmpty();
	MouseEvent readEvent();

private:
	std::queue<MouseEvent> eventBuffer;
	bool leftDown = false,
		rightDown = false, 
		middleDown = false;
	int x = 0,
		y = 0;
};

