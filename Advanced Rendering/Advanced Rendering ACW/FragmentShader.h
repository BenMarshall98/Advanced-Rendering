#pragma once

#include "ShaderProgram.h"

class FragmentShader : public ShaderProgram
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;

public:
	FragmentShader(const std::wstring & pFilename) : ShaderProgram(pFilename) {};
	~FragmentShader() { Reset(); };

	void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void Reset() override;
	void UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void ReleaseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
};

