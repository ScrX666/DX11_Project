#pragma once
#include "RenderWindow.h"
#include "Graphics.h"
#include"KeyboardClass.h"
#include "MouseClass.h"
class Engine
{
public:
	RenderWindow render_window;
	Graphics gfx;
	KeyboardClass keyboard;
	MouseClass mouse;
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
};