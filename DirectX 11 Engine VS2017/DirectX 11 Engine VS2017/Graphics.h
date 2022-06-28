#pragma once
#include "AdapterReader.h"
#include <d3d11.h>
#include "ErrorLogger.h"
#include "Shaders.h"
#include "Vertex.h"
#include <WICTextureLoader.h>
#include "VertexBuffer.h"
#include "indexBuffer.h"
#include "ConstantBuffer.h"
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
	ConstantBuffer<CB_VS_vertexshader> constantBuffer;

	
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indicesBuffer;


	Microsoft::WRL::ComPtr <ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> rasterizerState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> myTexture;
};