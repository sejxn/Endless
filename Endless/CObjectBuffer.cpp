#include "global.h"
#include "CObjectBuffer.h"

#include "CCentral.h"

CObjectBuffer::CObjectBuffer()
	: m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
{
}

CObjectBuffer::~CObjectBuffer()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	if (m_indexBuffer)
		m_indexBuffer->Release();
}

void CObjectBuffer::Init(const vector<ModelType>& _modelVtx, const vector<UINT>& _modelIdx)
{
	ID3D11Device* device = CCentral::Instance().GetDevice();

	UINT vertexCount = (UINT)_modelVtx.size();
	UINT indexCount = (UINT)_modelIdx.size();

	// ���� �迭�� �����.
	VertexType* vertices = new VertexType[vertexCount];

	// �ε��� �迭�� �����.
	unsigned long* indices = new unsigned long[indexCount];

	// ���� �迭�� �ε��� �迭�� �����ͷ� �о�´�.
	for (UINT i = 0; i < vertexCount; ++i)
	{
		vertices[i].position = XMFLOAT3(_modelVtx[i].position.x, _modelVtx[i].position.y, _modelVtx[i].position.z);
		vertices[i].texture = XMFLOAT2(_modelVtx[i].texture.x, _modelVtx[i].texture.y);
		vertices[i].normal = XMFLOAT3(_modelVtx[i].normal.x, _modelVtx[i].normal.y, _modelVtx[i].normal.z);
	}

	for (UINT i = 0; i < indexCount; ++i)
	{
		indices[i] = _modelIdx[i];
	}

	// ���� ���� ������ ����ü ����.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� ������ ����.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	// ���� ���� ����.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// ���� �ε��� ������ ����ü ����.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���ҽ� ����ü�� �ε��� ������ ����.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����
	device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	// �����ǰ� ���� �Ҵ�� ���� �迭 �ε��� �迭 ����
	delete[] vertices;

	delete[] indices;
}

void CObjectBuffer::BufferSetting(ID3D11DeviceContext* _deviceContext)
{
	// ���� ������ ������ ������ ����.
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// ���ؽ� ����, �ε��� ���� ����.
	_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �ﰢ�� ����.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}