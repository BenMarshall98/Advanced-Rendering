#include "pch.h"
#include "Texture.h"
#include "DDSTextureLoader.h"
#include <codecvt>

using namespace Advanced_Rendering;

Texture::Texture(const std::string & pTextureFile) : mTextureFile(pTextureFile)
{
}

void Texture::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	const auto temp = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(mTextureFile);
	auto device = pDeviceResources->GetD3DDevice();

	DirectX::CreateDDSTextureFromFile(device, temp.c_str(), nullptr, mTexture.ReleaseAndGetAddressOf());
}

Texture::~Texture()
{
}

void Texture::UseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pIndex) const
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->PSSetShaderResources(pIndex, 1, mTexture.GetAddressOf());
}

void Texture::ReleaseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, const unsigned int pIndex) const
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullTexture = nullptr;
	
	context->DSSetShaderResources(pIndex, 0, nullTexture.GetAddressOf());
}

void Texture::Reset()
{
	mTexture.Reset();
}