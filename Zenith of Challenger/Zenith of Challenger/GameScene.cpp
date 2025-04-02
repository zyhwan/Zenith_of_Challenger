#include "GameScene.h"

void GameScene::BuildObjects(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	m_meshes.clear();
	m_textures.clear();
	m_objects.clear();
	m_fbxMeshes.clear(); // FBX 메쉬 초기화

	BuildShaders(device, commandList, rootSignature);
	BuildMeshes(device, commandList);
	BuildTextures(device, commandList);
	BuildMaterials(device, commandList);

	// FBX 파일 로드
	m_fbxLoader = make_shared<FBXLoader>();
	if (m_fbxLoader->LoadFBXModel("Model/Challenge.fbx"))
	{
		m_fbxMeshes = m_fbxLoader->GetMeshes(); // FBX에서 로드한 메쉬 저장
	}

	// FBX 모델을 GameObject로 변환 후 m_fbxObjects에 추가
	for (const auto& fbxMesh : m_fbxMeshes)
	{
		auto gameObject = make_shared<GameObject>(device);
		gameObject->SetMesh(fbxMesh);
		gameObject->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f }); // 원래 크기로 유지
		gameObject->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 0.0f }); // Y축 위치 조정
		m_fbxObjects.push_back(gameObject);
	}

	BuildObjects(device);
}

void GameScene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
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

	// 플레이어 위치 가져오기
	if (gGameFramework->GetPlayer())
	{
		const XMFLOAT3& playerPos = gGameFramework->GetPlayer()->GetPosition();
	}

}

void GameScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_camera->UpdateShaderVariable(commandList);
	m_lightSystem->UpdateShaderVariable(commandList);

	// OBJECT 셰이더 적용 후 인스턴싱 오브젝트 렌더링
	if (m_instanceObject)
	{
		m_shaders.at("OBJECT")->UpdateShaderVariable(commandList);
		m_instanceObject->Render(commandList);
	}

	m_shaders.at("DETAIL")->UpdateShaderVariable(commandList);
	//m_terrain->Render(commandList);

	m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
	m_skybox->Render(commandList);

	//FBX 모델 렌더링 (GameObject)
	if (!m_fbxObjects.empty())
	{
		m_shaders.at("FBX")->UpdateShaderVariable(commandList);
		for (const auto& object : m_fbxObjects)
		{
			object->Render(commandList);
		}
	}
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
	//FBX 전용 쉐이더 추가
	auto fbxShader = make_shared<FBXShader>(device, rootSignature);
	m_shaders.insert({ "FBX", fbxShader });

	auto uiShader = make_shared<UIScreenShader>(device, rootSignature);
	m_shaders.insert({ "UI", uiShader });
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

	// BuildTextures 내부에 추가
	auto fbxTexture = make_shared<Texture>(device);
	fbxTexture->LoadTexture(device, commandList, TEXT("Image/Base Map.dds"), RootParameter::Texture);
	fbxTexture->CreateShaderVariable(device);
	m_textures.insert({ "FBX", fbxTexture });
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
	m_sun->SetStrength(XMFLOAT3{ 1.0f, 1.0f, 1.0f }); //디렉셔널 라이트 세기 줄이기

	auto player = make_shared<Player>();  // 플레이어 객체 생성
	player->SetScale(XMFLOAT3{ 1.f, 1.5f, 1.f });
	player->SetPosition(XMFLOAT3{ -180.f, 55.f, 180.f });
	player->SetTextureIndex(0);
	// GameFramework에 player를 설정
	gGameFramework->SetPlayer(player);

	// GameScene에서도 같은 Player 객체를 사용하도록 설정
	m_player = gGameFramework->GetPlayer();

	//for (int x = -10; x <= 10; x += 10) {
	//	for (int y = 0; y <= 20; y += 10) {
	//		for (int z = -10; z <= 10; z += 10) {
	//			auto light = make_shared<SpotLight>(
	//				XMFLOAT3{ 0.7f, 0.7f, 0.7f },
	//				XMFLOAT3{ 1.f, 0.f, 0.f },
	//				XMFLOAT3{ 0.f, 0.f, 0.f },
	//				1.f, 50.f, 80.f);
	//			m_lightSystem->SetLight(light);
	//			auto object = make_shared<LightObject>(light);
	//			object->SetPosition(XMFLOAT3{
	//				static_cast<FLOAT>(x),
	//				static_cast<FLOAT>(y),
	//				static_cast<FLOAT>(z) });
	//			object->SetTextureIndex(1);
	//			m_objects.push_back(object);
	//		}
	//	}
	//}

	m_instanceObject = make_unique<Instance>(device,
		static_pointer_cast<Mesh<TextureVertex>>(m_meshes["CUBE"]), static_cast<UINT>(m_objects.size() + 1));//m_objects.size() + 1
	//m_instanceObject->SetObjects(m_objects);
	m_instanceObject->SetObject(m_player);
	m_instanceObject->SetTexture(m_textures["CUBE"]);
	m_instanceObject->SetMaterial(m_materials["CUBE"]);

	m_camera = make_shared<QuarterViewCamera>(device);
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


	for (auto& obj : m_fbxObjects)
	{
		obj->SetTexture(m_textures["FBX"]);
	}

}