#include "pch.h"
#include "PointModel.h"

using namespace Advanced_Rendering;

PointModel::PointModel(const std::vector<VertexPositionColor> & pVertices, const std::vector<unsigned int> & pIndices) :
	mVertices(pVertices), mIndices(pIndices)
{
}

PointModel::~PointModel()
{
	Reset();
}

void PointModel::Load(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = mVertices.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(mVertices.size() * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);

	DX::ThrowIfFailed(
		pDeviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&mVertexBuffer
		)
	);

	mIndexCount = mIndices.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = mIndices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(mIndices.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		pDeviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&mIndexBuffer
		)
	);
}

void PointModel::Reset()
{
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
}

void PointModel::UseModel(std::shared_ptr<DX::DeviceResources> pDeviceResources)
{
	auto context = pDeviceResources->GetD3DDeviceContext();

	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		mVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		mIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(
		mIndexCount,
		0,
		0
	);
}