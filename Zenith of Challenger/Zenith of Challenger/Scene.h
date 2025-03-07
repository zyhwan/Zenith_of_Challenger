//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------
#pragma once
#include "mesh.h"
#include "Shader.h"
#include "texture.h"
#include "Camera.h"
#include "Player.h"
#include "object.h"
#include "stdafx.h"
#include "Instance.h"
#include "Lighting.h"

class Scene
{
public:
	Scene();
	virtual ~Scene() = default;

	virtual void MouseEvent(HWND hWnd, FLOAT timeElapsed);
	virtual void KeyboardEvent(FLOAT timeElapsed);
	virtual void Update(FLOAT timeElapsed);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	virtual void BuildObjects(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const ComPtr<ID3D12RootSignature>& rootSignature);
	virtual void ReleaseUploadBuffer();

	virtual void MouseEvent(UINT message, LPARAM lParam);
	virtual void KeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void KeyboardEvent(UINT message, WPARAM wParam);

	virtual void ClearSceneResources(); //���ҽ� ���� ���� �Լ� �߰�

	void SetDevice(const ComPtr<ID3D12Device>& device) { m_device = device; }
	void SetCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList) { m_commandList = commandList; }
	void SetRootSignature(const ComPtr<ID3D12RootSignature>& rootSignature) { m_rootSignature = rootSignature; }
protected:
	virtual void BuildShaders(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const ComPtr<ID3D12RootSignature>& rootSignature);
	virtual void BuildMeshes(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList);
	virtual void BuildTextures(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList);
	virtual void BuildMaterials(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList);
	virtual void BuildObjects(const ComPtr<ID3D12Device>& device);

protected:
	unordered_map<string, shared_ptr<Shader>> m_shaders;
	unordered_map<string, shared_ptr<MeshBase>> m_meshes;
	unordered_map<string, shared_ptr<Texture>> m_textures;
	unordered_map<string, shared_ptr<Material>> m_materials;

	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	unique_ptr<LightSystem> m_lightSystem;
	unique_ptr<Sun>		m_sun;
	shared_ptr<Camera> m_camera;
	shared_ptr<Player> m_player;
	vector<shared_ptr<InstanceObject>> m_objects;
	shared_ptr<GameObject> m_skybox;
	shared_ptr<Terrain> m_terrain;

	unique_ptr<Instance> m_instanceObject;
};
