#pragma once

#include "ErrorLogger.h"

class RenderWindow
{
public:
	bool Initialize( HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	
	bool ProcessMessages();
	~RenderWindow();
	HWND GetWindowHandle() { return handle; }
	int GetWindowWidth() { return width; }
	int GetWindowHeight() { return height; }

private:
	void RegisterWindowClass();
	HWND handle = NULL;
	HINSTANCE hInstance = NULL;
	std::string window_title = "";
	std::wstring window_title_wide = L"";
	std::string window_class = "";
	std::wstring window_class_wide = L"";
	int width = 0;
	int height = 0;

};
