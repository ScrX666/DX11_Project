#pragma once
#include "ConstantBufferTypes.h"
#include<wrl/client.h>
#include"ErrorLogger.h"

template <class BufferStructType>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<BufferStructType>& rhs);

public:
	ConstantBuffer() = default;
	~ConstantBuffer() = default;

	BufferStructType m_data;

	ID3D11Buffer* Get()const
	{
		return m_buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return m_buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* in_device)
	{		
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(BufferStructType) + (16 - (sizeof(BufferStructType) % 16))); //must be 16 bits
		desc.StructureByteStride = 0;
		
		HRESULT hr = in_device->CreateBuffer(&desc, 0, m_buffer.GetAddressOf());
		return hr;
	}

	bool ApplyChanges(ID3D11DeviceContext* in_deviceContext)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = in_deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create constant buffer. ");
			return false;
		}
		CopyMemory(mappedResource.pData, &m_data, sizeof(BufferStructType));

		in_deviceContext->Unmap(m_buffer.Get(), 0);
		return true;
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};

