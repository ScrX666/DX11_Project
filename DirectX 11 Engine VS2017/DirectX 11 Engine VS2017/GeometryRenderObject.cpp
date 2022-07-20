//#include "GeometryRenderObject.h"
//
//
//void GeometryRenderObject::SetupVSConstantBuffer()
//{
//	for (size_t i = 0; i < m_vs_constantBufferTableDescs.size(); i++)
//	{
//		auto packedConstantBufferSize = CalcPackedConstantBufferByteSize(m_vs_constantBufferTableDescs[i].second);
//
//		ComPtr<ID3D11Buffer> constantBuffer;
//		D3D11_BUFFER_DESC desc;
//		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
//		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		desc.Usage = D3D11_USAGE_DYNAMIC;
//		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		desc.MiscFlags = NULL;
//		desc.ByteWidth = static_cast<UINT>(packedConstantBufferSize);
//		desc.StructureByteStride = 0;
//
//		HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
//		return hr;
//
//		m_vs_constantBuffers.push_back(constantBuffer);
//	}
//}