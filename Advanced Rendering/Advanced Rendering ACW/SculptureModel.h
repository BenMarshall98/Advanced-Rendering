#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "..\Common\DirectXHelper.h"
#include "..\Common\DeviceResources.h"

namespace Advanced_Rendering
{
	class SculptureModel
	{
		std::vector<DirectX::XMFLOAT3> mPositions;
		std::vector<DirectX::XMFLOAT3> mNormals;
		std::vector<DirectX::XMFLOAT2> mTexCoords;
		std::vector<unsigned int> mIndices;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mPositionBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mNormalBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mTexCoordBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		unsigned int mIndexCount;

	public:
		SculptureModel(const std::string & pFilename);
		~SculptureModel();

		void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources);
		void Reset();
		void UseModel(std::shared_ptr<DX::DeviceResources> pDeviceResources);
	};
}
