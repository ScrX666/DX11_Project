#pragma once
#include "Vertex.h"
#include"VertexBuffer.h"
#include"indexBuffer.h"
#include"ConstantBuffer.h"
#include"Mesh.h"
using namespace DirectX;

class StaticMesh
{
public:

	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<ContantBuffer_VS>* cb_vs_vertexshader);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetConstantBuffer(const XMMATRIX& viewProjectionMatrix);
	void Draw();
private:
	std::vector<Mesh> meshes;
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ConstantBuffer<ContantBuffer_VS>* cb_vs_vertexshader;
	ID3D11ShaderResourceView* texture;

};