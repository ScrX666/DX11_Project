#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include<wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride;
	UINT bufferSize = 0;

public:
	VertexBuffer(){}
	VertexBuffer(const VertexBuffer<T>& rhs)
	{
		this->buffer = rhs.buffer;
		this->bufferSize = rhs.bufferSize;
		this->stride = rhs.stride;
	}

	VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->bufferSize = a.bufferSize;
		this->stride = a.stride;
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
		return this->bufferSize;
	}
	const UINT Stride() const
	{
		return this->stride;
	}

	const UINT* StridePtr() const
	{
		return &this->stride;
	}
	HRESULT Initialize(ID3D11Device* device, T* data, UINT numVertices)
	{
		this->bufferSize = numVertices;
		this->stride = (sizeof(T));

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