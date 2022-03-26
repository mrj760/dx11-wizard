#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.startTimer();

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
	float dt = timer.elapsedMS();
	timer.restartTimer();

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
			float sens = 0.008f;
			gfx.cam.adjustRotation(me.getPosY() * sens, me.getPosX() * sens, 0.0f);
		}
	}

	// /* TEMP */Rotate test model around the y axis slowly
	this->gfx.model.adjustRotation(0.f, 0.001f * dt, 0.f);

	/* CAMERA */
	const float camspeed = 0.002f;

	if (keyboard.isPressed('W'))
	{
		gfx.cam.adjustPosition(gfx.cam.getForward() * camspeed * dt);
	}
	if (keyboard.isPressed('A'))
	{
		gfx.cam.adjustPosition(gfx.cam.getLeft() * camspeed * dt);
	}
	if (keyboard.isPressed('S'))
	{
		gfx.cam.adjustPosition(gfx.cam.getBackward() * camspeed * dt);
	}
	if (keyboard.isPressed('D'))
	{
		gfx.cam.adjustPosition(gfx.cam.getRight() * camspeed * dt);
	}
	if (keyboard.isPressed(VK_SPACE))
	{
		gfx.cam.adjustPosition(gfx.cam.getUp() * camspeed * dt);
	}
	if (keyboard.isPressed('Z'))
	{
		gfx.cam.adjustPosition(gfx.cam.getDown() * camspeed * dt);
	}
}

void Engine::RenderFrame()
{
	gfx.renderFrame();
}

void Engine::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
