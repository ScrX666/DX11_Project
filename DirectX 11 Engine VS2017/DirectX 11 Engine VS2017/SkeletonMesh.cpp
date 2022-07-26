#include "SkeletonMesh.h"
#include <stdint.h>

aiNode* SkeletonMesh::FindNodeToParent(aiNode* childNode, const std::string& destNodeName)
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

aiNode* SkeletonMesh::FindNodeToChild(aiNode* parentNode, const std::string& destNodeName)
{
	if (parentNode != nullptr)
	{
		for (unsigned int child = 0; child < parentNode->mNumChildren; child++)
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
/*
template<typename VertexTypeA>
bool  SkeletonMesh::SetVertices(ID3D11Device* device, const VertexTypeA* in_vertices, UINT in_count, UINT in_offset, SkeletonMesh* pBuffer)
{
	m_vertexStride = sizeof(VertexTypeA);
	
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeA) * in_count;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = in_vertices;


	if (device == nullptr)
	{
		//TODO: add some alerts here.
		ErrorLogger::Log(("The device is null"));
		return false;
	}

	HRESULT HR(device->CreateBuffer(&vertexBufferDesc, &vertexData, pBuffer->m_vertexBuffers[in_offset].GetAddressOf()));

	return true;
}

bool SkeletonMesh::SetIndices(ID3D11Device* device, const vector<DWORD> index, UINT offset, SkeletonMesh* pBuffer)
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

	HRESULT HR(device->CreateBuffer(&indexBufferDesc, &indexData, pBuffer->m_indexBuffers[offset].GetAddressOf()));

	return true;
}
*/
bool SkeletonMesh::CreateVertexBuffer(ID3D11Device* device)
{
	if (device == nullptr)
	{
		ErrorLogger::Log(("The device is null"));
		return false;
	}

	for (MeshSection& meshSection : m_meshSections)
	{
		meshSection.m_vertexDataStride = sizeof(SkinnedVertexIn);

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(SkinnedVertexIn) * meshSection.m_vertexs.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subResourceData;
		ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		subResourceData.pSysMem = meshSection.m_vertexs.data();

		HRESULT HR(device->CreateBuffer(&bufferDesc, &subResourceData, meshSection.m_vertexBuffer.GetAddressOf()));
		if (FAILED(HR))
		{
			ErrorLogger::Log(("Create SkinModel Vertex Buffer Failed."));
			return false;
		}
	}

	return true;
}

bool SkeletonMesh::CreateIndexBuffer(ID3D11Device* device)
{
	for (MeshSection& meshSection : m_meshSections)
	{
		D3D11_BUFFER_DESC  bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(DWORD) * meshSection.m_indices.size());
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subResourceData;
		subResourceData.pSysMem = meshSection.m_indices.data();
		subResourceData.SysMemPitch = 0;
		subResourceData.SysMemSlicePitch = 0;

		HRESULT HR(device->CreateBuffer(&bufferDesc, &subResourceData, meshSection.m_indexBuffer.GetAddressOf()));
		if (FAILED(HR))
		{
			ErrorLogger::Log(("Create SkinModel Index Buffer Failed."));
			return false;
		}
	}

	return true;
}
bool SkeletonMesh::InitializeSkinModel(std::string in_filePath, ID3D11Device *in_device)
{
	LoadDataFromFlie(in_filePath);
	CreateVertexBuffer(in_device);
	CreateIndexBuffer(in_device);
	return true;
}

bool SkeletonMesh::LoadDataFromFlie(const std::string& filePath)
{
	//aiProcess_FlipUVs ||
	unsigned int importingFlags =  aiProcess_Triangulate;
	m_assimpScene = m_importer.ReadFile(filePath, importingFlags);

	if (m_assimpScene == nullptr)
	{
		// TODO: Handle error here.

		return false;
	}

	if (m_assimpScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
	{
		// TODO: Handle error here.

		return false;
	}

	if (m_assimpScene->mRootNode == nullptr)
	{
		// TODO: Handle error here.

		return false;
	}


	aiMatrix4x4 tempGloabTransform = m_assimpScene->mRootNode->mTransformation;
	tempGloabTransform = tempGloabTransform.Inverse();
	m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(tempGloabTransform);

	UINT maxNum = 0;
	for (unsigned int i = 0; i < m_assimpScene->mNumMeshes; i++)
	{
		if (m_assimpScene->mMeshes[i]->mNumVertices > maxNum)
		{
			maxNum = m_assimpScene->mMeshes[i]->mNumVertices;
		}
	}
	m_bones.resize(maxNum);


	for (unsigned int i = 0; i < m_assimpScene->mNumMeshes; i++)
	{
		MeshSection newMeshSection;

		aiMesh* aimesh = m_assimpScene->mMeshes[i];
		

		vector<XMFLOAT4X4> boneOffset;
		vector<int> boneToParentIndex;

		SkeletonMesh::SkinnedVertexIn temp;
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
			if (aimesh->mTextureCoords[0])
			{
				temp.uv.x = aimesh->mTextureCoords[0][j].x;
				temp.uv.y = aimesh->mTextureCoords[0][j].y;
			}
			//TODO add bone indicies and weights
			


			newMeshSection.m_vertexs.push_back(temp);
		}

		for (UINT j = 0; j < aimesh->mNumFaces; j++)
		{
			aiFace face = aimesh->mFaces[j];
			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				newMeshSection.m_indices.push_back(face.mIndices[k]);
			}
		}



		//UINT AllVertexCount = 0;
		UINT AllVertexCount = static_cast<UINT>(newMeshSection.m_vertexs.size());
		/*for (int vc = 0; vc < m_meshTable.size(); vc++)
		{
			AllVertexCount += m_meshTable[vc]->m_vetexCount;
		}*/

		if (m_bones.size() < AllVertexCount + aimesh->mNumVertices)
		{
			m_bones.resize(AllVertexCount + aimesh->mNumVertices);
		}

		for (UINT k = 0; k < aimesh->mNumBones; k++)
		{
			UINT boneIndex = 0;
			string boneName = aimesh->mBones[k]->mName.data;

			if (m_boneMapping.find(boneName) == m_boneMapping.end())
			{
				boneIndex = m_numBones;
				m_numBones++;
				BoneInfo bi;
				m_boneInfo.push_back(bi);
			}
			else
			{
				boneIndex = m_boneMapping[boneName];
			}

			m_boneMapping[boneName] = boneIndex;
			XMFLOAT4X4 tempMatrix = AiMatrixToXMFLOAT4x4(aimesh->mBones[k]->mOffsetMatrix);
			m_boneInfo[boneIndex].boneOffsetMatrix = tempMatrix;
			m_boneInfo[boneIndex].name = aimesh->mBones[k]->mName.data;

			for (UINT l = 0; l < aimesh->mBones[k]->mNumWeights; l++)
			{
				UINT VertexID = AllVertexCount + aimesh->mBones[k]->mWeights[l].mVertexId;
				float Weight = aimesh->mBones[k]->mWeights[l].mWeight;
				m_bones[VertexID].AddBoneData(boneIndex, Weight);
			}

			for (DWORD j = 43079; j < aimesh->mNumVertices; j++)
			{
				XMFLOAT4 tempFloat;
				/*tempFloat.x = m_bones[AllVertexCount + j].weights[0];
				tempFloat.y = m_bones[AllVertexCount + j].weights[1];
				tempFloat.z = m_bones[AllVertexCount + j].weights[2];
				tempFloat.w = m_bones[AllVertexCount + j].weights[3];*/


				newMeshSection.m_vertexs[j].weights.x = m_bones[AllVertexCount + j].weights[0];
				newMeshSection.m_vertexs[j].weights.y = m_bones[AllVertexCount + j].weights[1];
				newMeshSection.m_vertexs[j].weights.z = m_bones[AllVertexCount + j].weights[2];
				newMeshSection.m_vertexs[j].weights.w = m_bones[AllVertexCount + j].weights[3];
				
				newMeshSection.m_vertexs[j].boneIndiecs[0] =  m_bones[AllVertexCount + j].iDs[0];
				newMeshSection.m_vertexs[j].boneIndiecs[1] =  m_bones[AllVertexCount + j].iDs[1];
				newMeshSection.m_vertexs[j].boneIndiecs[2] =  m_bones[AllVertexCount + j].iDs[2];
				newMeshSection.m_vertexs[j].boneIndiecs[3] =  m_bones[AllVertexCount + j].iDs[3];

				//newMeshSection.m_vertexs[j].weights = tempFloat;

				//newMeshSection.m_vertexs.push_back(temp);
			}
		}

		aiNode* sceneRootNode = m_assimpScene->mRootNode;
		for (unsigned int f = 0; f < aimesh->mNumBones; f++)
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
					for (unsigned int sonCount = 0; sonCount < newNode->mParent->mNumChildren; sonCount++)
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
			for (size_t tempNode = 0; tempNode < m_aiNode.size(); tempNode++)
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

		m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(m_assimpScene->mRootNode->mTransformation.Inverse());

		m_meshSections.push_back(std::move(newMeshSection));
	}

	return true;
}

