#include "pch.h"
#include "SculptureModel.h"

#include <fstream>

using namespace Advanced_Rendering;

SculptureModel::SculptureModel(const std::string & pFilename)
{
	std::ifstream myfile;
	myfile.open(pFilename);

	int size;
	myfile >> size;

	mPositions.resize(size);
	mTexCoords.resize(size);
	mNormals.resize(size);

	for (int i = 0; i < mPositions.size(); i++)
	{
		myfile >> mPositions[i].x >> mPositions[i].y >> mPositions[i].z;
		myfile >> mTexCoords[i].x >> mTexCoords[i].y;
		myfile >> mNormals[i].x >> mNormals[i].y >> mNormals[i].z;
	}

	myfile >> size;

	mIndices.resize(size);

	for (int i = 0; i < mIndices.size(); i++)
	{
		myfile >> mIndices[i];
	}
}

SculptureModel::~SculptureModel()
{
}

void SculptureModel::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
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

	//Create Normal Buffer
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * mNormals.size();
	initData.pSysMem = mNormals.data();

	device->CreateBuffer(&bufferDesc, &initData, &mNormalBuffer);

	//Create TexCoord Buffer
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT2) * mTexCoords.size();
	initData.pSysMem = mTexCoords.data();

	device->CreateBuffer(&bufferDesc, &initData, &mTexCoordBuffer);

	//Create Index Buffer
	bufferDesc.ByteWidth = sizeof(unsigned int) * mIndices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&initData, sizeof initData);
	initData.pSysMem = mIndices.data();

	device->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
}

void SculptureModel::Reset()
{
	mPositionBuffer.Reset();
	mNormalBuffer.Reset();
	mTexCoordBuffer.Reset();
	mIndexBuffer.Reset();
}

void SculptureModel::UseModel(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto deviceContext = pDeviceResources->GetD3DDeviceContext();

	const auto numberBuffers = 3;

	std::vector<ID3D11Buffer *> bufferArray;
	bufferArray.emplace_back(mPositionBuffer.Get());
	bufferArray.emplace_back(mNormalBuffer.Get());
	bufferArray.emplace_back(mTexCoordBuffer.Get());


	std::vector<UINT> strideArray;
	strideArray.emplace_back(sizeof(DirectX::XMFLOAT3));
	strideArray.emplace_back(sizeof(DirectX::XMFLOAT3));
	strideArray.emplace_back(sizeof(DirectX::XMFLOAT2));

	std::vector<UINT> offsetArray;
	offsetArray.emplace_back(0);
	offsetArray.emplace_back(0);
	offsetArray.emplace_back(0);

	deviceContext->IASetVertexBuffers(0, numberBuffers, bufferArray.data(), strideArray.data(), offsetArray.data());
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->DrawIndexed(mIndexCount, 0, 0);
}