#pragma once

#include "WindowContainer.h"
#include "Timer.h"
class Engine :public WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

public:
	static Engine& Get();

	struct CBufferReFreashFrequently
	{
		DirectX::XMMATRIX boneTransform[100];
	};


	struct CBufferReFreshOnResize
	{
		DirectX::XMMATRIX proj;
	};

private:
	Timer timer;
	Camera camera;
	//ConstantBuffer <CBufferReFreashFrequently> m_cBufferFrequently;
	ConstantBuffer <ContantBuffer_VS> m_cBufferFrequently;
	
};