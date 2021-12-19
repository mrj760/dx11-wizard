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
}
