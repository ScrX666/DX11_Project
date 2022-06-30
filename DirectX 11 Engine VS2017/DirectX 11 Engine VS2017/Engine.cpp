#include "Engine.h"
#include "Graphics.h"
#include "ErrorLogger.h"


bool Engine::Initialize( HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	bool InitializeWindowSuccessfully =	render_window.Initialize(hInstance, window_title, window_class, width, height);
	if (!InitializeWindowSuccessfully)
	{
		return false;
	}

	bool InitializeGraphicsSuccessfully = gfx.Graphics::Initialize(render_window.GetWindowHandle() , width, height);
	if(!InitializeGraphicsSuccessfully)
	{
		return false;
	}

	return true;
}
bool Engine::ProcessMessages() 
{
	return render_window.ProcessMessages();
}
void Engine::Update()
{
	
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvents kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
	}
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}
	}


	const float cameraSpeed = 0.02f;
	if (keyboard.KeyIsPressed('W'))
	{
		OutputDebugString(L" In\n");
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed);
		
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('Z'))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed, 0.0f);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed, 0.0f);
	}
}
void Engine::RenderFrame()
{
	gfx.RenderFrame();
}

Engine& Engine::Get()
{
	static Engine engineInstance;

	return engineInstance;
}
