//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "player.h"

// 키 입력 상태 저장용
static unordered_map<int, bool> keyStates;

Player::Player() :
	InstanceObject(), m_speed{ Settings::PlayerSpeed }
{
}

void Player::MouseEvent(FLOAT timeElapsed)
{
}

void Player::KeyboardEvent(FLOAT timeElapsed)
{
    if (!m_camera) return;

    XMFLOAT3 front{ m_camera->GetN() };
    front.y = 0.f;
    front = Vector3::Normalize(front);

    XMFLOAT3 right{ m_camera->GetU() };
    right.y = 0.f;
    right = Vector3::Normalize(right);

    XMFLOAT3 direction{ 0.f, 0.f, 0.f };

    // 키 입력 감지
    if (GetAsyncKeyState('W') & 0x8000) { direction = Vector3::Add(direction, front); }
    if (GetAsyncKeyState('S') & 0x8000) { direction = Vector3::Add(direction, Vector3::Negate(front)); }
    if (GetAsyncKeyState('A') & 0x8000) { direction = Vector3::Add(direction, Vector3::Negate(right)); }
    if (GetAsyncKeyState('D') & 0x8000) { direction = Vector3::Add(direction, right); }
    if (GetAsyncKeyState(VK_PRIOR) & 0x8000) { direction = Vector3::Add(direction, XMFLOAT3{ 0.f, 1.f, 0.f }); } // Page Up (위로 이동)
    if (GetAsyncKeyState(VK_NEXT) & 0x8000) { direction = Vector3::Add(direction, XMFLOAT3{ 0.f, -1.f, 0.f }); } // Page Down (아래로 이동)

    // 부드러운 이동 처리
    if (!Vector3::IsZero(direction))
    {
        direction = Vector3::Normalize(direction);
        Transform(Vector3::Mul(direction, m_speed * timeElapsed)); // 시간에 비례한 속도로 이동
    }
}

void Player::Update(FLOAT timeElapsed)
{
	if (m_camera) m_camera->UpdateEye(GetPosition());
}

void Player::SetCamera(const shared_ptr<Camera>& camera)
{
	m_camera = camera;
}

void Player::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
	UpdateWorldMatrix(); 
}

XMFLOAT3 Player::GetScale() const
{
	return m_scale;
}
