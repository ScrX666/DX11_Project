#pragma once

#include "RenderWindow.h"
#include "Graphics.h"
#include "MouseClass.h"
#include "KeyboardClass.h"

class WindowContainer
{
public:
	WindowContainer();

public:
	LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	RenderWindow render_window;
	KeyboardClass keyboard;
	MouseClass mouse;
	Graphics gfx;
private:
};
