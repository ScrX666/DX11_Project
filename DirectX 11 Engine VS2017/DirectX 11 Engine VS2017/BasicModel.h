#ifndef BASICMODEL_H
#define BASICMODEL_H

#include "MeshGeometry.h"

#include "Vertex.h"

class BasicModel
{
public:
	BasicModel(ID3D11Device* devic, const std::string& modelFilename, const std::wstring& texturePath);
	~BasicModel();

	UINT SubsetCount;

	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<USHORT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
};

struct BasicModelInstance
{
	BasicModel* Model;
	XMFLOAT4X4 World;
};

#endif // SKINNEDMODEL_H