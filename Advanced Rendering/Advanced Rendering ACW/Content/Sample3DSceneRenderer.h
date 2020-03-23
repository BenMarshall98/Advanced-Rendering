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
#include "GeometryShader.h"
#include "PointModel.h"
#include "TessModel.h"
#include "Texture.h"
#include "SplineModel.h"
#include "SculptureModel.h"

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
		std::unique_ptr<FragmentShader> mPingPongFragmentShader2;

		std::unique_ptr<Framebuffer> mPingPongFramebuffer1;
		std::unique_ptr<Framebuffer> mPingPongFramebuffer2;

		std::unique_ptr<Model> mModel;
		std::unique_ptr<PointModel> mPointModel;
		std::unique_ptr<PointModel> mFlagModel;
		std::unique_ptr<PointModel> mCloudModel;
		std::unique_ptr<TessModel> mTessModel;
		std::unique_ptr<SculptureModel> mSculptureModel;
		std::unique_ptr<SculptureModel> mPoleModel;
		std::unique_ptr<SplineModel> mSplineModel;
		std::unique_ptr<ConstantBuffer<ModelViewProjectionConstantBuffer>> mConstantBuffer;
		std::unique_ptr<ConstantBuffer<RayConstantBuffer>> mRayConstantBuffer;
		std::unique_ptr<ConstantBuffer<TessConstantBuffer>> mTessConstantBuffer;
		std::unique_ptr<ConstantBuffer<LightConstantBuffer>> mLightConstantBuffer;
		std::unique_ptr<ConstantBuffer<TimeConstantBuffer>> mTimeConstantBuffer;

		std::unique_ptr<VertexShader> mParametricVertexShader;
		std::unique_ptr<HullShader> mParametricHullShader;
		std::unique_ptr<DomainShader> mParametricSphereDomainShader;
		std::unique_ptr<DomainShader> mParametricElipsoidDomainShader;
		std::unique_ptr<DomainShader> mParametricTorusDomainShader;
		std::unique_ptr<FragmentShader> mParametricFragmentShader;

		std::unique_ptr<VertexShader> mRockVertexShader;
		std::unique_ptr<HullShader> mRockUserHullShader;
		std::unique_ptr<HullShader> mRockViewHullShader;
		std::unique_ptr<DomainShader> mRockDomainShader;
		std::unique_ptr<FragmentShader> mRockFragmentShader;

		std::unique_ptr<VertexShader> mSplineVertexShader;
		std::unique_ptr<HullShader> mSplineHullShader;
		std::unique_ptr<DomainShader> mSplineDomainShader;
		std::unique_ptr<FragmentShader> mSplineFragmentShader;

		std::unique_ptr<VertexShader> mBillboardVertexShader;
		std::unique_ptr<GeometryShader> mBillboardGeometryShader;
		std::unique_ptr<FragmentShader> mBillboardFragmentShader;

		std::unique_ptr<GeometryShader> mFlagGeometryShader;

		std::unique_ptr<VertexShader> mCloudVertexShader;
		std::unique_ptr<GeometryShader> mCloudGeometryShader;
		std::unique_ptr<FragmentShader> mCloudFragmentShader;

		std::unique_ptr<VertexShader> mSculptureVertexShader;
		std::unique_ptr<GeometryShader> mSculptureGeometryShader;
		std::unique_ptr<GeometryShader> mPoleGeometryShader;
		std::unique_ptr<FragmentShader> mSculptureFragmentShader;

		std::unique_ptr<Framebuffer> mGeometryFramebuffer;

		std::unique_ptr<Texture> mRockColorTexture;
		std::unique_ptr<Texture> mRockDisplacementTexture;
		std::unique_ptr<Texture> mRockNormalTexture;
		std::unique_ptr<Texture> mSoldierTexture;
		std::unique_ptr<Texture> mMarbleTexture;
		std::unique_ptr<Texture> mFlagTexture;
		std::unique_ptr<Texture> mCloudTexture;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_normalRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_tessRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		RayConstantBuffer m_rayConstantBufferData;
		TessConstantBuffer m_tessConstantBufferData;
		LightConstantBuffer m_lightConstantBufferData;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

