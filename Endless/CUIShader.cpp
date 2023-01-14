#include "global.h"
#include "CUIShader.h"

#include "CCentral.h"

CUIShader::CUIShader()
	: m_vertexShader(nullptr)
	, m_pixelShader(nullptr)
	, m_inputLayout(nullptr)
	, m_constantBuffer(nullptr)
	, m_samplerState(nullptr)
{
	
}

CUIShader::~CUIShader()
{
	if (m_vertexShader)
		m_vertexShader->Release();
	if (m_pixelShader)
		m_pixelShader->Release();
	if (m_inputLayout)
		m_inputLayout->Release();
	if (m_constantBuffer)
		m_constantBuffer->Release();
	if (m_samplerState)
		m_samplerState->Release();
}

void CUIShader::Init(ID3D11Device* _device, HWND _hwnd)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// ���� ���
	wstring absolutePath = CCentral::Instance().GetAbsolutePath();
	wstring vsPath = absolutePath + L"shader\\ui_vs.hlsl";
	wstring psPath = absolutePath + L"shader\\ui_ps.hlsl";
	const wchar_t* vsFileName = vsPath.c_str();
	const wchar_t* psFileName = psPath.c_str();

	// ���ؽ� ���̴� �ڵ� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DCompileFromFile(vsFileName, nullptr, nullptr, "main", "vs_5_0",
		dwShaderFlags, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� ������ ���н� ���� �޽��� ���
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, _hwnd, (WCHAR*)L"ui_vs");
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ���
		else
		{
			MessageBox(_hwnd, (WCHAR*)L"ui_vs", L"Missing Shader File", MB_OK);
		}
	}

	// �ȼ� ���̴� ������.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	result = D3DCompileFromFile(psFileName, nullptr, nullptr, "main", "ps_5_0",
		dwShaderFlags, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� ������ ���н� �����޽����� ���
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, _hwnd, (WCHAR*)L"ui_ps");
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ���
		else
		{
			MessageBox(_hwnd, (WCHAR*)L"ui_ps", L"Missing Shader File", MB_OK);
		}
	}

	// ���۷κ��� ���� ���̴��� ����.
	result = _device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		NULL, &m_vertexShader);
	if (FAILED(result))
	{
		assert(nullptr);
	}

	// ���ۿ��� �ȼ� ���̴� ����
	result = _device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		NULL, &m_pixelShader);
	if (FAILED(result))
	{
		assert(nullptr);
	}

	// ���� �Է� ���̾ƿ� ����ü ����
	// �����ǰ� �ؽ�����ǥ�� ���(UI������ ���ؽ� Ÿ�԰� ���ƾ� ��)
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� �� ��������
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ� �����
	result = _device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_inputLayout);

	if (FAILED(result))
	{
		assert(nullptr);
	}

	// ��� ����
	vertexShaderBuffer->Release();

	pixelShaderBuffer->Release();

	// ���� ���̴��� ��� ��� ���� ����ü
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// ��� ���� ����
	result = _device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer);
	if (FAILED(result))
	{
		assert(nullptr);
	}

	// �ؽ�ó ���÷� ���� ����ü
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// ���÷� ���� ����
	result = _device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result))
	{
		assert(nullptr);
	}
}

void CUIShader::OutputShaderErrorMessage(ID3D10Blob* _errorMessage, HWND _hwnd, WCHAR* _shaderFileName)
{
	// ���� �޽����� ���â�� ǥ��
	OutputDebugStringA(reinterpret_cast<const char*>(_errorMessage->GetBufferPointer()));

	// ���� �޼����� ��ȯ
	_errorMessage->Release();

	// ������ ������ ������ �˾� �޼����� �˸�
	MessageBox(_hwnd, L"Error compiling shader.", _shaderFileName, MB_OK);
}

void CUIShader::Render(ID3D11DeviceContext* _deviceContext, TransformBufferType _transformMatrix,
	ID3D11ShaderResourceView* _texture, UINT _indexCount)
{
	// ��� ���� ����
	SetShaderParameters(_deviceContext, _transformMatrix, _texture);

	// ���̴� ���� �� ����
	RenderShader(_deviceContext, _indexCount);
}

void CUIShader::SetShaderParameters(ID3D11DeviceContext* _deviceContext, TransformBufferType _transformMatrix, ID3D11ShaderResourceView* _texture)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� ��.
	_transformMatrix.worldMatrix = XMMatrixTranspose(_transformMatrix.worldMatrix);
	_transformMatrix.viewMatrix = XMMatrixTranspose(_transformMatrix.viewMatrix);
	_transformMatrix.projectionMatrix = XMMatrixTranspose(_transformMatrix.projectionMatrix);

	// -------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------
	// ��� ��ȯ ��� ���� ����
	// ��� ������ ������ �� �� �ֵ��� ���
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	_deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->worldMatrix = _transformMatrix.worldMatrix;
	dataPtr->viewMatrix = _transformMatrix.viewMatrix;
	dataPtr->projectionMatrix = _transformMatrix.projectionMatrix;

	_deviceContext->Unmap(m_constantBuffer, 0);

	unsigned bufferNumber = 0;

	_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);
	// -------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------

	// �ȼ� ���̴����� ���̴� �ؽ��� ���ҽ��� ����.
	_deviceContext->PSSetShaderResources(0, 1, &_texture);
}

void CUIShader::RenderShader(ID3D11DeviceContext* _deviceContext, UINT _indexCount)
{
	// ���� �Է� ���̾ƿ�, ���ؽ� ���̴�, �ȼ� ���̴� ����
	_deviceContext->IASetInputLayout(m_inputLayout);
	_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// ���÷� ���¸� ����
	_deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	// �ﰢ���� �׸���
	_deviceContext->DrawIndexed(_indexCount, 0, 0);
}