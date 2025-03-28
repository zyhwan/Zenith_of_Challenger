#include "StartScene.h"

std::shared_ptr<Mesh<TextureVertex>> CreateScreenQuad(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, float width, float height, float z)
{
    std::vector<TextureVertex> vertices =
    {
        { XMFLOAT3{-width / 2,  height / 2, z}, XMFLOAT3{0, 0, -1}, XMFLOAT2{0, 0} },
        { XMFLOAT3{ width / 2,  height / 2, z}, XMFLOAT3{0, 0, -1}, XMFLOAT2{1, 0} },
        { XMFLOAT3{-width / 2, -height / 2, z}, XMFLOAT3{0, 0, -1}, XMFLOAT2{0, 1} },

        { XMFLOAT3{-width / 2, -height / 2, z}, XMFLOAT3{0, 0, -1}, XMFLOAT2{0, 1} },
        { XMFLOAT3{ width / 2,  height / 2, z}, XMFLOAT3{0, 0, -1}, XMFLOAT2{1, 0} },
        { XMFLOAT3{ width / 2, -height / 2, z}, XMFLOAT3{0, 0, -1}, XMFLOAT2{1, 1} },
    };

    return std::make_shared<Mesh<TextureVertex>>(device, commandList, vertices);
}


const std::string correctUsername = "E";
const std::string correctPassword = "1";

StartScene::StartScene()
    : isTypingUsername(true), isLoginSuccess(false)
{
    username = "";
    password = "";
}

void StartScene::BuildObjects(const ComPtr<ID3D12Device>& device,
    const ComPtr<ID3D12GraphicsCommandList>& commandList,
    const ComPtr<ID3D12RootSignature>& rootSignature)
{
    std::cout << "==== �α��� ȭ�� ====" << std::endl;
    std::cout << "���̵� �Է��ϼ���: " << std::endl;

    m_meshes.clear();
    m_textures.clear();
    m_objects.clear();
    m_gameObjects.clear();

    BuildShaders(device, commandList, rootSignature);
    BuildMeshes(device, commandList);
    BuildTextures(device, commandList);

    BuildObjects(device);
}

void StartScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
    m_camera->UpdateShaderVariable(commandList);

    m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
    // ��ī�̹ڽ� ������
    if (m_skybox)
    {
        m_skybox->Render(commandList);
    }

    if (m_shaders.contains("UI"))
    {
        m_shaders.at("UI")->UpdateShaderVariable(commandList);
        for (const auto& obj : m_gameObjects)
        {
            obj->Render(commandList); // ��� �� Ÿ��Ʋ ������� push_back�� ����
        }
    }

}

void StartScene::Update(FLOAT timeElapsed)
{
    m_skybox->SetPosition(m_camera->GetEye());

    // �α��� ���� �� �� ��ȯ
    if (isLoginSuccess)
    {
        std::cout << "�α��� ����! GameScene���� ��ȯ ��..." << std::endl;
        gGameFramework->GetSceneManager()->ChangeScene("GameScene",
            gGameFramework->GetDevice(), gGameFramework->GetCommandList(), gGameFramework->GetRootSignature());
    }
}

void StartScene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
}

// ���̵�� ��й�ȣ �Է� ���¸� �ֿܼ� ���
void StartScene::DisplayLoginStatus()
{
    std::cout << "\r���̵�: " << username << (isTypingUsername ? "_" : " ")
        << "  ��й�ȣ: " << std::string(password.length(), '*') << (!isTypingUsername ? "_" : " ")
        << "    " << std::flush;
}

void StartScene::KeyboardEvent(UINT message, WPARAM wParam)
{
    if (message == WM_KEYDOWN)
    {
        if (wParam == VK_BACK) // �齺���̽� ó��
        {
            if (isTypingUsername && !username.empty())
                username.pop_back();
            else if (!isTypingUsername && !password.empty())
                password.pop_back();
        }
        else if (wParam == VK_TAB) // ���� ������ ���̵� -> ��й�ȣ �Է� ��� ��ȯ
        {
            isTypingUsername = !isTypingUsername;
        }
        else if (wParam == VK_RETURN) // Enter �Է� �� �α��� ����
        {
            if (username.empty() || password.empty())  // �� �� �Է� ����
            {
                std::cout << "\n[!] ���̵�� ��й�ȣ�� ��� �Է��ϼ���.\n";
                return;
            }

            if (username == correctUsername && password == correctPassword)
            {
                isLoginSuccess = true;
                std::cout << "\n[o] �α��� ����! GameScene���� ��ȯ ��..." << std::endl;
                gGameFramework->GetSceneManager()->ChangeScene("GameScene",
                    gGameFramework->GetDevice(), gGameFramework->GetCommandList(), gGameFramework->GetRootSignature());
            }
            else
            {
                std::cout << "\n[x] �α��� ����! �ٽ� �Է��ϼ���.\n";
                username.clear();
                password.clear();
                isTypingUsername = true;
            }
        }
        else if ((wParam >= 'A' && wParam <= 'Z') || (wParam >= '0' && wParam <= '9')) // ���� �� ���� �Է� ó��
        {
            if (isTypingUsername)
                username += static_cast<char>(wParam);
            else
                password += static_cast<char>(wParam);
        }

        // �Էµ� ������ ȭ���� ������Ʈ�Ͽ� �ǽð����� ���̰� ��
        DisplayLoginStatus();
    }
}

