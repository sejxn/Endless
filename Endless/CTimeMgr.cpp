#include "global.h"
#include "CTimeMgr.h"
#include "CCentral.h"

CTimeMgr::CTimeMgr()
	: m_titleString{}
	, m_deltaTime(0.0f)
	, m_accumulate(0.0f)
	, m_fps(0)
	, m_count(0)
	, m_timer{}
	, m_startTime{}
	, m_endTime{}
{
}

CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::Init()
{
	QueryPerformanceFrequency(&m_timer);
	QueryPerformanceCounter(&m_startTime);
}

void CTimeMgr::Update()
{
	// ��Ÿ Ÿ��
	QueryPerformanceCounter(&m_endTime);
	m_deltaTime = (m_endTime.QuadPart - m_startTime.QuadPart) / (float)m_timer.QuadPart;
	m_startTime = m_endTime;

	// dt ����
	if (m_deltaTime >= (1 / 60.0f))
		m_deltaTime = (1 / 60.0f);

	// �ð� ����
	m_accumulate += m_deltaTime;

	m_count++;

	// ���� 1�ʰ� �ƴٸ�
	if (m_accumulate >= 1.0f)
	{
		m_fps = m_count;
		m_count = 0;

		m_accumulate = 0.0f;
	}
}

void CTimeMgr::Render()
{
	// Ÿ��Ʋ �ٿ� FPS�� DT ��Ÿ��.
	swprintf(m_titleString, 128, L"FPS : %05d  DT : %f", m_fps, m_deltaTime);
	SetWindowTextW(CCentral::Instance().GetWindHandle(), m_titleString);
}