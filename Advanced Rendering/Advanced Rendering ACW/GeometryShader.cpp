#include "pch.h"
#include "GeometryShader.h"

void GeometryShader::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto loadPSTask = DX::ReadDataAsync(mFilename);

	auto createPSTask = loadPSTask.then([this, pDeviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			pDeviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mGeometryShader
			)
		);
	});
}

void GeometryShader::Reset()
{
	mGeometryShader.Reset();
}