#include "global.h"
#include "CCollisionMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"

#include "CObject.h"
#include "CCollider.h"

CCollisionMgr::CCollisionMgr()
	: m_colCheck{}
	, m_collisionState()
{
	
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::Init()
{
	size_t size = (size_t)OBJECT_TYPE::END;
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
		{
			m_colCheck[i][j] = false;
		}
	}
}

void CCollisionMgr::Update()
{
	CollisionCheck();
}

void CCollisionMgr::CollisionCheck()
{
	// ���� ���� ������Ʈ���� �����ͼ� �浹 �˻縦 ��.
	CScene* currentScene = CSceneMgr::Instance().GetCurrentScene();

	for (size_t i = 0; i < (size_t)OBJECT_TYPE::END; ++i)
	{
		for (size_t j = i; j < (size_t)OBJECT_TYPE::END; ++j)
		{
			if (true == m_colCheck[i][j])
			{
				// �浹 �˻� �ʿ�
				const vector<CObject*> obj1 = currentScene->GetSceneObj((OBJECT_TYPE)i);
				const vector<CObject*> obj2 = currentScene->GetSceneObj((OBJECT_TYPE)j);

				ObjectColCheck(obj1, obj2);
			}
		}
	}
}

void CCollisionMgr::ObjectColCheck(const vector<CObject*>& _objLeft, const vector<CObject*>& _objRight)
{
	for (size_t i = 0; i < _objLeft.size(); ++i)
	{
		CCollider* objLeftCollider = _objLeft[i]->GetCollider();
		
		// �������Ͱų� ���� ���¶�� continue
		if (nullptr == objLeftCollider || _objLeft[i]->GetIsDead())
			continue;

		for (size_t j = 0; j < _objRight.size(); ++j)
		{
			CCollider* objRightCollider = _objRight[j]->GetCollider();

			// ���������̰ų� ���� ���� �浹ü Ȥ�� ���� ���¶�� continue
			if (nullptr == objRightCollider || objLeftCollider == objRightCollider || _objRight[j]->GetIsDead())
				continue;

			UINT leftColID = objLeftCollider->GetColliderID();
			UINT rightColID = objRightCollider->GetColliderID();

			// �׻� ������ ���̵� �� ū ��
			if (rightColID < leftColID)
			{
				UINT temp = rightColID;
				rightColID = leftColID;
				leftColID = temp;
			}

			IDCombination idCom;
			idCom.leftID = leftColID;
			idCom.rightID = rightColID;

			map<ULONGLONG, COLLISION_STATE>::iterator iter = m_collisionState.find(idCom.uniqueID);

			// ���� ���°� ���ٸ� NONE���� ����
			if (m_collisionState.end() == iter)
			{
				m_collisionState.insert(make_pair(idCom.uniqueID, COLLISION_STATE::NONE));
				iter = m_collisionState.find(idCom.uniqueID);
			}

			// �浹 �ߴٸ�
			if (ColliderCheck(objLeftCollider, objRightCollider))
			{
				// ���� ���°� NONE�̶�� ENTER
				if (COLLISION_STATE::NONE == iter->second)
				{
					iter->second = COLLISION_STATE::ENTER;
					objLeftCollider->OnCollisionEnter(objRightCollider);
					objRightCollider->OnCollisionEnter(objLeftCollider);
				}
				// ���� ���°� ENTER���ٸ� COLLISION���� �ٲ���
				else if (COLLISION_STATE::ENTER == iter->second)
				{
					iter->second = COLLISION_STATE::COLLISION;
					objLeftCollider->OnCollision(objRightCollider);
					objRightCollider->OnCollision(objLeftCollider);
				}
				// ���� ���°� COLLISION�̶�� ��� COLLISION
				else if (COLLISION_STATE::COLLISION == iter->second)
				{
					objLeftCollider->OnCollision(objRightCollider);
					objRightCollider->OnCollision(objLeftCollider);
				}
				// ���� ���°� EXIT �̿��ٸ� �ٽ� ENTER��
				else if (COLLISION_STATE::EXIT == iter->second)
				{
					iter->second = COLLISION_STATE::ENTER;
					objLeftCollider->OnCollisionEnter(objRightCollider);
					objRightCollider->OnCollisionEnter(objLeftCollider);
				}
			}
			// �浹���� �ʾҴٸ�
			else
			{
				// ���� ���°� ���ٸ� continue
				if (m_collisionState.end() == iter)
					continue;
				
				// ���� ���°� ENTER�ų� COLLISION�̶�� EXIT ȣ��
				if (COLLISION_STATE::ENTER == iter->second || COLLISION_STATE::COLLISION == iter->second)
				{
					iter->second = COLLISION_STATE::EXIT;
					objLeftCollider->OnCollisionExit(objRightCollider);
					objRightCollider->OnCollisionExit(objLeftCollider);
				}
				else if (COLLISION_STATE::EXIT == iter->second)
				{
					iter->second = COLLISION_STATE::NONE;
				}
			}
		}
	}
}

bool CCollisionMgr::ColliderCheck(CCollider* _colLeft, CCollider* _colRight)
{
	XMFLOAT3 colLeftPos = _colLeft->GetPosition();
	XMFLOAT3 colRightPos = _colRight->GetPosition();
	XMFLOAT3 colLeftScale = _colLeft->GetScale();
	XMFLOAT3 colRightScale = _colRight->GetScale();

	XMFLOAT3 colLeftMin = XMFLOAT3(colLeftPos.x - colLeftScale.x,
		colLeftPos.y - colLeftScale.y, colLeftPos.z - colLeftScale.z);
	XMFLOAT3 colLeftMax = XMFLOAT3(colLeftPos.x + colLeftScale.x,
		colLeftPos.y + colLeftScale.y, colLeftPos.z + colLeftScale.z);

	XMFLOAT3 colRightMin = XMFLOAT3(colRightPos.x - colRightScale.x,
		colRightPos.y - colRightScale.y, colRightPos.z - colRightScale.z);
	XMFLOAT3 colRightMax = XMFLOAT3(colRightPos.x + colRightScale.x,
		colRightPos.y + colRightScale.y, colRightPos.z + colRightScale.z);
	
	// AABB �浹�˻�
	if ((colLeftMin.x <= colRightMax.x && colRightMin.x <= colLeftMax.x) &&
		(colLeftMin.y <= colRightMax.y && colRightMin.y <= colLeftMax.y) &&
		(colLeftMin.z <= colRightMax.z && colRightMin.z <= colLeftMax.z))
	{
		return true;
	}

	return false;
}

void CCollisionMgr::SetColCheck(OBJECT_TYPE _objType1, OBJECT_TYPE _objType2)
{
	// �浹 ����(objType1�� objType2�� �浹 ����)
	m_colCheck[(size_t)_objType1][(size_t)_objType2] = true;
	m_colCheck[(size_t)_objType2][(size_t)_objType1] = true;
}