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

public:
	static Engine& Get();


private:
	GameTimer gtime;
	Camera camera;
	ConstantBuffer <ContantBuffer_VS> m_cBufferFrequently;
	
};