#include "pch.h"
#include "Framebuffer.h"

bool Framebuffer::LoadFramebuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	const auto width = pDeviceResources->GetOutputSize().Width;
	const auto height = pDeviceResources->GetOutputSize().Height;

	const auto device = pDeviceResources->GetD3DDevice();

	D3D11_TEXTURE2D_DESC renderTextureDesc;
	ZeroMemory(&renderTextureDesc, sizeof D3D11_TEXTURE2D_DESC);
	renderTextureDesc.Width = width;
	renderTextureDesc.Height = height;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.MiscFlags = 0;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;

	auto result = device->CreateTexture2D(&renderTextureDesc, nullptr, mColorTexture.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof D3D11_RENDER_TARGET_VIEW_DESC);
	renderTargetViewDesc.Format = renderTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateRenderTargetView(mColorTexture.Get(), &renderTargetViewDesc, mColorTextureTargetView.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof D3D11_SHADER_RESOURCE_VIEW_DESC);
	shaderResourceViewDesc.Format = renderTextureDesc.Format;

	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(mColorTexture.Get(), &shaderResourceViewDesc, mColorTextureResourceView.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof D3D11_TEXTURE2D_DESC);
	depthTextureDesc.Width = width;
	depthTextureDesc.Height = height;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.MiscFlags = 0;
	depthTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTextureDesc.CPUAccessFlags = 0;

	result = device->CreateTexture2D(&depthTextureDesc, nullptr, mDepthTexture.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStateDesc;
	ZeroMemory(&depthStateDesc, sizeof D3D11_DEPTH_STENCIL_DESC);
	depthStateDesc.DepthEnable = false;
	depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	depthStateDesc.StencilEnable = false;
	depthStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	
	depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	result = device->CreateDepthStencilState(&depthStateDesc, mDepthState.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
	ZeroMemory(&depthViewDesc, sizeof D3D11_DEPTH_STENCIL_VIEW_DESC);
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.Flags = 0;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(mDepthTexture.Get(), &depthViewDesc, mDepthTextureTargetView.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&shaderResourceViewDesc, sizeof D3D11_SHADER_RESOURCE_VIEW_DESC);
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(mDepthTexture.Get(), &shaderResourceViewDesc, mDepthTextureResourceView.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Framebuffer::UseFramebuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources) const
{
	const auto deviceContext = pDeviceResources->GetD3DDeviceContext();

	static float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	deviceContext->ClearRenderTargetView(mColorTextureTargetView.Get(), clearColor);
	deviceContext->ClearDepthStencilView(mDepthTextureTargetView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

	deviceContext->OMSetDepthStencilState(mDepthState.Get(), 0);

	deviceContext->OMSetRenderTargets(1, mColorTextureTargetView.GetAddressOf(), mDepthTextureTargetView.Get());
}

void Framebuffer::ReleaseFramebuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources) const
{
	const auto deviceContext = pDeviceResources->GetD3DDeviceContext();

	deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void Framebuffer::UseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	const auto deviceContext = pDeviceResources->GetD3DDeviceContext();

	deviceContext->PSSetShaderResources(pSlot, 1, mColorTextureResourceView.GetAddressOf());
	deviceContext->PSSetShaderResources(pSlot + 1, 1, mDepthTextureResourceView.GetAddressOf());
}

void Framebuffer::ReleaseTexture(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	const auto deviceContext = pDeviceResources->GetD3DDeviceContext();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView = nullptr;

	deviceContext->PSSetShaderResources(pSlot, 1, shaderResourceView.GetAddressOf());
	deviceContext->PSSetShaderResources(pSlot + 1, 1, shaderResourceView.GetAddressOf());
}