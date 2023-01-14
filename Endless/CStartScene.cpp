#include "global.h"
#include "CStartScene.h"

#include "CObject.h"
#include "CPlayer.h"
#include "CPlane.h"
#include "CNormalMonster.h"
#include "CMonster.h"
#include "CWall.h"
#include "CBullet.h"

#include "CCollider.h"

#include "CUI.h"
#include "CStage1Button.h"
#include "CStage2Button.h"
#include "CStage3Button.h"
#include "CExitButton.h"

#include "CCentral.h"

#include "CCamera.h"
#include "CLight.h"
#include "CDepthTexture.h"

#include "CCollisionMgr.h"

#include "CIdleState.h"
#include "CChaseState.h"
#include "CPatrolState.h"
#include "CAttackState.h"

#include "CPlayerController.h"
#include "CTextureMgr.h"
#include "CTexture.h"

CStartScene::CStartScene()
{
}

CStartScene::~CStartScene()
{
	
}

void CStartScene::Update()
{
	CScene::Update();
}

void CStartScene::Render(ID3D11DeviceContext* _deviceContext)
{
	CScene::Render(_deviceContext);
}

void CStartScene::Enter()
{
	CScene::Enter();

	ID3D11Device* device = CCentral::Instance().GetDevice();

	// ������Ʈ ������ �� ����.
	// �÷��̾�
	CPlayer* player = new CPlayer;
	player->SetScale(XMFLOAT3(0.5f, 1.0f, 0.5f));
	player->SetRotation(XMFLOAT3(30.0f, 45.0f, 0.0f));
	player->SetPosition(XMFLOAT3(-13.2f, 7.5f, -13.2f));
	player->GetController()->SetJumpVel(0.0f);
	player->GetController()->SetMouseSens(0.0f);
	CCollider* collider = new CCollider;
	collider->SetOwner(player);
	collider->SetScale(XMFLOAT3(0.5f, 1.0f, 0.5f));
	collider->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	player->SetComponent(collider, COMPONENT_TYPE::COLLIDER);
	PushObject(player, OBJECT_TYPE::PLAYER);
	SetPlayer(player);

	// ī�޶�	
	CCamera* myCamera = new CCamera;
	myCamera->SetOwner(player);
	myCamera->SetCamPos(XMFLOAT3(0.0f, 3.0f, -5.0f));
	SetCamera(myCamera);

	CreateStructure();

	CreateMonster();

	InitBullet(50);

	// �� ����
	CLight* light = new CLight;
	light->SetAmbientColor(0.8f, 0.8f, 0.8f, 1.0f);
	light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetLightDirection(0.0f, -1.0f, 0.0f);
	light->SetSpecularPower(64.0f);
	light->SetLightPos(0.0f, 20.0f, -1.0f);
	light->SetLightLookAt(0.0f, 0.0f, 0.0f);
	light->GenerateProjectionMatrix();
	SetLight(light);

	// �׸��� ������ ���� ���� �ؽ��� ����
	CDepthTexture* depthTexture = new CDepthTexture;
	depthTexture->Init(device);
	SetDepthTexture(depthTexture);

	// �浹 ����
	CCollisionMgr::Instance().SetColCheck(OBJECT_TYPE::STRUCTURE, OBJECT_TYPE::PLAYER);
	CCollisionMgr::Instance().SetColCheck(OBJECT_TYPE::STRUCTURE, OBJECT_TYPE::MONSTER);

	CreateUI();

	SetGameState(GAME_STATE::PLAYING);

	// Ŀ�� ����
	//ShowCursor(false);
}

void CStartScene::Exit()
{
	CScene::Exit();
}

