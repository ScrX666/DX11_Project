//#pragma once
//
//#include <map>
//#include <vector>
//
//#include<Windows.h>
//
//#include "D3Dcommon.h"  
//#include "Vertex.h"
//#include "VertexBuffer.h"
//#include "indexBuffer.h"
//#include "ConstantBuffer.h"
//#include "Mesh.h"
//
//using namespace DirectX;
//using namespace std;
//using namespace Assimp;
//
//#define NUM_BONE_PER_VERTEX 4
//#define NUM_BONE_PER_MESH 100
//
//template <class T>
//using ComPtr = Microsoft::WRL::ComPtr<T>;
//
//class Graphics;
//
//class Model
//{
//
//public:
//	//·Ö×é
//	struct SkinnedVertexIn
//	{
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT3 normal;
//		DirectX::XMFLOAT2 uv;
//		DirectX::XMFLOAT4 weights;
//		DirectX::XMUINT4 boneIndiecs;
//
//		static const D3D11_INPUT_ELEMENT_DESC inputLayout[5];
//	};
//
//	struct VertexPosNormalTex
//	{
//		XMFLOAT3 pos;
//		XMFLOAT3 normal;
//		XMFLOAT2 uv;
//
//		static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
//	};
//		
//	struct VertexBoneData
//	{
//		unsigned int iDs[NUM_BONE_PER_VERTEX];
//		float weights[NUM_BONE_PER_VERTEX];
//		void AddBoneData(UINT boneId, float weight)
//		{
//			for (UINT i = 0; i < sizeof(iDs); i++)
//			{
//				if (weights[i] == 0)
//				{
//					iDs[i] = boneId;
//					weights[i] = weight;
//					return;
//				}
//			}
//		}
//	};
//
//	struct BoneInfo
//	{
//		string name;
//		XMFLOAT4X4 boneOffsetMatrix;
//		XMFLOAT4X4 finalTransformation;
//	};
//
//public:
//	Model() = default;
//	~Model() = default;
//	
//	Model(const Model& rhs) = delete;
//	Model(Model&& rhs) = delete;
//	Model& operator=(const Model&) = delete;
//
//
//
//public:
//	bool Initalize(Graphics* in_graphics);
//
//	template<typename VertexTypeA>
//	bool SetVertices(const VertexTypeA* vertices, UINT count, UINT offset);
//
//	bool SetIndices(const vector<DWORD> index, UINT offset, Model* g);
//
//	bool LoadMeshesWithSkinnedAnimation(const std::string& filePath);
//
//	void DrawAllMesh(ID3D11DeviceContext* deviceContext,
//		ID3D11InputLayout* inputLayout,
//		ID3D11VertexShader* vShader,
//		ID3D11PixelShader* pShader
//	);
//
//	void Draw(ID3D11DeviceContext* deviceContext,
//		ID3D11InputLayout* inputLayout,
//		ID3D11VertexShader* vShader,
//		ID3D11PixelShader* pShader,
//		UINT meshOffset,
//		Model* g
//	);
//
//	// ~ Skinned animation related stuffs.
//	XMFLOAT4X4 AiMatrixToXMFLOAT4x4(aiMatrix4x4 ai);
//	aiMatrix4x4 XMFLOAT4x4ToAiMatrix(XMFLOAT4X4 xm);
//	aiMatrix4x4  InitScaleTransform(float x, float y, float z);
//	aiMatrix4x4  InitTranslationTransform(float x, float y, float z);
//	void BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& transforms);
//	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform);
//	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
//	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
//	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
//	aiNode* FindNodeToParent(aiNode* childNode, const std::string& destNodeName);
//	aiNode* FindNodeToChild(aiNode* childNode, const std::string& destNodeName);
//	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<ContantBuffer_VS>* cb_vs_vertexshader);
//	void DrawMVP(const XMMATRIX& viewProjectionMatrix, ID3D11DeviceContext* deviceContext);
//	aiNodeAnim* FindNodeAnim(const aiAnimation* Animation, string nodeName);
//	UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
//	UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
//	UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
//	// ~ Skinned animation related stuffs.
//
//protected:
//
//	UINT m_ID;
//	UINT m_vertexStart;
//	UINT m_vetexCount;
//	UINT m_faceStart;
//	UINT m_faceCount;
//
//	ComPtr<ID3D11Buffer> m_pVertices;
//	ComPtr<ID3D11Buffer> m_pNormals;
//
//	std::vector<ComPtr<ID3D11Buffer>> m_pTexcoordArrays;
//
//	ComPtr<ID3D11Buffer> m_tangents;
//	ComPtr<ID3D11Buffer> m_colors;
//	ComPtr<ID3D11Buffer> m_indices;
//
//
//	DXGI_FORMAT m_indexBufferFormat; //unsigned long
//	UINT m_vertexStride;
//
//	vector<VertexPosNormalTex> m_verteice;
//	vector<SkinnedVertexIn> m_verteiceWithSkinnedAnimation;
//	vector<DWORD> m_index;
//
//	VertexBuffer<Vertex> m_vertexbuffer;
//	IndexBuffer m_indexBuffers;
//	ID3D11DeviceContext* m_deviceContext;
//	ConstantBuffer<ContantBuffer_VS>* m_VSConstantBuffer;
//	ComPtr<ID3D11Device> m_dev;
//
//	ID3D11Device* m_device;
//
//	vector<Model> m_meshTable;
//
//	XMMATRIX m_worldMatrix = XMMatrixIdentity();
//	XMMATRIX m_viewMatrix = XMMatrixIdentity();
//
//	vector<VertexBoneData> m_bones;
//	map<string, unsigned int > m_boneMapping;
//	map<string, unsigned int > m_boneMappingCollapsed;
//	vector<BoneInfo> m_boneInfo;
//	vector<BoneInfo> m_boneInfoCollapsed;
//	vector<aiNode*> m_aiNode;
//
//	size_t m_numMeshs;
//	std::vector<ComPtr<ID3D11Buffer>> m_vertexBuffers;
//	std::vector<ComPtr<ID3D11Buffer>> m_indexBuffers;
//
//
//	unsigned int m_numBones;
//	//XMFLOAT4X4 m_gloabInverseTransform;
//	vector<XMFLOAT4X4> m_calculatedBoneTransforms;
//
//	//--
//	Graphics* m_graphics;
//};
//
