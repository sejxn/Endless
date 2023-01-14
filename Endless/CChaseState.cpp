#include "global.h"
#include "CChaseState.h"

#include "CMonster.h"
#include "CPlayer.h"

#include "CTimeMgr.h"
#include "CSceneMgr.h"
#include "CScene.h"

#include "CEventMgr.h"

CChaseState::CChaseState()
{
}

CChaseState::~CChaseState()
{
}

void CChaseState::Update()
{
	CMonster* owner = GetOwner();

	float sensingDist = owner->GetSensingDist();
	float attackDist = owner->GetAttackDist();

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

	// ���� ���� �ٱ��� �ִ� ��� ���� ���·� ����
	if (sensingDist <= distance)
	{
		CEventMgr::Instance().ChangeState(owner, MONSTER_STATE::PATROL);
		return;
	}
	// ���� ���� �ȿ� �ִ� ��� ���� ���·� ����
	else if (distance <= attackDist)
	{
		CEventMgr::Instance().ChangeState(owner, MONSTER_STATE::ATTACK);
		return;
	}
	// �� �ܿ��� �÷��̾ ����
	else
	{
		// ��ġ ���󰡱�
		float dt = CTimeMgr::Instance().GetDT();
		float ownerSpeed = owner->GetSpeed();
		distVec = XMVector3Normalize(distVec);
		ownerPosVec += distVec * ownerSpeed * dt;
		ownerPos = XMFLOAT3(XMVectorGetX(ownerPosVec), ownerPos.y, XMVectorGetZ(ownerPosVec));
		owner->SetPosition(ownerPos);

		// ȸ�� ���󰡱�
		// ���ʿ� �ִ��� �����ʿ� �ִ��� �Ǻ��Ͽ� ���� �������� ȸ��
		RotationToTarget(distVec);
	}
}