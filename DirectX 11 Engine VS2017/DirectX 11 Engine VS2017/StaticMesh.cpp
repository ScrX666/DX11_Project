#include "StaticMesh.h"

bool StaticMesh::Initialize(const std::string& filePath, ID3D11Device* in_device, ID3D11DeviceContext* in_deviceContext, ID3D11ShaderResourceView* in_texture, ConstantBuffer<ContantBuffer_VS>* in_cb_vs_vertexshader)
{
	device = in_device;
	deviceContext = in_deviceContext;
	texture = in_texture;
	cb_vs_vertexshader = in_cb_vs_vertexshader;


	if (!this->LoadModel(filePath))
	{
		return false;
	}


	return true;
}

void StaticMesh::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void StaticMesh::SetConstantBuffer(const XMMATRIX& viewProjectionMatrix)
{
	this->cb_vs_vertexshader->data.worldMat = XMMatrixIdentity();
	this->cb_vs_vertexshader->data.VPMat =  viewProjectionMatrix;
	this->cb_vs_vertexshader->data.VPMat = XMMatrixTranspose(this->cb_vs_vertexshader->data.VPMat);
	this->cb_vs_vertexshader->ApplyChanges();

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->deviceContext->PSSetShaderResources(0, 1, &this->texture);//set to pixel shader

	

}
void StaticMesh::Draw()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}

bool StaticMesh::LoadModel(const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
	{
		return false;
	}
	this->ProcessNode(pScene->mRootNode, pScene);
	return true;
}

void StaticMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
	}

}

Mesh StaticMesh::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(this->device, this->deviceContext, vertices, indices);
}



