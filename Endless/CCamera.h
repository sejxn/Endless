#pragma once

class CObject;

class CCamera
{
private:
	XMFLOAT3 m_position;	// ī�޶��� ���� ��ġ
	XMFLOAT3 m_rotation;	// ȸ�� ����. ������ ȸ�� ������ ����
	XMFLOAT3 m_up;			// ��� ����
	XMFLOAT3 m_offset;

	CObject* m_camOwner;	// ����ٴ� ������Ʈ

public:
	void Update();

public:
	XMMATRIX GetViewMatrix();
	XMMATRIX GetUIWorldMatrix();

	void SetCamPos(XMFLOAT3 _camPos) { m_position = _camPos; }
	XMFLOAT3 GetCamPos() { return m_position; }

	void SetCamRot(XMFLOAT3 _camRot) { m_rotation = _camRot; }
	XMFLOAT3 GetCamRot() { return m_rotation; }

	void SetOwner(CObject* _owner) { m_camOwner = _owner; }
	CObject* GetOwner() { return m_camOwner; }

	XMMATRIX GetRotationMat();

private:
	XMVECTOR GetCamLookAt();

public:
	CCamera();
	~CCamera();
};

