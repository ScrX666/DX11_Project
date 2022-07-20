#include"MeshGeometryClass.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/MathFunctions.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <DirectXCollision.h>
#include <iostream>

typedef unsigned int uint;

using namespace Assimp;
using namespace DirectX;
using namespace std;


MeshGeometryClass::MeshGeometryClass(ID3D11Device* dev)
{
	m_pdev = dev;
}
MeshGeometryClass::MeshGeometryClass(const MeshGeometryClass& obj)
{

}


MeshGeometryClass::~MeshGeometryClass()
{

}
template<typename VertexTypeA>
bool  MeshGeometryClass::Mesh::SetVertices(ID3D11Device* device, const VertexTypeA* vertices, UINT count, UINT offset
	, MeshGeometryClass* g)
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


bool MeshGeometryClass::Mesh::SetIndices(ID3D11Device* device, const vector<DWORD> index, UINT offset, MeshGeometryClass* g)
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


//void MeshGeometryClass::Mesh::SetMeshTable(vector<Mesh>& SubsetTable)
//{
//	mMeshTable = SubsetTable;
//}

void MeshGeometryClass::Mesh::Draw(ID3D11DeviceContext* deviceContext,
	ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader,
	UINT meshOffset,
	MeshGeometryClass* g,
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

	this->cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->cb_vs_vertexshader->data.mat);
	this->cb_vs_vertexshader->ApplyChanges();

	deviceContext->IASetVertexBuffers(0, 1, g->m_pVerticesArray[meshOffset].GetAddressOf(),
		&g->mMeshTable[meshOffset].mVertexStride, &offset);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->VSSetShader(vShader, nullptr, 0);
	deviceContext->IASetIndexBuffer(g->m_pIndicesArray[meshOffset].Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->VSSetConstantBuffers(0, 1, cBufferFrequently);
	deviceContext->VSSetConstantBuffers(2, 1, cBufferOnResize);
	if (cBufferInstance)
	{
		deviceContext->VSSetConstantBuffers(3, 1, cBufferInstance);
	}

	deviceContext->PSSetShader(pShader, nullptr, 0);
	deviceContext->PSSetConstantBuffers(1, 1, cBufferRarely);
	//deviceContext->DrawIndexed(g->mMeshTable[meshOffset].FaceCount * 3, 0, 0);
	deviceContext->DrawIndexedInstanced(g->mMeshTable[meshOffset].FaceCount * 3, 100, 0, 0, 0);



}
void MeshGeometryClass::DrawAllMesh(ID3D11DeviceContext* deviceContext,
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
		mMeshTable[i].Draw(deviceContext, inputLayout, vShader, pShader, i, this,
			cBufferFrequently, cBufferRarely, cBufferOnResize, cBufferInstance);
	}
	//mMeshTable[1].Draw(deviceContext, inputLayout, vShader, pShader,1,this, cBufferFrequently, cBufferRarely, cBufferOnResize);


}

