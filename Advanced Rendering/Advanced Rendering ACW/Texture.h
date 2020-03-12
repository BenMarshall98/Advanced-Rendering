#pragma once
#include <string>
#include <d3d11.h>
#include <wrl/client.h>

#include "..\Common\DirectXHelper.h"
#include "..\Common\DeviceResources.h"

namespace Advanced_Rendering
{
	class Texture
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture = nullptr;
		std::string mTextureFile;

	public:
		Texture(const std::string & pTextureFile);
		~Texture();

		Texture(const Texture &) = delete;
		Texture(Texture &&) = delete;
		Texture & operator= (const Texture &) = delete;
		Texture & operator= (Texture &&) = delete;

		void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources);
		void UseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pIndex) const;
		void ReleaseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pIndex) const;

		void UseDomainTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pIndex) const;
		void ReleaseDomainTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pIndex) const;
		void Reset();
	};
}