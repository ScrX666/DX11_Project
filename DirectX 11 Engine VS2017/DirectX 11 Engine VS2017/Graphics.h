#pragma once
#include "AdapterReader.h"
#include <d3d11.h>
#include "ErrorLogger.h"
#include "Shaders.h"
#include "Vertex.h"
class Graphics
{
public :
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	bool InitializeDirectX(HWND hwnd, int width, int height);
	bool InitializeShader();
	bool InitializeScene();
private:
	

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr < IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr < ID3D11RenderTargetView> renderTargetView;

	
	VertexShader vertexShader;
	PixelShader pixelShader;
	

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> inputLayout;
};