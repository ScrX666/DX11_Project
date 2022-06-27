#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>&device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	hr = device->CreateVertexShader(shader_buffer.Get()->GetBufferPointer(), shader_buffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return shader.Get();
}
ID3D10Blob* VertexShader::GetBuffer()
{
	return shader_buffer.Get();
}





//PixelShader
bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	hr = device->CreatePixelShader(shader_buffer.Get()->GetBufferPointer(), shader_buffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	return true;
}
ID3D11PixelShader* PixelShader::GetShader()
{
	return shader.Get();
}
ID3D10Blob* PixelShader::GetBuffer()
{
	return shader_buffer.Get();
}