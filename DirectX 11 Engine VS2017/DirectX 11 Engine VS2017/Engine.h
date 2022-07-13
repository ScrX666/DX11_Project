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
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX worldInvTranspose;
		DirectX::XMMATRIX boneTransform[100];
	};

	struct CBufferReFreashRarely
	{
		DirectX::XMINT4 g_InstanceMatricesWidth;
	};

	struct CBufferReFreshOnResize
	{
		DirectX::XMMATRIX proj;
	};

private:
	Timer timer;
	Camera camera;
	CBufferReFreashFrequently m_cBufferFrequently;

	ConstantBuffer<CB_VS_vertexshader> *m_pConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;
};