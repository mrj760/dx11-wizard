#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	//keyboard.setAutoRepeatChars(true);
	if /* we fail to initialize our render window or our graphics, return false*/
	(!this->render_window.Initialize
		(this, hInstance, window_title, window_class, width, height) ||
	(!gfx.initialize
		(this->render_window.getHWND(), width, height)))
	{
		return false;
	}

	/* initializations successful */
	return true;
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

		/* CAMERA */
		if (mouse.isRightDown() && me.getType() == MouseEvent::EventType::RAW_MOVE)
		{
			// adjust camera by change in mouse direction * sensitivity
			gfx.cam.adjustRotation(me.getPosY() * 0.01f, me.getPosX() * 0.01f, 0.0f);
		}
	}

	/* CAMERA */
	const float camspeed = 0.02f;

	if (keyboard.isPressed('W'))
	{
		gfx.cam.adjustPosition(gfx.cam.getForward() * camspeed);
	}
	if (keyboard.isPressed('A'))
	{
		gfx.cam.adjustPosition(gfx.cam.getLeft() * camspeed);
	}
	if (keyboard.isPressed('S'))
	{
		gfx.cam.adjustPosition(gfx.cam.getBackward() * camspeed);
	}
	if (keyboard.isPressed('D'))
	{
		gfx.cam.adjustPosition(gfx.cam.getRight() * camspeed);
	}
	if (keyboard.isPressed(VK_SPACE))
	{
		gfx.cam.adjustPosition(gfx.cam.getUp() * camspeed);
	}
	if (keyboard.isPressed('Z'))
	{
		gfx.cam.adjustPosition(gfx.cam.getDown() * camspeed);
	}
}

void Engine::RenderFrame()
{
	gfx.renderFrame();
}
