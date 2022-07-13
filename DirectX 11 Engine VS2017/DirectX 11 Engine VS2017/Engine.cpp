#include "Engine.h"
#include "Graphics.h"
#include "ErrorLogger.h"


bool Engine::Initialize( HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.Start();
	bool InitializeWindowSuccessfully =	render_window.Initialize(hInstance, window_title, window_class, width, height);
	if (!InitializeWindowSuccessfully)
	{
		return false;
	}

	bool InitializeGraphicsSuccessfully = gfx.Graphics::Initialize(render_window.GetWindowHandle() , width, height);
	if(!InitializeGraphicsSuccessfully)
	{
		return false;
	}

	return true;
}
bool Engine::ProcessMessages() 
{
	return render_window.ProcessMessages();
}
void Engine::Update()
{
	float deltaTime = timer.GetMilisecondsElapsed();
	timer.Restart();



	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvents kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}
	}


	const float cameraSpeed = 0.01f;
	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * deltaTime);
		
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('F'))
	{
		this->gfx.camera.AdjustPosition(0.0f, 0.0f, -0.5f);
		this->gfx.camera.AdjustRotation(0.0f, 0.0f, 0.0f);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetUpVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetUpVector() * cameraSpeed * -1 * deltaTime);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetUpVector() * cameraSpeed * deltaTime);
	}

	
	//**************************
	/*
	float runningTime = 1;//timer.TotalTime();

	double perFrameTime = 1.0f / 60.0f;
	float currentFrames = runningTime / perFrameTime;

	

	gfx.model.Transforms.resize(gfx.model.m_NumBone);


	gfx.model.BoneTransform(runningTime, gfx.model.Transforms);;
	for (UINT i = 0; i < gfx.model.m_NumBone; i++)
	{
		m_cBufferFrequently.boneTransform[i] = XMLoadFloat4x4(&gfx.model.Transforms[i]);
	}

	//calculate bone animation
	gfx.model.BoneTransform(runningTime, gfx.model.Transforms);;
	for (UINT i = 0; i < gfx.model.m_NumBone; i++)
	{
		m_cBufferFrequently.boneTransform[i] = XMLoadFloat4x4(&gfx.model.Transforms[i]);
	}

	m_pConstantBuffer->Initialize(m_pd3dDevice.Get(), m_pd3dImmediateContext.Get());
	
	this->m_cBufferFrequently.view = camera.GetViewMatrix();
	this->m_cBufferFrequently.world = camera.GetWorldMatrix();
	D3D11_MAPPED_SUBRESOURCE mappedData;

	HRESULT HR(m_pd3dImmediateContext->Map(m_pConstantBuffer[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBufferReFreashFrequently), &m_cBufferFrequently, sizeof(CBufferReFreashFrequently));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer[0].Get(), 0);


	this->m_pConstantBuffer->data.mat = XMMatrixTranspose(this->m_pConstantBuffer->data.mat);
	this->m_pConstantBuffer->ApplyChanges();
	*/
}

void Engine::RenderFrame()
{
	gfx.RenderFrame();
}

Engine& Engine::Get()
{
	static Engine engineInstance;

	return engineInstance;
}
