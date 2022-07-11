#pragma once
#ifndef  MESH_GEOMETRY_CLASS
#define MESH_GEOMETRY_CLASS

#include<Windows.h>
#include<iostream>
#include<vector>
#include<d3d11.h>
#include<D3Dcommon.h>  
#include <wrl/client.h>
#include <DirectXMath.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <map>
#include <assert.h>
#include <assimp/scene.h> 
#include "Model.h"

#define NUM_BONE_PER_VERTEX 4
#define NUM_BONE_PER_MESH 100
typedef unsigned int uint;
using namespace std;
using namespace DirectX;
using namespace Assimp;

class MeshGeometryClass
{
public:
	//·Ö×é
	struct SkinnedVertexIn
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT4 weights;
		DirectX::XMUINT4 boneIndiecs;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[5];
	};
	struct VertexPosNormalTex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	};

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Device> m_pdev;

	class Mesh
	{
	public:
		UINT Id;
		UINT VertexStart;
		UINT VertexCount;
		UINT FaceStart;
		UINT FaceCount;

		ComPtr<ID3D11Buffer> m_pVertices;

		ComPtr<ID3D11Buffer> m_pNormals;
		std::vector<ComPtr<ID3D11Buffer>> m_pTexcoordArrays;
		ComPtr<ID3D11Buffer> m_pTangents;
		ComPtr<ID3D11Buffer> m_pColors;
		ComPtr<ID3D11Buffer> m_pIndices;


		DXGI_FORMAT mIndexBufferFormat; //unsigned long
		UINT mVertexStride;

		vector<VertexPosNormalTex> verteice;
		vector<SkinnedVertexIn> verteiceWithSkinnedAnimation;
		vector<DWORD> index;

		ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;


	public:
		Mesh() :Id(-1), VertexStart(0), VertexCount(0), FaceStart(0), FaceCount(0),
			mIndexBufferFormat(DXGI_FORMAT_R32_UINT), mVertexStride(sizeof(SkinnedVertexIn))
		{

		}
		~Mesh()
		{

		}
		template<typename VertexTypeA>
		bool SetVertices(ID3D11Device* device, const VertexTypeA* vertices, UINT count, UINT offset, MeshGeometryClass* g);
		bool SetIndices(ID3D11Device* device, const vector<DWORD> index, UINT offset, MeshGeometryClass* g);

		//void SetMeshTable(vector<Mesh>& SubsetTable);

		void Draw(ID3D11DeviceContext* deviceContext,
			ID3D11InputLayout* inputLayout,
			ID3D11VertexShader* vShader,
			ID3D11PixelShader* pShader,
			UINT meshOffset,
			MeshGeometryClass* g,
			ID3D11Buffer* const* cBufferFrequently,
			ID3D11Buffer* const* cBufferRarely,
			ID3D11Buffer* const* cBufferOnResize,
			ID3D11Buffer* const* cBufferInstance
		);

	};

private:

	vector<Mesh> mMeshTable;

private:
	MeshGeometryClass(const MeshGeometryClass& rhs);
	MeshGeometryClass& operator=(const MeshGeometryClass& rhs);


public:
	MeshGeometryClass() = default;
	MeshGeometryClass(ID3D11Device* dev);
	~MeshGeometryClass();

public:
	struct VertexBoneData
	{
		unsigned int iDs[NUM_BONE_PER_VERTEX];
		float weights[NUM_BONE_PER_VERTEX];
		void AddBoneData(uint boneId, float weight)
		{
			for (uint i = 0; i < sizeof(iDs); i++)
			{
				if (weights[i] == 0)
				{
					iDs[i] = boneId;
					weights[i] = weight;
					return;
				}
			}
		}
	};


	struct BoneInfo
	{
		string name;
		XMFLOAT4X4 boneOffsetMatrix;
		XMFLOAT4X4 finalTransformation;
	};

	Importer m_Importer;
	const aiScene* m_aiscene;
	void LoadMesh(std::string filePath);
	bool LoadMeshWithSkinnedAnimation(std::string filePath);
	void DrawAllMesh(ID3D11DeviceContext* deviceContext,
		ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vShader,
		ID3D11PixelShader* pShader,
		ID3D11Buffer* const* cBufferFrequently,
		ID3D11Buffer* const* cBufferRarely,
		ID3D11Buffer* const* cBufferOnResize,
		ID3D11Buffer* const* cBufferInstance
	);
	XMFLOAT4X4 AiMatrixToXMFLOAT4x4(aiMatrix4x4 ai);
	aiMatrix4x4 XMFLOAT4x4ToAiMatrix(XMFLOAT4X4 xm);
	aiMatrix4x4  InitScaleTransform(float x, float y, float z);
	aiMatrix4x4  InitTranslationTransform(float x, float y, float z);
	void BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& transforms);
	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	aiNode* FindNodeToParent(aiNode* childNode, const std::string& destNodeName);
	aiNode* FindNodeToChild(aiNode* childNode, const std::string& destNodeName);
	aiNodeAnim* FindNodeAnim(const aiAnimation* Animation, string nodeName);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);


	vector<VertexBoneData> mbones;
	map<string, unsigned int > m_BoneMapping;
	map<string, unsigned int > m_BoneMappingCollapsed;
	vector<BoneInfo> m_BoneInfo;
	vector<BoneInfo> m_BoneInfoCollapsed;
	vector<aiNode*> m_aiNode;

	ComPtr<ID3D11Buffer> m_pVerticesArray[NUM_BONE_PER_MESH];
	ComPtr<ID3D11Buffer> m_pIndicesArray[NUM_BONE_PER_MESH];
	//vector<SkinnedVertexIn> m_verteiceWithSkinnedAnimationAll;
	//vector<DWORD> m_Allindex;
	//ComPtr<ID3D11Buffer> m_pAllIndices;
	//ComPtr<ID3D11Buffer> m_pAllVertices;
	//UINT mAllMeshVertexStride;
	unsigned int m_NumBone;
	XMFLOAT4X4 m_gloabInverseTransform;
	vector<XMFLOAT4X4> Transforms;
};
#endif // ! MESH_GEOMETRY_CLASS





