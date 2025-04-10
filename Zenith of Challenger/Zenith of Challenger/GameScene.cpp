#include "GameScene.h"

void GameScene::BuildObjects(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	m_meshes.clear();
	m_textures.clear();
	m_objects.clear();
	m_fbxMeshes.clear(); // FBX �޽� �ʱ�ȭ


	BuildShaders(device, commandList, rootSignature);
	BuildMeshes(device, commandList);
	BuildTextures(device, commandList);
	BuildMaterials(device, commandList);

	// FBX ���� �ε�
	cout << "���� �� �ε� ��!!!!" << endl;
	m_fbxLoader = make_shared<FBXLoader>();
	if (m_fbxLoader->LoadFBXModel("Model/Challenge.fbx",
		XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixRotationY(XMConvertToRadians(180.0f))))
	{
		m_fbxMeshes = m_fbxLoader->GetMeshes();
	}

	// FBX ���� GameObject�� ��ȯ �� m_fbxObjects�� �߰�
	for (const auto& fbxMesh : m_fbxMeshes)
	{
		auto gameObject = make_shared<GameObject>(device);
		gameObject->SetMesh(fbxMesh);
		gameObject->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f }); // ���� ũ��� ����
		gameObject->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 0.0f }); // Y�� ��ġ ����
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

	// �÷��̾� ��ġ ��������
	if (gGameFramework->GetPlayer())
	{
		const XMFLOAT3& playerPos = gGameFramework->GetPlayer()->GetPosition();
	}

}

void GameScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->SetGraphicsRootSignature(gGameFramework->GetRootSignature().Get());

	m_camera->UpdateShaderVariable(commandList);
	m_lightSystem->UpdateShaderVariable(commandList);

	m_shaders.at("DETAIL")->UpdateShaderVariable(commandList);
	m_terrain->Render(commandList);

	m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
	m_skybox->Render(commandList);

	if (!m_fbxObjects.empty())
	{
		m_shaders.at("FBX")->UpdateShaderVariable(commandList);
		for (const auto& obj : m_fbxObjects)
			obj->Render(commandList);
	}

	if (m_player)
	{
		m_shaders.at("CHARACTER")->UpdateShaderVariable(commandList);
		m_player->Render(commandList);
	}
}

