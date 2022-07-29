#pragma once

#include <map>

#include <unordered_map>
#include <array>

#include<Windows.h>
#include<DirectXMath.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Vertex.h"
#include"VertexBuffer.h"
#include"indexBuffer.h"
#include"ConstantBuffer.h"
#include"Mesh.h"
#include "Shaders.h"

#define NUM_BONE_PER_VERTEX 4
#define NUM_BONE_PER_MESH 100

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

using namespace std;
using namespace DirectX;

class SkeletonMesh
{

public:
	SkeletonMesh() = default;
	~SkeletonMesh() = default;

	SkeletonMesh(const SkeletonMesh& rhs) = delete;
	SkeletonMesh(SkeletonMesh&& rhs) = delete;
	SkeletonMesh& operator=(const SkeletonMesh&) = delete;

protected:

	XMMATRIX m_worldMatrix = XMMatrixIdentity();
	XMMATRIX m_viewMatrix = XMMatrixIdentity();

protected:
	struct SkinnedVertexIn
	{
		XMFLOAT3 pos;
		XMUINT4 boneIndiecs;
		XMFLOAT4 weights;
		XMFLOAT3 normal;
		XMFLOAT2 uv;

		static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 5> inputLayout =
		{
			{
				{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							   0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEINDICES",    0, DXGI_FORMAT_R32G32B32A32_UINT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEWEIGHTS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			}
		};

	};

	struct VertexBoneInfluence
	{
		UINT boneIDs[NUM_BONE_PER_VERTEX];
		float weights[NUM_BONE_PER_VERTEX];

		UINT numBones = 0;

		void AddBoneData(UINT in_boneID, float in_weight)
		{
			assert(numBones < ARRAYSIZE(boneIDs));

			boneIDs[numBones] = in_boneID;
			weights[numBones] = in_weight;

			++numBones;
		}
	};

	struct BoneInfo
	{
		BoneInfo() = default;
		BoneInfo(const XMMATRIX& in_OffsetMatrix)
			: offsetMatrix(in_OffsetMatrix)
			, finalTransformation()
		{
		}

		XMMATRIX offsetMatrix;
		XMMATRIX finalTransformation;
	};


	struct MeshSection
	{
		UINT numIndices;
		UINT baseVertex;
		UINT baseIndex;
	};


	std::vector<MeshSection> m_meshSections;

	XMMATRIX m_rootNoeInverseTransformInWorldSpace;

	std::vector<SkinnedVertexIn> m_totalVertices;
	std::vector<WORD> m_totalIndices;
	std::vector<VertexBoneInfluence> m_vertexBoneInfluences;
	std::unordered_map<std::string, UINT> m_boneNameToIDMap;
	std::vector<BoneInfo> m_boneInfos;
	const aiScene* m_aiScene;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	std::unique_ptr<VertexShader> m_vertexShader;
	std::unique_ptr<PixelShader> m_pixelShader;

	ComPtr<ID3D11InputLayout> m_inputLayout;

	ComPtr<ID3D11SamplerState> m_samplerState;

	std::unique_ptr<ConstantBuffer<ContantBuffer_VS>> m_ContantBuffer_VS;


	void CountTotalVerticesAndIndices(UINT& out_numVertices, UINT& out_numIndices, const aiScene* in_aiScene);

	bool LoadModelFromFile(const std::string& in_filePath);

	void TransferBoneInfluences();

	void LoadAllMeshes(const aiScene* in_aiScene);

	void LoadMesh(UINT in_meshIndex, const aiMesh* in_aiMesh);

	void LoadAllMeshBones(UINT in_meshIndex, const aiMesh* in_aiMesh);

	void LoadMeshBone(UINT in_meshIndex, const aiBone* in_aiBone);

	const aiNodeAnim* TryFindNodeAnim(const aiAnimation* in_aiAnimation, PCSTR in_nodeName);
	UINT FindPosition(FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim);
	UINT FindRotationKeyIndex(FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim);
	UINT FindScalingKeyIndex(FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim);
	UINT GetBoneID(const aiBone* in_aiBone);

	void InterpolatePosition(XMFLOAT3& out_translation, FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim);

	void InterpolateRotation(XMVECTOR& out_quaternion, FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim);

	void InterpolateScaling(XMFLOAT3& out_scale, FLOAT in_animationTimeTicks, const aiNodeAnim* in_aiNodeAnim);

	void ReadNodeHierarchy(FLOAT in_animationTimeTicks, const aiNode* in_aiNode, const XMMATRIX& in_parentTransform);

	const SkinnedVertexIn* GetVertices() const;
	const WORD* GetIndices() const;


protected:
	static std::unique_ptr<Assimp::Importer> sm_pImporter;

public:
	void DrawAllMesh(ID3D11DeviceContext* in_deviceContext);

	bool CreateVertexBuffer(ID3D11Device* in_device);

	bool CreateVSConstantBuffer(ID3D11Device* in_device);

	bool CreateIndexBuffer(ID3D11Device* in_device);

	bool Initialize(ID3D11Device* in_device, std::string in_filePath);

	bool CreateTextureResources(ID3D11Device* in_device);

	bool CreateRasterizerState(ID3D11Device* in_device);

	bool CreateSamplerState(ID3D11Device* in_device);

	bool CreatePixelShader(ID3D11Device* in_device);

	bool CreateVertexShader(ID3D11Device* in_device);

	bool CreateInputLayout(ID3D11Device* in_device);

};