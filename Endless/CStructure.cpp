#include "global.h"
#include "CStructure.h"

#include "CShaderMgr.h"

#include "CCentral.h"

#include "CCamera.h"
#include "CLight.h"

#include "CScene.h"
#include "CSceneMgr.h"

#include "CTextureMgr.h"
#include "CTexture.h"
#include "CDepthTexture.h"

#include "CObjectBuffer.h"
#include "CCommonShader.h"

#include "CCollider.h"

CStructure::CStructure()
    : m_buffer(nullptr)
{
    
}

CStructure::CStructure(const CStructure& _other)
    : CObject(_other)
    , m_buffer(_other.m_buffer)
{
    
}

CStructure::~CStructure()
{
    
}

void CStructure::Update()
{
    // component update
    CObject::Update();
}

void CStructure::DepthRender(ID3D11DeviceContext* _deviceContext)
{
    CLight* light = CSceneMgr::Instance().GetCurrentScene()->GetLight();
    UINT indexCount = GetIndexCount();

    // ���� ���
    XMMATRIX worldMatrix = GetWorldMatrix();

    // �� �� ���
    XMMATRIX lightViewMatrix = light->GetViewMatrix();

    // �� ���� ���
    XMMATRIX lightProjectionMatrix = light->GetProjectionMatrix();

    // ����Ʈ �������� �׸� ���̹Ƿ� ����Ʈ �� ��İ� ����Ʈ ���� ��� ����
    MatrixBufferType transformMatrix;
    transformMatrix.worldMatrix = worldMatrix;
    transformMatrix.viewMatrix = lightViewMatrix;
    transformMatrix.projectionMatrix = lightProjectionMatrix;

    // ���� ���̴��� �׷���
    m_buffer->BufferSetting(_deviceContext);
    CShaderMgr::Instance().RenderDepthShader(_deviceContext, transformMatrix, indexCount);
}

void CStructure::Render(ID3D11DeviceContext* _deviceContext)
{
    CCamera* camera = CSceneMgr::Instance().GetCurrentScene()->GetCamera();
    CLight* light = CSceneMgr::Instance().GetCurrentScene()->GetLight();
    ID3D11ShaderResourceView* texture = GetTexture()->GetTextureView();
    CDepthTexture* depthTexture = CSceneMgr::Instance().GetCurrentScene()->GetDepthTexture();
    ID3D11ShaderResourceView* depthMapTexture = depthTexture->GetShaderResoucreView();
    UINT indexCount = GetIndexCount();

    // ���� ���
    XMMATRIX worldMatrix = GetWorldMatrix();

    // �� ���
    XMMATRIX viewMatrix = camera->GetViewMatrix();

    // ���� ���
    XMMATRIX projectionMatrix = CCentral::Instance().GetProjectionMatrix();

    // �� �� ���
    XMMATRIX lightViewMatrix = light->GetViewMatrix();

    // �� ���� ���
    XMMATRIX lightProjectionMatrix = light->GetProjectionMatrix();

    // �ٲ� ��� ����
    MatrixBufferType transformMatrix;
    transformMatrix.worldMatrix = worldMatrix;
    transformMatrix.viewMatrix = viewMatrix;
    transformMatrix.projectionMatrix = projectionMatrix;
    transformMatrix.lightViewMatrix = lightViewMatrix;
    transformMatrix.lightProjectionMatrix = lightProjectionMatrix;

    // ���� ���� �� ���̴� ����
    m_buffer->BufferSetting(_deviceContext);
    CShaderMgr::Instance().RenderShadowShader(_deviceContext, transformMatrix, texture, depthMapTexture, light, indexCount);
}

void CStructure::OnCollisionEnter(CCollider* _otherCol)
{
    // �������� ����� �� ������ ���������� �о
    RePosition(_otherCol);
}

void CStructure::OnCollision(CCollider* _otherCol)
{
    // �������� ����� �� ������ ���������� �о
    RePosition(_otherCol);
}

void CStructure::OnCollisionExit(CCollider* _otherCol)
{
    CObject* otherObj = _otherCol->GetOwner();
    otherObj->SetIsGround(false);
}

