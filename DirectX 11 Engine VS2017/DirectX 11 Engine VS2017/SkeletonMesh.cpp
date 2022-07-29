#include "SkeletonMesh.h"
#include <stdint.h>

XMMATRIX ConvertMatrix(const aiMatrix4x4& in_matrix)
{
	return XMMATRIX(
		in_matrix.a1,
		in_matrix.b1,
		in_matrix.c1,
		in_matrix.d1,
		in_matrix.a2,
		in_matrix.b2,
		in_matrix.c2,
		in_matrix.d2,
		in_matrix.a3,
		in_matrix.b3,
		in_matrix.c3,
		in_matrix.d3,
		in_matrix.a4,
		in_matrix.b4,
		in_matrix.c4,
		in_matrix.d4
	);
}

XMFLOAT3 ConvertVector3dToFloat3(const aiVector3D& in_vector)
{
	return XMFLOAT3(in_vector.x, in_vector.y, in_vector.z);
}

XMVECTOR ConvertQuaternionToVector(const aiQuaternion& in_quaternion)
{
	XMFLOAT4 float4 = XMFLOAT4(in_quaternion.x, in_quaternion.y, in_quaternion.z, in_quaternion.w);
	return XMLoadFloat4(&float4);
}

bool SkeletonMesh::Initialize(ID3D11Device* in_device, std::string in_filePath)
{
	LoadModelFromFile(in_filePath);

	CreateVertexShader(in_device);

	CreateInputLayout(in_device);

	CreatePixelShader(in_device);

	CreateSamplerState(in_device);

	CreateTextureResources(in_device);

	CreateVertexBuffer(in_device);

	CreateIndexBuffer(in_device);

	CreateRasterizerState(in_device);

	return true;
}

bool SkeletonMesh::CreateTextureResources(ID3D11Device* in_device)
{
	//TODO

	/*HRESULT hr = DirectX::CreateWICTextureFromFile(m_device.Get(), L"..\\DirectX 11 Engine VS2017\\Texture\\maria_diffuse.png", NULL, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file. ");
		return false;
	}*/


	return true;
}

bool SkeletonMesh::CreateRasterizerState(ID3D11Device* in_device)
{


	return true;
}

bool SkeletonMesh::CreateSamplerState(ID3D11Device* in_device)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = in_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

	return SUCCEEDED(hr);
}

bool SkeletonMesh::CreateVertexShader(ID3D11Device* in_device)
{
	m_vertexShader = std::make_unique<VertexShader>();
	bool result = m_vertexShader->Initialize(in_device, L"..\\x64\\Debug\\SkinVS.cso");

	return result;
}

bool SkeletonMesh::CreatePixelShader(ID3D11Device* in_device)
{
	m_pixelShader = std::make_unique<PixelShader>();
	bool result = m_pixelShader->Initialize(in_device, L"..\\x64\\Debug\\SkinPS.cso");

	return result;
}

bool SkeletonMesh::CreateInputLayout(ID3D11Device* in_device)
{
	UINT numInputLayoutElements =  static_cast<UINT>(SkinnedVertexIn::inputLayout.size());
	HRESULT hr = in_device->CreateInputLayout(
		SkinnedVertexIn::inputLayout.data(),
		numInputLayoutElements, 
		m_vertexShader->GetBuffer()->GetBufferPointer(), 
		m_vertexShader->GetBuffer()->GetBufferSize(), 
		m_inputLayout.ReleaseAndGetAddressOf()
	);

	return SUCCEEDED(hr);
}

void SkeletonMesh::CountTotalVerticesAndIndices(UINT& out_numVertices, UINT& out_numIndices, const aiScene* in_aiScene)
{
	for (UINT meshSectionIndex = 0; meshSectionIndex < m_meshSections.size(); ++meshSectionIndex)
	{
		m_meshSections[meshSectionIndex].baseIndex = out_numIndices;
		m_meshSections[meshSectionIndex].baseVertex = out_numVertices;
		m_meshSections[meshSectionIndex].numIndices = in_aiScene->mMeshes[meshSectionIndex]->mNumFaces * 3;

		out_numIndices += in_aiScene->mMeshes[meshSectionIndex]->mNumFaces * 3;
		out_numVertices += in_aiScene->mMeshes[meshSectionIndex]->mNumVertices;
	}
}

