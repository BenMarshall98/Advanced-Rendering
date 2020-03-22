#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "Main.h"

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

	m_rayConstantBufferData.aspectRatio = aspectRatio;
	m_rayConstantBufferData.farPlane = 1000.0f;
	m_rayConstantBufferData.nearPlane = 1.0f;
	m_rayConstantBufferData.fov = fovAngleY;
	m_rayConstantBufferData.width = outputSize.Width;
	m_rayConstantBufferData.height = outputSize.Height;

	// Note that the OrientationTrawnsform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.
	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		1.0f,
		1000.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMFLOAT3 eye = { 0.0f, 5.0f, -5.0f };
	static const XMFLOAT3 at = { 0.0f, 5.0f, 0.0f };
	static const XMFLOAT3 up = { 0.0f, -1.0f, 0.0f };

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

		TimeConstantBuffer timeBuffer;

		timeBuffer.time = timer.GetTotalSeconds();

		mTimeConstantBuffer->UpdateBuffer(m_deviceResources, timeBuffer);
		mTimeConstantBuffer->UseGSBuffer(m_deviceResources, 5);
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

	const auto context = m_deviceResources->GetD3DDeviceContext();

	context->RSSetState(m_normalRasterizerState.Get());
	context->PSSetSamplers(0, 1, mSampler.GetAddressOf());
	context->DSSetSamplers(0, 1, mSampler.GetAddressOf());

	m_tessConstantBufferData.tess = Main::tesselation;
	m_tessConstantBufferData.height = Main::height;

	mTessConstantBuffer->UpdateBuffer(m_deviceResources, m_tessConstantBufferData);
	mTessConstantBuffer->UseHSBuffer(m_deviceResources, 1);
	mTessConstantBuffer->UseDSBuffer(m_deviceResources, 1);

	DirectX::XMFLOAT3 position;
	mCamera->getViewPosition(position);

	m_lightConstantBufferData.lightPos = DirectX::XMFLOAT4(position.x, position.y + 5.0f, position.z, 1.0f);
	m_lightConstantBufferData.lightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	mLightConstantBuffer->UpdateBuffer(m_deviceResources, m_lightConstantBufferData);
	mLightConstantBuffer->UsePSBuffer(m_deviceResources, 2);

	// Prepare the constant buffer to send it to the graphics device.
	mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
	mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
	mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
	mConstantBuffer->UsePSBuffer(m_deviceResources, 0);

	mRayConstantBuffer->UpdateBuffer(m_deviceResources, m_rayConstantBufferData);
	mRayConstantBuffer->UsePSBuffer(m_deviceResources, 1);

	

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

	//Tessellated / Geometric Shapes
	{
		mGeometryFramebuffer->UseFramebuffer(m_deviceResources);
		
		mParametricVertexShader->UseProgram(m_deviceResources);
		mParametricHullShader->UseProgram(m_deviceResources);
		mParametricFragmentShader->UseProgram(m_deviceResources);

		context->RSSetState(m_tessRasterizerState.Get());

		mMarbleTexture->UseTexture(m_deviceResources, 0);

		////Sphere
		//{
		//	mParametricSphereDomainShader->UseProgram(m_deviceResources);
		//	
		//	DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 5.0f, 10.0f)));
		//	DirectX::XMStoreFloat4x4(&m_constantBufferData.inverseModel, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0.0f, 5.0f, 10.0f)));
		//
		//	mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
		//	mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UseGSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UsePSBuffer(m_deviceResources, 0);
		//
		//	context->IASetInputLayout(nullptr);
		//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		//
		//	context->Draw(4, 0);
		//}
		//
		////Elipsoid
		//{
		//	mParametricElipsoidDomainShader->UseProgram(m_deviceResources);
		//	
		//	DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 5.0f, 20.0f)));
		//	DirectX::XMStoreFloat4x4(&m_constantBufferData.inverseModel, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0.0f, 5.0f, 20.0f)));
		//
		//	mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
		//	mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UseGSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UsePSBuffer(m_deviceResources, 0);
		//
		//	context->IASetInputLayout(nullptr);
		//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		//
		//	context->Draw(4, 0);
		//}
		//
		////Torus
		//{
		//	mParametricTorusDomainShader->UseProgram(m_deviceResources);
		//
		//	DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 5.0f, 30.0f)));
		//	DirectX::XMStoreFloat4x4(&m_constantBufferData.inverseModel, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0.0f, 5.0f, 30.0f)));
		//
		//	mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
		//	mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UseGSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
		//	mConstantBuffer->UsePSBuffer(m_deviceResources, 0);
		//
		//	context->IASetInputLayout(nullptr);
		//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		//
		//	context->Draw(4, 0);
		//}

		context->RSSetState(m_normalRasterizerState.Get());

		mMarbleTexture->ReleaseTexture(m_deviceResources, 0);

		if (Main::wireframe)
		{
			context->RSSetState(m_wireframeRasterizerState.Get());
		}

		mRockDisplacementTexture->UseDomainTexture(m_deviceResources, 0);
		mRockColorTexture->UseTexture(m_deviceResources, 0);
		mRockNormalTexture->UseTexture(m_deviceResources, 1);

		//Rock1
		{
			DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f), DirectX::XMMatrixTranslation(10.0f, 5.0f, 40.0f))));
			DirectX::XMStoreFloat4x4(&m_constantBufferData.inverseModel, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f), DirectX::XMMatrixTranslation(10.0f, 5.0f, 40.0f))));

			mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
			mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseGSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseHSBuffer(m_deviceResources, 0);
			mConstantBuffer->UsePSBuffer(m_deviceResources, 0);

			mRockVertexShader->UseProgram(m_deviceResources);
			mRockViewHullShader->UseProgram(m_deviceResources);
			mRockDomainShader->UseProgram(m_deviceResources);
			mRockFragmentShader->UseProgram(m_deviceResources);

			mTessModel->UseModel(m_deviceResources);
		}

		//Rock2
		{
			DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f), DirectX::XMMatrixTranslation(-10.0f, 5.0f, 40.0f))));
			DirectX::XMStoreFloat4x4(&m_constantBufferData.inverseModel, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f), DirectX::XMMatrixTranslation(-10.0f, 5.0f, 40.0f))));

			mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
			mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseGSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseHSBuffer(m_deviceResources, 0);
			mConstantBuffer->UsePSBuffer(m_deviceResources, 0);

			mRockVertexShader->UseProgram(m_deviceResources);
			mRockUserHullShader->UseProgram(m_deviceResources);
			mRockDomainShader->UseProgram(m_deviceResources);
			mRockFragmentShader->UseProgram(m_deviceResources);

			mTessModel->UseModel(m_deviceResources);
		}

		mRockDisplacementTexture->ReleaseDomainTexture(m_deviceResources, 0);
		mRockColorTexture->ReleaseTexture(m_deviceResources, 0);
		mRockNormalTexture->ReleaseTexture(m_deviceResources, 1);

		//Vase
		{
			DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 5.0f, 15.0f)));
			DirectX::XMStoreFloat4x4(&m_constantBufferData.inverseModel, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0.0f, 5.0f, 15.0f)));

			mConstantBuffer->UpdateBuffer(m_deviceResources, m_constantBufferData);
			mConstantBuffer->UseVSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseGSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseDSBuffer(m_deviceResources, 0);
			mConstantBuffer->UseHSBuffer(m_deviceResources, 0);
			mConstantBuffer->UsePSBuffer(m_deviceResources, 0);

			mSplineVertexShader->UseProgram(m_deviceResources);
			mSplineHullShader->UseProgram(m_deviceResources);
			mSplineDomainShader->UseProgram(m_deviceResources);
			mSplineFragmentShader->UseProgram(m_deviceResources);

			mMarbleTexture->UseTexture(m_deviceResources, 0);

			mSplineModel->UseModel(m_deviceResources);

			mMarbleTexture->ReleaseTexture(m_deviceResources, 0);
		}

		context->RSSetState(m_normalRasterizerState.Get());
	
		mParametricVertexShader->ReleaseProgram(m_deviceResources);
		mParametricHullShader->ReleaseProgram(m_deviceResources);
		mParametricSphereDomainShader->ReleaseProgram(m_deviceResources);
		mParametricFragmentShader->ReleaseProgram(m_deviceResources);

		//Billboard Soldiers
		{
			mBillboardVertexShader->UseProgram(m_deviceResources);
			mBillboardGeometryShader->UseProgram(m_deviceResources);
			mBillboardFragmentShader->UseProgram(m_deviceResources);

			mSoldierTexture->UseTexture(m_deviceResources, 0);

			mPointModel->UseModel(m_deviceResources);

			mSoldierTexture->ReleaseTexture(m_deviceResources, 0);

			mBillboardVertexShader->ReleaseProgram(m_deviceResources);
			mBillboardGeometryShader->ReleaseProgram(m_deviceResources);
			mBillboardFragmentShader->ReleaseProgram(m_deviceResources);
		}

		//Flags
		{
			mBillboardVertexShader->UseProgram(m_deviceResources);
			mFlagGeometryShader->UseProgram(m_deviceResources);
			mBillboardFragmentShader->UseProgram(m_deviceResources);

			mFlagTexture->UseTexture(m_deviceResources, 0);

			mFlagModel->UseModel(m_deviceResources);

			mFlagTexture->ReleaseTexture(m_deviceResources, 0);

			mBillboardVertexShader->ReleaseProgram(m_deviceResources);
			mFlagGeometryShader->ReleaseProgram(m_deviceResources);
			mBillboardFragmentShader->ReleaseProgram(m_deviceResources);
		}

		//Billboard Sculpture
		{
			mSculptureVertexShader->UseProgram(m_deviceResources);
			mSculptureGeometryShader->UseProgram(m_deviceResources);
			mSculptureFragmentShader->UseProgram(m_deviceResources);

			mMarbleTexture->UseTexture(m_deviceResources, 0);

			mSculptureModel->UseModel(m_deviceResources);

			mMarbleTexture->ReleaseTexture(m_deviceResources, 0);

			mSculptureVertexShader->ReleaseProgram(m_deviceResources);
			mSculptureGeometryShader->ReleaseProgram(m_deviceResources);
			mSculptureFragmentShader->ReleaseProgram(m_deviceResources);
		}

		//Billboard Sculpture
		{
			mSculptureVertexShader->UseProgram(m_deviceResources);
			mPoleGeometryShader->UseProgram(m_deviceResources);
			mSculptureFragmentShader->UseProgram(m_deviceResources);

			mMarbleTexture->UseTexture(m_deviceResources, 0);

			mPoleModel->UseModel(m_deviceResources);

			mMarbleTexture->ReleaseTexture(m_deviceResources, 0);

			mSculptureVertexShader->ReleaseProgram(m_deviceResources);
			mPoleGeometryShader->ReleaseProgram(m_deviceResources);
			mSculptureFragmentShader->ReleaseProgram(m_deviceResources);
		}

	
		mGeometryFramebuffer->ReleaseFramebuffer(m_deviceResources);
	}
	

	mPingPongFramebuffer1->UseFramebuffer(m_deviceResources);

	mRayTracingFramebuffer->UseTexture(m_deviceResources, 0);
	mRayMarchingFramebuffer->UseTexture(m_deviceResources, 2);

	mPingPongVertexShader->UseProgram(m_deviceResources);
	mPingPongFragmentShader->UseProgram(m_deviceResources);

	mModel->UseModel(m_deviceResources);

	mRayTracingFramebuffer->ReleaseTexture(m_deviceResources, 0);
	mRayMarchingFramebuffer->ReleaseTexture(m_deviceResources, 2);

	mPingPongFramebuffer1->ReleaseFramebuffer(m_deviceResources);

	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	mPingPongFramebuffer1->UseTexture(m_deviceResources, 0);
	mGeometryFramebuffer->UseTexture(m_deviceResources, 2);

	mPingPongVertexShader->UseProgram(m_deviceResources);
	mPingPongFragmentShader2->UseProgram(m_deviceResources);

	mModel->UseModel(m_deviceResources);

	mPingPongFramebuffer1->ReleaseTexture(m_deviceResources, 0);
	mGeometryFramebuffer->ReleaseTexture(m_deviceResources, 2);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);

	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, m_tessRasterizerState.GetAddressOf());
	
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, m_normalRasterizerState.GetAddressOf());

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, m_wireframeRasterizerState.GetAddressOf());

	std::vector<D3D11_INPUT_ELEMENT_DESC> normalInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mRayVertexShader = std::make_unique<VertexShader>(L"RayVertexShader.cso", normalInputLayout);
	mRayTracingFragmentShader = std::make_unique<FragmentShader>(L"RayTracingPixelShader.cso");
	mRayMarchingFragmentShader = std::make_unique<FragmentShader>(L"RayMarchingPixelShader.cso");
	mPingPongVertexShader = std::make_unique<VertexShader>(L"PingPongVertexShader.cso", normalInputLayout);
	mPingPongFragmentShader = std::make_unique<FragmentShader>(L"PingPongPixelShader.cso");
	mPingPongFragmentShader2 = std::make_unique<FragmentShader>(L"PingPongPixelShader2.cso");

	mRayVertexShader->Load(m_deviceResources);
	mRayTracingFragmentShader->Load(m_deviceResources);
	mRayMarchingFragmentShader->Load(m_deviceResources);
	mPingPongVertexShader->Load(m_deviceResources);
	mPingPongFragmentShader->Load(m_deviceResources);
	mPingPongFragmentShader2->Load(m_deviceResources);

	mParametricVertexShader = std::make_unique<VertexShader>(L"ParametricVertexShader.cso", normalInputLayout);
	mParametricHullShader = std::make_unique<HullShader>(L"ParametricHullShader.cso");
	mParametricSphereDomainShader = std::make_unique<DomainShader>(L"ParametricSphereDomainShader.cso");
	mParametricElipsoidDomainShader = std::make_unique<DomainShader>(L"ParametricElipsoidDomainShader.cso");
	mParametricTorusDomainShader = std::make_unique<DomainShader>(L"ParametricTorusDomainShader.cso");
	mParametricFragmentShader = std::make_unique<FragmentShader>(L"ParametricFragmentShader.cso");

	mParametricVertexShader->Load(m_deviceResources);
	mParametricHullShader->Load(m_deviceResources);
	mParametricSphereDomainShader->Load(m_deviceResources);
	mParametricElipsoidDomainShader->Load(m_deviceResources);
	mParametricTorusDomainShader->Load(m_deviceResources);
	mParametricFragmentShader->Load(m_deviceResources);

	std::vector<D3D11_INPUT_ELEMENT_DESC> tessInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mRockVertexShader = std::make_unique<VertexShader>(L"RockVertexShader.cso", tessInputLayout);
	mRockUserHullShader = std::make_unique<HullShader>(L"RockUserHullShader.cso");
	mRockViewHullShader = std::make_unique<HullShader>(L"RockViewHullShader.cso");
	mRockDomainShader = std::make_unique<DomainShader>(L"RockDomainShader.cso");
	mRockFragmentShader = std::make_unique<FragmentShader>(L"RockPixelShader.cso");

	mRockVertexShader->Load(m_deviceResources);
	mRockUserHullShader->Load(m_deviceResources);
	mRockViewHullShader->Load(m_deviceResources);
	mRockDomainShader->Load(m_deviceResources);
	mRockFragmentShader->Load(m_deviceResources);

	std::vector<D3D11_INPUT_ELEMENT_DESC> splineInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mSplineVertexShader = std::make_unique<VertexShader>(L"SplineVertexShader.cso", splineInputLayout);
	mSplineHullShader = std::make_unique<HullShader>(L"SplineHullShader.cso");
	mSplineDomainShader = std::make_unique<DomainShader>(L"SplineDomainShader.cso");
	mSplineFragmentShader = std::make_unique<FragmentShader>(L"SplinePixelShader.cso");

	mSplineVertexShader->Load(m_deviceResources);
	mSplineHullShader->Load(m_deviceResources);
	mSplineDomainShader->Load(m_deviceResources);
	mSplineFragmentShader->Load(m_deviceResources);

	mBillboardVertexShader = std::make_unique<VertexShader>(L"BillboardVertexShader.cso", normalInputLayout);
	mBillboardGeometryShader = std::make_unique<GeometryShader>(L"BillboardGeometryShader.cso");
	mBillboardFragmentShader = std::make_unique<FragmentShader>(L"BillboardPixelShader.cso");

	mBillboardVertexShader->Load(m_deviceResources);
	mBillboardGeometryShader->Load(m_deviceResources);
	mBillboardFragmentShader->Load(m_deviceResources);

	mFlagGeometryShader = std::make_unique<GeometryShader>(L"FlagGeometryShader.cso");
	mFlagGeometryShader->Load(m_deviceResources);

	std::vector<D3D11_INPUT_ELEMENT_DESC> sculptureInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mSculptureVertexShader = std::make_unique<VertexShader>(L"SculptureVertexShader.cso", sculptureInputLayout);
	mSculptureGeometryShader = std::make_unique<GeometryShader>(L"SculptureGeometryShader.cso");
	mPoleGeometryShader = std::make_unique<GeometryShader>(L"PoleGeometryShader.cso");
	mSculptureFragmentShader = std::make_unique<FragmentShader>(L"SculpturePixelShader.cso");

	mSculptureVertexShader->Load(m_deviceResources);
	mSculptureGeometryShader->Load(m_deviceResources);
	mPoleGeometryShader->Load(m_deviceResources);
	mSculptureFragmentShader->Load(m_deviceResources);

	mRayTracingFramebuffer = std::make_unique<Framebuffer>();
	mRayMarchingFramebuffer = std::make_unique<Framebuffer>();
	mGeometryFramebuffer = std::make_unique<Framebuffer>();

	mRayTracingFramebuffer->LoadFramebuffer(m_deviceResources);
	mRayMarchingFramebuffer->LoadFramebuffer(m_deviceResources);
	mGeometryFramebuffer->LoadFramebuffer(m_deviceResources);

	mPingPongFramebuffer1 = std::make_unique<Framebuffer>();
	mPingPongFramebuffer2 = std::make_unique<Framebuffer>();

	mPingPongFramebuffer1->LoadFramebuffer(m_deviceResources);
	mPingPongFramebuffer2->LoadFramebuffer(m_deviceResources);

	mConstantBuffer = std::make_unique<ConstantBuffer<ModelViewProjectionConstantBuffer>>();
	mConstantBuffer->Load(m_deviceResources);

	mRayConstantBuffer = std::make_unique<ConstantBuffer<RayConstantBuffer>>();
	mRayConstantBuffer->Load(m_deviceResources);

	mTessConstantBuffer = std::make_unique<ConstantBuffer<TessConstantBuffer>>();
	mTessConstantBuffer->Load(m_deviceResources);

	mLightConstantBuffer = std::make_unique<ConstantBuffer<LightConstantBuffer>>();
	mLightConstantBuffer->Load(m_deviceResources);

	mTimeConstantBuffer = std::make_unique<ConstantBuffer<TimeConstantBuffer>>();
	mTimeConstantBuffer->Load(m_deviceResources);

	mRockColorTexture = std::make_unique<Texture>("Texture.DDS");
	mRockColorTexture->Load(m_deviceResources);

	mRockDisplacementTexture = std::make_unique<Texture>("Displacement.DDS");
	mRockDisplacementTexture->Load(m_deviceResources);

	mRockNormalTexture = std::make_unique<Texture>("Normal map.DDS");
	mRockNormalTexture->Load(m_deviceResources);

	mSoldierTexture = std::make_unique<Texture>("Soldier.DDS");
	mSoldierTexture->Load(m_deviceResources);

	mMarbleTexture = std::make_unique<Texture>("Marble.DDS");
	mMarbleTexture->Load(m_deviceResources);

	mFlagTexture = std::make_unique<Texture>("Flag.DDS");
	mFlagTexture->Load(m_deviceResources);

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

	std::vector<VertexPositionColor> pointVertices;
	std::vector<unsigned int> pointIndices;

	for (int i = 55; i < 150; i += 5)
	{
		for (int j = 5; j < 100; j += 5)
		{
			pointVertices.push_back({ XMFLOAT3(i, 0.0f, j), XMFLOAT3(0.0f, 0.0f, 0.0f) });
			pointVertices.push_back({ XMFLOAT3(-i, 0.0f, j), XMFLOAT3(0.0f, 0.0f, 0.0f) });

			pointIndices.push_back(pointIndices.size());
			pointIndices.push_back(pointIndices.size());
		}
	}

	mPointModel = std::make_unique<PointModel>(pointVertices, pointIndices);
	mPointModel->Load(m_deviceResources);

	std::vector<VertexPositionColor> flagVertices;
	std::vector<unsigned int> flagIndices;

	for (int i = 0; i <= 100; i += 20)
	{
		flagVertices.push_back({ XMFLOAT3(20, 5.0f, i), XMFLOAT3(0.0f, 0.0f, 0.0f) });
		flagVertices.push_back({ XMFLOAT3(-20, 5.0f, i), XMFLOAT3(0.0f, 0.0f, 0.0f) });
		flagIndices.push_back(flagIndices.size());
		flagIndices.push_back(flagIndices.size());
	}

	mFlagModel = std::make_unique<PointModel>(flagVertices, flagIndices);
	mFlagModel->Load(m_deviceResources);

	mTessModel = std::make_unique<TessModel>("rock.sim");
	mTessModel->Load(m_deviceResources);

	mSculptureModel = std::make_unique<SculptureModel>("Sculpture.sim");
	mSculptureModel->Load(m_deviceResources);

	mPoleModel = std::make_unique<SculptureModel>("Cylinder.sim");
	mPoleModel->Load(m_deviceResources);

	mSplineModel = std::make_unique<SplineModel>("vase.cur");
	mSplineModel->Load(m_deviceResources);

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