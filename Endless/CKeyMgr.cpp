#include "global.h"
#include "CKeyMgr.h"

#include "CCentral.h"

CKeyMgr::CKeyMgr()
	: m_directInput(nullptr)
	, m_keyboardDevice(nullptr)
	, m_mouseDevice(nullptr)
	, m_keyboardState{}
	, m_mouseState{}
	, m_mousePos(XMFLOAT2(0.0f, 0.0f))
{
}

CKeyMgr::~CKeyMgr()
{
	if (m_mouseDevice)
	{
		m_mouseDevice->Unacquire();
		m_mouseDevice->Release();
	}
	
	if (m_keyboardDevice)
	{
		m_keyboardDevice->Unacquire();
		m_keyboardDevice->Release();
	}

	if (m_directInput)
	{
		m_directInput->Release();
	}
}

void CKeyMgr::Init(HINSTANCE _hinstance, HWND _hwnd)
{
	// Direct Input �������̽� �ʱ�ȭ
	DirectInput8Create(_hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_directInput, NULL);

	// Ű������ Direct Input �������̽� ���� �� ����
	m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboardDevice, NULL);
	m_keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	m_keyboardDevice->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	m_keyboardDevice->Acquire();

	// ���콺 Direct Input �������̽� ���� �� ����
	m_directInput->CreateDevice(GUID_SysMouse, &m_mouseDevice, NULL);
	m_mouseDevice->SetDataFormat(&c_dfDIMouse);
	m_mouseDevice->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_mouseDevice->Acquire();

	// Ű, ���콺 ������Ʈ �ʱ�ȭ
	for (size_t i = 0; i < (size_t)KEY_BUTTON::END; ++i)
		m_currentKeyState[i] = KEY_STATE::NONE;
	for (size_t i = 0; i < (size_t)MOUSE_BUTTON::END; ++i)
		m_currentMouseState[i] = KEY_STATE::NONE;
}

void CKeyMgr::Update()
{
	// Ű���� �Է� �ޱ�
	HRESULT result = m_keyboardDevice->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// INPUTLOST �̰ų� NOTACQUIRED��� �ٽ� �����Ѵ�.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboardDevice->Acquire();
		}
	}

	// ���콺 �Է� �ޱ�
	result = m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// INPUTLOST �̰ų� NOTACQUIRED��� �ٽ� �����Ѵ�.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouseDevice->Acquire();
		}
	}

	// Ű����� ���콺�� ������Ʈ. ���� ������ �Ѵ�.
	KeyUpdate();
	MouseUpdate();

	// Mouse ��ġ ���
	POINT ptPos = {};
	GetCursorPos(&ptPos);
	ScreenToClient(CCentral::Instance().GetWindHandle(), &ptPos);
	m_mousePos = XMFLOAT2((float)ptPos.x, (float)ptPos.y);
}

void CKeyMgr::KeyUpdate()
{
	for (size_t i = 0; i < (size_t)KEY_BUTTON::END; ++i)
	{
		// Ű�� ������ ��
		if (m_keyboardState[m_key[i]] & 0x80)
		{
			// ���� Ű ���¿� ���� ������Ʈ ����
			if (KEY_STATE::NONE == m_currentKeyState[i] ||
				KEY_STATE::AWAY == m_currentKeyState[i])
			{
				m_currentKeyState[i] = KEY_STATE::TAP;
			}
			else if (KEY_STATE::TAP == m_currentKeyState[i])
			{
				m_currentKeyState[i] = KEY_STATE::HOLD;
			}
		}
		else
		{
			// ������ ���̰ų� Ȧ�忴�ٸ� ����̷� ����
			if (KEY_STATE::TAP == m_currentKeyState[i] ||
				KEY_STATE::HOLD == m_currentKeyState[i])
			{
				m_currentKeyState[i] = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_currentKeyState[i])
			{
				m_currentKeyState[i] = KEY_STATE::NONE;
			}
		}
	}
}

void CKeyMgr::MouseUpdate()
{
	for (size_t i = 0; i < (size_t)MOUSE_BUTTON::END; ++i)
	{
		// ���콺�� ������ ��
		if (m_mouseState.rgbButtons[i] & 0x80)
		{
			// ���� ���콺�� ���� ���¿� ���� ����
			// ������ ���� ���¿��ų� ���� ���¿��ٸ� �� ���·� ����
			if (KEY_STATE::NONE == m_currentMouseState[i] ||
				KEY_STATE::AWAY == m_currentMouseState[i])
			{
				m_currentMouseState[i] = KEY_STATE::TAP;
			}
			// �� ���¿��ٸ� Ȧ�� ���·� ����
			else if (KEY_STATE::TAP == m_currentMouseState[i])
			{
				m_currentMouseState[i] = KEY_STATE::HOLD;
			}
			// Ȧ�� ���¿��ٸ� �������� ����
		}
		// ������ �ʾ��� ��
		else
		{
			// ������ ���̰ų� Ȧ�忴�ٸ� ����̷� ����
			if (KEY_STATE::TAP == m_currentMouseState[i] ||
				KEY_STATE::HOLD == m_currentMouseState[i])
			{
				m_currentMouseState[i] = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_currentMouseState[i])
			{
				m_currentMouseState[i] = KEY_STATE::NONE;
			}
		}
	}
}

bool CKeyMgr::CheckKeyState(KEY_BUTTON _keyButton, KEY_STATE _keyState)
{
	// �ش� Ű�� �ش� ���¿� ���ٸ� true ��ȯ
	if (_keyState == m_currentKeyState[(size_t)_keyButton])
	{
		return true;
	}

	return false;
}

bool CKeyMgr::CheckMouseState(MOUSE_BUTTON _mouseButton, KEY_STATE _keyState)
{
	// �ش� ���콺 Ű�� �ش� ���¿� ���ٸ� true ��ȯ
	if (_keyState == m_currentMouseState[(size_t)_mouseButton])
	{
		return true;
	}

	return false;
}
