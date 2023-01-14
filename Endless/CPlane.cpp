#include "global.h"
#include "CPlane.h"

#include "CTextureMgr.h"
#include "CTexture.h"

#include "CObjectBuffer.h"
#include "CBufferMgr.h"

#include "CCollider.h"

CPlane::CPlane()
{
    // �̸� ����
    wstring objName = L"PLANE";
    SetObjectName(objName);

    // �� �ε�. ModelType����ü �迭 ����.
    LoadModel(L"cube.txt");

    CTexture* texture = CTextureMgr::Instance().GetTexture(L"gray1.dds");
    SetTexture(texture);

    CObjectBuffer* buffer = CBufferMgr::Instance().GetObjectBuffer(GetModelVtx(), GetModelIdx());
    SetBuffer(buffer);
}

CPlane::CPlane(const CPlane& _other)
    : CStructure(_other)
{
}

CPlane::~CPlane()
{
}

void CPlane::OnCollisionEnter(CCollider* _otherCol)
{
    CStructure::OnCollisionEnter(_otherCol);
}

void CPlane::OnCollision(CCollider* _otherCol)
{
    CStructure::OnCollision(_otherCol);
}

void CPlane::OnCollisionExit(CCollider* _otherCol)
{
    CStructure::OnCollisionExit(_otherCol);
}