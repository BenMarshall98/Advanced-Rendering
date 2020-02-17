#include "pch.h"
#include "DomainShader.h"

void DomainShader::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto loadDSTask = DX::ReadDataAsync(mFilename);

	auto createDSTask = loadDSTask.then([this, pDeviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			pDeviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mDomainShader
			)
		);
	});
}

void DomainShader::Reset()
{
	mDomainShader.Reset();
}