std::unique_ptr<Assimp::Importer> SkeletonMesh::sm_pImporter = std::make_unique<Assimp::Importer>();

bool SkeletonMesh::LoadModelFromFile(const std::string& in_filepath)
{
	unsigned int importingFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ConvertToLeftHanded;
	m_aiScene = sm_pImporter->ReadFile(in_filepath, importingFlags);

	if (m_aiScene == nullptr)
	{
		// TODO: Handle error here.

		return false;
	}

	if (m_aiScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
	{
		// TODO: Handle error here.

		return false;
	}

	if (m_aiScene->mRootNode == nullptr)
	{
		// TODO: Handle error here.

		return false;
	}

	m_meshSections.resize(m_aiScene->mNumMeshes);

	UINT numTotalVertices = 0;
	UINT numTotalIndices = 0;
	CountTotalVerticesAndIndices(numTotalVertices, numTotalIndices, m_aiScene);

	m_totalVertices.reserve(numTotalVertices);
	m_totalIndices.reserve(numTotalIndices);
	m_vertexBoneInfluences.resize(numTotalVertices);

	LoadAllMeshes(m_aiScene);

	TransferBoneInfluences();

	return true;
}

void SkeletonMesh::TransferBoneInfluences()
{
	for (size_t vertexIndex = 0; vertexIndex < m_totalVertices.size(); ++vertexIndex)
	{
		m_totalVertices[vertexIndex].boneIndiecs = XMUINT4(m_vertexBoneInfluences.at(vertexIndex).boneIDs);
		m_totalVertices[vertexIndex].weights = XMFLOAT4(m_vertexBoneInfluences.at(vertexIndex).weights);
	}
}

void SkeletonMesh::LoadAllMeshes(const aiScene* in_aiScene)
{
	for (UINT meshIndex = 0; meshIndex < m_meshSections.size(); ++meshIndex)
	{
		const aiMesh* aiMeshPtr = in_aiScene->mMeshes[meshIndex];
		LoadMesh(meshIndex, aiMeshPtr);
	}
}

void SkeletonMesh::LoadMesh(UINT in_meshIndex, const aiMesh* in_aiMesh)
{
	const aiVector3D zero3d(0.0f, 0.0f, 0.0f);

	for (UINT vertexIndex = 0; vertexIndex < in_aiMesh->mNumVertices; ++vertexIndex)
	{
		const aiVector3D& position = in_aiMesh->mVertices[vertexIndex];
		const aiVector3D& normal = in_aiMesh->mNormals[vertexIndex];
		const aiVector3D& texCoord = in_aiMesh->HasTextureCoords(0) ? in_aiMesh->mTextureCoords[0][vertexIndex] : zero3d;

		SkinnedVertexIn vertex;
		vertex.pos = XMFLOAT3(position.x, position.y, position.z);
		vertex.uv = XMFLOAT2(texCoord.x, texCoord.y);
		vertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);

		m_totalVertices.push_back(vertex);
	}

	for (UINT faceIndex = 0; faceIndex < in_aiMesh->mNumFaces; ++faceIndex)
	{
		const aiFace& face = in_aiMesh->mFaces[faceIndex];
		assert(face.mNumIndices == 3u);

		WORD aIndices[3] =
		{
			static_cast<WORD>(face.mIndices[0]),
			static_cast<WORD>(face.mIndices[1]),
			static_cast<WORD>(face.mIndices[2]),
		};

		m_totalIndices.push_back(aIndices[0]);
		m_totalIndices.push_back(aIndices[1]);
		m_totalIndices.push_back(aIndices[2]);
	}

	LoadAllMeshBones(in_meshIndex, in_aiMesh);
}

