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


	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	//Before Draw
	this->deviceContext->IASetInputLayout(inputLayout.Get());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	this->deviceContext->Draw(3, 0);

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
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(),NULL);
	
	//creat viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	
	//Set ViewPort
	deviceContext->RSSetViewports(1, &viewport);
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
	{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 ,
	"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0};
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
	//Vertex
	Vertex v[] =
	{
		// must be clockwise
		Vertex(-0.5f, -0.5f,1.0f,0.0f,0.0f),//left bottom
		Vertex(0.0f, 0.5f,0.0f,1.0f,0.0f),//top middle
		Vertex(0.5f, -0.5f,0.0f,0.0f,1.0f)//right middle
		//Vertex(1.0f, 0.1f)
	};
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer. ");
		return false;
	}


	return true;
}
