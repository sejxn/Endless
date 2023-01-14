#include "global.h"
#include "CEventMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"

#include "CMonster.h"

CEventMgr::CEventMgr()
	: m_pushObjects()
	, m_deleteObjects()
	, m_changeScene(SCENE_TYPE::END)
	, m_changeState()
{
}

CEventMgr::~CEventMgr()
{
}

void CEventMgr::Update()
{
	for (size_t i = 0; i < m_pushObjects.size(); ++i)
	{
		CSceneMgr::Instance().GetCurrentScene()->PushObject(m_pushObjects[i].first, m_pushObjects[i].second);
	}
	// �ӽ� ��ü�� �������� �뷮 �ʱ�ȭ
	m_pushObjects.clear();
	vector<pair<CObject*, OBJECT_TYPE>>().swap(m_pushObjects);

	for (size_t i = 0; i < m_pullObjects.size(); ++i)
	{
		CSceneMgr::Instance().GetCurrentScene()->PullObject(m_pullObjects[i].first, m_pullObjects[i].second);
	}
	// �ӽ� ��ü�� �������� �뷮 �ʱ�ȭ
	m_pullObjects.clear();
	vector<pair<CObject*, OBJECT_TYPE>>().swap(m_pullObjects);

	for (size_t i = 0; i < m_deleteObjects.size(); ++i)
	{
		CSceneMgr::Instance().GetCurrentScene()->DeleteObject(m_deleteObjects[i].first, m_deleteObjects[i].second);
	}
	// �ӽ� ��ü�� �������� �뷮 �ʱ�ȭ
	m_deleteObjects.clear();
	vector<pair<CObject*, OBJECT_TYPE>>().swap(m_deleteObjects);

	// ����� ���� �ִٸ�
	if (SCENE_TYPE::END != m_changeScene)
	{
		CSceneMgr::Instance().ChangeScene(m_changeScene);
		m_changeScene = SCENE_TYPE::END;
	}

	// ���� ���� ����
	for (size_t i = 0; i < m_changeState.size(); ++i)
	{
		m_changeState[i].first->ChangeState(m_changeState[i].second);
	}
	// �ӽ� ��ü�� �������� �뷮 �ʱ�ȭ
	m_changeState.clear();
	vector<pair<CMonster*, MONSTER_STATE>>().swap(m_changeState);
}