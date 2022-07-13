#include "Model.h"
#include <map>
#include <string>
#include <vector>
#include"Graphics.h"
#include <iostream>
#include <vector>
#include <DirectXCollision.h>
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

Model::Model(ID3D11Device* dev)
{
	m_pdev = dev;
}
Model::Model(const Model& obj)
{

}


Model::~Model()
{

}


//***************
template<typename VertexTypeA>
bool  Model::Mesh2::SetVertices(ID3D11Device* device, const VertexTypeA* vertices, UINT count, UINT offset
	, Model* g)
{
	mVertexStride = sizeof(VertexTypeA);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeA) * count;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	//vertexData.SysMemPitch = 0;
	//vertexData.SysMemSlicePitch = 0;
	HRESULT HR(device->CreateBuffer(&vertexBufferDesc, &vertexData, g->m_pVerticesArray[offset].GetAddressOf()));

	return true;
}


bool Model::Mesh2::SetIndices(ID3D11Device* device, const vector<DWORD> index, UINT offset, Model* g)
{
	D3D11_BUFFER_DESC  indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * index.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = index.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT HR(device->CreateBuffer(&indexBufferDesc, &indexData, g->m_pIndicesArray[offset].GetAddressOf()));

	return true;
}




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


aiNode* Model::FindNodeToParent(aiNode* childNode, const std::string& destNodeName)
{
	if (childNode->mParent != nullptr)
	{
		std::string name;
		name = childNode->mParent->mName.data;
		string::size_type idx, idx2;
		idx = name.find(destNodeName);
		idx2 = name.find(name);
		if (name != "" && (idx2 == string::npos || idx != string::npos))
		{
			return childNode->mParent;
		}
		else
		{
			aiNode* node = FindNodeToParent(childNode->mParent, destNodeName);
			if (node != nullptr)
			{
				return node;
			}
		}

	}
	return nullptr;
}
aiNode* Model::FindNodeToChild(aiNode* parentNode, const std::string& destNodeName)
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
				aiNode* node = FindNodeToChild(childNode, destNodeName);
				if (node != nullptr)
				{
					return node;
				}
			}
		}
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

