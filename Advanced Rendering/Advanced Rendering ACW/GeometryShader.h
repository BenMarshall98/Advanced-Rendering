#pragma once

#include "ShaderProgram.h"

class GeometryShader : public ShaderProgram
{
private:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGeometryShader;

public:
	GeometryShader(const std::wstring & pFilename) : ShaderProgram(pFilename) {};
	~GeometryShader() { Reset(); };

	void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void Reset() override;
};

