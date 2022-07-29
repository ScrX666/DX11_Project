#pragma once
#include "AdapterReader.h"
#include <d3d11.h>
#include "ErrorLogger.h"
#include "Shaders.h"
#include "Vertex.h"
#include <WICTextureLoader.h>
#include "Camera.h"
#include "Model.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include"ConstantBuffer.h"
#include "StaticMesh.h"
#include "SkeletonMesh.h"
#include "GameTimer.h"

class Graphics
{
public :
	bool Initialize(HWND hwnd, int width, int height);

	void RenderFrame();
	bool InitializeDirectX(HWND hwnd);
	bool InitializeScene();

	ID3D11Device* GetDirectXDevice() {	return m_device.Get(); }

	StaticMesh staMesh;
	SkeletonMesh skeletonMesh;

private:
	

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
	Microsoft::WRL::ComPtr < IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr < ID3D11RenderTargetView> m_renderTargetView;

protected:

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;


	int windowWidth = 0;
	int windowHeight = 0;
};