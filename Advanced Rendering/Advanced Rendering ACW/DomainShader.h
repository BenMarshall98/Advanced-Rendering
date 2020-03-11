#pragma once

#include "ShaderProgram.h"

class DomainShader : public ShaderProgram
{
private:
	Microsoft::WRL::ComPtr<ID3D11DomainShader> mDomainShader;

public:
	DomainShader(const std::wstring & pFilename) : ShaderProgram(pFilename) {};
	~DomainShader() { Reset(); };

	void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void Reset() override;
	void UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void ReleaseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
};

