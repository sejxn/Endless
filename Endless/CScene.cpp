#include "global.h"
#include "CScene.h"

#include "CCentral.h"

#include "CObject.h"
#include "CUI.h"

#include "CCamera.h"
#include "CLight.h"
#include "CDepthTexture.h"

#include "CBullet.h"
#include "CCollider.h"

#include "CEventMgr.h"
#include "CKeyMgr.h"
#include "CCollisionMgr.h"

CScene::CScene()
	: m_objects()
	, m_uis()
	, m_bulletPool()
	, m_player(nullptr)
	, m_camera(nullptr)
	, m_light(nullptr)
	, m_depthTexture(nullptr)
	, m_menuPanel(nullptr)
	, m_gameState(GAME_STATE::END)
{
}

CScene::~CScene()
{
	
}

void CScene::Update()
{
	// ���� ������Ʈ ������Ʈ
	for (size_t i = 0; i < (size_t)OBJECT_TYPE::END; ++i)
	{
		for (size_t j = 0; j < m_objects[i].size(); ++j)
		{
			if (!m_objects[i][j]->GetIsDead())
				m_objects[i][j]->Update();
		}
	}

	// ���� UI ������Ʈ
	for (size_t i = 0; i < m_uis.size(); ++i)
	{
		if (m_uis[i]->GetVisualize())
			m_uis[i]->Update();
	}

	// ī�޶��� ������Ʈ. owner�� ����
	// ī�޶� ���ٸ� ����. ������ �־�� �Ѵ�.
	if (nullptr == m_camera)
		assert(nullptr);
	m_camera->Update();

	if (GAME_STATE::PLAYING == m_gameState)
	{
		if (CKeyMgr::Instance().CheckKeyState(KEY_BUTTON::KEY_ESC, KEY_STATE::TAP) && nullptr != m_menuPanel)
		{
			bool visualize = m_menuPanel->GetVisualize();
			if (visualize)
			{
				m_menuPanel->SetVisualize(false);
				ShowCursor(false);
			}
			else
			{
				m_menuPanel->SetVisualize(true);
				ShowCursor(true);
			}
		}
	}
}

void CScene::Render(ID3D11DeviceContext* _deviceContext)
{
	// ���� �ؽ��Ŀ� �׸� ���̹Ƿ� ���� Ÿ�� ����
	m_depthTexture->SetRenderTarget(_deviceContext);
	for (size_t i = 0; i < (size_t)OBJECT_TYPE::END; ++i)
	{
		// �׸��� ������ ���� ���� ���� ����
		for (size_t j = 0; j < m_objects[i].size(); ++j)
		{
			if (!m_objects[i][j]->GetIsDead())
				m_objects[i][j]->DepthRender(_deviceContext);
		}
	}
	// ���� Ÿ�� �ǵ�������
	CCentral::Instance().SetBackRenderTarget();

	// ���� �ִ� ������Ʈ ����
	for (size_t i = 0; i < (size_t)OBJECT_TYPE::END; ++i)
	{
		for (size_t j = 0; j < m_objects[i].size(); ++j)
		{
			if (!m_objects[i][j]->GetIsDead())
			{
				m_objects[i][j]->Render(_deviceContext);

				// collider render
				/*CCollider* collider = m_objects[i][j]->GetCollider();
				if (nullptr != collider)
					collider->Render(_deviceContext);*/
			}
		}
	}

	// Z���� �����ϰ� UI �׸���
	CCentral::Instance().TurnZBufferOff();
	const vector<CUI*> ui = GetUI();
	for (size_t i = 0; i < ui.size(); ++i)
	{
		if (ui[i]->GetVisualize())
			ui[i]->Render(_deviceContext);
	}
	CCentral::Instance().TurnZBufferOn();
}

