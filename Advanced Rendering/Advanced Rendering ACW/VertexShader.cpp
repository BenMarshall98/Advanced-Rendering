#include "pch.h"
#include "VertexShader.h"

void VertexShader::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto loadVSTask = DX::ReadDataAsync(mFilename);

	auto createVSTask = loadVSTask.then([this, pDeviceResources](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			pDeviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				mVertexShader.ReleaseAndGetAddressOf()
			)
		);

		DX::ThrowIfFailed(
			pDeviceResources->GetD3DDevice()->CreateInputLayout(
				this->mLayout.data(),
				this->mLayout.size(),
				&fileData[0],
				fileData.size(),
				mInputLayout.ReleaseAndGetAddressOf()
			)
		);
	});
}

void VertexShader::Reset()
{
	mVertexShader.Reset();
	mInputLayout.Reset();
}

void VertexShader::UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto context = pDeviceResources->GetD3DDeviceContext();
	
	context->IASetInputLayout(mInputLayout.Get());

	context->VSSetShader(mVertexShader.Get(), nullptr, 0);
}

void VertexShader::ReleaseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->IASetInputLayout(nullptr);

	context->VSSetShader(nullptr, nullptr, 0);
}