#pragma once
#include "Vertex.h"
#include"VertexBuffer.h"
#include"indexBuffer.h"
#include"ConstantBuffer.h"
#include"Mesh.h"
using namespace DirectX;

class Model
{
public:
	
	aiNode* FindNodeWithNameToChild(aiNode* parentNode, const std::string& destNodeName);

	aiNode* FindNodeWithNameToParent(aiNode* childNode, const std::string& destNodeName);

	bool Initialize(const std::string &filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView*texture, ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(const XMMATRIX& viewProjectionMatrix);
private:
	std::vector<Mesh> meshes;
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void UpdateWorldMatrix();
	//**************
	void parse_meshes(const aiScene* pScene);
	void parse_single_bone(int mesh_index, const aiBone* pBone);
	void parse_mesh_bones(int mesh_index, const aiMesh* pMesh);
	void parse_node(const aiNode* pNode);
	void parse_hierarchy(const aiScene* pScene);
	void parse_scene(const aiScene* pScene);


	//**************
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;


	XMMATRIX worldMatrix = XMMatrixIdentity();
};