
#include "Engine.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Engine engine;
	if (engine.Initialize(hInstance, "Test1", "First Window Class", 800, 600))
	{
		while (engine.ProcessMessages() == true)
		{
			
			engine.RenderFrame();
			//engine.Update();
		}
	}
	return 0;
}