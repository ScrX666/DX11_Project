
#include <map>
#include<Windows.h>
#include "Vertex.h"
#include<DirectXMath.h>
#include"VertexBuffer.h"
#include"indexBuffer.h"
#include"ConstantBuffer.h"
#include"Mesh.h"



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define NUM_BONE_PER_VERTEX 4
#define NUM_BONE_PER_MESH 100
template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
using namespace std;
using namespace DirectX;

class SkeletonMesh
{
	struct SkinnedVertexIn
	{
		XMFLOAT3 pos;
		uint8_t boneIndiecs[4];
		XMFLOAT4 weights;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		
		
	};
	struct VertexPosNormalTex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;

		static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	};
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
	struct BoneInfo
	{
		string name;
		XMFLOAT4X4 boneOffsetMatrix;
		XMFLOAT4X4 finalTransformation;
	};
	struct MeshSection
	{
		vector<SkinnedVertexIn> m_vertexs;
		vector<UINT> m_indices;
		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11Buffer> m_indexBuffer;
	};

public:
	SkeletonMesh() = default;
	~SkeletonMesh() = default;

	SkeletonMesh(const SkeletonMesh& rhs) = delete;
	SkeletonMesh(SkeletonMesh&& rhs) = delete;
	SkeletonMesh& operator=(const SkeletonMesh&) = delete;


	template<typename VertexTypeA>
	bool SetVertices(ID3D11Device* device, const VertexTypeA* vertices, UINT count, UINT offset, SkeletonMesh* pBuffer);
	bool SetIndices(ID3D11Device* device, const vector<DWORD> index, UINT offset, SkeletonMesh* pBuffer);
	XMFLOAT4X4  AiMatrixToXMFLOAT4x4(aiMatrix4x4 ai);
	aiMatrix4x4 XMFLOAT4x4ToAiMatrix(XMFLOAT4X4 xm);
	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMFLOAT4X4& ParentTransform);
	bool InitializeSkinModel(std::string filePath, ID3D11Device *device);
	bool LoadDataFromFlie(const std::string &filePath);
	void  BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& transforms);
	aiNode* FindNodeToParent(aiNode* childNode, const std::string& destNodeName);
	aiNode* FindNodeToChild(aiNode* childNode, const std::string& destNodeName);
	aiNodeAnim* FindNodeAnim(const aiAnimation* Animation, string nodeName);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	bool CreateVertexBuffer(ID3D11Device* device);
	bool CreateIndexBuffer(ID3D11Device* device);

	aiMatrix4x4  InitScaleTransform(float x, float y, float z);
	aiMatrix4x4  InitTranslationTransform(float x, float y, float z);

	void Draw(ID3D11DeviceContext* deviceContext,ID3D11InputLayout* inputLayout,ID3D11VertexShader* vShader,ID3D11PixelShader* pShader,UINT meshOffset);
	void DrawAllMesh(ID3D11DeviceContext* deviceContext,ID3D11InputLayout* inputLayout,ID3D11VertexShader* vShader,ID3D11PixelShader* pShader, ConstantBuffer<ContantBuffer_VS>* cb_vs_vertexshader, const XMMATRIX& viewProjectionMatrix);


public:
	ConstantBuffer<ContantBuffer_VS> *cb_vertexshader;
	unsigned int m_numBones;
	vector<XMFLOAT4X4> m_calculatedBoneTransforms;

protected:
	Assimp::Importer m_importer;
	const aiScene* m_assimpScene;
	UINT m_ID;
	UINT m_vertexStart;
	UINT m_vetexCount;
	UINT m_faceStart;
	UINT m_faceCount;

	ComPtr<ID3D11Buffer> m_pVertices;
	ComPtr<ID3D11Buffer> m_pNormals;

	std::vector<ComPtr<ID3D11Buffer>> m_pTexcoordArrays;



	DXGI_FORMAT m_indexBufferFormat; //unsigned long
	UINT m_vertexStride;

	ID3D11Buffer *vertexbuffer;
	ID3D11Buffer *indexbuffer;
	

	ID3D11DeviceContext* m_deviceContext;
	ConstantBuffer<ContantBuffer_VS>* m_VSConstantBuffer;
	ComPtr<ID3D11Device> m_dev;

	ID3D11Device* m_device;

	vector<std::shared_ptr<SkeletonMesh>> m_meshTable;

	XMMATRIX m_worldMatrix = XMMatrixIdentity();
	XMMATRIX m_viewMatrix = XMMatrixIdentity();

	vector<VertexBoneData> m_bones;
	map<string, unsigned int > m_boneMapping;
	map<string, unsigned int > m_boneMappingCollapsed;
	vector<BoneInfo> m_boneInfo;
	vector<BoneInfo> m_boneInfoCollapsed;
	vector<aiNode*> m_aiNode;

	size_t m_numMeshs;

	//vector<MeshSection> meshSection;
	MeshSection m_meshSection;

	//vector<std::shared_ptr<MeshSection>> m_meshSection;

	XMFLOAT4X4 m_gloabInverseTransform;

};