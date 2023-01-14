#include "global.h"
#include "CState.h"

#include "CTimeMgr.h"
#include "CMonster.h"

CState::CState()
	: m_owner(nullptr)
{
}

CState::~CState()
{
}

void CState::RotationToTarget(XMVECTOR _targetVec)
{
	// Ÿ���� ���� ȸ��
	float dt = CTimeMgr::Instance().GetDT();
	XMFLOAT3 ownerRotation = m_owner->GetRotation();
	float ownerRotSpeed = m_owner->GetRotationSpeed();

	XMVECTOR ownerForwardVec = m_owner->GetForwardVec();
	_targetVec = XMVector3Normalize(_targetVec);

	// �� ������ ������ 0�̶��(0�� ������) ȸ���� ���� ����
	XMVECTOR angleVec = XMVector3AngleBetweenNormals(ownerForwardVec, _targetVec);
	float angle = XMVectorGetX(angleVec);
	angle = XMConvertToDegrees(angle);
	float epsilon = 0.001f;
	if (epsilon <= angle)
	{
		XMVECTOR crossVec = XMVector3Cross(ownerForwardVec, _targetVec);

		// �ڱⰡ �����ִ� �����, �÷��̾���� ������ ������ �� �� ���Ϳ� �����Ͽ� ������ ������, ������� ����
		XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR dotVec = XMVector3Dot(crossVec, upVec);
		float dot = XMVectorGetX(dotVec);

		// ����
		if (dot < 0.0f)
		{
			ownerRotation.y -= ownerRotSpeed * dt;
		}
		// ������
		else
		{
			ownerRotation.y += ownerRotSpeed * dt;
		}
		m_owner->SetRotation(ownerRotation);
	}
}