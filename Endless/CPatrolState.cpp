#include "global.h"
#include "CPatrolState.h"

#include "CMonster.h"

#include "CTimeMgr.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CPlayer.h"

#include "CEventMgr.h"

CPatrolState::CPatrolState()
	: m_currentPatrolPos(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_nextPatrolPos(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
}

CPatrolState::~CPatrolState()
{
}

void CPatrolState::Update()
{
	CMonster* owner = GetOwner();
	float sensingDist = owner->GetSensingDist();

	// �÷��̾���� �Ÿ����� ����Ͽ� ���� ���� �ȿ� ��ٸ� �Ѵ� ���·� ����
	CPlayer* player = CSceneMgr::Instance().GetCurrentScene()->GetPlayer();

	XMFLOAT3 ownerPos = owner->GetPosition();
	XMFLOAT3 playerPos = player->GetPosition();

	// x��� z�࿡ ���ؼ���
	XMVECTOR ownerPosVec = XMLoadFloat3(&ownerPos);
	ownerPosVec = XMVectorSet(XMVectorGetX(ownerPosVec), 0.0f, XMVectorGetZ(ownerPosVec), 0.0f);
	XMVECTOR playerPosVec = XMLoadFloat3(&playerPos);
	playerPosVec = XMVectorSet(XMVectorGetX(playerPosVec), 0.0f, XMVectorGetZ(playerPosVec), 0.0f);
	XMVECTOR distVec = playerPosVec - ownerPosVec;
	
	float distance = XMVectorGetX(XMVector3Length(distVec));

	// �÷��̾���� �Ÿ��� ���� �������� �۴ٸ� �Ѵ� ���·� ����
	if (distance <= sensingDist)
	{
		CEventMgr::Instance().ChangeState(owner, MONSTER_STATE::CHASE);
		return;
	}

	XMVECTOR currentPatrolPos = XMLoadFloat3(&m_currentPatrolPos);
	XMVECTOR nextPatrolPos = XMLoadFloat3(&m_nextPatrolPos);

	XMVECTOR goDirectionVec = currentPatrolPos - ownerPosVec;

	// ���� ����
	float epsilon = 0.01f;

	// ���� ���� ���������� �̵��� �� ��� currentPatrolPos �� nextPatrolPos ���� �� Idle���·� ����
	if (XMVectorGetX(XMVector3Length(goDirectionVec)) <= epsilon)
	{
		XMStoreFloat3(&m_currentPatrolPos, nextPatrolPos);
		XMStoreFloat3(&m_nextPatrolPos, currentPatrolPos);
		CEventMgr::Instance().ChangeState(owner, MONSTER_STATE::IDLE);
		return;
	}
	else
	{
		// ���� ���� ���������� �̵�
		float dt = CTimeMgr::Instance().GetDT();
		float ownerSpeed = owner->GetSpeed();
		goDirectionVec = XMVector3Normalize(goDirectionVec);
		ownerPosVec += goDirectionVec * ownerSpeed * dt;
		ownerPos = XMFLOAT3(XMVectorGetX(ownerPosVec), ownerPos.y, XMVectorGetZ(ownerPosVec));
		owner->SetPosition(ownerPos);

		// ȸ�� ���󰡱�
		// ���ʿ� �ִ��� �����ʿ� �ִ��� �Ǻ��Ͽ� ���� �������� ȸ��
		RotationToTarget(goDirectionVec);
	}
}