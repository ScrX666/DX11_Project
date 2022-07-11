#include "Model.h"
#include <map>
#include <string>
#include <vector>
#include"Graphics.h"

#define MAX_NUM_BONES_PER_VERTEX 4
using namespace DirectX;



struct SkinWeight
{
	unsigned int mVertexId;
	float mWeight;
};
struct SkinBone
{
	std::string mBoneName;
	std::vector<SkinWeight>mWeights;


};

//****************
 struct VertexBoneData
{
	UINT BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
	float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

	VertexBoneData()
	{
	}

	void AddBoneData(UINT BoneID, float Weight)
	{
		for (UINT i = 0; i < ARRAYSIZE(BoneIDs); i++) {
			if (Weights[i] == 0.0) {
				BoneIDs[i] = BoneID;
				Weights[i] = Weight;
				//printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, i);
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}
};


std::vector<VertexBoneData> vertex_to_bones;
std::vector<int> mesh_base_vertex;
std::map<std::string, UINT> bone_name_to_index_map;
//***************


aiNode* Model::FindNodeWithNameToChild(aiNode* parentNode, const std::string& destNodeName)
{
	if (parentNode != nullptr)
	{
		for (int child = 0; child < parentNode->mNumChildren; child++)
		{
			aiNode* childNode = parentNode->mChildren[child];
			std::string name;
			name = childNode->mName.data;
			if (name == destNodeName)
			{
				return childNode;
			}
			else
			{
				aiNode* node = FindNodeWithNameToChild(childNode, destNodeName);
				if (node != nullptr)
				{
					return node;
				}
			}
		}
	}

	return nullptr;
}

aiNode* Model::FindNodeWithNameToParent(aiNode* childNode, const std::string& destNodeName)
{
	if (childNode->mParent != nullptr)
	{
		std::string name;
		name = childNode->mParent->mName.data;
			return childNode->mParent;
	
	}

	return nullptr;
}




bool Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->cb_vs_vertexshader = cb_vs_vertexshader;


	if (!this->LoadModel(filePath))
	{
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
	this->deviceContext->PSSetShaderResources(0, 1, &this->texture);//set to pixel shader
	
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}

}
///***********
int get_bone_id(const aiBone* pBone)
{
	int bone_id = 0;
	std::string bone_name(pBone->mName.C_Str());

	if (bone_name_to_index_map.find(bone_name) == bone_name_to_index_map.end()) {
		// Allocate an index for a new bone
		bone_id = (int)bone_name_to_index_map.size();
		bone_name_to_index_map[bone_name] = bone_id;
	}
	else {
		bone_id = bone_name_to_index_map[bone_name];
	}

	return bone_id;
}

void Model::parse_single_bone(int mesh_index, const aiBone* pBone)
{
	int bone_id = get_bone_id(pBone);

	for (unsigned int i = 0; i < pBone->mNumWeights; i++)
	{
		//        if (i == 0) printf("\n");
		const aiVertexWeight& vw = pBone->mWeights[i];
		//          printf("       %d: vertex id %d weight %.2f\n", i, vw.mVertexId, vw.mWeight);

		UINT global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
		//        printf("Vertex id %d ", global_vertex_id);

		assert(global_vertex_id < vertex_to_bones.size());
		vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
	}

}

void Model::parse_mesh_bones(int mesh_index, const aiMesh* pMesh)
{
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
		parse_single_bone(mesh_index, pMesh->mBones[i]);
	}
}

void Model::parse_meshes(const aiScene* pScene)
{

	int total_vertices = 0;
	int total_indices = 0;
	int total_bones = 0;

	mesh_base_vertex.resize(pScene->mNumMeshes);

	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		 aiMesh* pMesh = pScene->mMeshes[i];
		int num_vertices = pMesh->mNumVertices;
		int num_indices = pMesh->mNumFaces * 3;
		int num_bones = pMesh->mNumBones;
		mesh_base_vertex[i] = total_vertices;
		total_vertices += num_vertices;
		total_indices += num_indices;
		total_bones += num_bones;

		vertex_to_bones.resize(total_vertices);
		
		if (pMesh->HasBones()) {
			parse_mesh_bones(i, pMesh);
		}
		
		Mesh mesh = ProcessMesh(pMesh, pScene);
		meshes.push_back(mesh);
	}


}