void CStartScene::CreateStructure()
{
	// �ٴ�
	CPlane* plane = new CPlane;
	plane->SetScale(XMFLOAT3(12.0f, 0.05f, 12.0f));
	plane->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	plane->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	PushObject(plane, OBJECT_TYPE::STRUCTURE);
	// �ٴ� �浹ü
	CCollider* planeCollider = new CCollider;
	planeCollider->SetOwner(plane);
	planeCollider->SetScale(XMFLOAT3(12.0f, 0.05f, 12.0f));
	plane->SetComponent(planeCollider, COMPONENT_TYPE::COLLIDER);

	// �ٴ�
	CPlane* plane1 = new CPlane;
	plane1->SetScale(XMFLOAT3(1.0f, 0.3f, 1.0f));
	plane1->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	plane1->SetPosition(XMFLOAT3(-14.0f, 7.0f, -14.0f));
	PushObject(plane1, OBJECT_TYPE::STRUCTURE);
	// �ٴ� �浹ü
	CCollider* plane1Collider = new CCollider;
	plane1Collider->SetOwner(plane1);
	plane1Collider->SetScale(XMFLOAT3(1.0f, 0.3f, 1.0f));
	plane1->SetComponent(plane1Collider, COMPONENT_TYPE::COLLIDER);

	// �� ����
	CWall* wall = new CWall;
	wall->SetScale(XMFLOAT3(12.0f, 2.0f, 1.0f));
	wall->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	wall->SetPosition(XMFLOAT3(0.0f, 2.0f, 13.0f));
	// �� �浹ü
	CCollider* wallCollider = new CCollider;
	wallCollider->SetOwner(wall);
	wallCollider->SetScale(XMFLOAT3(12.0f, 2.0f, 1.0f));
	wall->SetComponent(wallCollider, COMPONENT_TYPE::COLLIDER);
	PushObject(wall, OBJECT_TYPE::STRUCTURE);

	CWall* wall1 = new CWall(*wall);
	wall1->SetScale(XMFLOAT3(12.0f, 2.0f, 1.0f));
	wall1->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	wall1->SetPosition(XMFLOAT3(0.0f, 2.0f, -13.0f));
	CCollider* wall1Collider = wall1->GetCollider();
	wall1Collider->SetScale(XMFLOAT3(12.0f, 2.0f, 1.0f));
	PushObject(wall1, OBJECT_TYPE::STRUCTURE);

	CWall* wall2 = new CWall(*wall);
	wall2->SetScale(XMFLOAT3(1.0f, 2.0f, 12.0f));
	wall2->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	wall2->SetPosition(XMFLOAT3(-13.0f, 2.0f, 0.0f));
	CCollider* wall2Collider = wall2->GetCollider();
	wall2Collider->SetScale(XMFLOAT3(1.0f, 2.0f, 12.0f));
	PushObject(wall2, OBJECT_TYPE::STRUCTURE);

	CWall* wall3 = new CWall(*wall);
	wall3->SetScale(XMFLOAT3(1.0f, 2.0f, 12.0f));
	wall3->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	wall3->SetPosition(XMFLOAT3(13.0f, 2.0f, 0.0f));
	CCollider* wall3Collider = wall3->GetCollider();
	wall3Collider->SetScale(XMFLOAT3(1.0f, 2.0f, 12.0f));
	PushObject(wall3, OBJECT_TYPE::STRUCTURE);
}

