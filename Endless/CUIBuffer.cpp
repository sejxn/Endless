#include "global.h"
#include "CUIBuffer.h"

#include "CCentral.h"

#include "CUI.h"

CUIBuffer::CUIBuffer()
	: m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_prevPosX(-1.0f)
	, m_prevPosY(-1.0f)
	, m_screenWidth(0)
	, m_screenHeight(0)
{
	m_screenWidth = CCentral::Instance().GetScreenWidth();
	m_screenHeight = CCentral::Instance().GetScreenHeight();
}

CUIBuffer::~CUIBuffer()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	if (m_indexBuffer)
		m_indexBuffer->Release();
}

void CUIBuffer::Init(UINT _vertexCount, UINT _indexCount)
{
	ID3D11Device* device = CCentral::Instance().GetDevice();

	// ���� �迭�� �����.
	UIVertexType* vertices = new UIVertexType[_vertexCount];

	// �ε��� �迭�� �����.
	unsigned long* indices = new unsigned long[_indexCount];

	memset(vertices, 0, (sizeof(UIVertexType) * _vertexCount));

	// �ε��� �迭
	for (UINT i = 0; i < _indexCount; ++i)
	{
		indices[i] = i;
	}

	// ���� ���� ���� ����ü
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(UIVertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� ������ ����.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// ���� �ε��� ������ ����ü ����.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
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

void CUIBuffer::BufferSetting(ID3D11DeviceContext* _deviceContext)
{
	// ���� ������ ������ ������ ����.
	UINT stride = sizeof(UIVertexType);
	UINT offset = 0;

	// ���ؽ� ����, �ε��� ���� ����.
	_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �ﰢ�� ����.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CUIBuffer::UpdateBuffer(CUI* _ui)
{
	XMFLOAT2 uiPos = _ui->GetUIPos();
	XMFLOAT2 uiRealPos = _ui->GetUIRealPos();
	int uiWidth = _ui->GetUIWidth();
	int uiHeight = _ui->GetUIHeight();
	int uiVertexCount = _ui->GetVertexCount();
	CUI* parentUI = _ui->GetParentUI();

	if (nullptr == parentUI)
	{
		// �θ� UI�� ���ٸ� ����� �������� ���� ������
		uiRealPos = XMFLOAT2(uiPos.x, uiPos.y);
	}
	else
	{
		// �θ� UI�� �ִٸ� �θ� UI�� ���� ������ �������� �������� ������.
		uiRealPos = parentUI->GetUIRealPos();
		uiRealPos = XMFLOAT2(uiRealPos.x + uiPos.x, uiRealPos.y + uiPos.y);
	}

	_ui->SetUIRealPos(uiRealPos);

	float positionX = uiRealPos.x;
	float positionY = uiRealPos.y;

	float left, right, top, bottom;
	UIVertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UIVertexType* verticesPtr;
	ID3D11DeviceContext* deviceContext = CCentral::Instance().GetDeviceContext();


	// ������ ���� �������̶�� ������Ʈ ���� ����
	if ((positionX == m_prevPosX) && (positionY == m_prevPosY))
		return;
	
	// ������ ����
	m_prevPosX = positionX;
	m_prevPosY = positionY;

	// ���� ȭ�� ��ǥ
	left = (float)((m_screenWidth / 2) * -1) + positionX;

	// ������ ȭ�� ��ǥ
	right = left + (float)uiWidth;

	// ��� ȭ�� ��ǥ
	top = (float)(m_screenHeight / 2) - positionY;

	// �ϴ� ȭ�� ��ǥ
	bottom = top - (float)uiHeight;

	// ���� �迭
	vertices = new UIVertexType[uiVertexCount];

	// ù ��° �ﰢ��
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// �� ��° �ﰢ��
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// ���ؽ� ���۸� �� �� �ֵ��� ���.
	deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// ���� ������ �����͸� ����Ű�� ������
	verticesPtr = (UIVertexType*)mappedResource.pData;

	// �����͸� ���� ���ۿ� ����
	memcpy(verticesPtr, (void*)vertices, (sizeof(UIVertexType) * uiVertexCount));

	// ���� ������ ��� ����
	deviceContext->Unmap(m_vertexBuffer, 0);

	// ���ؽ� �迭 ����
	delete[]vertices;
}
