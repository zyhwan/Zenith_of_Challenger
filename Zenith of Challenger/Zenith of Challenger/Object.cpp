//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------
#include "object.h"

GameObject::GameObject() : m_right{ 1.f, 0.f, 0.f }, m_up{ 0.f, 1.f, 0.f }, m_front{ 0.f, 0.f, 1.f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

void GameObject::Update(FLOAT timeElapsed)
{

}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	UpdateShaderVariable(commandList);
	m_mesh->Render(commandList);
}

void GameObject::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(RootParameter::GameObject, 16, &worldMatrix, 0);

	if (m_texture) m_texture->UpdateShaderVariable(commandList);
}

void GameObject::Transform(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(GetPosition(), shift));
}

void GameObject::Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4(&m_worldMatrix, rotate * XMLoadFloat4x4(&m_worldMatrix));

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void GameObject::SetMesh(const shared_ptr<MeshBase>& mesh)
{
	m_mesh = mesh;
}

void GameObject::SetTexture(const shared_ptr<Texture>& texture)
{
	m_texture = texture;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

void GameObject::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
	UpdateWorldMatrix(); // Ensure the world matrix is updated when scale changes
}

XMFLOAT3 GameObject::GetScale() const
{
	return m_scale;
}

void GameObject::UpdateWorldMatrix()
{
	XMMATRIX scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	XMMATRIX translationMatrix = XMMatrixTranslation(m_right.x, m_up.y, m_front.z); 

	XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	XMStoreFloat4x4(&m_worldMatrix, worldMatrix);
}

RotatingObject::RotatingObject() : GameObject()
{
	std::random_device rd;
	std::mt19937 randomEngine(rd());
	std::uniform_real_distribution<float> dis(10.0f, 50.0f);
	m_rotatingSpeed = dis(randomEngine);
}

void RotatingObject::Update(FLOAT timeElapsed)
{
	Rotate(0.f, m_rotatingSpeed * timeElapsed, 0.f);
}