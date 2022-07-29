#include "Engine.h"
#include "Graphics.h"
#include "ErrorLogger.h"


bool Engine::Initialize( HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	m_timer.Start();

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


	m_camera.SetPosition(0.0f, 0.0f, -5.0f);
	float aspectRatioOfCurrentWindow = static_cast<float>(render_window.GetWindowWidth()) / static_cast<float>(render_window.GetWindowHeight());
	m_camera.SetProjectionValues(90.0f, aspectRatioOfCurrentWindow, 0.1f, 1000.0f);

	return true;
}
bool Engine::ProcessMessages() 
{
	return render_window.ProcessMessages();
}
void Engine::Update()
{
	m_timer.Tick();
	float deltaTime = static_cast<float>(m_timer.GetMilisecondsElapsed());

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
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				m_camera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}
	}

	const float cameraSpeed = 0.1f;
	if (keyboard.KeyIsPressed('W'))
	{
		m_camera.AdjustPosition(m_camera.GetForwardVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		m_camera.AdjustPosition(m_camera.GetLeftVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		m_camera.AdjustPosition(m_camera.GetBackwardVector() * cameraSpeed);

	}
	if (keyboard.KeyIsPressed('D'))
	{
		m_camera.AdjustPosition(m_camera.GetRightVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('F'))
	{
		m_camera.AdjustPosition(0.0f, 0.0f, -0.5f);
		m_camera.AdjustRotation(0.0f, 0.0f, 0.0f);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		m_camera.AdjustPosition(m_camera.GetUpVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		m_camera.AdjustPosition(m_camera.GetUpVector() * cameraSpeed * -1);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		m_camera.AdjustPosition(m_camera.GetUpVector() * cameraSpeed);
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
