#include "RenderWindow.h"

bool RenderWindow::Initialize( HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	this->hInstance = hInstance;
	this->window_title = window_title;
	this->window_title_wide = StringConverter::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringConverter::StringToWide(this->window_class);
	this->width = width;
	this->height = height;
	//Rigister Window
	this->RegisterWindowClass();
	//Create Window
	this->handle = CreateWindowEx(
		0,
		this->window_class_wide.c_str(),
		this->window_title_wide.c_str(),
		WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 100,//X,Y Position
		this->width, this->height,
		nullptr, nullptr, this->hInstance, nullptr
	);
	//Log
	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "Create Window Failed : " + this->window_title);
		return false;
	}

	//Show Window
	ShowWindow(this->handle, SW_SHOW);

}
bool RenderWindow::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (PeekMessage(&msg, this->handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//check if the window closed
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL;
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return false;
		}
	}
	return true;
}

//Destroy Window
RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}
//custom window proc
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParm)
{
	switch (msg)
	{
	case WM_NCCREATE:
	{
		OutputDebugStringA("The window was created.\n");
		return DefWindowProc(hWnd, msg, wParam, lParm);
	}

	}
	return DefWindowProc(hWnd, msg, wParam, lParm);
}

void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = this->window_class_wide.c_str();
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);
}