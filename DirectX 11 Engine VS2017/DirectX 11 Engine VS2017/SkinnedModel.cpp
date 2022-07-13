#include "SkinnedModel.h"
#include "LoadM3d.h"

SkinnedModel::SkinnedModel(ID3D11Device* device, const std::string& modelFilename, const std::wstring& texturePath)
{
	std::vector<M3dMaterial> mats;
	M3DLoader m3dLoader;
	m3dLoader.LoadM3d(modelFilename, Vertices, Indices, Subsets, mats, SkinnedData);

	ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
	ModelMesh.SetIndices(device, &Indices[0], Indices.size());
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = mats.size();

	for (UINT i = 0; i < SubsetCount; ++i)
	{
		Mat.push_back(mats[i].Mat);
	}
}

SkinnedModel::~SkinnedModel()
{
}

void SkinnedModelInstance::Update(float dt)
{
	TimePos += dt;
	Model->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);

	// Loop animation
	if (TimePos > Model->SkinnedData.GetClipEndTime(ClipName))
		TimePos = 0.0f;
}