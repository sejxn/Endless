#include "global.h"
#include "CGameMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CPlayer.h"
#include "CObject.h"

#include "CUI.h"

CGameMgr::CGameMgr()
	: m_gameOverPanel(nullptr)
	, m_gameClearPanel(nullptr)
{
}

CGameMgr::~CGameMgr()
{
}

void CGameMgr::Update()
{
	// �÷��̾ �׾��ٸ� gameover�ǳ��� ���� ���� ���� ����
	CPlayer* player = CSceneMgr::Instance().GetCurrentScene()->GetPlayer();
	if (player->GetIsDead() && CSceneMgr::Instance().GetCurrentScene()->GetGameState() == GAME_STATE::PLAYING)
	{
		if (nullptr != m_gameOverPanel)
		{
			CSceneMgr::Instance().GetCurrentScene()->SetGameState(GAME_STATE::OVER);
			if (CSceneMgr::Instance().GetCurrentScene()->GetMenuPanel()->GetVisualize())
			{
				CSceneMgr::Instance().GetCurrentScene()->GetMenuPanel()->SetVisualize(false);
				m_gameOverPanel->SetVisualize(true);
			}
			else
			{
				m_gameOverPanel->SetVisualize(true);
				ShowCursor(true);
			}
		}
	}

	// ���Ͱ� ���� �׾��ٸ� clear�ǳ��� ���� ���� ���� ����
	const vector<CObject*> monsterObj = CSceneMgr::Instance().GetCurrentScene()->GetSceneObj(OBJECT_TYPE::MONSTER);
	bool flag = false;
	for (size_t i = 0; i < monsterObj.size(); ++i)
	{
		if (!monsterObj[i]->GetIsDead())
		{
			flag = true;
			break;
		}
	}
	if (!flag && CSceneMgr::Instance().GetCurrentScene()->GetGameState() == GAME_STATE::PLAYING)
	{
		if (nullptr != m_gameClearPanel)
		{
			CSceneMgr::Instance().GetCurrentScene()->SetGameState(GAME_STATE::CLEAR);
			if (CSceneMgr::Instance().GetCurrentScene()->GetMenuPanel()->GetVisualize())
			{
				CSceneMgr::Instance().GetCurrentScene()->GetMenuPanel()->SetVisualize(false);
				m_gameClearPanel->SetVisualize(true);
			}
			else
			{
				m_gameClearPanel->SetVisualize(true);
				ShowCursor(true);
			}
		}
	}
}