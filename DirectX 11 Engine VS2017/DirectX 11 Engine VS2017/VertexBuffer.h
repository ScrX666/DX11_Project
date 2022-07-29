#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include<wrl/client.h>
#include <memory>

template<class BufferStructType>
class VertexBuffer
{

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT m_stride;
	UINT m_bufferSize = 0;

public:
	VertexBuffer(){}
	VertexBuffer(const VertexBuffer<BufferStructType>& rhs)
	{
		this->buffer = rhs.buffer;
		this->m_bufferSize = rhs.m_bufferSize;
		this->m_stride = rhs.m_stride;
	}

	VertexBuffer<BufferStructType>& operator=(const VertexBuffer<BufferStructType>& a)
	{
		this->buffer = a.buffer;
		this->m_bufferSize = a.m_bufferSize;
		this->m_stride = a.m_stride;
		return *this;
	}


	ID3D11Buffer* Get() const
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}
	UINT BufferSize() const
	{
		return this->m_bufferSize;
	}
	const UINT Stride() const
	{
		return this->m_stride;
	}

	const UINT* StridePtr() const
	{
		return &this->m_stride;
	}
	HRESULT Initialize(ID3D11Device* device, BufferStructType* data, UINT numVertices)
	{
		this->m_bufferSize = numVertices;
		this->m_stride = (sizeof(BufferStructType));

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;


	}
};

#endif