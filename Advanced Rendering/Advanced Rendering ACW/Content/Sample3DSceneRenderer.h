#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "FragmentShader.h"
#include "Model.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "Framebuffer.h"

namespace Advanced_Rendering
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }

		std::unique_ptr<Camera> mCamera; //TODO: Move


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		std::unique_ptr<VertexShader> mRayVertexShader;
		std::unique_ptr<FragmentShader> mRayMarchingFragmentShader;
		std::unique_ptr<FragmentShader> mRayTracingFragmentShader;

		std::unique_ptr<Framebuffer> mRayTracingFramebuffer;
		std::unique_ptr<Framebuffer> mRayMarchingFramebuffer;

		std::unique_ptr<VertexShader> mPingPongVertexShader;
		std::unique_ptr<FragmentShader> mPingPongFragmentShader;

		std::unique_ptr<Framebuffer> mPingPongFramebuffer1;
		std::unique_ptr<Framebuffer> mPingPongFramebuffer2;

		std::unique_ptr<Model> mModel;
		std::unique_ptr<ConstantBuffer<ModelViewProjectionConstantBuffer>> mConstantBuffer;

		std::unique_ptr<VertexShader> mParametricVertexShader;
		std::unique_ptr<HullShader> mParametricHullShader;
		std::unique_ptr<DomainShader> mParametricSphereDomainShader;
		std::unique_ptr<DomainShader> mParametricElipsoidDomainShader;
		std::unique_ptr<DomainShader> mParametricTorusDomainShader;
		std::unique_ptr<DomainShader> mParametricKleinDomainShader;
		std::unique_ptr<FragmentShader> mParametricFragmentShader;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;


		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

