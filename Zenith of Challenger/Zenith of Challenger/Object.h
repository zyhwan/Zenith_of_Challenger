//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once
#include "stdafx.h"
#include "mesh.h"
#include "texture.h"
#include "buffer.h"
#include "Material.h"
#include "Lighting.h"

class Object abstract
{
public:
	Object();

	virtual void Update(FLOAT timeElapsed) = 0;

	void Transform(XMFLOAT3 shift);
	void Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll);

	void SetPosition(XMFLOAT3 position);
	XMFLOAT3 GetPosition() const;

	void UpdateWorldMatrix();

	virtual void SetScale(XMFLOAT3 scale);
	virtual XMFLOAT3 GetScale() const;
protected:
	XMFLOAT4X4			m_worldMatrix;

	XMFLOAT3			m_right;
	XMFLOAT3			m_up;
	XMFLOAT3			m_front;

	XMFLOAT3             m_scale;
};

struct InstanceData;
class InstanceObject : public Object
{
public:
	InstanceObject();
	virtual ~InstanceObject() = default;

	virtual void Update(FLOAT timeElapsed) override;
	void UpdateShaderVariable(InstanceData& buffer);

	void SetTextureIndex(UINT textureIndex);
	void SetMaterialIndex(UINT materialIndex);

	//FBX �޽� ���� �Լ� �߰�
	void SetMesh(const shared_ptr<MeshBase>& mesh) { m_mesh = mesh; }
protected:
	UINT				m_textureIndex;
	UINT				m_materialIndex;
	shared_ptr<MeshBase> m_mesh; //FBX ���� ������ ���� �߰�
};

struct ObjectData : public BufferBase
{
	XMFLOAT4X4 worldMatrix;    // 64 bytes
	XMFLOAT4 baseColor;        // 16 bytes
	UINT useTexture;           // 4 bytes
	UINT textureIndex;          // 4 bytes
	XMFLOAT2 padding = {};     // 12 bytes �� �� 32 bytes�� 16����Ʈ ���� ����
};

class GameObject : public Object
{
public:
	GameObject(const ComPtr<ID3D12Device>& device);
	virtual ~GameObject() = default;

	virtual void Update(FLOAT timeElapsed);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetMesh(const shared_ptr<MeshBase>& mesh);
	void SetTexture(const shared_ptr<Texture>& texture);
	void SetMaterial(const shared_ptr<Material>& material);
	void SetBaseColor(const XMFLOAT4& color);            // �߰�
	void SetUseTexture(bool use);                        // �߰�

	void SetTextureIndex(int index) { m_textureIndex = index; }

	int GetTextureIndex() const { return m_textureIndex; }

	// ��ȯ ��� ���� �Լ� �߰�
	void SetWorldMatrix(const XMMATRIX& worldMatrix);

	void SetSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);
protected:
	shared_ptr<MeshBase> m_mesh;
	shared_ptr<Texture> m_texture;
	shared_ptr<Material> m_material;

	unique_ptr<UploadBuffer<ObjectData>> m_constantBuffer;

	XMFLOAT4 m_baseColor{ 1.f, 1.f, 1.f, 1.f };           // �⺻ ����
	BOOL m_useTexture = FALSE;                           // �ؽ�ó ��� ����

	D3D12_GPU_DESCRIPTOR_HANDLE m_srvHandle{};  // �ؽ�ó SRV �ڵ�
	int m_textureIndex = 0;
};

class RotatingObject : public InstanceObject
{
public:
	RotatingObject();
	~RotatingObject() override = default;

	void Update(FLOAT timeElapsed) override;

private:
	FLOAT m_rotatingSpeed;
};

class LightObject : public RotatingObject
{
public:
	LightObject(const shared_ptr<SpotLight>& light);
	~LightObject() override = default;

	virtual void Update(FLOAT timeElapsed) override;
private:
	shared_ptr<SpotLight> m_light;
};

class Sun : public InstanceObject
{
public:
	Sun(const shared_ptr<DirectionalLight>& light);
	~Sun() override = default;

	void SetStrength(XMFLOAT3 strength);

	void Update(FLOAT timeElapsed) override;

private:
	shared_ptr<DirectionalLight> m_light;

	XMFLOAT3 m_strength;
	FLOAT m_phi;
	FLOAT m_theta;
	const FLOAT m_radius;
};

class Terrain : public GameObject
{
public:
	Terrain(const ComPtr<ID3D12Device>& device);
	~Terrain() override = default;

	FLOAT GetHeight(FLOAT x, FLOAT z);
};