void Model::Mesh2::Draw2(ID3D11DeviceContext* deviceContext,
	ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader,
	UINT meshOffset,
	Model* g,
	ID3D11Buffer* const* cBufferFrequently,
	ID3D11Buffer* const* cBufferRarely,
	ID3D11Buffer* const* cBufferOnResize,
	ID3D11Buffer* const* cBufferInstance
)
{

	UINT offset, indexOffset;
	offset = indexOffset = 0;
	//if (meshOffset > 0)
	//{
	//	for (int i = 1; i <= meshOffset; ++i)
	//	{
	//		offset += g->mMeshTable[i - 1].VertexCount;
	//		indexOffset += g->mMeshTable[i - 1].FaceCount*3;
	//	}
	//}


	deviceContext->IASetVertexBuffers(0, 1, g->m_pVerticesArray[meshOffset].GetAddressOf(),
		&g->mMeshTable[meshOffset].mVertexStride, &offset);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->VSSetShader(vShader, nullptr, 0);
	deviceContext->IASetIndexBuffer(g->m_pIndicesArray[meshOffset].Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->VSSetConstantBuffers(0, 1, cBufferFrequently);
	deviceContext->VSSetConstantBuffers(0, 1, cBufferOnResize);
	if (cBufferInstance)
	{
		deviceContext->VSSetConstantBuffers(0, 1, cBufferInstance);
	}

	deviceContext->PSSetShader(pShader, nullptr, 0);
	deviceContext->PSSetConstantBuffers(0, 1, cBufferRarely);
	//deviceContext->DrawIndexed(g->mMeshTable[meshOffset].FaceCount * 3, 0, 0);



}

void Model::DrawAllMesh(ID3D11DeviceContext* deviceContext,
	ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader,
	ID3D11Buffer* const* cBufferFrequently,
	ID3D11Buffer* const* cBufferRarely,
	ID3D11Buffer* const* cBufferOnResize,
	ID3D11Buffer* const* cBufferInstance)
{
	for (int i = 0; i < mMeshTable.size(); i++)
	{
		mMeshTable[i].Draw2(deviceContext, inputLayout, vShader, pShader, i, this,
			cBufferFrequently, cBufferRarely, cBufferOnResize, cBufferInstance);
	}
	//mMeshTable[1].Draw(deviceContext, inputLayout, vShader, pShader,1,this, cBufferFrequently, cBufferRarely, cBufferOnResize);


}

void Model::LoadMesh(std::string filePath)
{
	Importer import;
	const aiScene* scene = import.ReadFile(filePath, aiProcess_FlipUVs || aiProcess_Triangulate);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "读取模型出现错误: " << import.GetErrorString() << std::endl;
		exit(-1);
	}
	//std::string rootPath = filePath.substr(0, filePath.find_last_of('/'));

	static DWORD start, end;
	static DWORD currentFace;
	start = end = currentFace = 0;

	//Model::Mesh::VertexPosNormalTex verteics[] = {};

	for (int i = 0; i < scene->mNumMeshes; i++)
	{

		mMeshTable.push_back(Mesh2());
		Mesh2& mesh = mMeshTable.back();
		mesh.Id = scene->mNumMeshes;
		aiMesh* aimesh = scene->mMeshes[i];

		mesh.VertexCount = aimesh->mNumVertices;
		end += start;
		end += aimesh->mNumVertices - 1;



		for (DWORD j = 0; j < aimesh->mNumVertices; j++)
		{
			// vertex

			Model::VertexPosNormalTex temp;
			temp.pos.x = aimesh->mVertices[j].x;
			temp.pos.y = aimesh->mVertices[j].y;
			temp.pos.z = aimesh->mVertices[j].z;


			// normal
			temp.normal.x = aimesh->mNormals[j].x;
			temp.normal.y = aimesh->mNormals[j].y;
			temp.normal.z = aimesh->mNormals[j].z;


			// texcoord
			XMFLOAT2 texcoord;
			if (aimesh->mTextureCoords[0])
			{
				temp.uv.x = aimesh->mTextureCoords[0][j].x;
				temp.uv.y = aimesh->mTextureCoords[0][j].y;
			}
			mesh.verteice.push_back(temp);


		}

		for (DWORD j = 0; j < aimesh->mNumFaces; j++)
		{
			aiFace face = aimesh->mFaces[j];

			for (DWORD k = 0; k < face.mNumIndices; k++)
			{
				mesh.index.push_back(face.mIndices[k]);
				mesh.FaceCount++;
			}

		}

		mesh.FaceStart = currentFace;
		currentFace += aimesh->mNumFaces;
		mesh.FaceCount = aimesh->mNumFaces;
		mesh.VertexStart = start;
		end = start;

		mesh.SetVertices(m_pdev.Get(), mesh.verteice.data(), mesh.verteice.size(), i, this);
		mesh.SetIndices(m_pdev.Get(), mesh.index, i, this);
	}

}

