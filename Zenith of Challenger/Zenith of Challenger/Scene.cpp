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
	SetCursor(NULL);
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);

	POINT lastMousePosition{
		windowRect.left + static_cast<LONG>(gGameFramework->GetWindowWidth() / 2),
		windowRect.top + static_cast<LONG>(gGameFramework->GetWindowHeight() / 2) };
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	float dx = XMConvertToRadians(-0.15f * static_cast<FLOAT>(mousePosition.x - lastMousePosition.x));
	float dy = XMConvertToRadians(-0.15f * static_cast<FLOAT>(mousePosition.y - lastMousePosition.y));

	if (m_camera) {
		m_camera->RotateYaw(dx);
		m_camera->RotatePitch(dy);
	}
	SetCursorPos(lastMousePosition.x, lastMousePosition.y);

	m_player->MouseEvent(timeElapsed);
}

void Scene::KeyboardEvent(FLOAT timeElapsed)
{
	m_player->KeyboardEvent(timeElapsed);
}

void Scene::Update(FLOAT timeElapsed)
{
	m_player->Update(timeElapsed);
	for (auto& object : m_objects) {
		object->Update(timeElapsed);
	}
	m_skybox->SetPosition(m_camera->GetEye());
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_camera->UpdateShaderVariable(commandList);

	m_shaders.at("OBJECT")->UpdateShaderVariable(commandList);
	for (auto& object : m_objects) {
		object->Render(commandList);
	}
	m_player->Render(commandList);

	m_shaders.at("DETAIL")->UpdateShaderVariable(commandList);
	m_terrain->Render(commandList);

	m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
	m_skybox->Render(commandList);
}

void Scene::BuildObjects(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	BuildShaders(device, commandList, rootSignature);
	BuildMeshes(device, commandList);
	BuildTextures(device, commandList);

	BuildObjects();
}

inline void Scene::BuildShaders(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	auto objectShader = make_shared<ObjectShader>(device, rootSignature);
	m_shaders.insert({ "OBJECT", objectShader });
	auto skyboxShader = make_shared<SkyboxShader>(device, rootSignature);
	m_shaders.insert({ "SKYBOX", skyboxShader });
	auto detailShader = make_shared<DetailShader>(device, rootSignature);
	m_shaders.insert({ "DETAIL", detailShader });
}

inline void Scene::BuildMeshes(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeMesh = make_shared<Mesh<TextureVertex>>(device, commandList,
		TEXT("Model/CubeMesh.binary"));
	m_meshes.insert({ "CUBE", cubeMesh });
	auto skyboxMesh = make_shared<Mesh<Vertex>>(device, commandList,
		TEXT("Model/SkyboxMesh.binary"));
	m_meshes.insert({ "SKYBOX", skyboxMesh });
	auto terrainMesh = make_shared<TerrainMesh>(device, commandList,
		TEXT("Model/HeightMap.raw"));
	m_meshes.insert({ "TERRAIN", terrainMesh });
}

inline void Scene::BuildTextures(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto checkboardTexture = make_shared<Texture>(device, commandList,
		TEXT("Image/Rock01.dds"), RootParameter::Texture0);
	m_textures.insert({ "PLAYER", checkboardTexture });
	auto brickTextire = make_shared<Texture>(device, commandList,
		TEXT("Image/Stone01.dds"), RootParameter::Texture0);
	m_textures.insert({ "OBJECT", brickTextire });
	auto skyboxTexture = make_shared<Texture>(device, commandList,
		TEXT("Skybox/SkyBox_0.dds"), RootParameter::TextureCube);
	m_textures.insert({ "SKYBOX", skyboxTexture });

	auto terrainTexture = make_shared<Texture>(device);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("Image/Base_Texture.dds"), RootParameter::Texture0);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("Image/Detail_Texture_7.dds"), RootParameter::Texture1);

	terrainTexture->CreateShaderVariable(device);
	m_textures.insert({ "TERRAIN", terrainTexture });
}

inline void Scene::BuildObjects()
{
	m_player = make_shared<Player>();
	m_player->SetMesh(m_meshes["CUBE"]);
	m_player->SetTexture(m_textures["PLAYER"]);
	m_player->SetPosition(XMFLOAT3{ 0.f, 0.f, 0.f });

	for (int x = -15; x <= 15; x += 5) {
		for (int y = -15; y <= 15; y += 5) {
			for (int z = -15; z <= 15; z += 5) {
				auto object = make_shared<RotatingObject>();
				object->SetMesh(m_meshes["CUBE"]);
				object->SetTexture(m_textures["OBJECT"]);
				object->SetPosition(XMFLOAT3{
					static_cast<FLOAT>(x),
					static_cast<FLOAT>(y),
					static_cast<FLOAT>(z) });
				m_objects.push_back(object);
			}
		}
	}

	m_camera = make_shared<ThirdPersonCamera>();
	m_camera->SetLens(0.25 * XM_PI, gGameFramework->GetAspectRatio(), 0.1f, 1000.f);
	m_player->SetCamera(m_camera);

	m_skybox = make_shared<GameObject>();
	m_skybox->SetMesh(m_meshes["SKYBOX"]);
	m_skybox->SetTexture(m_textures["SKYBOX"]);

	m_terrain = make_shared<GameObject>();
	m_terrain->SetMesh(m_meshes["TERRAIN"]);
	m_terrain->SetTexture(m_textures["TERRAIN"]);
	m_terrain->SetScale(XMFLOAT3{ 5.f, 0.5f, 5.f });
	m_terrain->SetPosition(XMFLOAT3{ 0.f, -50.f, 0.f });
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