#include "global.h"
#include "CRigidBody.h"

#include "CTimeMgr.h"

#include "CObject.h"

CRigidBody::CRigidBody()
	: m_gravity(-15.0f)
	, m_acceleration(0.0f)
	, m_friction(0.0f)
{
	
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Update()
{
	RigidBodyUpdate();
}

void CRigidBody::RigidBodyUpdate()
{
	CObject* owner = GetOwner();
	float dt = CTimeMgr::Instance().GetDT();
	float maxSpeed = owner->GetMaxSpeed();
	XMFLOAT3 gravityForce;

	// ���� �ִ� ���¶�� �߷� ����
	if (owner->GetIsGround())
		gravityForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else
		gravityForce = XMFLOAT3(0.0f, owner->GetMass() * m_gravity, 0.0f);

	// ������ ���� ���ӵ�. �ӵ��� �����ǵ� ������
	XMFLOAT3 acceleration = XMFLOAT3(gravityForce.x / owner->GetMass(),
		gravityForce.y / owner->GetMass(), gravityForce.z / owner->GetMass());
	XMFLOAT3 velocity = owner->GetVelocity();
	XMFLOAT3 position = owner->GetPosition();

	XMVECTOR accelerationVec = XMLoadFloat3(&acceleration);
	XMVECTOR velocityVec = XMLoadFloat3(&velocity);
	XMVECTOR posVec = XMLoadFloat3(&position);

	// �ӵ��� (���ӵ� * �ð�), �������� (�ӵ� * �ð�)
	velocityVec += accelerationVec * dt;

	// �ӵ� ����(�ִ�ӷ� �Ѿ�� ���ϰ�)
	AdjustVelocity(velocityVec, maxSpeed);

	// ������ ����
	FrictionUpdate(velocityVec, dt);

	// ���� ����� �ӵ� ����
	XMStoreFloat3(&velocity, velocityVec);
	owner->SetVelocity(velocity);

	// ���� ����� �ӵ������� ������ ��� �� ����
	posVec += velocityVec * dt;
	XMStoreFloat3(&position, posVec);
	owner->SetPosition(position);
}

void CRigidBody::AdjustVelocity(XMVECTOR& _velocityVec, float _maxSpeed)
{
	// �ִ� �ӵ� �Ѿ�� ���ϰ� (x, z�ุ �ش��)
	XMVECTOR adjustVel = XMVectorSet(XMVectorGetX(_velocityVec), 0.0f, XMVectorGetZ(_velocityVec), 0.0f);
	if (_maxSpeed <= XMVectorGetX(XMVector3Length(adjustVel)))
	{
		adjustVel = XMVector3Normalize(adjustVel) * _maxSpeed;
	}

	// ������ �ӵ��� ���� (x, z��)
	_velocityVec = XMVectorSet(XMVectorGetX(adjustVel), XMVectorGetY(_velocityVec), XMVectorGetZ(adjustVel), 0.0f);
}

void CRigidBody::FrictionUpdate(XMVECTOR& _velocityVec, float _dt)
{
	// ������ ���� �ӵ��� �ݴ�������� ����� (x, z�ุ ����)
	XMVECTOR velocityVec = XMVectorSet(XMVectorGetX(_velocityVec), 0.0f, XMVectorGetZ(_velocityVec), 0.0f);
	XMVECTOR frictionVec = velocityVec;
	frictionVec = XMVector3Normalize(frictionVec);
	frictionVec = -frictionVec * _dt * m_friction;

	// ���� ���� ����� �ӵ�����(x, z���� �ӵ�) ������ �� ũ�ٸ� �ӵ��� 0���� ���� (y���� ����)
	// �ӵ��� �� ũ�ٸ� ���� ����
	if (XMVectorGetX(XMVector3Length(velocityVec)) <= XMVectorGetX(XMVector3Length(frictionVec)))
		_velocityVec = XMVectorSet(0.0f, XMVectorGetY(_velocityVec), 0.0f, 0.0f);
	else
		_velocityVec += frictionVec;
}

void CRigidBody::AddVelocity(XMFLOAT3 _velocity)
{
	CObject* owner = GetOwner();
	XMFLOAT3 velocity = owner->GetVelocity();
	velocity = XMFLOAT3(velocity.x + _velocity.x, velocity.y + _velocity.y, velocity.z + _velocity.z);
	owner->SetVelocity(velocity);
}