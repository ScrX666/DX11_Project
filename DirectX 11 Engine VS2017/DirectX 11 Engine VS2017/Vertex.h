#pragma once
#include<DirectXMath.h>

struct Vertex
{

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT4 boneWeights;
	DirectX::XMFLOAT4 boneIds;
};