#include "pch.h"
#include "HullShader.h"

void HullShader::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto loadPSTask = DX::ReadDataAsync(mFilename);

	auto createPSTask = loadPSTask.then([this, pDeviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			pDeviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mHullShader
			)
		);
	});
}

void HullShader::Reset()
{
	mHullShader.Reset();
}

void HullShader::UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->HSSetShader(
		mHullShader.Get(),
		nullptr,
		0
	);
}