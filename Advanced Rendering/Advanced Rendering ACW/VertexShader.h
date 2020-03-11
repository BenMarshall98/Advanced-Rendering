#pragma once

#include "..\Common\DirectXHelper.h"
#include "ShaderProgram.h"

class VertexShader : public ShaderProgram
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	
public:
	VertexShader(const std::wstring & pFilename) : ShaderProgram(pFilename) {};
	~VertexShader() { Reset(); };

	void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void Reset() override;
	void UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
	void ReleaseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) override;
};

