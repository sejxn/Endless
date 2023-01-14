#pragma once
#include "CComponent.h"

#define MAXIMUM_ANGLE 89.5f  // �ִ� ����. �� ������ �Ѿ�� ����

class CPlayerController :
    public CComponent
{
private:
    float       m_mouseSens;        // ���콺 �ΰ���
    float       m_jumpVel;          // ���� �ӷ�

private:
    void RotationUpdate();
    void TransformUpdate();
    void JumpUpdate();

public:
    void Update() override;

public:
    void SetMouseSens(float _mouseSens) { m_mouseSens = _mouseSens; }
    void SetJumpVel(float _jumpVel) { m_jumpVel = _jumpVel; }

public:
    CPlayerController();
    ~CPlayerController();
};

