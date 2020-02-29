#pragma once

#include "..\Common\DirectXHelper.h"
#include "..\Common\DeviceResources.h"

class ShaderProgram
{
protected:
	std::wstring mFilename;

public:
	ShaderProgram(const std::wstring & pFilename);
	virtual ~ShaderProgram();

	virtual void Load(std::shared_ptr<DX::DeviceResources> pDeviceResources) = 0;
	virtual void Reset() = 0;
	virtual void UseProgram(std::shared_ptr<DX::DeviceResources> pDeviceResources) = 0;
};

