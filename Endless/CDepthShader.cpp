#include "global.h"
#include "CDepthShader.h"

#include "CCentral.h"

CDepthShader::CDepthShader()
	: m_vertexShader(nullptr)
	, m_pixelShader(nullptr)
	, m_inputLayout(nullptr)
	, m_matrixBuffer(nullptr)
{
	
}

CDepthShader::~CDepthShader()
{
	if (m_vertexShader)
		m_vertexShader->Release();
	if (m_pixelShader)
		m_pixelShader->Release();
	if (m_inputLayout)
		m_inputLayout->Release();
	if (m_matrixBuffer)
		m_matrixBuffer->Release();
}

void CDepthShader::Init(ID3D11Device* _device, HWND _hwnd)
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
	wstring vsPath = absolutePath + L"shader\\depth_vs.hlsl";
	wstring psPath = absolutePath + L"shader\\depth_ps.hlsl";
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
			OutputShaderErrorMessage(errorMessage, _hwnd, (WCHAR*)L"depth_vs");
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ���
		else
		{
			MessageBox(_hwnd, (WCHAR*)L"depth_vs", L"Missing Shader File", MB_OK);
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
			OutputShaderErrorMessage(errorMessage, _hwnd, (WCHAR*)L"depth_ps");
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ���
		else
		{
			MessageBox(_hwnd, (WCHAR*)L"depth_ps", L"Missing Shader File", MB_OK);
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
	// �����Ǹ� ���
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

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
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� ����
	result = _device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		assert(nullptr);
	}
}

void CDepthShader::OutputShaderErrorMessage(ID3D10Blob* _errorMessage, HWND _hwnd, WCHAR* _shaderFileName)
{
	// ���� �޽����� ���â�� ǥ��
	OutputDebugStringA(reinterpret_cast<const char*>(_errorMessage->GetBufferPointer()));

	// ���� �޼����� ��ȯ
	_errorMessage->Release();

	// ������ ������ ������ �˾� �޼����� �˸�
	MessageBox(_hwnd, L"Error compiling shader.", _shaderFileName, MB_OK);
}

void CDepthShader::Render(ID3D11DeviceContext* _deviceContext, MatrixBufferType _transformMatrix, UINT _indexCount)
{
	// ��� ���� ����
	SetShaderParameters(_deviceContext, _transformMatrix);

	// ���̴� ���� �� ����
	RenderShader(_deviceContext, _indexCount);
}

void CDepthShader::SetShaderParameters(ID3D11DeviceContext* _deviceContext, MatrixBufferType _transformMatrix)
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
	_deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->worldMatrix = _transformMatrix.worldMatrix;
	dataPtr->viewMatrix = _transformMatrix.viewMatrix;
	dataPtr->projectionMatrix = _transformMatrix.projectionMatrix;

	_deviceContext->Unmap(m_matrixBuffer, 0);

	unsigned bufferNumber = 0;

	_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	// -------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------
}

void CDepthShader::RenderShader(ID3D11DeviceContext* _deviceContext, UINT _indexCount)
{
	// ���� �Է� ���̾ƿ�, ���ؽ� ���̴�, �ȼ� ���̴� ����
	_deviceContext->IASetInputLayout(m_inputLayout);
	_deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	_deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �׸���
	_deviceContext->DrawIndexed(_indexCount, 0, 0);
}