#include "Shaders.h"

bool VertexShader::Initialize(ID3D11Device* in_device, const std::wstring& in_shaderFilepath)
{
	HRESULT hr = D3DReadFileToBlob(in_shaderFilepath.c_str(), this->shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader";
		errorMsg += in_shaderFilepath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	hr = in_device->CreateVertexShader(shader_buffer.Get()->GetBufferPointer(), shader_buffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader";
		errorMsg += in_shaderFilepath;
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
bool PixelShader::Initialize(ID3D11Device* in_device, const std::wstring& in_shaderFilepath)
{
	HRESULT hr = D3DReadFileToBlob(in_shaderFilepath.c_str(), this->shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader";
		errorMsg += in_shaderFilepath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	hr = in_device->CreatePixelShader(shader_buffer.Get()->GetBufferPointer(), shader_buffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader";
		errorMsg += in_shaderFilepath;
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