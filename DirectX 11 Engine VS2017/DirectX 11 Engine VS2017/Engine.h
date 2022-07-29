#pragma once

#include "WindowContainer.h"
#include "GameTimer.h"
class Engine :public WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

	Camera& GetCamera() { return m_camera; }
	GameTimer& GetGameTimer() { return m_timer; }

private:
	GameTimer m_timer;
	Camera m_camera;

public:
	static Engine& Get();
};