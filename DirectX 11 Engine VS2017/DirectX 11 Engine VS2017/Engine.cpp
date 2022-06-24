#include "Engine.h"
#include "Graphics.h"

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
void Engine::RenderFrame()
{
	gfx.RenderFrame();
}