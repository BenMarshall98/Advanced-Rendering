#pragma once

#include "..\Common\DeviceResources.h"

template <class T>
class ConstantBuffer
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
	
public:
	ConstantBuffer();

	void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources);
	void Reset();
	void UpdateBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, const T & pData);
	void UseVSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
	void UsePSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
	void UseGSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
	void UseHSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
	void UseDSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot);
	
	~ConstantBuffer();
};

template <class T>
ConstantBuffer<T>::ConstantBuffer()
{
}

template<class T>
ConstantBuffer<T>::~ConstantBuffer()
{
	Reset();
}

template<class T>
void ConstantBuffer<T>::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		pDeviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&mConstantBuffer
		)
	);
}

template<class T>
void ConstantBuffer<T>::Reset()
{
	mConstantBuffer.Reset();
}

template<class T>
void ConstantBuffer<T>::UpdateBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, const T & pData)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->UpdateSubresource1(mConstantBuffer.Get(), 0, nullptr, &pData, 0, 0, 0);
}

template<class T>
void ConstantBuffer<T>::UseVSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->VSSetConstantBuffers1(pSlot, 1, mConstantBuffer.GetAddressOf(), nullptr, nullptr);
}

template<class T>
void ConstantBuffer<T>::UsePSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->PSSetConstantBuffers1(pSlot, 1, mConstantBuffer.GetAddressOf(), nullptr, nullptr);
}

template<class T>
void ConstantBuffer<T>::UseGSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->GSSetConstantBuffers1(pSlot, 1, mConstantBuffer.GetAddressOf(), nullptr, nullptr);
}

template<class T>
void ConstantBuffer<T>::UseHSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->HSSetConstantBuffers1(pSlot, 1, mConstantBuffer.GetAddressOf(), nullptr, nullptr);
}

template<class T>
void ConstantBuffer<T>::UseDSBuffer(std::shared_ptr<DX::DeviceResources> pDeviceResources, unsigned int pSlot)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	context->DSSetConstantBuffers1(pSlot, 1, mConstantBuffer.GetAddressOf(), nullptr, nullptr);
}