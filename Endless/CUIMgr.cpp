#include "global.h"
#include "CUIMgr.h"

#include "CKeyMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"

#include "CUI.h"

CUIMgr::CUIMgr()
	: m_targetedUI(nullptr)
{

}

CUIMgr::~CUIMgr()
{

}

void CUIMgr::Update()
{
	// Ŭ���� UI�� ���ٸ�
	if (nullptr == m_targetedUI)
	{
		// ���콺 ���� ��ư ���� �����Ͽ� ���� �ߴٸ�
		// ���� �� ������ ���콺 �����ǿ� �ش��ϴ� UI�� �����´�.
		if (CKeyMgr::Instance().CheckMouseState(MOUSE_BUTTON::LEFT, KEY_STATE::TAP))
		{
			m_targetedUI = GetTargetUI();
		}
	}
	// Ŭ���� UI�� �ִٸ�
	else
	{
		// ���콺 ���� ��ư ����̸� �����Ͽ� ����� ���¶��
		// �׶��� ���콺 �����ǿ� �ش��ϴ� UI�� ���� �ߴ� UI�� ���Ͽ�
		// ���ٸ� ���콺 Ŭ�� �̺�Ʈ �߻�
		if (CKeyMgr::Instance().CheckMouseState(MOUSE_BUTTON::LEFT, KEY_STATE::AWAY))
		{
			CUI* awayUI = GetTargetUI();
			if (m_targetedUI == awayUI)
			{
				m_targetedUI->MouseClicked();
			}
			m_targetedUI = nullptr;
		}
	}
}

CUI* CUIMgr::GetTargetUI()
{
	const vector<CUI*> uis = CSceneMgr::Instance().GetCurrentScene()->GetUI();
	CUI* targetedUI = nullptr;

	XMFLOAT2 mousePos = CKeyMgr::Instance().GetMousePos();
	float mousePosX = mousePos.x;
	float mousePosY = mousePos.y;

	queue<CUI*> uiQueue;
	for (size_t i = 0; i < uis.size(); ++i)
	{
		uiQueue.push(uis[i]);
	}

	while (!uiQueue.empty())
	{
		CUI* temp = uiQueue.front();
		uiQueue.pop();

		// Ȱ��ȭ �����϶���
		if (temp->GetVisualize())
		{
			if (temp->CheckIsMouseOn(mousePosX, mousePosY))
			{
				targetedUI = temp;
				const vector<CUI*> childUIs = temp->GetChildUI();
				for (size_t i = 0; i < childUIs.size(); ++i)
				{
					uiQueue.push(childUIs[i]);
				}
			}
		}
	}

	return targetedUI;
}
