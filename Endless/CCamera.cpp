#include "global.h"
#include "CCamera.h"

#include "CObject.h"

CCamera::CCamera()
	: m_position(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_up(XMFLOAT3(0.0f, 1.0f, 0.0f))
	, m_offset(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_camOwner(nullptr)
{
}

CCamera::~CCamera()
{
	
}

void CCamera::Update()
{	
	if (nullptr != m_camOwner)
	{
		m_position = m_camOwner->GetPosition();
		m_rotation = m_camOwner->GetRotation();
	}
}

XMMATRIX CCamera::GetViewMatrix()
{
	XMVECTOR posVec = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVec = GetCamLookAt();
	XMVECTOR upVec = XMLoadFloat3(&m_up);

	// �� ��Ʈ���� �����Ͽ� ��ȯ
	return XMMatrixLookAtLH(posVec, lookAtVec, upVec);
}

XMMATRIX CCamera::GetUIWorldMatrix()
{
	XMVECTOR posVec = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVec = GetCamLookAt();
	XMVECTOR upVec = XMLoadFloat3(&m_up);

	// ������ ����
	XMVECTOR forwardVec = lookAtVec - posVec;
	forwardVec = XMVector3Normalize(forwardVec);

	// ȸ�� ���.
	XMMATRIX rotationMatrix = GetRotationMat();

	// �̵� ���. forwardVec��ŭ �̵���(��������)
	XMMATRIX translateMatrix = XMMatrixTranslation(XMVectorGetX(posVec + forwardVec),
		XMVectorGetY(posVec + forwardVec), XMVectorGetZ(posVec + forwardVec));
	return rotationMatrix * translateMatrix;
}

XMVECTOR CCamera::GetCamLookAt()
{
	XMMATRIX rotationMat = GetRotationMat();

	// ȸ�� ����� z�� ������ �����ִ� ����
	XMVECTOR forwardVec = rotationMat.r[2];
	XMVECTOR posVec = XMLoadFloat3(&m_position);
	return (posVec + forwardVec);
}

XMMATRIX CCamera::GetRotationMat()
{
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y), XMConvertToRadians(m_rotation.z));
	XMMATRIX rotationMat = XMMatrixRotationQuaternion(rotationQuat);

	return rotationMat;
}