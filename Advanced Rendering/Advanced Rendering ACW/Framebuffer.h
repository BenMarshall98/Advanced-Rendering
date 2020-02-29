#pragma once

#include "..\Common\DirectXHelper.h"
#include "..\Common\DeviceResources.h"

class Framebuffer
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mColorTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mColorTextureTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorTextureResourceView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthTextureTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthTextureResourceView = nullptr;

public:
	Framebuffer() = default;
	~Framebuffer() = default;

	Framebuffer(const Framebuffer &) = delete;
	Framebuffer(Framebuffer &&) = delete;
	Framebuffer & operator= (const Framebuffer &) = delete;
	Framebuffer & operator= (Framebuffer &&) = delete;

	bool LoadFramebuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources);

	void UseFramebuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources) const;
	void ReleaseFramebuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources) const;

	void UseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
	void ReleaseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
};

