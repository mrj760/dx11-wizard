#include "MouseClass.h"

// stores a queue of mouse events


bool MouseClass::isLeftDown()
{
	return this->leftDown;
}

bool MouseClass::isMiddleDown()
{
	return this->middleDown;
}

bool MouseClass::isRightDown()
{
	return this->rightDown;
}

int MouseClass::getPosX()
{
	return this->x;
}

int MouseClass::getPosY()
{
	return this->y;
}

MouseEvent::MousePoint MouseClass::getPos()
{
	return { this->x, this->y };
}

bool MouseClass::isEventBufferEmpty()
{
	return this->eventBuffer.empty();
}

MouseEvent MouseClass::readEvent()
{
	if (this->eventBuffer.empty()) // return an invalid mouse event if trying to read from empty queue
		return MouseEvent();

	MouseEvent e = this->eventBuffer.front();
	this->eventBuffer.pop();
	return e;
}

void MouseClass::leftPressed(const int& x, const int& y)
{
	this->leftDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
}

void MouseClass::leftReleased(const int& x, const int& y)
{
	leftDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LRealease, x, y));
}

void MouseClass::rightPressed(const int& x, const int& y)
{
	rightDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void MouseClass::rightReleased(const int& x, const int& y)
{
	rightDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void MouseClass::middlePressed(const int& x, const int& y)
{
	middleDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void MouseClass::middleReleased(const int& x, const int& y)
{
	middleDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void MouseClass::wheelUp(const int& x, const int& y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void MouseClass::wheelDown(const int& x, const int& y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void MouseClass::moved(const int& x, const int& y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}