void SkeletonMesh::LoadAllMeshBones(UINT in_meshIndex, const aiMesh* in_aiMesh)
{
	for (UINT boneIndex = 0; boneIndex < in_aiMesh->mNumBones; ++boneIndex)
	{
		LoadMeshBone(in_meshIndex, in_aiMesh->mBones[boneIndex]);
	}
}

void SkeletonMesh::LoadMeshBone(UINT in_meshIndex, const aiBone* in_aiBone)
{
	UINT boneID = GetBoneID(in_aiBone);

	if (boneID == m_boneInfos.size())
	{
		BoneInfo boneInfo(ConvertMatrix(in_aiBone->mOffsetMatrix));
		m_boneInfos.push_back(boneInfo);
	}

	for (UINT weightIndex = 0; weightIndex < in_aiBone->mNumWeights; ++weightIndex)
	{
		const aiVertexWeight& vertexWeight = in_aiBone->mWeights[weightIndex];
		UINT vertexID = m_meshSections[in_meshIndex].baseVertex + vertexWeight.mVertexId;
		m_vertexBoneInfluences[vertexID].AddBoneData(boneID, vertexWeight.mWeight);
	}
}

const aiNodeAnim* SkeletonMesh::TryFindNodeAnim(const aiAnimation* in_aiAnimation, PCSTR in_nodeName)
{
	for (UINT chanelIndex = 0; chanelIndex < in_aiAnimation->mNumChannels; ++chanelIndex)
	{
		const aiNodeAnim* aiNodeAnimPtr = in_aiAnimation->mChannels[chanelIndex];

		if (strncmp(aiNodeAnimPtr->mNodeName.data, in_nodeName, aiNodeAnimPtr->mNodeName.length) == 0)
		{
			return aiNodeAnimPtr;
		}
	}

	return nullptr;
}

UINT SkeletonMesh::FindPositionKeyIndex(FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim)
{
	assert(in_aiNodeAnim->mNumPositionKeys > 0);

	for (UINT keyFrameIndex = 0; keyFrameIndex < in_aiNodeAnim->mNumPositionKeys - 1; ++keyFrameIndex)
	{
		FLOAT t = static_cast<FLOAT>(in_aiNodeAnim->mPositionKeys[keyFrameIndex + 1].mTime);

		if (in_animationTimeTicks < t)
		{
			return keyFrameIndex;
		}
	}

	return 0;
}

UINT SkeletonMesh::FindRotationKeyIndex(FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim)
{
	assert(in_aiNodeAnim->mNumRotationKeys > 0);

	for (UINT keyFrameIndex = 0; keyFrameIndex < in_aiNodeAnim->mNumRotationKeys - 1; ++keyFrameIndex)
	{
		FLOAT t = static_cast<FLOAT>(in_aiNodeAnim->mRotationKeys[keyFrameIndex + 1].mTime);

		if (in_animationTimeTicks < t)
		{
			return keyFrameIndex;
		}
	}

	return 0;
}

UINT SkeletonMesh::FindScalingKeyIndex(FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim)
{
	assert(in_aiNodeAnim->mNumScalingKeys > 0);

	for (UINT keyFrameIndex = 0; keyFrameIndex < in_aiNodeAnim->mNumScalingKeys - 1; ++keyFrameIndex)
	{
		FLOAT t = static_cast<FLOAT>(in_aiNodeAnim->mScalingKeys[keyFrameIndex + 1].mTime);

		if (in_animationTimeTicks < t)
		{
			return keyFrameIndex;
		}
	}

	return 0;
}

UINT SkeletonMesh::GetBoneID(const aiBone* in_aiBone)
{
	UINT boneID = 0;
	PCSTR boneName = in_aiBone->mName.C_Str();
	auto search = m_boneNameToIDMap.find(boneName);
	if (search != m_boneNameToIDMap.end())
	{
		boneID = static_cast<UINT>(m_boneNameToIDMap.size());
		m_boneNameToIDMap[boneName] = boneID;
	}
	else
	{
		boneID = m_boneNameToIDMap[boneName];
	}

	return boneID;
}

