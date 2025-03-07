//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "player.h"

// Ű �Է� ���� �����
static unordered_map<int, bool> keyStates;
XMFLOAT3 velocity = { 0.f, 0.f, 0.f };  // ���� �̵� �ӵ�
const float maxSpeed = Settings::PlayerSpeed; // �ִ� �ӵ�
//const float acceleration = 20.0f; // ���ӵ�
//const float deceleration = 30.0f; // ���ӵ�
const float acceleration = 50.0f; // �ﰢ���� ������ ���� ���ӵ� ����

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

    XMFLOAT3 targetDirection{ 0.f, 0.f, 0.f };

    // Ű �Է� ���� ������Ʈ
    keyStates['W'] = (GetAsyncKeyState('W') & 0x8000);
    keyStates['S'] = (GetAsyncKeyState('S') & 0x8000);
    keyStates['A'] = (GetAsyncKeyState('A') & 0x8000);
    keyStates['D'] = (GetAsyncKeyState('D') & 0x8000);
    keyStates[VK_PRIOR] = (GetAsyncKeyState(VK_PRIOR) & 0x8000); // Page Up
    keyStates[VK_NEXT] = (GetAsyncKeyState(VK_NEXT) & 0x8000); // Page Down

    // �̵� ���� ���
    if (keyStates['W']) { targetDirection = Vector3::Add(targetDirection, front); }
    if (keyStates['S']) { targetDirection = Vector3::Add(targetDirection, Vector3::Negate(front)); }
    if (keyStates['A']) { targetDirection = Vector3::Add(targetDirection, Vector3::Negate(right)); }
    if (keyStates['D']) { targetDirection = Vector3::Add(targetDirection, right); }
    if (keyStates[VK_PRIOR]) { targetDirection = Vector3::Add(targetDirection, XMFLOAT3{ 0.f, 1.f, 0.f }); } // ���� �̵�
    if (keyStates[VK_NEXT]) { targetDirection = Vector3::Add(targetDirection, XMFLOAT3{ 0.f, -1.f, 0.f }); } // �Ʒ��� �̵�

    // �ﰢ���� �̵� (����X, �ٷ� ����)
    if (!Vector3::IsZero(targetDirection))
    {
        targetDirection = Vector3::Normalize(targetDirection);
        velocity = Vector3::Mul(targetDirection, maxSpeed); // �ִ� �ӵ��� ��� �̵�
    }
    else
    {
        // ��� ���ߵ��� ����
        velocity = { 0.f, 0.f, 0.f };
    }

    // ���� �̵�
    Transform(Vector3::Mul(velocity, timeElapsed));
}

void Player::Update(FLOAT timeElapsed)
{
	if (m_camera) m_camera->UpdateEye(GetPosition());
}

void Player::Move(XMFLOAT3 direction, FLOAT speed)
{
    direction = Vector3::Normalize(direction);
    Transform(Vector3::Mul(direction, speed));
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
