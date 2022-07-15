#pragma once
#include<DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX MVPMat;
	DirectX::XMMATRIX transfomMat[6];

};