#pragma once
#include "ErrorLogger.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class VertexShader {
public:
	bool Initialize(ID3D11Device* in_device, const std::wstring& in_shaderFilepath);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();

private:
	Microsoft::WRL::ComPtr< ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr< ID3D10Blob> shader_buffer;
	
};

class PixelShader {
public:
	bool Initialize(ID3D11Device* in_device, const std::wstring& in_shaderFilepath);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
private:
	Microsoft::WRL::ComPtr< ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr< ID3D10Blob> shader_buffer;
	
};