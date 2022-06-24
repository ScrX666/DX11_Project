#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"DirectXTK.lib")
#pragma comment(lib,"DXGI.lib")
#include <wrl/client.h>
#include<vector>
#include <dxgi.h>

class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = NULL;
	DXGI_ADAPTER_DESC description;
};
class AdapterReader
{
public:
	static std::vector<AdapterData>GetAdapters();
private:
	static std::vector<AdapterData> adapters;
};
