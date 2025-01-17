//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "player.h"

Player::Player() :
	InstanceObject(), m_speed{ Settings::PlayerSpeed }
{
}

void Player::MouseEvent(FLOAT timeElapsed)
{
}

void Player::KeyboardEvent(FLOAT timeElapsed)
{
	XMFLOAT3 front{ m_camera->GetN() }; front.y = 0.f;
	front = Vector3::Normalize(front);
	XMFLOAT3 back{ Vector3::Negate(front) };
	XMFLOAT3 right{ m_camera->GetU() };
	XMFLOAT3 left{ Vector3::Negate(right) };
	XMFLOAT3 direction{};

	if (GetAsyncKeyState('W') && GetAsyncKeyState('A') & 0x8000) {
		direction = Vector3::Normalize(Vector3::Add(front, left));
	}
	else if (GetAsyncKeyState('W') && GetAsyncKeyState('D') & 0x8000) {
		direction = Vector3::Normalize(Vector3::Add(front, right));
	}
	else if (GetAsyncKeyState('S') && GetAsyncKeyState('A') & 0x8000) {
		direction = Vector3::Normalize(Vector3::Add(back, left));
	}
	else if (GetAsyncKeyState('S') && GetAsyncKeyState('D') & 0x8000) {
		direction = Vector3::Normalize(Vector3::Add(back, right));
	}
	else if (GetAsyncKeyState('W') & 0x8000) {
		direction = front;
	}
	else if (GetAsyncKeyState('A') & 0x8000) {
		direction = left;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		direction = back;
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		direction = right;
	}

	if (GetAsyncKeyState('Q') & 0x8000) {
		exit(1);
	}

	if (GetAsyncKeyState('W') || GetAsyncKeyState('A') ||
		GetAsyncKeyState('S') || (GetAsyncKeyState('D') & 0x8000)) {
		XMFLOAT3 angle{ Vector3::Angle(m_front, direction) };
		XMFLOAT3 cross{ Vector3::Cross(m_front, direction) };
		if (cross.y >= 0.f) {
			Rotate(0.f, XMConvertToDegrees(angle.y) * 10.f * timeElapsed, 0.f);
		}
		else {
			Rotate(0.f, -XMConvertToDegrees(angle.y) * 10.f * timeElapsed, 0.f);
		}
		Transform(Vector3::Mul(m_front, m_speed * timeElapsed));
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
