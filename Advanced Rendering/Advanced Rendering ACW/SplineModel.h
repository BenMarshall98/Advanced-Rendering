#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "..\Common\DirectXHelper.h"
#include "..\Common\DeviceResources.h"

namespace Advanced_Rendering
{
	class SplineModel
	{
		std::vector<DirectX::XMFLOAT3> mPositions;
		std::vector<unsigned int> mIndices;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mPositionBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		unsigned int mIndexCount;

	public:
		SplineModel(const std::string & pFilename);
		~SplineModel();

		void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources);
		void Reset();
		void UseModel(std::shared_ptr<DX::DeviceResources> pDeviceResources);
	};
};