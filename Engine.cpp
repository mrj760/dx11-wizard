#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	//keyboard.setAutoRepeatChars(true);
	return this->render_window.Initialize(this, hInstance, window_title, window_class, width, height);
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	while (/*Char buffer NOT empty*/!keyboard.isCharBufferEmpty())
	{
		unsigned char ch = keyboard.readChar();
	}

	while (/*Key buffer NOT empty*/!keyboard.isKeyBufferEmpty())
	{
		KeyboardEvent kbe = keyboard.readKey();
		unsigned char key = kbe.getKeyCode();
		
	}

	while (/*Mouse event buffer is NOT empty*/!mouse.isEventBufferEmpty())
	{
		MouseEvent me = mouse.readEvent();
		// Move Test
		/*std::string outmsg = 
			"X: " + std::to_string(me.getPosX()) +
			"Y: " + std::to_string(me.getPosY()) + "\n";
		OutputDebugStringA(outmsg.c_str());*/

		// Scroll Test
		/*if (me.getType() == MouseEvent::EventType::WheelUp)
		{
			OutputDebugStringA("Scroll up\n");
		}
		else if (me.getType() == MouseEvent::EventType::WheelDown)
		{
			OutputDebugStringA("Scroll Down\n");
		}*/

		// Raw input Test
		if (me.getType() == MouseEvent::EventType::RAW_MOVE)
		{
			std::string outmsg =
				"X: " + std::to_string(me.getPosX()) +
				"Y: " + std::to_string(me.getPosY()) + "\n";
			OutputDebugStringA(outmsg.c_str());
		}
	}
}
