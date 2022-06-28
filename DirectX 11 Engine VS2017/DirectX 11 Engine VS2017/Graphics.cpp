#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	if (!InitializeDirectX(hwnd, width, height))
	{
		return false;
	}
	if (!InitializeShader())
	{
		return false;
	}
	if (!InitializeScene())
	{
		return false;
	}

	return true;
}
void Graphics::RenderFrame()
{
	//background Color
	float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//clear view
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//Before Draw
	this->deviceContext->IASetInputLayout(inputLayout.Get());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);


	UINT offset = 0;

	//Updata Constant Buffer
	CB_VS_vertexshader data;
	//offset
	constantBuffer.data.xOffset = 0.0f;
	constantBuffer.data.yOffset = 0.5f;
	if (!constantBuffer.ApplyChanges())
		return;
	this->deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	//draw green

	//draw red
	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());//set to pixel shader
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);




	this->swapChain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No IDXGI Adapter found");
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;

	hr = D3D11CreateDeviceAndSwapChain(
		adapters[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
		NULL, NULL, NULL, 0, D3D11_SDK_VERSION,
		&scd, this->swapChain.GetAddressOf(),
		this->device.GetAddressOf(),NULL,
		this->deviceContext.GetAddressOf()
	);

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create a swapChain.");
			return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "GetBuffer Failed");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to Create Render Target View");
		return false;
	}

	//Describe the Depth Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	 
	hr = device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer");
		return false;
	}
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view");
		return false;
	}


	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(),this->depthStencilView.Get());
	
	//create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = TRUE;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state");
		return false;
	}

	//creat viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	//Set ViewPort
	deviceContext->RSSetViewports(1, &viewport);


	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;//can change to rasterizer WIREFRAME or SOLID
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	// change rasterizer to counter clockwise
	//rasterizerDesc.FrontCounterClockwise = TRUE;

	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	//create sampler description for sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create sampler state.");
		return false;
	}


	return true;
}




bool Graphics::InitializeShader()
{
	if (!vertexShader.Initialize(device, L"D:\\ÈíäÖÈ¾Æ÷\\DirectX-11-Engine-VS2017-Tutorial_1\\DirectX-11-Engine-VS2017-Tutorial_1\\DirectX 11 Engine VS2017\\x64\\Debug\\VertexShader.cso"))
	{
		return false;
	}
	if (!pixelShader.Initialize(device, L"D:\\ÈíäÖÈ¾Æ÷\\DirectX-11-Engine-VS2017-Tutorial_1\\DirectX-11-Engine-VS2017-Tutorial_1\\DirectX 11 Engine VS2017\\x64\\Debug\\PixelShader.cso"))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 ,
	"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0};
	UINT numElements = ARRAYSIZE(layout);

	HRESULT hr = this->device->CreateInputLayout(layout, numElements, vertexShader.GetBuffer()->GetBufferPointer(), vertexShader.GetBuffer()->GetBufferSize(), this->inputLayout.GetAddressOf());
	
	if (FAILED(hr))
	{
		ErrorLogger::Log(GetLastError(), "Create InputLayout Failed  " );
		return false;
	}
	return true;
}

bool Graphics::InitializeScene()
{
	//Red
	//Vertex
	Vertex v[] =
	{
		// must be clockwise
		Vertex(-1.0f, -1.0f,1.0f,	0.0f,1.0f),//left botAtom -[0]
		Vertex(-1.0f,  1.0f, 1.0f,	0.0f,0.0f),//top middle		-[1]
		Vertex(1.0f, 1.0f,	1.0f,	1.0f,0.0f),//right middle	-[2]
		Vertex(1.0f, -1.0f, 1.0f,	1.0f,1.0f)					//-[3]
		//right middle
	};
	DWORD indices[] =
	{
		0,1,2,
		0,2,3
	};
	//Load Vertex Buffer


	HRESULT hr = this->vertexBuffer.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer. ");
		return false;
	}
	//Load Index buffer
	hr = this->indicesBuffer.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer ");
		return false;
	}

	//Load texture from file
	hr = DirectX::CreateWICTextureFromFile(device.Get(), L"..\\DirectX 11 Engine VS2017\\Texture\\OIP-C.jpg", NULL, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file. ");
		return false;
	}

	//Load constant buffer

	hr = this->constantBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create constant buffer. ");
		return false;
	}
	//Green

	return true;
}
