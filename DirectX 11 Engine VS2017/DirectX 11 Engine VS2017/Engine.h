#pragma once
#include "RenderWindow.h"
#include "Graphics.h"
class Engine
{
public:
	RenderWindow render_window;
	Graphics gfx;
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
};