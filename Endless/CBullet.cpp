#include "global.h"
#include "CBullet.h"

#include "CCentral.h"

#include "CTextureMgr.h"
#include "CTexture.h"

#include "CObjectBuffer.h"
#include "CBufferMgr.h"

#include "CLight.h"
#include "CCamera.h"

#include "CSceneMgr.h"
#include "CScene.h"

#include "CShaderMgr.h"
#include "CTimeMgr.h"

#include "CDepthTexture.h"

CBullet::CBullet()
    : m_buffer(nullptr)
    , m_forwardDir(XMFLOAT3(0.0f, 0.0f, 0.0f))
    , m_accumulateTime(0.0f)
    , m_lifeTime(0.0f)
{
    // �̸� ����
    wstring objName = L"MONSTER";
    SetObjectName(objName);

    // �� �ε�, �ؽ��� ����, ���� ����
    LoadModel(L"sphere.txt");

    CTexture* texture = CTextureMgr::Instance().GetTexture(L"black.dds");
    SetTexture(texture);

    m_buffer = CBufferMgr::Instance().GetBulletBuffer(GetModelVtx(), GetModelIdx());
}

CBullet::CBullet(const CBullet& _other)
    : CObject(_other)
    , m_buffer(_other.m_buffer)
    , m_forwardDir(_other.m_forwardDir)
    , m_accumulateTime(_other.m_accumulateTime)
    , m_lifeTime(_other.m_lifeTime)
{

}

CBullet::~CBullet()
{
    
}

void CBullet::Update()
{
    // ������Ʈ ������Ʈ
    CObject::Update();

    // �ٶ󺸰� �ִ� �������� �̵��� ��
    float dt = CTimeMgr::Instance().GetDT();
    float speed = GetSpeed();

    XMVECTOR forwardDirVec = XMLoadFloat3(&m_forwardDir);
    forwardDirVec = XMVector3Normalize(forwardDirVec);

    XMFLOAT3 position = GetPosition();
    XMVECTOR posVec = XMLoadFloat3(&position);

    posVec += forwardDirVec * speed * dt;
    XMStoreFloat3(&position, posVec);
    SetPosition(position);

    // �ð� ���� �� ���� �ð��� �� �Ǹ� �ڱ� �ڽ��� �ݳ�
    m_accumulateTime += dt;
    if (m_lifeTime <= m_accumulateTime)
    {
        CSceneMgr::Instance().GetCurrentScene()->ReturnBullet(this);
        m_accumulateTime = 0.0f;
    }
}

void CBullet::DepthRender(ID3D11DeviceContext* _deviceContext)
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

void CBullet::Render(ID3D11DeviceContext* _deviceContext)
{
    CCamera* camera = CSceneMgr::Instance().GetCurrentScene()->GetCamera();
    CLight* light = CSceneMgr::Instance().GetCurrentScene()->GetLight();
    ID3D11ShaderResourceView* texture = GetTexture()->GetTextureView();
    CDepthTexture* depthTexture = CSceneMgr::Instance().GetCurrentScene()->GetDepthTexture();
    ID3D11ShaderResourceView* depthMapTexture = depthTexture->GetShaderResoucreView();
    UINT indexCount = GetIndexCount();

    // ���� ���
    XMMATRIX worldMatrix = GetWorldMatrix();

    // ī�޶� �� ���
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

void CBullet::OnCollisionEnter(CCollider* _otherCol)
{
    // �������� ������ �����
    CSceneMgr::Instance().GetCurrentScene()->ReturnBullet(this);
    m_accumulateTime = 0.0f;
}

void CBullet::OnCollision(CCollider* _otherCol)
{
}

void CBullet::OnCollisionExit(CCollider* _otherCol)
{
}