void CStartScene::CreateMonster()
{
	CNormalMonster* normMonster = new CNormalMonster;
	normMonster->SetPosition(XMFLOAT3(0.0f, 2.0f, 7.0f));
	normMonster->SetScale(XMFLOAT3(0.005f, 0.005f, 0.005f));
	normMonster->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	normMonster->SetSensingDist(8.0f);
	normMonster->SetAttackDist(4.0f);
	normMonster->SetSpeed(2.0f);
	normMonster->SetRotationSpeed(250.0f);
	normMonster->SetHP(50.0f);
	normMonster->SetOffensivePower(5.0f);
	// ���� �浹ü
	CCollider* monsterCollider = new CCollider;
	monsterCollider->SetOwner(normMonster);
	monsterCollider->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	normMonster->SetComponent(monsterCollider, COMPONENT_TYPE::COLLIDER);
	// ���� ����
	CState** states = normMonster->GetStates();
	((CIdleState*)states[(size_t)MONSTER_STATE::IDLE])->SetStayTime(2.0f);
	((CPatrolState*)states[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(-2.0f, 0.0f, 7.0f), XMFLOAT3(2.0f, 0.0f, 7.0f));
	((CAttackState*)states[(size_t)MONSTER_STATE::ATTACK])->SetAttackSpeed(1.0f);
	PushObject(normMonster, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster1 = new CNormalMonster(*normMonster);
	normMonster1->SetPosition(XMFLOAT3(-5.5f, 2.0f, 5.5f));
	CState** states1 = normMonster1->GetStates();
	((CPatrolState*)states1[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(-5.5f, 0.0f, 7.0f), XMFLOAT3(-5.5f, 0.0f, 4.0f));
	PushObject(normMonster1, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster2 = new CNormalMonster(*normMonster);
	normMonster2->SetPosition(XMFLOAT3(-7.0f, 2.0f, 0.0f));
	CState** states2 = normMonster2->GetStates();
	((CPatrolState*)states2[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(-8.0f, 0.0f, -1.0f), XMFLOAT3(-6.0f, 0.0f, 1.0f));
	PushObject(normMonster2, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster3 = new CNormalMonster(*normMonster);
	normMonster3->SetPosition(XMFLOAT3(-5.5f, 2.0f, -5.5f));
	CState** states3 = normMonster3->GetStates();
	((CPatrolState*)states3[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(-6.5f, 0.0f, -5.5f), XMFLOAT3(-4.5f, 0.0f, -5.5f));
	PushObject(normMonster3, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster4 = new CNormalMonster(*normMonster);
	normMonster4->SetPosition(XMFLOAT3(0.0f, 2.0f, -7.0f));
	CState** states4 = normMonster4->GetStates();
	((CPatrolState*)states4[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(-2.0f, 0.0f, -7.0f), XMFLOAT3(2.0f, 0.0f, -7.0f));
	PushObject(normMonster4, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster5 = new CNormalMonster(*normMonster);
	normMonster5->SetPosition(XMFLOAT3(5.5f, 2.0f, -5.5f));
	CState** states5 = normMonster5->GetStates();
	((CPatrolState*)states5[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(4.5f, 0.0f, -4.5f), XMFLOAT3(6.5f, 0.0f, -6.5f));
	PushObject(normMonster5, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster6 = new CNormalMonster(*normMonster);
	normMonster6->SetPosition(XMFLOAT3(7.0f, 2.0f, 0.0f));
	CState** states6 = normMonster6->GetStates();
	((CPatrolState*)states6[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(6.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 0.0f, 0.0f));
	PushObject(normMonster6, OBJECT_TYPE::MONSTER);

	CNormalMonster* normMonster7 = new CNormalMonster(*normMonster);
	normMonster7->SetPosition(XMFLOAT3(5.5f, 2.0f, 5.5f));
	CState** states7 = normMonster7->GetStates();
	((CPatrolState*)states7[(size_t)MONSTER_STATE::PATROL])->SetPatrolPos(XMFLOAT3(6.5f, 0.0f, 6.5f), XMFLOAT3(4.5f, 0.0f, 4.5f));
	PushObject(normMonster7, OBJECT_TYPE::MONSTER);
}

void CStartScene::CreateUI()
{
	CTexture* texture;

	int screenWidth = CCentral::Instance().GetScreenWidth();
	int screenHeight = CCentral::Instance().GetScreenHeight();

	// UI ����
	CStage1Button* stage1Button = new CStage1Button;
	stage1Button->SetUIPos(XMFLOAT2(screenWidth / 2.0f - 250.0f, screenHeight / 2.0f + 100.0f));
	stage1Button->SetUIWidth(100);
	stage1Button->SetUIHeight(100);
	stage1Button->SetVisualize(true);
	texture = CTextureMgr::Instance().GetTexture(L"level1.dds");
	stage1Button->SetTexture(texture);
	PushUI(stage1Button);

	CStage2Button* stage2Button = new CStage2Button;
	stage2Button->SetUIPos(XMFLOAT2(screenWidth / 2.0f - 50.0f, screenHeight / 2.0f + 100.0f));
	stage2Button->SetUIWidth(100);
	stage2Button->SetUIHeight(100);
	stage2Button->SetVisualize(true);
	texture = CTextureMgr::Instance().GetTexture(L"level2.dds");
	stage2Button->SetTexture(texture);
	PushUI(stage2Button);

	CStage3Button* stage3Button = new CStage3Button;
	stage3Button->SetUIPos(XMFLOAT2(screenWidth / 2.0f + 150.0f, screenHeight / 2.0f + 100.0f));
	stage3Button->SetUIWidth(100);
	stage3Button->SetUIHeight(100);
	stage3Button->SetVisualize(true);
	texture = CTextureMgr::Instance().GetTexture(L"level3.dds");
	stage3Button->SetTexture(texture);
	PushUI(stage3Button);

	CExitButton* exitButton = new CExitButton;
	exitButton->SetUIPos(XMFLOAT2(screenWidth / 2.0f + 200.0f, screenHeight / 2.0f + 250.0f));
	exitButton->SetUIWidth(50);
	exitButton->SetUIHeight(50);
	exitButton->SetVisualize(true);
	PushUI(exitButton);
}