void CStructure::RePosition(CCollider* _otherCol)
{
    CObject* otherObj = _otherCol->GetOwner();
    XMFLOAT3 objectPos = otherObj->GetPosition();
    XMFLOAT3 velocity = otherObj->GetVelocity();

    CCollider* myCol = GetCollider();
    
    XMFLOAT3 myPos = myCol->GetPosition();
    XMFLOAT3 otherPos = _otherCol->GetPosition();
    XMFLOAT3 myScale = myCol->GetScale();
    XMFLOAT3 otherScale = _otherCol->GetScale();

    // �ڽ���(structure) �浹ü�� �������� ū��
    float myMinX = myPos.x - myScale.x, myMaxX = myPos.x + myScale.x;
    float myMinY = myPos.y - myScale.y, myMaxY = myPos.y + myScale.y;
    float myMinZ = myPos.z - myScale.z, myMaxZ = myPos.z + myScale.z;

    // �ڽŰ� �ε�ģ �ٸ� �浹ü�� �������� ū��
    float otherMinX = otherPos.x - otherScale.x, otherMaxX = otherPos.x + otherScale.x;
    float otherMinY = otherPos.y - otherScale.y, otherMaxY = otherPos.y + otherScale.y;
    float otherMinZ = otherPos.z - otherScale.z, otherMaxZ = otherPos.z + otherScale.z;

    float epsilon = 0.00001f;

    // ������ ũ��
    float overlapX;
    float overlapY;
    float overlapZ;
    
    // ������ ���� �ʴٸ� ����
    if (myMaxX <= otherMinX || otherMaxX <= myMinX || myMaxY <= otherMinY || otherMaxY <= myMinY
        || myMaxZ <= otherMinZ || otherMaxZ <= myMinZ)
        return;

    // �浹ü �������� �������ִ� ũ�⸦ ����, ���� �۰� ������ �� �������� �о��.
    // �о ���� ������Ʈ �������� �о (�浹ü�� ������Ʈ�� ���󰡹Ƿ�)
    //
    // ������ x�� ������
    if ((myMinX <= otherMinX && otherMaxX <= myMaxX) || (otherMinX <= myMinX && myMaxX <= otherMaxX))
    {
        if (myMinX <= otherMinX && otherMaxX <= myMaxX)
            overlapX = otherMaxX - otherMinX;
        else
            overlapX = myMaxX - myMinX;
    }
    else
    {
        if (otherMinX <= myMaxX && myMaxX <= otherMaxX)
            overlapX = myMaxX - otherMinX;
        else
            overlapX = otherMaxX - myMinX;
    }

    // ������ y�� ������
    if ((myMinY <= otherMinY && otherMaxY <= myMaxY) || (otherMinY <= myMinY && myMaxY <= otherMaxY))
    {
        if (myMinY <= otherMinY && otherMaxY <= myMaxY)
            overlapY = otherMaxY - otherMinY;
        else
            overlapY = myMaxY - myMinY;
    }
    else
    {
        if (otherMinY <= myMaxY && myMaxY <= otherMaxY)
            overlapY = myMaxY - otherMinY;
        else
            overlapY = otherMaxY - myMinY;
    }

    // ������ z�� ������
    if ((myMinZ <= otherMinZ && otherMaxZ <= myMaxZ) || (otherMinZ <= myMinZ && myMaxZ <= otherMaxZ))
    {
        if (myMinZ <= otherMinZ && otherMaxZ <= myMaxZ)
            overlapZ = otherMaxZ - otherMinZ;
        else
            overlapZ = myMaxZ - myMinZ;
    }
    else
    {
        if (otherMinZ <= myMaxZ && myMaxZ <= otherMaxZ)
            overlapZ = myMaxZ - otherMinZ;
        else
            overlapZ = otherMaxZ - myMinZ;
    }

    // ���� ���� ������ �� �������� �о
    // x�� �������� ���� �۴ٸ� x��������� �о
    // �о���� ������Ʈ�� �������� �о (epsilon���� ������ ����)
    float checkPos;
    if (overlapX <= overlapY && overlapX <= overlapZ)
    {
        checkPos = otherPos.x - myPos.x;

        // �����ʿ� �ִٸ� ���������� �о
        if (checkPos >= 0.0f)
        {
            //objectPos.x = myPos.x + myScale.x + otherScale.x;
            objectPos.x += overlapX - epsilon;
        }
        // ���ʿ� �ִٸ� �������� �о
        else
        {
            //objectPos.x = myPos.x - myScale.x - otherScale.x;
            objectPos.x -= overlapX + epsilon;
        }

        // �浹���� �� �ӵ��� 0���� ����
        velocity.x = 0.0f;
    }

    // y�� �������� ���� �۴ٸ� y��������� �о
    else if (overlapY <= overlapX && overlapY <= overlapZ)
    {
        checkPos = otherPos.y - myPos.y;

        // ���ʿ� �ִٸ� �������� �о
        if (checkPos >= 0.0f)
        {
            // y������ �浹���� �ÿ��� IsGround�� true�� ����
            //objectPos.y = myPos.y + myScale.y + otherScale.y;
            objectPos.y += overlapY - epsilon;
            otherObj->SetIsGround(true);
        }
        // �Ʒ��ʿ� �ִٸ� �Ʒ������� �о
        else
        {
            //objectPos.y = myPos.y - myScale.y - otherScale.y;
            objectPos.y -= overlapY + epsilon;
        }

        // �浹���� �� �ӵ��� 0���� ����
        velocity.y = 0.0f;
    }

    // z�� �������� ���� �۴ٸ� z��������� �о
    else
    {
        checkPos = otherPos.z - myPos.z;

        // ���ʿ� �ִٸ� �������� �о
        if (checkPos >= 0.0f)
        {
            //objectPos.z = myPos.z + myScale.z + otherScale.z;
            objectPos.z += overlapZ - epsilon;
        }
        // ���ʿ� �ִٸ� �������� �о
        else
        {
            //objectPos.z = myPos.z - myScale.z - otherScale.z;
            objectPos.z -= overlapZ + epsilon;
        }

        // �浹���� �� �ӵ��� 0���� ����
        velocity.z = 0.0f;
    }

    otherObj->SetVelocity(velocity);
    otherObj->SetPosition(objectPos);
}