#pragma once

class CObject;
class CMonster;
class CScene;

class CEventMgr
{
public:
	static CEventMgr& Instance() { static CEventMgr instance; return instance; }

private:
	vector<pair<CObject*, OBJECT_TYPE>>		m_pushObjects;		// ���� ���� Ǫ���� ������Ʈ�� �ӽ� ����
	vector<pair<CObject*, OBJECT_TYPE>>		m_pullObjects;		// ���� ������ ���� ������Ʈ�� �ӽ� ����
	vector<pair<CObject*, OBJECT_TYPE>>		m_deleteObjects;	// ���� ������ ������ ������Ʈ�� �ӽ� ����
	SCENE_TYPE								m_changeScene;		// �ٲ� ���� ���� ����
	vector<pair<CMonster*, MONSTER_STATE>>	m_changeState;		// �ٲ� ���� ������ ���� ����

public:
	void Update();

	void PushObject(CObject* _object, OBJECT_TYPE _objType) { m_pushObjects.push_back(make_pair(_object, _objType)); }
	void PullObject(CObject* _object, OBJECT_TYPE _objType) { m_pullObjects.push_back(make_pair(_object, _objType)); }
	void DeleteObject(CObject* _object, OBJECT_TYPE _objType) { m_deleteObjects.push_back(make_pair(_object, _objType)); }
	void ChangeScene(SCENE_TYPE _sceneType) { m_changeScene = _sceneType; }
	void ChangeState(CMonster* _monster, MONSTER_STATE _monsterState) { m_changeState.push_back(make_pair(_monster, _monsterState)); }

public:
	CEventMgr();
	~CEventMgr();
};