//void StartScene::KeyboardEvent(FLOAT timeElapsed)
//{
//}

void StartScene::BuildShaders(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature)
{
    auto skyboxShader = make_shared<SkyboxShader>(device, rootSignature);
    m_shaders.insert({ "SKYBOX", skyboxShader });

    auto uiShader = make_shared<UIScreenShader>(device, rootSignature);
    m_shaders.insert({ "UI", uiShader });
}

void StartScene::BuildMeshes(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    auto skyboxMesh = make_shared<Mesh<Vertex>>(device, commandList, TEXT("Model/SkyboxMesh.binary"));
    m_meshes.insert({ "SKYBOX", skyboxMesh });

    auto startMesh = CreateScreenQuad(device, commandList, 2.0f, 2.0f, 0.0f); // ��üȭ��
    auto titleMesh = CreateScreenQuad(device, commandList, 1.0f, 0.3f, 0.1f); // Ÿ��Ʋ ��ġ ����
}

void StartScene::BuildTextures(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    auto skyboxTexture = make_shared<Texture>(device, commandList,
        TEXT("Skybox/SkyBox_0.dds"), RootParameter::TextureCube);
    m_textures.insert({ "SKYBOX", skyboxTexture });

    auto startTex = make_shared<Texture>(device);
    startTex->LoadTexture(device, commandList, TEXT("Image/StartScene/StartScreen.dds"), RootParameter::Texture);
    startTex->CreateShaderVariable(device);
    m_textures.insert({ "START", startTex });

    auto titleTex = make_shared<Texture>(device);
    titleTex->LoadTexture(device, commandList, TEXT("Image/StartScene/Title_transparent.dds"), RootParameter::Texture);
    titleTex->CreateShaderVariable(device);
    m_textures.insert({ "TITLE", titleTex });
}

void StartScene::BuildObjects(const ComPtr<ID3D12Device>& device)
{
    m_camera = make_shared<ThirdPersonCamera>(device);
    m_camera->SetLens(0.25 * XM_PI, gGameFramework->GetAspectRatio(), 0.1f, 1000.f);

    m_skybox = make_shared<GameObject>(device);
    m_skybox->SetMesh(m_meshes["SKYBOX"]);
    m_skybox->SetTexture(m_textures["SKYBOX"]);

    // ȭ�� ���� ���
    float screenWidth = 2.0f;
    float screenAspect = static_cast<float>(FRAME_BUFFER_WIDTH) / FRAME_BUFFER_HEIGHT;
    float screenHeight = screenWidth / screenAspect; // ��: 2.0 / 1.8  1.11

    // StartScreen: ȭ�� ���� ä���
    auto screen = make_shared<GameObject>(device);

    // ȭ�鿡 �ణ ���� �ְ� ���� ä��
    float fullScreenWidth = 1.47f;  // �������� �а� (�⺻ 2.0  2.4)
    float fullScreenHeight = fullScreenWidth / screenAspect;

    screen->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.1f, 0.9f, 0.99f));
    screen->SetTexture(m_textures["START"]);
    screen->SetUseTexture(true);
    screen->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
    m_gameObjects.push_back(screen);

    // Title (�ΰ� �̹���): �տ� ���, ��¦ ����
    auto title = make_shared<GameObject>(device);
    title->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.8f, 1.6f, 0.98f));
    title->SetTexture(m_textures["TITLE"]);
    title->SetUseTexture(true);
    title->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
    title->SetPosition(XMFLOAT3(0.f, 0.2f, 0.98f));
    m_gameObjects.push_back(title);

}
