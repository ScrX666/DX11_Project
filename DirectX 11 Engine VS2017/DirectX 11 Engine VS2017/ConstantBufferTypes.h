#pragma once
#include<DirectXMath.h>

struct ContantBuffer_VS
{
	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX VPMat;
	DirectX::XMMATRIX transfomMat[256];
};