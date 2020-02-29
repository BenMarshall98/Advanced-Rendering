#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace Advanced_Rendering;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMFLOAT3 eye = { 0.0f, 0.0f, 5.0f };
	static const XMFLOAT3 at = { 0.0f, 0.0f, 0.0f };
	static const XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };

	mCamera = std::make_unique<Camera>(eye, up, at);

	//XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		mCamera->update(m_constantBufferData, timer);

		Rotate(radians);
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	// Prepare the constant buffer to send it to the graphics device.
	mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
	mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
	mConstantBuffer->UsePSBuffer(m_deviceResources, 0);

	//Ray Tracing
	{
		mRayTracingFramebuffer->UseFramebuffer(m_deviceResources);

		mRayVertexShader->UseProgram(m_deviceResources);
		mRayTracingFragmentShader->UseProgram(m_deviceResources);

		mModel->UseModel(m_deviceResources);

		mRayTracingFramebuffer->ReleaseFramebuffer(m_deviceResources);
	}

	//Ray Marching
	{
		mRayMarchingFramebuffer->UseFramebuffer(m_deviceResources);

		mRayVertexShader->UseProgram(m_deviceResources);
		mRayMarchingFragmentShader->UseProgram(m_deviceResources);

		mModel->UseModel(m_deviceResources);

		mRayTracingFramebuffer->ReleaseFramebuffer(m_deviceResources);
	}

	const auto context = m_deviceResources->GetD3DDeviceContext();

	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	context->PSSetSamplers(0, 1, mSampler.GetAddressOf());

	mRayTracingFramebuffer->UseTexture(m_deviceResources, 0);
	mRayMarchingFramebuffer->UseTexture(m_deviceResources, 2);

	mPingPongVertexShader->UseProgram(m_deviceResources);
	mPingPongFragmentShader->UseProgram(m_deviceResources);

	mModel->UseModel(m_deviceResources);

	mRayTracingFramebuffer->ReleaseTexture(m_deviceResources, 0);
	mRayMarchingFramebuffer->ReleaseTexture(m_deviceResources, 2);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	mRayVertexShader = std::make_unique<VertexShader>(L"RayVertexShader.cso");
	mRayTracingFragmentShader = std::make_unique<FragmentShader>(L"RayTracingPixelShader.cso");
	mRayMarchingFragmentShader = std::make_unique<FragmentShader>(L"RayMarchingPixelShader.cso");
	mPingPongVertexShader = std::make_unique<VertexShader>(L"PingPongVertexShader.cso");
	mPingPongFragmentShader = std::make_unique<FragmentShader>(L"PingPongPixelShader.cso");

	mRayVertexShader->Load(m_deviceResources);
	mRayTracingFragmentShader->Load(m_deviceResources);
	mRayMarchingFragmentShader->Load(m_deviceResources);
	mPingPongVertexShader->Load(m_deviceResources);
	mPingPongFragmentShader->Load(m_deviceResources);

	mRayTracingFramebuffer = std::make_unique<Framebuffer>();
	mRayMarchingFramebuffer = std::make_unique<Framebuffer>();

	mRayTracingFramebuffer->LoadFramebuffer(m_deviceResources);
	mRayMarchingFramebuffer->LoadFramebuffer(m_deviceResources);

	mConstantBuffer = std::make_unique<ConstantBuffer<ModelViewProjectionConstantBuffer>>();
	mConstantBuffer->Load(m_deviceResources);

	// Load mesh vertices. Each vertex has a position and a color.
	static const std::vector<VertexPositionColor> cubeVertices = 
	{
		{XMFLOAT3(-0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		{XMFLOAT3(-0.5f, 0.0f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
		{XMFLOAT3( 0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
		{XMFLOAT3( 0.5f, 0.0f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)}
	};

	// Load mesh indices. Each trio of indices represents
	// a triangle to be rendered on the screen.
	// For example: 0,2,1 means that the vertices with indexes
	// 0, 2 and 1 from the vertex buffer compose the 
	// first triangle of this mesh.
	static const std::vector<unsigned int> cubeIndices =
	{
		0,1,2,
		1,3,2
	};

	mModel = std::make_unique<Model>(cubeVertices, cubeIndices);
	mModel->Load(m_deviceResources);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof samplerDesc);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	m_deviceResources->GetD3DDevice()->CreateSamplerState(&samplerDesc, mSampler.ReleaseAndGetAddressOf());

	// Once the cube is loaded, the object is ready to be rendered.
	m_loadingComplete = true;
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	mRayVertexShader->Reset();
	mRayTracingFragmentShader->Reset();
	mConstantBuffer->Reset();
	mModel->Reset();
}