void GameScene::PreRender(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	ID3D12DescriptorHeap* heaps[] = { gGameFramework->GetDescriptorHeap().Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
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
	//FBX ���� ���̴� �߰�
	auto fbxShader = make_shared<FBXShader>(device, rootSignature);
	m_shaders.insert({ "FBX", fbxShader });
	auto uiShader = make_shared<UIScreenShader>(device, rootSignature);
	m_shaders.insert({ "UI", uiShader });
	// Character �ִϸ��̼� ���� ���̴� �߰�
	auto characterShader = make_shared<CharacterShader>(device, rootSignature);
	m_shaders.insert({ "CHARACTER", characterShader });
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
	auto skyboxTexture = make_shared<Texture>(device, commandList,
		TEXT("Skybox/SkyBox_0.dds"), RootParameter::TextureCube);
	skyboxTexture->CreateShaderVariable(device, true);
	m_textures.insert({ "SKYBOX", skyboxTexture });

	auto terrainTexture = make_shared<Texture>(device);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("Image/Base_Texture.dds"), RootParameter::Texture);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("Image/Detail_Texture_7.dds"), RootParameter::Texture);
	terrainTexture->CreateShaderVariable(device, true);
	m_textures.insert({ "TERRAIN", terrainTexture });

	auto fbxTexture = make_shared<Texture>(device, commandList,
		TEXT("Image/Base Map.dds"), RootParameter::Texture);
	fbxTexture->CreateShaderVariable(device, true);
	m_textures.insert({ "FBX", fbxTexture });

	auto characterTexture = make_shared<Texture>(device, commandList,
		TEXT("Image/Texture_Modular_Characters.dds"), RootParameter::Texture);
	characterTexture->CreateShaderVariable(device, true);
	m_textures.insert({ "CHARACTER", characterTexture });

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
	m_sun->SetStrength(XMFLOAT3{ 1.3f, 1.3f, 1.3f }); //�𷺼ų� ����Ʈ ���� ���̱�

	// [1] �÷��̾� �𵨿� ������ ��� ���� (ũ�� ����)
	XMMATRIX playerTransform = XMMatrixScaling(0.05f, 0.05f, 0.05);

	// [2] FBX �δ� ���� �� �� �ε�
	m_playerLoader = make_shared<FBXLoader>();
	cout << "ĳ���� �ε� ��!!!!" << endl;

	if (m_playerLoader->LoadFBXModel("Model/Player/Player2.fbx", playerTransform))
	{
		auto& meshes = m_playerLoader->GetMeshes();
		if (meshes.empty()) {
			OutputDebugStringA("[ERROR] FBX���� �޽ø� ã�� �� �����ϴ�.\n");
			return;
		}

		// [3] Player ��ü ����
		auto player = make_shared<Player>(device);

		// [4] ��ġ �� ������ ����
		player->SetPosition(XMFLOAT3{ -185.f, 53.f, 177.f });
		//player->SetScale(XMFLOAT3{ 1.f, 1.f, 1.f }); // �ʿ�� ���� ����

		// [5] FBX �޽� ���� ���
		for (int i = 0; i < meshes.size(); ++i)
		{
			player->AddMesh(meshes[i]);
		}

		// [6] �ִϸ��̼� Ŭ�� �� �� ���� ����
		player->SetAnimationClips(m_playerLoader->GetAnimationClips());
		player->SetCurrentAnimation("Idle");
		player->SetBoneOffsets(m_playerLoader->GetBoneOffsets());
		player->SetBoneNameToIndex(m_playerLoader->GetBoneNameToIndex());

		// [7] �ؽ�ó, ��Ƽ���� ����
		player->SetTexture(m_textures["CHARACTER"]);
		player->SetMaterial(m_materials["CHARACTER"]); // ������ ���� �ʿ�

		// [8] �� ��� StructuredBuffer�� SRV ����
		auto [cpuHandle, gpuHandle] = gGameFramework->AllocateDescriptorHeapSlot();
		player->CreateBoneMatrixSRV(device, cpuHandle, gpuHandle);

		// [9] Player ��� �� GameScene ���ο� ����
		gGameFramework->SetPlayer(player);
		m_player = gGameFramework->GetPlayer();
	}
	else
	{
		OutputDebugStringA("[ERROR] �÷��̾� FBX �ε� ����!\n");
	}

	m_camera = make_shared<QuarterViewCamera>(device);
	m_camera->SetLens(0.25 * XM_PI, gGameFramework->GetAspectRatio(), 0.1f, 1000.f);

	m_player->SetCamera(m_camera);

	m_skybox = make_shared<GameObject>(device);
	m_skybox->SetMesh(m_meshes["SKYBOX"]);
	m_skybox->SetTextureIndex(m_textures["SKYBOX"]->GetTextureIndex());
	m_skybox->SetTexture(m_textures["SKYBOX"]);

	m_terrain = make_shared<Terrain>(device);
	m_terrain->SetMesh(m_meshes["TERRAIN"]);
	m_terrain->SetTextureIndex(m_textures["TERRAIN"]->GetTextureIndex());
	m_terrain->SetTexture(m_textures["TERRAIN"]);
	m_terrain->SetScale(XMFLOAT3{ 5.f, 0.25f, 5.f });
	m_terrain->SetPosition(XMFLOAT3{ 0.f, -100.f, 0.f });


	for (auto& obj : m_fbxObjects)
	{
		obj->SetTexture(m_textures["FBX"]);
		obj->SetTextureIndex(m_textures["FBX"]->GetTextureIndex());
		obj->SetUseTexture(true); // UV ��� �ؽ�ó ����
	}
	
}