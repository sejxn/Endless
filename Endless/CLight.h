#pragma once

class CLight : public AlignedAllocationPolicy<16>
{
private:
	XMFLOAT4	m_ambientColor;		// �⺻ ����
	XMFLOAT4	m_diffuseColor;		// Ȯ�� ����

	XMFLOAT3	m_lightDirection;	// CommonShader���� �ʿ�
	XMFLOAT3	m_position;			// ��ġ
	XMFLOAT3	m_lookAt;			// �����ִ� ��

	XMMATRIX	m_viewMatrix;		// �� ������ �� ��ȯ ���
	XMMATRIX	m_projectionMatrix;	// �� ������ ���� ��ȯ ���

	float		m_specularPower;	// �� �ݻ� ����

public:
	void SetAmbientColor(float _r, float _g, float _b, float _a) { m_ambientColor = XMFLOAT4(_r, _g, _b, _a); }
	XMFLOAT4 GetAmbientColor() { return m_ambientColor; }
	void SetDiffuseColor(float _r, float _g, float _b, float _a) { m_diffuseColor = XMFLOAT4(_r, _g, _b, _a); }
	XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; }

	void SetLightDirection(float _x, float _y, float _z) { m_lightDirection = XMFLOAT3(_x, _y, _z); }
	XMFLOAT3 GetLightDirection() { return m_lightDirection; }

	void SetLightPos(float _x, float _y, float _z) { m_position = XMFLOAT3(_x, _y, _z); }
	XMFLOAT3 GetLightPos() { return m_position; }
	void SetLightLookAt(float _x, float _y, float _z) { m_lookAt = XMFLOAT3(_x, _y, _z); }
	XMFLOAT3 GetLightLookAt() { return m_lookAt; }

	void SetSpecularPower(float _power) { m_specularPower = _power; }
	float GetSpecularPower() { return m_specularPower; }

	XMMATRIX GetViewMatrix();

	void GenerateProjectionMatrix();
	XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

public:
	CLight();
	~CLight();
};