void CScene::PullObject(CObject* _object, OBJECT_TYPE _objectType)
{
	// ������Ʈ�� �����⸸ ��
	vector<CObject*>::iterator iter = m_objects[(size_t)_objectType].begin();
	for (; iter != m_objects[(size_t)_objectType].end(); ++iter)
	{
		if (*iter == _object)
		{
			m_objects[(size_t)_objectType].erase(iter);
			return;
		}
	}
}

void CScene::DeleteObject(CObject* _object, OBJECT_TYPE _objectType)
{
	// ������Ʈ�� ������ ����
	vector<CObject*>::iterator iter = m_objects[(size_t)_objectType].begin();
	for (; iter != m_objects[(size_t)_objectType].end(); ++iter)
	{
		if (*iter == _object)
		{
			delete _object;
			m_objects[(size_t)_objectType].erase(iter);
			return;
		}
	}
}

void CScene::InitBullet(int _bulletNum)
{
	if (_bulletNum <= 0)
		assert(nullptr);

	CBullet* bullet = new CBullet;
	bullet->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	bullet->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	bullet->SetScale(XMFLOAT3(0.05f, 0.05f, 0.05f));
	CCollider* collider = new CCollider;
	collider->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	collider->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	collider->SetScale(XMFLOAT3(0.05f, 0.05f, 0.05f));
	collider->SetOwner(bullet);
	bullet->SetComponent(collider, COMPONENT_TYPE::COLLIDER);
	m_bulletPool.push(bullet);

	for (int i = 1; i < _bulletNum; ++i)
	{
		CBullet* copiedBullet = new CBullet(*bullet);
		m_bulletPool.push(copiedBullet);
	}
}

CBullet* CScene::GetBullet()
{
	// �Ҹ� Ǯ�� ����ִٸ� ���� ����
	if (m_bulletPool.empty())
	{
		CBullet* bullet = new CBullet;
		bullet->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		bullet->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		bullet->SetScale(XMFLOAT3(0.05f, 0.05f, 0.05f));
		bullet->SetIsDead(false);
		CCollider* collider = new CCollider;
		collider->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		collider->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		collider->SetScale(XMFLOAT3(0.05f, 0.05f, 0.05f));
		collider->SetOwner(bullet);
		bullet->SetComponent(collider, COMPONENT_TYPE::COLLIDER);

		CEventMgr::Instance().PushObject(bullet, OBJECT_TYPE::BULLET);

		return bullet;
	}
	else
	{
		CBullet* bullet = m_bulletPool.front();
		m_bulletPool.pop();
		bullet->SetIsDead(false);
		CEventMgr::Instance().PushObject(bullet, OBJECT_TYPE::BULLET);

		return bullet;
	}
}

void CScene::ReturnBullet(CBullet* _bullet)
{
	CEventMgr::Instance().PullObject(_bullet, OBJECT_TYPE::BULLET);
	_bullet->SetIsDead(true);
	m_bulletPool.push(_bullet);
}

void CScene::Enter()
{
	CCollisionMgr::Instance().Init();
}

void CScene::Exit()
{
	// �����ִ� ������Ʈ ����
	for (size_t i = 0; i < (size_t)OBJECT_TYPE::END; ++i)
	{
		for (size_t j = 0; j < m_objects[i].size(); ++j)
		{
			if (m_objects[i][j])
				delete m_objects[i][j];
		}
		m_objects[i].clear();
	}

	// �Ҹ� Ǯ ����
	while (!m_bulletPool.empty())
	{
		CBullet* bullet = m_bulletPool.front();
		m_bulletPool.pop();
		delete bullet;
	}

	// ���� �ִ� UI ����
	for (size_t i = 0; i < m_uis.size(); ++i)
	{
		if (m_uis[i])
			delete m_uis[i];
	}
	m_uis.clear();

	if (m_camera)
		delete m_camera;

	if (m_light)
		delete m_light;

	if (m_depthTexture)
		delete m_depthTexture;
}

void CScene::CreateStructure()
{
}

void CScene::CreateMonster()
{
}

void CScene::CreateUI()
{
}
