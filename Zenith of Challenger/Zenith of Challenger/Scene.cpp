//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------
#include "Scene.h"
#include "stdafx.h"
#include "GameFramework.h"

Scene::Scene()
{
}

void Scene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
}

void Scene::KeyboardEvent(FLOAT timeElapsed)
{
}

void Scene::Update(FLOAT timeElapsed)
{
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
}

void Scene::BuildObjects(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
}

void Scene::BuildShaders(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
}

void Scene::BuildMeshes(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
}

void Scene::BuildTextures(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
}

void Scene::BuildMaterials(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
}


void Scene::BuildObjects(const ComPtr<ID3D12Device>& device)
{
}

void Scene::ReleaseUploadBuffer()
{
	for (auto& mesh : views::values(m_meshes)) {
		mesh->ReleaseUploadBuffer();
	}
	for (auto& texture : views::values(m_textures)) {
		texture->ReleaseUploadBuffer();
	}
}

void Scene::MouseEvent(UINT message, LPARAM lParam)
{
}

void Scene::KeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}

void Scene::ClearSceneResources()
{
	std::cout << "Scene ���ҽ� ���� ���� ��..." << std::endl;

	//��� �޽�, �ؽ�ó, ������Ʈ ����
	m_meshes.clear();
	m_textures.clear();
	m_objects.clear();

	//GPU ���ҽ� ����
	if (m_device)
	{
		m_device->GetDeviceRemovedReason();
	}
}

//�ӽ� ����
