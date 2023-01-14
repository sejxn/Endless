#include "global.h"
#include "CPlayerController.h"

#include "CCamera.h"
#include "CScene.h"

#include "CSceneMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"

#include "CObject.h"
#include "CPlayer.h"

#include "CRigidBody.h"

CPlayerController::CPlayerController()
    : m_mouseSens(0.0f)
    , m_jumpVel(0.0f)
{
}

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Update()
{
	RotationUpdate();
	TransformUpdate();
    JumpUpdate();
}

void CPlayerController::RotationUpdate()
{
    // ���콺 ����
    DIMOUSESTATE mouseState = CKeyMgr::Instance().GetMouseState();

    CObject* owner = GetOwner();

    float dt = CTimeMgr::Instance().GetDT();

    XMFLOAT3 positon = owner->GetPosition();
    XMVECTOR positionVec = XMLoadFloat3(&positon);				// ���� ������
    XMFLOAT3 up = owner->GetUpVector();
    XMVECTOR upVec = XMLoadFloat3(&up);					        // �� ����

    // ���콺�� ������ �ִٸ�
    if (0 != mouseState.lX || 0 != mouseState.lY)
    {
        // owner�� ȸ������ ����
        XMFLOAT3 rotate = owner->GetRotation();
        rotate.y += dt * m_mouseSens * mouseState.lX;
        rotate.x += dt * m_mouseSens * mouseState.lY;

        // �ִ� ���� �̻� �Ѿ�� �ʰ� ����
        if (MAXIMUM_ANGLE < rotate.x)
            rotate.x = MAXIMUM_ANGLE;
        else if (rotate.x < -MAXIMUM_ANGLE)
            rotate.x = -MAXIMUM_ANGLE;

        if (360.0f <= abs(rotate.y))
            rotate.y = 0.0f;

        owner->SetRotation(rotate);
    }
}

void CPlayerController::TransformUpdate()
{
    CCamera* camera = CSceneMgr::Instance().GetCurrentScene()->GetCamera();
    CObject* owner = GetOwner();

    // �ٶ󺸰��ִ� ���� �������� ����(��, ������)�� ����
    XMVECTOR forwardVec = owner->GetForwardVec();
    XMVECTOR rightVec = owner->GetRightVec();

    XMFLOAT3 up = owner->GetUpVector();
    XMVECTOR upVec = XMLoadFloat3(&up);

    // owner�� �ӵ�
    XMFLOAT3 velocity = owner->GetVelocity();
    XMVECTOR velocityVec = XMLoadFloat3(&velocity);

    // ���� �����ų �ӵ�
    XMVECTOR finalVel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    forwardVec = XMVector3Cross(rightVec, upVec);                   // �÷��̾� ���� ī�޶�
    //forwardVec = owner->GetForwardVec();                          // ���� ���� ī�޶�, �߷� ���ŵ� �ؾߵ�

    float speed = owner->GetSpeed();
    float dt = CTimeMgr::Instance().GetDT();

    if (CKeyMgr::Instance().CheckKeyState(KEY_BUTTON::KEY_A, KEY_STATE::HOLD))
    {
        finalVel -= rightVec;
    }
    if (CKeyMgr::Instance().CheckKeyState(KEY_BUTTON::KEY_D, KEY_STATE::HOLD))
    {
        finalVel += rightVec;
    }
    if (CKeyMgr::Instance().CheckKeyState(KEY_BUTTON::KEY_W, KEY_STATE::HOLD))
    {
        finalVel += forwardVec;
    }
    if (CKeyMgr::Instance().CheckKeyState(KEY_BUTTON::KEY_S, KEY_STATE::HOLD))
    {
        finalVel -= forwardVec;
    }

    // �ӵ� ����
    velocityVec = XMVector3Normalize(finalVel) * dt * speed;

    // ������ٵ��� AddVelocity�� �ӵ��� ������.
    XMStoreFloat3(&velocity, velocityVec);
    owner->GetRigidBody()->AddVelocity(velocity);
}

void CPlayerController::JumpUpdate()
{
    CObject* owner = GetOwner();
    if (CKeyMgr::Instance().CheckKeyState(KEY_BUTTON::KEY_SPACE, KEY_STATE::TAP) && owner->GetIsGround())
    {
        owner->GetRigidBody()->AddVelocity(XMFLOAT3(0.0f, m_jumpVel, 0.0f));
    }
}
