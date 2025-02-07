#include "GameScene.h"

void GameScene::BuildObjects(const ComPtr<ID3D12Device>& device,
    const ComPtr<ID3D12GraphicsCommandList>& commandList,
    const ComPtr<ID3D12RootSignature>& rootSignature)
{
    m_meshes.clear();
    m_textures.clear();
    m_objects.clear();

	BuildShaders(device, commandList, rootSignature);
	BuildMeshes(device, commandList);
	BuildTextures(device, commandList);
	BuildMaterials(device, commandList);

	BuildObjects(device);
}

void GameScene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
	static bool isDragging = false; // 드래깅 상태를 추적
	static POINT lastMousePosition; // 마지막 마우스 위치

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {// 좌클릭 상태 확인
		if (!isDragging) // 드래깅 시작
		{
			isDragging = true;

			// 현재 마우스 위치 저장
			GetCursorPos(&lastMousePosition);
		}

		POINT currentMousePosition;
		GetCursorPos(&currentMousePosition);

		// 마우스 이동 거리 계산
		float dx = XMConvertToRadians(-0.15f * static_cast<FLOAT>(currentMousePosition.x - lastMousePosition.x));
		float dy = XMConvertToRadians(-0.15f * static_cast<FLOAT>(currentMousePosition.y - lastMousePosition.y));

		// 카메라 회전
		if (m_camera)
		{
			m_camera->RotateYaw(dx);
			m_camera->RotatePitch(dy);
		}

		// 마지막 마우스 위치 갱신
		lastMousePosition = currentMousePosition;
	}
	else {
		isDragging = false; // 드래깅 상태 해제
	}

	// 다른 마우스 관련 이벤트 처리
	m_player->MouseEvent(timeElapsed);
}

void GameScene::KeyboardEvent(FLOAT timeElapsed)
{
	m_player->KeyboardEvent(timeElapsed);
}

void GameScene::Update(FLOAT timeElapsed)
{
	m_player->Update(timeElapsed);
	m_sun->Update(timeElapsed);

	for (auto& object : m_objects) {
		object->Update(timeElapsed);
	}
	m_skybox->SetPosition(m_camera->GetEye());
}

void GameScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_camera->UpdateShaderVariable(commandList);
	m_lightSystem->UpdateShaderVariable(commandList);

	m_shaders.at("OBJECT")->UpdateShaderVariable(commandList);
	m_instanceObject->Render(commandList);

	m_shaders.at("DETAIL")->UpdateShaderVariable(commandList);
	m_terrain->Render(commandList);

	m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
	m_skybox->Render(commandList);
}

void GameScene::BuildShaders(const ComPtr<ID3D12Device>& device, 
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

void GameScene::BuildMeshes(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeMesh = make_shared<Mesh<TextureVertex>>(device, commandList,
		TEXT("Model/CubeNormalMesh.binary"));
	m_meshes.insert({ "CUBE", cubeMesh });
	auto skyboxMesh = make_shared<Mesh<Vertex>>(device, commandList,
		TEXT("Model/SkyboxMesh.binary"));
	m_meshes.insert({ "SKYBOX", skyboxMesh });
	auto terrainMesh = make_shared<TerrainMesh>(device, commandList,
		TEXT("Model/HeightMap.raw"));
	m_meshes.insert({ "TERRAIN", terrainMesh });
}

void GameScene::BuildTextures(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeTexture = make_shared<Texture>(device);
	cubeTexture->LoadTexture(device, commandList,
		TEXT("Image/Rock01.dds"), RootParameter::Texture);
	cubeTexture->LoadTexture(device, commandList,
		TEXT("Image/Stone01.dds"), RootParameter::Texture);
	cubeTexture->CreateShaderVariable(device);
	m_textures.insert({ "CUBE", cubeTexture });

	auto skyboxTexture = make_shared<Texture>(device, commandList,
		TEXT("Skybox/SkyBox_0.dds"), RootParameter::TextureCube);
	m_textures.insert({ "SKYBOX", skyboxTexture });

	auto terrainTexture = make_shared<Texture>(device);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("Image/Base_Texture.dds"), RootParameter::Texture);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("Image/Detail_Texture_7.dds"), RootParameter::Texture);

	terrainTexture->CreateShaderVariable(device);
	m_textures.insert({ "TERRAIN", terrainTexture });
}

void GameScene::BuildMaterials(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeMaterial = make_shared<Material>();
	cubeMaterial->SetMaterial(XMFLOAT3{ 0.95f, 0.93f, 0.88f }, 0.125f, XMFLOAT3{ 0.1f, 0.1f, 0.1f });
	cubeMaterial->CreateShaderVariable(device);
	m_materials.insert({ "CUBE", cubeMaterial });
}

void GameScene::BuildObjects(const ComPtr<ID3D12Device>& device)
{
	m_lightSystem = make_unique<LightSystem>(device);
	auto sunLight = make_shared<DirectionalLight>();
	m_lightSystem->SetLight(sunLight);

	m_sun = make_unique<Sun>(sunLight);
	m_sun->SetStrength(XMFLOAT3{ 1.3f, 1.2f, 1.2f });

	m_player = make_shared<Player>();
	m_player->SetScale(XMFLOAT3{ 1.f, 1.5f, 1.f });
	m_player->SetPosition(XMFLOAT3{ 0.f, 20.f, 0.f });
	m_player->SetTextureIndex(0);

	for (int x = -10; x <= 10; x += 10) {
		for (int y = 0; y <= 20; y += 10) {
			for (int z = -10; z <= 10; z += 10) {
				auto light = make_shared<SpotLight>(
					XMFLOAT3{ 0.7f, 0.7f, 0.7f },
					XMFLOAT3{ 1.f, 0.f, 0.f },
					XMFLOAT3{ 0.f, 0.f, 0.f },
					1.f, 50.f, 80.f);
				m_lightSystem->SetLight(light);
				auto object = make_shared<LightObject>(light);
				object->SetPosition(XMFLOAT3{
					static_cast<FLOAT>(x),
					static_cast<FLOAT>(y),
					static_cast<FLOAT>(z) });
				object->SetTextureIndex(1);
				m_objects.push_back(object);
			}
		}
	}

	m_instanceObject = make_unique<Instance>(device,
		static_pointer_cast<Mesh<TextureVertex>>(m_meshes["CUBE"]), static_cast<UINT>(m_objects.size() + 1));
	m_instanceObject->SetObjects(m_objects);
	m_instanceObject->SetObject(m_player);
	m_instanceObject->SetTexture(m_textures["CUBE"]);
	m_instanceObject->SetMaterial(m_materials["CUBE"]);


	m_camera = make_shared<ThirdPersonCamera>(device);
	m_camera->SetLens(0.25 * XM_PI, gGameFramework->GetAspectRatio(), 0.1f, 1000.f);
	m_player->SetCamera(m_camera);

	m_skybox = make_shared<GameObject>(device);
	m_skybox->SetMesh(m_meshes["SKYBOX"]);
	m_skybox->SetTexture(m_textures["SKYBOX"]);

	m_terrain = make_shared<Terrain>(device);
	m_terrain->SetMesh(m_meshes["TERRAIN"]);
	m_terrain->SetTexture(m_textures["TERRAIN"]);
	m_terrain->SetScale(XMFLOAT3{ 5.f, 0.25f, 5.f });
	m_terrain->SetPosition(XMFLOAT3{ 0.f, -100.f, 0.f });
}