void SkeletonMesh::InterpolatePosition(XMFLOAT3& out_translation, FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim)
{
	if (in_aiNodeAnim->mNumPositionKeys == 1)
	{
		out_translation = ConvertVector3dToFloat3(in_aiNodeAnim->mPositionKeys[0].mValue);
		return;
	}

	UINT positionKeyIndex = FindPositionKeyIndex(in_animationTimeTicks, in_aiNodeAnim);
	UINT nextPositionKeyIndex = positionKeyIndex + 1;
	assert(nextPositionKeyIndex < in_aiNodeAnim->mNumPositionKeys);

	FLOAT t1 = static_cast<FLOAT>(in_aiNodeAnim->mPositionKeys[positionKeyIndex].mTime);
	FLOAT t2 = static_cast<FLOAT>(in_aiNodeAnim->mPositionKeys[nextPositionKeyIndex].mTime);
	FLOAT deltaTime = t2 - t1;
	FLOAT factor = (in_animationTimeTicks - t1) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	const aiVector3D& start = in_aiNodeAnim->mPositionKeys[positionKeyIndex].mValue;
	const aiVector3D& end = in_aiNodeAnim->mPositionKeys[nextPositionKeyIndex].mValue;
	aiVector3D delta = end - start;

	out_translation = ConvertVector3dToFloat3(start + factor * delta);
}

void SkeletonMesh::InterpolateRotation(XMVECTOR& out_quaternion, FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim)
{
	if (in_aiNodeAnim->mNumRotationKeys == 1)
	{
		out_quaternion = ConvertQuaternionToVector(in_aiNodeAnim->mRotationKeys->mValue);
		return;
	}

	aiQuaternion quaternion;

	UINT rotationKeyIndex = FindRotationKeyIndex(in_animationTimeTicks, in_aiNodeAnim);
	UINT nextRotationKeyIndex = rotationKeyIndex + 1u;
	assert(nextRotationKeyIndex < in_aiNodeAnim->mNumRotationKeys);

	FLOAT t1 = static_cast<FLOAT>(in_aiNodeAnim->mRotationKeys[rotationKeyIndex].mTime);
	FLOAT t2 = static_cast<FLOAT>(in_aiNodeAnim->mRotationKeys[nextRotationKeyIndex].mTime);
	FLOAT deltaTime = t2 - t1;
	FLOAT factor = (in_animationTimeTicks - t1) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	const aiQuaternion& start = in_aiNodeAnim->mRotationKeys[rotationKeyIndex].mValue;
	const aiQuaternion& end = in_aiNodeAnim->mRotationKeys[nextRotationKeyIndex].mValue;
	aiQuaternion::Interpolate(quaternion, start, end, factor);
	quaternion.Normalize();

	out_quaternion = ConvertQuaternionToVector(quaternion);
}

void SkeletonMesh::InterpolateScaling(XMFLOAT3& out_scale, FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim)
{
	if (in_aiNodeAnim->mNumScalingKeys == 1)
	{
		out_scale = ConvertVector3dToFloat3(in_aiNodeAnim->mScalingKeys->mValue);
		return;
	}

	UINT scalingKeyIndex = FindScalingKeyIndex(in_animationTimeTicks, in_aiNodeAnim);
	UINT nextScalingKeyIndex = scalingKeyIndex + 1u;
	assert(nextScalingKeyIndex < in_aiNodeAnim->mNumScalingKeys);

	FLOAT t1 = static_cast<FLOAT>(in_aiNodeAnim->mScalingKeys[scalingKeyIndex].mTime);
	FLOAT t2 = static_cast<FLOAT>(in_aiNodeAnim->mScalingKeys[nextScalingKeyIndex].mTime);
	FLOAT deltaTime = t2 - t1;
	FLOAT factor = (in_animationTimeTicks - t1) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	const aiVector3D& start = in_aiNodeAnim->mScalingKeys[scalingKeyIndex].mValue;
	const aiVector3D& end = in_aiNodeAnim->mScalingKeys[nextScalingKeyIndex].mValue;
	aiVector3D delta = end - start;

	out_scale = ConvertVector3dToFloat3(start + factor * delta);
}

