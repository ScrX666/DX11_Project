#pragma once
#include "AdapterReader.h"
#include <d3d11.h>
#include "ErrorLogger.h"
class Graphics
{
public :
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	bool InitializeDirectX(HWND hwnd, int width, int height);
private:
	

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr < IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr < ID3D11RenderTargetView> renderTargetView;
};