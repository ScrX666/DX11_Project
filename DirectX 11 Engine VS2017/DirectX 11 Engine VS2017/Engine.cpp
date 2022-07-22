#include "Engine.h"
#include "Graphics.h"
#include "ErrorLogger.h"


bool Engine::Initialize( HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	gtime.Start();

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

	//gfx.model.m_calculatedBoneTransforms.resize(gfx.model.m_numBones);
	//gfx.model.BoneTransform(0.0f, gfx.model.m_calculatedBoneTransforms);;
	//for (UINT i = 0; i < gfx.model.m_numBones; i++)
	//{
	//	m_cBufferFrequently.data.transfomMat[i] = XMLoadFloat4x4(&gfx.model.m_calculatedBoneTransforms[i]);
	//}



	return true;
}
bool Engine::ProcessMessages() 
{
	return render_window.ProcessMessages();
}
void Engine::Update()
{

	gtime.Tick();
	float deltaTime = gtime.GetMilisecondsElapsed();


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


	const float cameraSpeed = 0.1f;
	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed  );
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed );
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed );
		
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed );
	}
	if (keyboard.KeyIsPressed('F'))
	{
		this->gfx.camera.AdjustPosition(0.0f, 0.0f, -0.5f);
		this->gfx.camera.AdjustRotation(0.0f, 0.0f, 0.0f);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetUpVector() * cameraSpeed );
	}
	if (keyboard.KeyIsPressed('E'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetUpVector() * cameraSpeed * -1 );
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetUpVector() * cameraSpeed );
	}

	
	//**************************
	vector<XMFLOAT4X4> arBoneMatrixs;
	
	
	float runningTime = gtime.GetTotalTime();
	double perFrameTime = 1.0f / 60.0f;
	double currtFrame = runningTime / perFrameTime;

	//calculate bone animation
	gfx.skeletonMesh.BoneTransform(runningTime, gfx.skeletonMesh.m_calculatedBoneTransforms);
	
	//for (UINT i = 0; i < gfx.skeletonMesh.m_numBones; i++)
	//{

	//	XMMATRIX XmTransforms = XMLoadFloat4x4(&gfx.skeletonMesh.m_calculatedBoneTransforms[i]);
	//	//XmTransforms = XMMatrixTranspose(XmTransforms);
	//	gfx.skeletonMesh.cb_vertexshader->data.transfomMat[i] = XmTransforms;
	//	
	//}
	//CopyMemory(m_cBufferFrequently.data.transfomMat, gfx.skeletonMesh.m_calculatedBoneTransforms.data(), arBoneMatrixs.size() * sizeof(arBoneMatrixs[0]));
	
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