void Model::parse_node(const aiNode* pNode)
{

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		parse_node(pNode->mChildren[i]);
	}
}

void Model::parse_hierarchy(const aiScene* pScene)
{
	parse_node(pScene->mRootNode);
}
 


void Model::parse_scene(const aiScene* pScene)
{
	parse_meshes(pScene);

	parse_hierarchy(pScene);
}

//****
bool Model::LoadModel(const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
	{
		return false;
	}
	this->parse_scene(pScene);
	//this->ProcessNode(pScene->mRootNode, pScene);
	return true;
	
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
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

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;
	std::vector<INT> Bones;
	std::vector<SkinBone>boneArray;
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
		vertex.boneWeights.x = 0.000f;
		vertex.boneWeights.y = 0.000f;
		vertex.boneWeights.z = 0.000f;


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




void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixIdentity();
}



//**************************************************
//**************************************************
/*
void Model::LoadMeshWithSkinnedAnimation(const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
	{
		exit(-1);
	}

	static DWORD start, end;
	static DWORD currentFace;
	start = end = currentFace = 0;
	aiMatrix4x4 tempGloabTransform = pScene->mRootNode->mTransformation;
	tempGloabTransform = tempGloabTransform.Inverse();
	m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(tempGloabTransform);

	UINT maxNum = 0;
	for (int i = 0; i < pScene->mNumMeshes; i++)
	{
		if (pScene->mMeshes[i]->mNumVertices > maxNum)
		{
			maxNum = pScene->mMeshes[i]->mNumVertices;
		}
	}
	mbones.resize(maxNum);

	//MeshGeometryClass::Mesh::VertexPosNormalTex verteics[] = {};
	for (int i = 0; i < pScene->mNumMeshes; i++)
	{
		mMeshTable.emplace_back();
		Mesh& mesh = mMeshTable.back();
		mesh.Id = pScene->mNumMeshes;

		aiMesh* aimesh = pScene->mMeshes[i];

		//mbones.resize(aimesh->mNumVertices*aimesh->mNumBones);
		//mbones.resize(aimesh->mNumBones);


		end += start;
		end += aimesh->mNumVertices;

		vector<XMFLOAT4X4> boneOffset;
		vector<int> boneToParentIndex;
	}
}

	XMFLOAT4X4  Model::AiMatrixToXMFLOAT4x4(aiMatrix4x4 ai)
	{
		XMFLOAT4X4 xm;
		xm._11 = ai.a1;
		xm._12 = ai.a2;
		xm._13 = ai.a3;
		xm._14 = ai.a4;
		xm._21 = ai.b1;
		xm._22 = ai.b2;
		xm._23 = ai.b3;
		xm._24 = ai.b4;
		xm._31 = ai.c1;
		xm._32 = ai.c2;
		xm._33 = ai.c3;
		xm._34 = ai.c4;
		xm._41 = ai.d1;
		xm._42 = ai.d2;
		xm._43 = ai.d3;
		xm._44 = ai.d4;
		return xm;
	}

	aiMatrix4x4 Model::XMFLOAT4x4ToAiMatrix(XMFLOAT4X4 xm)
	{
		aiMatrix4x4 ai;
		ai.a1 = xm._11;
		ai.a2 = xm._12;
		ai.a3 = xm._13;
		ai.a4 = xm._14;
		ai.b1 = xm._21;
		ai.b2 = xm._22;
		ai.b3 = xm._23;
		ai.b4 = xm._24;
		ai.c1 = xm._31;
		ai.c2 = xm._32;
		ai.c3 = xm._33;
		ai.c4 = xm._34;
		ai.d1 = xm._41;
		ai.d2 = xm._42;
		ai.d3 = xm._43;
		ai.d4 = xm._44;
		return ai;
	}
	*/