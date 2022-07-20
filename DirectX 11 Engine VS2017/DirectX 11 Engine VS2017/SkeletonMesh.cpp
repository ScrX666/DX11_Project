#include "SkeletonMesh.h"

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

bool SkeletonMesh::InitializeSkinModel(std::string filePath, ComPtr<ID3D11Device> device)
{
	Assimp::Importer assimpImporter;
	unsigned int importingFlags = aiProcess_FlipUVs || aiProcess_Triangulate;
	const aiScene* assimpScene = assimpImporter.ReadFile(filePath, importingFlags);

	if (assimpScene == nullptr)
	{
		// TODO: Handle error here.

		return false;
	}

	if (assimpScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
	{
		// TODO: Handle error here.

		return false;
	}

	if (assimpScene->mRootNode == nullptr)
	{
		// TODO: Handle error here.

		return false;
	}

	static DWORD start;
	static DWORD end;
	static DWORD currentFace;
	start = end = currentFace = 0;


	aiMatrix4x4 tempGloabTransform = assimpScene->mRootNode->mTransformation;
	tempGloabTransform = tempGloabTransform.Inverse();
	m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(tempGloabTransform);

	UINT maxNum = 0;
	for (int i = 0; i < assimpScene->mNumMeshes; i++)
	{
		if (assimpScene->mMeshes[i]->mNumVertices > maxNum)
		{
			maxNum = assimpScene->mMeshes[i]->mNumVertices;
		}
	}
	m_bones.resize(maxNum);


	for (int i = 0; i < assimpScene->mNumMeshes; i++)
	{
		m_meshTable.push_back(std::make_shared<SkeletonMesh>());
		std::shared_ptr<SkeletonMesh> model = m_meshTable.back();
		model->m_ID = assimpScene->mNumMeshes;

		aiMesh* aimesh = assimpScene->mMeshes[i];

		//mbones.resize(aimesh->mNumVertices*aimesh->mNumBones);
		//mbones.resize(aimesh->mNumBones);


		end += start;
		end += aimesh->mNumVertices;

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
			XMFLOAT2 texcoord;
			if (aimesh->mTextureCoords[0])
			{
				temp.uv.x = aimesh->mTextureCoords[0][j].x;
				temp.uv.y = aimesh->mTextureCoords[0][j].y;
			}
			//TODO add bone indicies and weights

			model->m_verteiceWithSkinnedAnimation.push_back(temp);
		}

		for (DWORD j = 0; j < aimesh->mNumFaces; j++)
		{
			aiFace face = aimesh->mFaces[j];
			//UINT t1 = aimesh->mFaces[j].mIndices[0];
			//UINT t2 = aimesh->mFaces[j].mIndices[1];
			//UINT t3 = aimesh->mFaces[j].mIndices[2];

			for (DWORD k = 0; k < face.mNumIndices; k++)
			{
				model->m_index.push_back(face.mIndices[k]);
				model->m_faceCount++;
			}
		}

		

		UINT AllVertexCount = 0;
		for (int vc = 0; vc < m_meshTable.size(); vc++)
		{
			AllVertexCount += m_meshTable[vc]->m_vetexCount;
		}

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
				//UINT VertexID =  aimesh->mBones[k]->mWeights[l].mVertexId;
				float Weight = aimesh->mBones[k]->mWeights[l].mWeight;
				m_bones[VertexID].AddBoneData(boneIndex, Weight);
			}


			for (DWORD j = 0; j < aimesh->mNumVertices; j++)
			{
				XMUINT4 tempUint;
				tempUint.x = m_bones[AllVertexCount + j].iDs[0];
				tempUint.y = m_bones[AllVertexCount + j].iDs[1];
				tempUint.z = m_bones[AllVertexCount + j].iDs[2];
				tempUint.w = m_bones[AllVertexCount + j].iDs[3];
				XMFLOAT4 tempFloat;
				tempFloat.x = m_bones[AllVertexCount + j].weights[0];
				tempFloat.y = m_bones[AllVertexCount + j].weights[1];
				tempFloat.z = m_bones[AllVertexCount + j].weights[2];
				tempFloat.w = m_bones[AllVertexCount + j].weights[3];

				model->m_verteiceWithSkinnedAnimation[j].boneIndiecs = tempUint;
				model->m_verteiceWithSkinnedAnimation[j].weights = tempFloat;
			}
		}

		aiNode* sceneRootNode = assimpScene->mRootNode;
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

		m_gloabInverseTransform = AiMatrixToXMFLOAT4x4(assimpScene->mRootNode->mTransformation.Inverse());

		model->m_vetexCount = aimesh->mNumVertices;
		model->m_faceStart = currentFace;
		currentFace += aimesh->mNumFaces;
		model->m_faceCount = aimesh->mNumFaces;
		model->m_vertexStart = start;
		start = end;

		m_dev = device;
		model->SetVertices(m_dev.Get(), model->m_verteiceWithSkinnedAnimation.data(), model->m_verteiceWithSkinnedAnimation.size(), i, this);
		model->SetIndices(m_dev.Get(), model->m_index, i, this);
	}
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

	float TickPerSecond = (float)m_aiScene->mAnimations[0]->mTicksPerSecond != 0 ? m_aiScene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = (float)TimeInSeconds * TickPerSecond;
	float AnimationTime = (float)fmod(TimeInTicks, m_aiScene->mAnimations[0]->mDuration);

	ReadNodeHierarchy(AnimationTime, m_aiNode[0], identify);

	transforms.resize(m_numBones);

	for (UINT i = 0; i < m_numBones; i++) 
	{
		m_calculatedBoneTransforms[i] = m_boneInfo[i].finalTransformation;
	}

}

void SkeletonMesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform)
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
	if (m_boneMapping.find(nodeName) != m_boneMapping.end())
	{
		UINT BoneIndex = m_boneMapping[nodeName];
		XMMATRIX m_gloabInverseTransformM = XMLoadFloat4x4(&m_gloabInverseTransform);
		XMFLOAT4X4 gloabTransform = AiMatrixToXMFLOAT4x4(gloabTransformAI);
		XMMATRIX gloabTransform_m = XMLoadFloat4x4(&gloabTransform);
		XMMATRIX boneOffsetMatrixM = XMLoadFloat4x4(&m_boneInfo[BoneIndex].boneOffsetMatrix);

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

UINT SkeletonMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
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

aiNodeAnim* SkeletonMesh::FindNodeAnim(const aiAnimation* Animation, string nodeName)
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

void SkeletonMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}

UINT SkeletonMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
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
	UINT offset = 0;
	//in_deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffers[0].GetAddressOf(), &m_meshTable[0]->m_vertexStride, &offset);
	//in_deviceContext->IASetIndexBuffer(m_indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, offset);
	in_deviceContext->DrawIndexed(m_meshTable[0]->m_faceCount * 3, 0, 0);
	
}

void SkeletonMesh::DrawAllMesh(ID3D11DeviceContext* in_deviceContext,ID3D11InputLayout* inputLayout,ID3D11VertexShader* vShader,ID3D11PixelShader* pShader)
{

	for (int i = 0; i < m_meshTable.size(); i++)
	{
		m_meshTable[i]->Draw(in_deviceContext, inputLayout, vShader, pShader, i);
	}


}