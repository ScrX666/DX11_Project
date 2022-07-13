#ifndef SKINNEDMODEL_H
#define SKINNEDMODEL_H

#include "SkinData.h"
#include "MeshGeometry.h"

#include "VS.h"
#include "LightHelper.h"

class SkinnedModel
{
public:
	SkinnedModel(ID3D11Device* device, const std::string& modelFilename, const std::wstring& texturePath);
	~SkinnedModel();

	UINT SubsetCount;

	std::vector<Material> Mat;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<VT::PosNormalTexTanSkinned> Vertices;
	std::vector<USHORT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
	SkinnedData SkinnedData;
};

struct SkinnedModelInstance
{
	SkinnedModel* Model;
	float TimePos;
	std::string ClipName;
	XMFLOAT4X4 World;
	std::vector<XMFLOAT4X4> FinalTransforms;

	void Update(float dt);
};

#endif // SKINNEDMODEL_H