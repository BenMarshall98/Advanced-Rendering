#include "pch.h"
#include "SplineModel.h"

#include <fstream>

using namespace Advanced_Rendering;

SplineModel::SplineModel(const std::string & pFilename)
{
	std::ifstream myfile;
	myfile.open(pFilename);

	int size;
	myfile >> size;

	size *= 4;

	mPositions.resize(size);
	mBitangents.resize(size);
	mIndices.resize(size);

	for (int i = 0; i < mPositions.size(); i++)
	{
		myfile >> mPositions[i].x >> mPositions[i].y >> mPositions[i].z;
		myfile >> mBitangents[i].x >> mBitangents[i].y >> mBitangents[i].z;

		mIndices[i] = i;
	}
}

SplineModel::~SplineModel()
{
}

void SplineModel::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	mIndexCount = mIndices.size();

	auto device = pDeviceResources->GetD3DDevice();

	//None changing data for buffers;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof bufferDesc);

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof initData);

	//Create Position Buffer
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * mPositions.size();
	initData.pSysMem = mPositions.data();

	device->CreateBuffer(&bufferDesc, &initData, &mPositionBuffer);
	
	//Create BiTangent Buffer
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * mBitangents.size();
	initData.pSysMem = mBitangents.data();

	device->CreateBuffer(&bufferDesc, &initData, &mBiTangentBuffer);

	//Create Index Buffer
	bufferDesc.ByteWidth = sizeof(unsigned int) * mIndices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&initData, sizeof initData);
	initData.pSysMem = mIndices.data();

	device->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
}

void SplineModel::Reset()
{
	mPositionBuffer.Reset();
	mBiTangentBuffer.Reset();
	mIndexBuffer.Reset();
}

void SplineModel::UseModel(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto deviceContext = pDeviceResources->GetD3DDeviceContext();

	const auto numberBuffers = 2;

	std::vector<ID3D11Buffer *> bufferArray;
	bufferArray.emplace_back(mPositionBuffer.Get());
	bufferArray.emplace_back(mBiTangentBuffer.Get());

	std::vector<UINT> strideArray;
	strideArray.emplace_back(sizeof(DirectX::XMFLOAT3));
	strideArray.emplace_back(sizeof(DirectX::XMFLOAT3));

	std::vector<UINT> offsetArray;
	offsetArray.emplace_back(0);
	offsetArray.emplace_back(0);

	deviceContext->IASetVertexBuffers(0, numberBuffers, bufferArray.data(), strideArray.data(), offsetArray.data());
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	deviceContext->DrawIndexed(mIndexCount, 0, 0);
}