#pragma once
#include "Vertex.h"
#include"VertexBuffer.h"
#include"indexBuffer.h"
#include"ConstantBuffer.h"
#include"Mesh.h"
#include <map>
#include<D3Dcommon.h>  
#include<Windows.h>
using namespace DirectX;

#define NUM_BONE_PER_VERTEX 4
#define NUM_BONE_PER_MESH 100
using namespace std;
using namespace Assimp;
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

	class Mesh2
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
		Mesh2() :Id(-1), VertexStart(0), VertexCount(0), FaceStart(0), FaceCount(0),
			mIndexBufferFormat(DXGI_FORMAT_R32_UINT), mVertexStride(sizeof(SkinnedVertexIn))
		{

		}
		~Mesh2()
		{

		}
		template<typename VertexTypeA>
		bool SetVertices(ID3D11Device* device, const VertexTypeA* vertices, UINT count, UINT offset, Model* g);
		bool SetIndices(ID3D11Device* device, const vector<DWORD> index, UINT offset, Model* g);

		//void SetMeshTable(vector<Mesh>& SubsetTable);

		void Draw2(ID3D11DeviceContext* deviceContext,
			ID3D11InputLayout* inputLayout,
			ID3D11VertexShader* vShader,
			ID3D11PixelShader* pShader,
			UINT meshOffset,
			Model* g,
			ID3D11Buffer* const* cBufferFrequently,
			ID3D11Buffer* const* cBufferRarely,
			ID3D11Buffer* const* cBufferOnResize,
			ID3D11Buffer* const* cBufferInstance
		);

	};


	//**************
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;


	XMMATRIX worldMatrix = XMMatrixIdentity();

private:

	vector<Mesh2> mMeshTable;
	//********************

public:
	void LoadMesh(std::string filePath);
	bool LoadMeshWithSkinnedAnimation(std::string filePath);
	void DrawAllMesh(ID3D11DeviceContext* deviceContext,
		ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vShader,
		ID3D11PixelShader* pShader,
		ID3D11Buffer* const* cBufferFrequently,
		ID3D11Buffer* const* cBufferRarely,
		ID3D11Buffer* const* cBufferOnResize,
		ID3D11Buffer* const* cBufferInstance);

private:
	Model(const Model& rhs);
	Model& operator=(const Model& rhs);


public:
	Model() = default;
	Model(ID3D11Device* dev);
	~Model();


public:
		struct VertexBoneData
		{
			unsigned int iDs[NUM_BONE_PER_VERTEX];
			float weights[NUM_BONE_PER_VERTEX];
			void AddBoneData(UINT boneId, float weight)
			{
				for (UINT i = 0; i < sizeof(iDs); i++)
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

public:
		struct BoneInfo
		{
			string name;
			XMFLOAT4X4 boneOffsetMatrix;
			XMFLOAT4X4 finalTransformation;
		};

		Importer m_Importer;
		const aiScene* m_aiscene;

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
		UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

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