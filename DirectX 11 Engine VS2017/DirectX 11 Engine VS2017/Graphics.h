#pragma once
#include "AdapterReader.h"
#include <d3d11.h>
#include "ErrorLogger.h"
#include "Shaders.h"
#include "Vertex.h"
#include <WICTextureLoader.h>
#include "Camera.h"
#include "Model.h"
class Graphics
{
public :
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShader();
	bool InitializeScene();
	Camera camera;
private:
	

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr < IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr < ID3D11RenderTargetView> renderTargetView;

	
	VertexShader vertexShader;
	PixelShader pixelShader;

protected:
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;

	
	Model model;


	Microsoft::WRL::ComPtr <ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> rasterizerState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> myTexture;

	int windowWidth = 0;
	int windowHeight = 0;

};