bool Model::LoadMeshWithSkinnedAnimation(std::string filePath)
{
	m_aiscene = m_Importer.ReadFile(filePath, aiProcess_FlipUVs || aiProcess_Triangulate);
	if (!m_aiscene || m_aiscene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_aiscene->mRootNode)
	{
		std::cout << "读取模型出现错误: " << m_Importer.GetErrorString() << std::endl;
		exit(-1);
	}
	//std::string rootPath = filePath.substr(0, filePath.find_last_of('/'));

	static DWORD start, end;
	static DWORD currentFace;
	start = end = currentFace = 0;
	aiMatrix4x4 tempGloabTransform = m_aiscene->mRootNode->mTransformation;
	tempGloabTransform = tempGloabTransform.Inverse();
	m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(tempGloabTransform);

	UINT maxNum = 0;
	for (int i = 0; i < m_aiscene->mNumMeshes; i++)
	{
		if (m_aiscene->mMeshes[i]->mNumVertices > maxNum)
		{
			maxNum = m_aiscene->mMeshes[i]->mNumVertices;
		}
	}
	mbones.resize(maxNum);

	//Model::Mesh::VertexPosNormalTex verteics[] = {};
	for (int i = 0; i < m_aiscene->mNumMeshes; i++)
	{
		mMeshTable.emplace_back();
		Mesh2& mesh = mMeshTable.back();
		mesh.Id = m_aiscene->mNumMeshes;

		aiMesh* aimesh = m_aiscene->mMeshes[i];

		//mbones.resize(aimesh->mNumVertices*aimesh->mNumBones);
		//mbones.resize(aimesh->mNumBones);


		end += start;
		end += aimesh->mNumVertices;

		vector<XMFLOAT4X4> boneOffset;
		vector<int> boneToParentIndex;


		Model::SkinnedVertexIn temp;


		for (DWORD j = 0; j < aimesh->mNumVertices; j++)
		{
			// vertex
			temp.pos.x = aimesh->mVertices[j].x;
			temp.pos.y = aimesh->mVertices[j].y;
			temp.pos.z = aimesh->mVertices[j].z;

			// normal
			temp.normal.x = aimesh->mNormals[j].x;
			temp.normal.y = aimesh->mNormals[j].y;
			temp.normal.z = aimesh->mNormals[j].z;

			// texcoord
			XMFLOAT2 texcoord;
			if (aimesh->mTextureCoords[0])
			{
				temp.uv.x = aimesh->mTextureCoords[0][j].x;
				temp.uv.y = aimesh->mTextureCoords[0][j].y;
			}

			mesh.verteiceWithSkinnedAnimation.push_back(temp);


		}

		for (DWORD j = 0; j < aimesh->mNumFaces; j++)
		{
			aiFace face = aimesh->mFaces[j];
			//UINT t1 = aimesh->mFaces[j].mIndices[0];
			//UINT t2 = aimesh->mFaces[j].mIndices[1];
			//UINT t3 = aimesh->mFaces[j].mIndices[2];

			for (DWORD k = 0; k < face.mNumIndices; k++)
			{
				mesh.index.push_back(face.mIndices[k]);
				mesh.FaceCount++;
			}

		}

		UINT AllVertexCount = 0;
		for (int vc = 0; vc < mMeshTable.size(); vc++)
		{
			AllVertexCount += mMeshTable[vc].VertexCount;
		}
		if (mbones.size() < AllVertexCount + aimesh->mNumVertices)
		{
			mbones.resize(AllVertexCount + aimesh->mNumVertices);
		}

		for (UINT k = 0; k < aimesh->mNumBones; k++)
		{
			UINT boneIndex = 0;
			string boneName = aimesh->mBones[k]->mName.data;

			if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
			{
				boneIndex = m_NumBone;
				m_NumBone++;
				BoneInfo bi;
				m_BoneInfo.push_back(bi);
			}
			else
			{
				boneIndex = m_BoneMapping[boneName];
			}

			m_BoneMapping[boneName] = boneIndex;
			XMFLOAT4X4 tempMatrix = AiMatrixToXMFLOAT4x4(aimesh->mBones[k]->mOffsetMatrix);
			m_BoneInfo[boneIndex].boneOffsetMatrix = tempMatrix;
			m_BoneInfo[boneIndex].name = aimesh->mBones[k]->mName.data;


			for (UINT l = 0; l < aimesh->mBones[k]->mNumWeights; l++)
			{

				UINT VertexID = AllVertexCount + aimesh->mBones[k]->mWeights[l].mVertexId;
				//UINT VertexID =  aimesh->mBones[k]->mWeights[l].mVertexId;
				float Weight = aimesh->mBones[k]->mWeights[l].mWeight;
				mbones[VertexID].AddBoneData(boneIndex, Weight);

			}


			for (DWORD j = 0; j < aimesh->mNumVertices; j++)
			{
				XMUINT4 tempUint;
				tempUint.x = mbones[AllVertexCount + j].iDs[0];
				tempUint.y = mbones[AllVertexCount + j].iDs[1];
				tempUint.z = mbones[AllVertexCount + j].iDs[2];
				tempUint.w = mbones[AllVertexCount + j].iDs[3];
				XMFLOAT4 tempFloat;
				tempFloat.x = mbones[AllVertexCount + j].weights[0];
				tempFloat.y = mbones[AllVertexCount + j].weights[1];
				tempFloat.z = mbones[AllVertexCount + j].weights[2];
				tempFloat.w = mbones[AllVertexCount + j].weights[3];

				mesh.verteiceWithSkinnedAnimation[j].boneIndiecs = tempUint;
				mesh.verteiceWithSkinnedAnimation[j].weights = tempFloat;
			}


		}

		aiNode* sceneRootNode = m_aiscene->mRootNode;
		//m_aiNode.push_back(sceneRootNode);
		for (int f = 0; f < aimesh->mNumBones; f++)
		{
			aiNode* newNode = FindNodeToChild(sceneRootNode, aimesh->mBones[f]->mName.C_Str());
			int fatherCount = 0;

			aiMatrix4x4 preRotation, translation, rotation, scaling;
			std::string newNodeName = newNode->mName.data;
			aiNode* preRotNode = FindNodeToParent(newNode, newNodeName + "_$AssimpFbx$_PreRotation");
			if (preRotNode)
			{
				preRotation = preRotNode->mTransformation;
				fatherCount++;
			}

			aiNode* transNode = FindNodeToParent(newNode, newNodeName + "_$AssimpFbx$_Translation");
			if (transNode)
			{
				translation = transNode->mTransformation;
				fatherCount++;
			}

			aiNode* rotNode = FindNodeToParent(newNode, newNodeName + "_$AssimpFbx$_Rotation");
			if (rotNode)
			{
				rotation = rotNode->mTransformation;
				fatherCount++;
			}

			aiNode* scaleNode = FindNodeToParent(newNode, newNodeName + "_$AssimpFbx$_Scaling");
			if (scaleNode)
			{
				scaling = scaleNode->mTransformation;
				fatherCount++;
			}
			//find father
			aiNode* tempFather, * tempSon;
			tempFather = newNode->mParent;
			tempSon = newNode;

			if (tempFather)
			{
				for (int father = 0; father < fatherCount; father++)
				{
					tempSon = tempFather;
					tempFather = tempSon->mParent;
				}
				string tempName = "RootNode";
				string tempFatherName = tempFather->mName.C_Str();
				if (!strcmp(tempName.c_str(), tempFatherName.c_str()))
				{
					newNode->mParent = nullptr;
				}
				else
				{
					newNode->mParent = tempFather;
					for (int sonCount = 0; sonCount < newNode->mParent->mNumChildren; sonCount++)
					{

						string fatherName = newNode->mParent->mChildren[sonCount]->mName.C_Str();
						string::size_type xb = fatherName.find(newNode->mName.C_Str());
						if (xb != string::npos)
						{
							newNode->mParent->mChildren[sonCount] = newNode;
						}
					}
				}
			}


			newNode->mTransformation = translation * preRotation * rotation * scaling * newNode->mTransformation;

			bool findTarget = false;
			for (int tempNode = 0; tempNode < m_aiNode.size(); tempNode++)
			{
				if (newNode->mName == m_aiNode[tempNode]->mName)
				{
					findTarget = true;
					tempNode = m_aiNode.size();
				}
			}
			if (!findTarget)
			{
				m_aiNode.push_back(newNode);
			}
		}

		m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(m_aiscene->mRootNode->mTransformation.Inverse());

		mesh.VertexCount = aimesh->mNumVertices;
		mesh.FaceStart = currentFace;
		currentFace += aimesh->mNumFaces;
		mesh.FaceCount = aimesh->mNumFaces;
		mesh.VertexStart = start;
		start = end;


		mesh.SetVertices(m_pdev.Get(), mesh.verteiceWithSkinnedAnimation.data(), mesh.verteiceWithSkinnedAnimation.size(), i, this);
		mesh.SetIndices(m_pdev.Get(), mesh.index, i, this);

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

void  Model::BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& transforms)
{
	XMFLOAT4X4 identify;
	XMMATRIX temp = XMMatrixIdentity();
	XMStoreFloat4x4(&identify, temp);

	float TickPerSecond = (float)m_aiscene->mAnimations[0]->mTicksPerSecond != 0 ? m_aiscene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = (float)TimeInSeconds * TickPerSecond;
	float AnimationTime = (float)fmod(TimeInTicks, m_aiscene->mAnimations[0]->mDuration);

	ReadNodeHierarchy(AnimationTime, m_aiNode[0], identify);

	transforms.resize(m_NumBone);

	for (UINT i = 0; i < m_NumBone; i++) {
		Transforms[i] = m_BoneInfo[i].finalTransformation;
	}

}

void Model::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform)
{
	string nodeName = pNode->mName.data;
	const aiAnimation* pAnimation = m_aiscene->mAnimations[0];
	aiMatrix4x4 nodeTransformation = pNode->mTransformation;

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);

	if (pNodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4  ScalingM = InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		aiMatrix4x4  TranslationXM = InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		nodeTransformation = TranslationXM * RotationM * ScalingM;

	}
	aiMatrix4x4 parentTransformAi = XMFLOAT4x4ToAiMatrix(ParentTransform);
	aiMatrix4x4 gloabTransformAI = parentTransformAi * nodeTransformation;
	XMFLOAT4X4 gloabTransform = AiMatrixToXMFLOAT4x4(gloabTransformAI);
	XMMATRIX result;
	if (m_BoneMapping.find(nodeName) != m_BoneMapping.end())
	{
		UINT BoneIndex = m_BoneMapping[nodeName];
		XMMATRIX m_gloabInverseTransformM = XMLoadFloat4x4(&m_gloabInverseTransform);
		XMFLOAT4X4 gloabTransform = AiMatrixToXMFLOAT4x4(gloabTransformAI);
		XMMATRIX gloabTransform_m = XMLoadFloat4x4(&gloabTransform);
		XMMATRIX boneOffsetMatrixM = XMLoadFloat4x4(&m_BoneInfo[BoneIndex].boneOffsetMatrix);

		result = m_gloabInverseTransformM * gloabTransform_m * boneOffsetMatrixM;  //gloabToRoot * currentAnimaTransform * boneOffset
		XMStoreFloat4x4(&m_BoneInfo[BoneIndex].finalTransformation, result);
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], gloabTransform);
	}
}

void Model::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);
	//assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

UINT Model::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	//assert(0);
	return 0;
}

aiNodeAnim* Model::FindNodeAnim(const aiAnimation* Animation, string nodeName)
{
	for (int i = 0; i < Animation->mNumChannels; i++)
	{
		if (Animation->mChannels[i]->mNodeName.data == nodeName)
		{
			return Animation->mChannels[i];
		}
	}
	return nullptr;
}

void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}
	//           deltaTime
	//key1-----------------------------key2
	//Factor =   key2 - key1 /DeltaTime  = 0.5
	UINT ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	UINT NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f );
	//assert(Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Model::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	UINT PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	UINT NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

UINT Model::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}

UINT Model::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}

aiMatrix4x4  Model::InitScaleTransform(float x, float y, float z)
{
	aiMatrix4x4  result;
	result.a1 = x;
	result.b2 = y;
	result.c3 = z;
	result.d4 = 1;
	return result;
}

aiMatrix4x4  Model::InitTranslationTransform(float x, float y, float z)
{
	aiMatrix4x4  result;
	result.a4 = x;
	result.b4 = y;
	result.c4 = z;
	result.d4 = 1;
	result.a1 = 1;
	result.b2 = 1;
	result.c3 = 1;
	return result;
}