XMFLOAT4X4  SkeletonMesh::AiMatrixToXMFLOAT4x4(aiMatrix4x4 ai)
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

aiMatrix4x4 SkeletonMesh::XMFLOAT4x4ToAiMatrix(XMFLOAT4X4 xm)
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

void  SkeletonMesh::BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& transforms)
{
	XMFLOAT4X4 identify;
	XMMATRIX temp = XMMatrixIdentity();
	XMStoreFloat4x4(&identify, temp);

	float TickPerSecond = (float)m_assimpScene->mAnimations[0]->mTicksPerSecond != 0 ? static_cast<float>(m_assimpScene->mAnimations[0]->mTicksPerSecond) : 25.0f;
	float TimeInTicks = (float)TimeInSeconds * TickPerSecond;
	float AnimationTime = (float)fmod(TimeInTicks, m_assimpScene->mAnimations[0]->mDuration);

	ReadNodeHierarchy(AnimationTime, m_aiNode[0], identify);

	transforms.resize(m_numBones);

	for (UINT i = 0; i < m_numBones; i++) 
	{
		//m_calculatedBoneTransforms[i] = m_boneInfo[i].finalTransformation;
		m_calculatedBoneTransforms[i] = m_boneInfo[i].finalTransformation;
		//Get Bone Pose
		
		//XMMATRIX identity = XMMatrixIdentity();
		//XMStoreFloat4x4(&m_calculatedBoneTransforms[i], identity);
	
	}
}

void SkeletonMesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform)
{
	string nodeName = pNode->mName.data;
	const aiAnimation* pAnimation = m_assimpScene->mAnimations[0];
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

		nodeTransformation = ScalingM * RotationM * TranslationXM ;

	}
	aiMatrix4x4 parentTransformAi = XMFLOAT4x4ToAiMatrix(ParentTransform);
	aiMatrix4x4 gloabTransformAI = parentTransformAi * nodeTransformation;
	//Use Assimp compute globTransform

	XMFLOAT4X4 gloabTransform = AiMatrixToXMFLOAT4x4(gloabTransformAI);
	XMMATRIX result;
	if (m_boneMapping.find(nodeName) != m_boneMapping.end())
	{
		UINT BoneIndex = m_boneMapping[nodeName];
		XMMATRIX m_gloabInverseTransformM = XMLoadFloat4x4(&m_gloabInverseTransform);
		XMFLOAT4X4 gloabTransform = AiMatrixToXMFLOAT4x4(gloabTransformAI);
		XMMATRIX gloabTransform_m = XMLoadFloat4x4(&gloabTransform);
		XMMATRIX boneOffsetMatrixM = XMLoadFloat4x4(&m_boneInfo[BoneIndex].boneOffsetMatrix);
		
		XMMatrixTranspose(gloabTransform_m);
		result = m_gloabInverseTransformM * gloabTransform_m * boneOffsetMatrixM;  //gloabToRoot * currentAnimaTransform * boneOffset
		XMStoreFloat4x4(&m_boneInfo[BoneIndex].finalTransformation, result);
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], gloabTransform);
	}
}

void SkeletonMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;

		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);
	//assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = static_cast<float>(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

UINT SkeletonMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) 
		{
			return i;
		}
	}

	return 0;
}

aiNodeAnim* SkeletonMesh::FindNodeAnim(const aiAnimation* Animation, string nodeName)
{
	for (unsigned int i = 0; i < Animation->mNumChannels; i++)
	{
		if (Animation->mChannels[i]->mNodeName.data == nodeName)
		{
			return Animation->mChannels[i];
		}
	}

	return nullptr;
}

void SkeletonMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) 
	{
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

void SkeletonMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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

UINT SkeletonMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) 
		{
			return i;
		}
	}

	return 0;
}

UINT SkeletonMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);
	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	return 0;
}

aiMatrix4x4  SkeletonMesh::InitScaleTransform(float x, float y, float z)
{
	aiMatrix4x4  result;
	result.a1 = x;
	result.b2 = y;
	result.c3 = z;
	result.d4 = 1;

	return result;
}

aiMatrix4x4  SkeletonMesh::InitTranslationTransform(float x, float y, float z)
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

void SkeletonMesh::Draw(ID3D11DeviceContext* in_deviceContext,ID3D11InputLayout* inputLayout,ID3D11VertexShader* vShader,ID3D11PixelShader* pShader,UINT meshOffset)
{
	
}

void SkeletonMesh::DrawAllMesh(ID3D11DeviceContext* in_deviceContext,ID3D11InputLayout* inputLayout,ID3D11VertexShader* vShader,ID3D11PixelShader* pShader, ConstantBuffer<ContantBuffer_VS>*in_cb_vs_vertexshader, const XMMATRIX& viewProjectionMatrix)
{
	cb_vertexshader = in_cb_vs_vertexshader;
	UINT offset = 0;

	//compute constantbuffer
	cb_vertexshader->data.worldMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//Scale Matrix
	//XMMatrixScaling(0.01f,0.01f,0.01f);
	cb_vertexshader->data.VPMat = viewProjectionMatrix;
	cb_vertexshader->data.VPMat = XMMatrixTranspose(cb_vertexshader->data.VPMat);
	for (UINT i = 0; i < m_numBones; i++)
	{

		XMMATRIX XmTransforms = XMLoadFloat4x4(&m_calculatedBoneTransforms[i]);
		//XmTransforms = XMMatrixTranspose(XmTransforms);
		cb_vertexshader->data.transfomMat[i] = XmTransforms;

	}
	cb_vertexshader->ApplyChanges();

	in_deviceContext->VSSetConstantBuffers(0, 1, cb_vertexshader->GetAddressOf());

	for (MeshSection& meshSection : m_meshSections)
	{
		in_deviceContext->IASetVertexBuffers(0, 1, meshSection.m_vertexBuffer.GetAddressOf(), &(meshSection.m_vertexDataStride), &offset);
		in_deviceContext->IASetIndexBuffer(meshSection.m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		in_deviceContext->DrawIndexed(static_cast<UINT>(meshSection.m_indices.size()), 0, 0);
	}
}