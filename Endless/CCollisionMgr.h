#pragma once

class CCollider;
class CObject;

class CCollisionMgr
{
public:
	static CCollisionMgr& Instance() { static CCollisionMgr instance; return instance; }

private:
	// �� ���̵��� �������� ������ id ����.
	union IDCombination
	{
		struct
		{
			UINT leftID;
			UINT rightID;
		};
		ULONGLONG uniqueID;
	};

	bool m_colCheck[(size_t)OBJECT_TYPE::END][(size_t)OBJECT_TYPE::END];	// �浹 üũ �ǽ� ���� ��Ÿ�� 2���� �迭.

	map<ULONGLONG, COLLISION_STATE> m_collisionState;						// �� ���� ID ������ ������ ���� ���� �浹 �������� ����

private:
	void CollisionCheck();													// �浹 üũ

	void ObjectColCheck(const vector<CObject*>& _objLeft, const vector<CObject*>& _objRight);

	bool ColliderCheck(CCollider* _colLeft, CCollider* _colRight);			// �浹ü ���� �浹 üũ

public:
	void Init();
	void Update();

public:
	void SetColCheck(OBJECT_TYPE _objType1, OBJECT_TYPE _objType2);

public:
	CCollisionMgr();
	~CCollisionMgr();
};

