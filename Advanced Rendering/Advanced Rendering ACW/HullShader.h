#pragma once

#include "ShaderProgram.h"

class HullShader : public ShaderProgram
{
private:
	Microsoft::WRL::ComPtr<ID3D11HullShader> mHullShader;

public:
	HullShader(const std::wstring & pFilename) : ShaderProgram(pFilename) {};
	~HullShader() { Reset(); };

	void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void Reset() override;
};

