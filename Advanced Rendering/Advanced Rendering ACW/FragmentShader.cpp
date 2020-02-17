#include "pch.h"
#include "FragmentShader.h"

void FragmentShader::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto loadPSTask = DX::ReadDataAsync(mFilename);

	auto createPSTask = loadPSTask.then([this, pDeviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			pDeviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShader
			)
		);
	});
}

void FragmentShader::Reset()
{
	mPixelShader.Reset();
}

void FragmentShader::UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto context = pDeviceResources->GetD3DDeviceContext();
	
	// Attach our pixel shader.
	context->PSSetShader(
		mPixelShader.Get(),
		nullptr,
		0
	);
}