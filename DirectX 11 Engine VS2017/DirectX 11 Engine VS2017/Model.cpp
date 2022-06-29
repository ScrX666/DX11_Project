#include "Model.h"

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->cb_vs_vertexshader = cb_vs_vertexshader;

	//VertexBuffer

	Vertex v[] =
	{
		// must be clockwise
		Vertex(-0.5f, -0.5f, -0.5f,	0.0f,1.0f),//left botAtom -[0]
		Vertex(-0.5f,  0.5f, -0.5f,	0.0f,0.0f),//top middle		-[1]
		Vertex( 0.5f,  0.5f, -0.5f,	1.0f,0.0f),//right middle	-[2]
		Vertex( 0.5f, -0.5f, -0.5f,	1.0f,1.0f),					//-[3]
		Vertex(-0.5f, -0.5f,  0.5f,	0.0f,1.0f),
		Vertex(-0.5f,  0.5f,  0.5f,	0.0f,0.0f),
		Vertex( 0.5f,  0.5f,  0.5f,	1.0f,0.0f),
		Vertex( 0.5f, -0.5f,  0.5f,	1.0f,1.0f),

	};

	HRESULT hr = this->vertexBuffer.Initialize(this->device, v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer. ");
		return false;
	}








	//IndexBuffer
	DWORD indices[] =
	{
		0,1,2,
		0,2,3,
		4,7,6,
		4,6,5,
		3,2,6,
		3,6,7,
		4,5,1,
		4,1,0,
		1,5,6,
		1,6,2,
		0,3,7,
		0,7,4,
	};
	hr = this->indexBuffer.Initialize(this->device, indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer ");
		return false;
	}

	this->UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void Model::Draw(const XMMATRIX& viewProjectionMatrix)
{
	this->cb_vs_vertexshader -> data.mat = this->worldMatrix * viewProjectionMatrix;
	this->cb_vs_vertexshader -> data.mat = XMMatrixTranspose(this->cb_vs_vertexshader->data.mat);
	this->cb_vs_vertexshader -> ApplyChanges();

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	UINT offset = 0;
	this->deviceContext->PSSetShaderResources(0, 1, &this->texture);//set to pixel shader
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(),DXGI_FORMAT:: DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0);
}

void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixIdentity();
}