void MeshGeometryClass::LoadMesh(std::string filePath)
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

	//MeshGeometryClass::Mesh::VertexPosNormalTex verteics[] = {};

	for (int i = 0; i < scene->mNumMeshes; i++)
	{

		mMeshTable.push_back(Mesh());
		Mesh& mesh = mMeshTable.back();
		mesh.Id = scene->mNumMeshes;
		aiMesh* aimesh = scene->mMeshes[i];

		mesh.VertexCount = aimesh->mNumVertices;
		end += start;
		end += aimesh->mNumVertices - 1;



		for (DWORD j = 0; j < aimesh->mNumVertices; j++)
		{
			// vertex

			MeshGeometryClass::VertexPosNormalTex temp;
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

bool MeshGeometryClass::LoadMeshWithSkinnedAnimation(std::string filePath)
{
	m_aiScene = m_Importer.ReadFile(filePath, aiProcess_FlipUVs || aiProcess_Triangulate);
	if (!m_aiScene || m_aiScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_aiScene->mRootNode)
	{
		std::cout << "读取模型出现错误: " << m_Importer.GetErrorString() << std::endl;
		exit(-1);
	}
	//std::string rootPath = filePath.substr(0, filePath.find_last_of('/'));

	static DWORD start, end;
	static DWORD currentFace;
	start = end = currentFace = 0;
	aiMatrix4x4 tempGloabTransform = m_aiScene->mRootNode->mTransformation;
	tempGloabTransform = tempGloabTransform.Inverse();
	m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(tempGloabTransform);

	UINT maxNum = 0;
	for (int i = 0; i < m_aiScene->mNumMeshes; i++)
	{
		if (m_aiScene->mMeshes[i]->mNumVertices > maxNum)
		{
			maxNum = m_aiScene->mMeshes[i]->mNumVertices;
		}
	}
	mbones.resize(maxNum);

	//MeshGeometryClass::Mesh::VertexPosNormalTex verteics[] = {};
	for (int i = 0; i < m_aiScene->mNumMeshes; i++)
	{
		mMeshTable.emplace_back();
		Mesh& mesh = mMeshTable.back();
		mesh.Id = m_aiScene->mNumMeshes;

		aiMesh* aimesh = m_aiScene->mMeshes[i];

		//mbones.resize(aimesh->mNumVertices*aimesh->mNumBones);
		//mbones.resize(aimesh->mNumBones);


		end += start;
		end += aimesh->mNumVertices;

		vector<XMFLOAT4X4> boneOffset;
		vector<int> boneToParentIndex;


		MeshGeometryClass::SkinnedVertexIn temp;


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
			//uint t1 = aimesh->mFaces[j].mIndices[0];
			//uint t2 = aimesh->mFaces[j].mIndices[1];
			//uint t3 = aimesh->mFaces[j].mIndices[2];

			for (DWORD k = 0; k < face.mNumIndices; k++)
			{
				mesh.index.push_back(face.mIndices[k]);
				mesh.FaceCount++;
			}

		}

		uint AllVertexCount = 0;
		for (int vc = 0; vc < mMeshTable.size(); vc++)
		{
			AllVertexCount += mMeshTable[vc].VertexCount;
		}
		if (mbones.size() < AllVertexCount + aimesh->mNumVertices)
		{
			mbones.resize(AllVertexCount + aimesh->mNumVertices);
		}

		for (uint k = 0; k < aimesh->mNumBones; k++)
		{
			uint boneIndex = 0;
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


			for (uint l = 0; l < aimesh->mBones[k]->mNumWeights; l++)
			{

				uint VertexID = AllVertexCount + aimesh->mBones[k]->mWeights[l].mVertexId;
				//uint VertexID =  aimesh->mBones[k]->mWeights[l].mVertexId;
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

		aiNode* sceneRootNode = m_aiScene->mRootNode;
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

		m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(m_aiScene->mRootNode->mTransformation.Inverse());

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
aiNode* MeshGeometryClass::FindNodeToParent(aiNode* childNode, const std::string& destNodeName)
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
aiNode* MeshGeometryClass::FindNodeToChild(aiNode* parentNode, const std::string& destNodeName)
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


XMFLOAT4X4  MeshGeometryClass::AiMatrixToXMFLOAT4x4(aiMatrix4x4 ai)
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

aiMatrix4x4 MeshGeometryClass::XMFLOAT4x4ToAiMatrix(XMFLOAT4X4 xm)
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

void  MeshGeometryClass::BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& transforms)
{
	XMFLOAT4X4 identify;
	XMMATRIX temp = XMMatrixIdentity();
	XMStoreFloat4x4(&identify, temp);

	float TickPerSecond = m_aiScene->mAnimations[0]->mTicksPerSecond != 0 ? m_aiScene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TickPerSecond;
	float AnimationTime = fmod(TimeInTicks, m_aiScene->mAnimations[0]->mDuration);

	ReadNodeHierarchy(AnimationTime, m_aiNode[0], identify);

	transforms.resize(m_NumBone);

	for (uint i = 0; i < m_NumBone; i++) {
		m_calculatedBoneTransforms[i] = m_BoneInfo[i].finalTransformation;
	}

}

void MeshGeometryClass::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform)
{
	string nodeName = pNode->mName.data;
	const aiAnimation* pAnimation = m_aiScene->mAnimations[0];
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
		uint BoneIndex = m_BoneMapping[nodeName];
		XMMATRIX m_gloabInverseTransformM = XMLoadFloat4x4(&m_gloabInverseTransform);
		XMFLOAT4X4 gloabTransform = AiMatrixToXMFLOAT4x4(gloabTransformAI);
		XMMATRIX gloabTransform_m = XMLoadFloat4x4(&gloabTransform);
		XMMATRIX boneOffsetMatrixM = XMLoadFloat4x4(&m_BoneInfo[BoneIndex].boneOffsetMatrix);

		result = m_gloabInverseTransformM * gloabTransform_m * boneOffsetMatrixM;  //gloabToRoot * currentAnimaTransform * boneOffset
		XMStoreFloat4x4(&m_BoneInfo[BoneIndex].finalTransformation, result);
	}

	for (uint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], gloabTransform);
	}
}

void MeshGeometryClass::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	//assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

uint MeshGeometryClass::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	//assert(0);
	return 0;
}
aiNodeAnim* MeshGeometryClass::FindNodeAnim(const aiAnimation* Animation, string nodeName)
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
void MeshGeometryClass::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}
	//           deltaTime
	//key1-----------------------------key2
	//Factor =   key2 - key1 /DeltaTime  = 0.5
	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
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

void MeshGeometryClass::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

uint MeshGeometryClass::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}

uint MeshGeometryClass::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}
aiMatrix4x4  MeshGeometryClass::InitScaleTransform(float x, float y, float z)
{
	aiMatrix4x4  result;
	result.a1 = x;
	result.b2 = y;
	result.c3 = z;
	result.d4 = 1;
	return result;
}
aiMatrix4x4  MeshGeometryClass::InitTranslationTransform(float x, float y, float z)
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