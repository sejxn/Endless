#include "global.h"
#include "CWall.h"

#include "CTextureMgr.h"
#include "CTexture.h"

#include "CObjectBuffer.h"
#include "CBufferMgr.h"

CWall::CWall()
{
    // �̸� ����
    wstring objName = L"WALL";
    SetObjectName(objName);

    // �� �ε�. ModelType����ü �迭 ����.
    LoadModel(L"cube.txt");
    
    CTexture* texture = CTextureMgr::Instance().GetTexture(L"white.dds");
    SetTexture(texture);

    CObjectBuffer* buffer = CBufferMgr::Instance().GetObjectBuffer(GetModelVtx(), GetModelIdx());
    SetBuffer(buffer);
}

CWall::CWall(const CWall& _other)
    : CStructure(_other)
{

}

CWall::~CWall()
{
}

void CWall::OnCollisionEnter(CCollider* _otherCol)
{
    CStructure::OnCollisionEnter(_otherCol);
}

void CWall::OnCollision(CCollider* _otherCol)
{
    CStructure::OnCollision(_otherCol);
}

void CWall::OnCollisionExit(CCollider* _otherCol)
{
    CStructure::OnCollisionExit(_otherCol);
}