void SkeletonMesh::ReadNodeHierarchy(FLOAT in_animationTimeTicks, const aiNode* in_aiNode, const XMMATRIX& in_parentTransformInWorldSpace)
{
	XMMATRIX nodeTransformationInLocalSpace = ConvertMatrix(in_aiNode->mTransformation);

	auto aiNodeAnimPtr = TryFindNodeAnim(m_aiScene->mAnimations[0], in_aiNode->mName.C_Str());
	if (aiNodeAnimPtr) 
	{

		XMFLOAT3 scaling = XMFLOAT3();
		XMVECTOR rotation = XMVECTOR();
		XMFLOAT3 translation = XMFLOAT3();

		InterpolateScaling(scaling, in_animationTimeTicks, aiNodeAnimPtr);
		InterpolateRotation(rotation, in_animationTimeTicks, aiNodeAnimPtr);
		InterpolatePosition(translation, in_animationTimeTicks, aiNodeAnimPtr);

		nodeTransformationInLocalSpace = XMMatrixScaling(scaling.x, scaling.y, scaling.z)
			* XMMatrixRotationQuaternion(rotation)
			* XMMatrixTranslation(translation.x, translation.y, translation.z);
	}

	XMMATRIX transformationInWorldSpace = nodeTransformationInLocalSpace * in_parentTransformInWorldSpace;

	auto search = m_boneNameToIDMap.find(in_aiNode->mName.C_Str());
	if (search != m_boneNameToIDMap.end())
	{
		auto boneNameIndex = m_boneNameToIDMap.find(in_aiNode->mName.C_Str());
		UINT boneIndex = boneNameIndex->second;
		m_boneInfos[boneIndex].finalTransformation = m_boneInfos[boneIndex].offsetMatrix * transformationInWorldSpace * m_rootNoeInverseTransformInWorldSpace;
	}

	for (UINT i = 0; i < in_aiNode->mNumChildren; ++i)
	{
		ReadNodeHierarchy(in_animationTimeTicks, in_aiNode->mChildren[i], transformationInWorldSpace);
	}
}

const SkeletonMesh::SkinnedVertexIn* SkeletonMesh::GetVertices() const
{
	return m_totalVertices.data();
}

const WORD* SkeletonMesh::GetIndices() const
{
	return m_totalIndices.data();

}

void SkeletonMesh::DrawAllMesh(ID3D11DeviceContext* in_deviceContext)
{

}

bool SkeletonMesh::CreateVertexBuffer(ID3D11Device* device)
{
	if (device == nullptr)
	{
		ErrorLogger::Log(("The device is null"));
		return false;
	}

	UINT vertexDataStride = sizeof(SkinnedVertexIn);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(vertexDataStride * m_totalVertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourceData;
	ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	subResourceData.pSysMem = m_totalVertices.data();

	HRESULT HR(device->CreateBuffer(&bufferDesc, &subResourceData, m_vertexBuffer.GetAddressOf()));
	if (FAILED(HR))
	{
		ErrorLogger::Log(("Create SkinModel Vertex Buffer Failed."));
		return false;
	}

	return true;
}

bool SkeletonMesh::CreateVSConstantBuffer(ID3D11Device* in_device)
{
	m_ContantBuffer_VS = std::make_unique<ConstantBuffer<ContantBuffer_VS>>();
	HRESULT hr = m_ContantBuffer_VS->Initialize(in_device);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create constant buffer. ");
		return false;
	}

	return true;
}

bool SkeletonMesh::CreateIndexBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC  bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(DWORD) * m_totalIndices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = m_totalIndices.data();

	HRESULT HR(device->CreateBuffer(&bufferDesc, &subResourceData, m_indexBuffer.GetAddressOf()));
	if (FAILED(HR))
	{
		ErrorLogger::Log(("Create SkinModel Index Buffer Failed."));
		return false;
	}

	return true;
}
