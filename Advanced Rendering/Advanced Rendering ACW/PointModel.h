#pragma once

#include <vector>
#include "Content/ShaderStructures.h"
#include "..\Common\DirectXHelper.h"
#include "..\Common\DeviceResources.h"

namespace Advanced_Rendering
{
	class PointModel
	{
		std::vector<VertexPositionColor> mVertices;
		std::vector<unsigned int> mIndices;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		unsigned int mIndexCount;

	public:
		PointModel(const std::vector<VertexPositionColor> & pVertices, const std::vector<unsigned int> & pIndices);
		~PointModel();

		void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources);
		void Reset();
		void UseModel(std::shared_ptr<DX::DeviceResources> pDeviceResources);
	};
}
