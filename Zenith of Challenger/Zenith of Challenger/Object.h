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
#include "Shader.h"

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

	//FBX 메쉬 설정 함수 추가
	void SetMesh(const shared_ptr<MeshBase>& mesh) { m_mesh = mesh; }
protected:
	UINT				m_textureIndex;
	UINT				m_materialIndex;
	shared_ptr<MeshBase> m_mesh; //FBX 모델을 저장할 변수 추가
};

struct ObjectData : public BufferBase
{
	XMFLOAT4X4 worldMatrix;    // 64 bytes
	XMFLOAT4 baseColor;        // 16 bytes
	UINT useTexture;           // 4 bytes
	UINT textureIndex;          // 4 bytes
	XMFLOAT2 padding = {};     // 12 bytes → 총 32 bytes로 16바이트 정렬 유지
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
	void SetShader(const shared_ptr<Shader>& shader);
	void SetBaseColor(const XMFLOAT4& color);            // 추가
	void SetUseTexture(bool use);                        // 추가

	void SetTextureIndex(int index) { m_textureIndex = index; }

	int GetTextureIndex() const { return m_textureIndex; }

	// 변환 행렬 설정 함수 추가
	void SetWorldMatrix(const XMMATRIX& worldMatrix);

	void SetSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);
protected:
	shared_ptr<MeshBase> m_mesh;
	shared_ptr<Texture> m_texture;
	shared_ptr<Material> m_material;
	shared_ptr<Shader> m_shader;

	unique_ptr<UploadBuffer<ObjectData>> m_constantBuffer;

	XMFLOAT4 m_baseColor{ 1.f, 1.f, 1.f, 1.f };           // 기본 색상
	BOOL m_useTexture = FALSE;                           // 텍스처 사용 여부

	D3D12_GPU_DESCRIPTOR_HANDLE m_srvHandle{};  // 텍스처 SRV 핸들
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
