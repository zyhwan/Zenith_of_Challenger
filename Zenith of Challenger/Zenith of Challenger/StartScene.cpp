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
    m_StartSceneObjects.clear();

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

    if (!m_isRoomSelectionActive) {
        if (m_shaders.contains("UI"))
        {
            m_shaders.at("UI")->UpdateShaderVariable(commandList);
            for (const auto& obj : m_StartSceneObjects)
            {
                obj->Render(commandList); // ��� �� Ÿ��Ʋ ������� push_back�� ����
            }
        }
    }
    else {
        if (m_shaders.contains("UI"))
        {
            m_shaders.at("UI")->UpdateShaderVariable(commandList);
            for (const auto& obj : m_SelectSceneObjects)
            {
                obj->Render(commandList);
            }
        }
    }

    if (m_isMouseOnStartBtn) {
        if (m_shaders.contains("UI"))
        {
            m_shaders.at("UI")->UpdateShaderVariable(commandList);
            for (const auto& obj : m_startBar)
            {
                obj->Render(commandList);
            }
        }
    }
}

void StartScene::Update(FLOAT timeElapsed)
{
    m_skybox->SetPosition(m_camera->GetEye());
}

void StartScene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
}

void StartScene::MouseEvent(UINT message, LPARAM lParam)
{
    if (!m_isRoomSelectionActive) return;

    int mouseX = LOWORD(lParam);
    int mouseY = HIWORD(lParam);

    // ȭ�� ���� ��ǥ [-1, 1]�� ��ȯ
    float ndcX = 2.0f * mouseX / gGameFramework->GetWindowWidth() - 1.0f;
    float ndcY = 1.0f - 2.0f * mouseY / gGameFramework->GetWindowHeight();

    // START ��ư�� ��ġ ���� ���
    XMFLOAT3 btnPos = m_SelectSceneObjects[4]->GetPosition();
    float btnW = 0.5f, btnH = 0.15f;

    // ��ư �浹 üũ
    if (ndcX >= 0.6f && ndcX <= 0.9 &&
        ndcY >= -0.25 && ndcY <= -0.18)
    {
        m_isMouseOnStartBtn = true;

        if (message == WM_LBUTTONDOWN) // Ŭ�� �� �� ��ȯ
        {
            std::cout << "START ��ư Ŭ���� -> GameFramework���� ó�� ����\n";
            m_isStartButtonClicked = true; // �� ��ȯ ��û �÷��׸� ����
        }
    }
    else
    {
        m_isMouseOnStartBtn = false;
    }
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
                m_isRoomSelectionActive = true; // �� ��ȯ ��� �� ���� UI Ȱ��ȭ
                std::cout << "\n[o] �α��� ����! �� ���� UI ǥ��.\n";
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

    // Room ���� ȭ���
    auto roomTex = make_shared<Texture>(device);
    roomTex->LoadTexture(device, commandList, TEXT("Image/Select_Server/Select_Room.dds"), RootParameter::Texture);
    roomTex->CreateShaderVariable(device);
    m_textures.insert({ "ROOM", roomTex });

    auto startBtnTex = make_shared<Texture>(device);
    startBtnTex->LoadTexture(device, commandList, TEXT("Image/Select_Server/START_Button.dds"), RootParameter::Texture);
    startBtnTex->CreateShaderVariable(device);
    m_textures.insert({ "STARTBTN", startBtnTex });

    auto blurOverlayTex = make_shared<Texture>(device);
    blurOverlayTex->LoadTexture(device, commandList, TEXT("Image/Select_Server/Blur.dds"), RootParameter::Texture);
    blurOverlayTex->CreateShaderVariable(device);
    m_textures.insert({ "BLUR", blurOverlayTex });

    auto StartBar = make_shared<Texture>(device);
    StartBar->LoadTexture(device, commandList, TEXT("Image/Select_Server/BAR.dds"), RootParameter::Texture);
    StartBar->CreateShaderVariable(device);
    m_textures.insert({ "BAR", StartBar });
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
    m_StartSceneObjects.push_back(screen);

    // Title (�ΰ� �̹���): �տ� ���, ��¦ ����
    auto title = make_shared<GameObject>(device);
    title->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.8f, 1.6f, 0.98f));
    title->SetTexture(m_textures["TITLE"]);
    title->SetUseTexture(true);
    title->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
    title->SetPosition(XMFLOAT3(0.f, 0.2f, 0.98f));
    m_StartSceneObjects.push_back(title);

    // Room ���� UI ����
    // �� ��������
    auto blur = make_shared<GameObject>(device);
    blur->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.1f, 0.9f, 0.99f));
    blur->SetTexture(m_textures["BLUR"]);
    blur->SetUseTexture(true);
    blur->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
    m_SelectSceneObjects.push_back(blur);

    float roomWidth = 1.0f;
    float roomHeight = 0.2f;

    for (int i = 0; i < 3; ++i)
    {
        auto room = make_shared<GameObject>(device);
        room->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.0f, 0.5f, 0.98f));
        room->SetTexture(m_textures["ROOM"]);
        room->SetScale(XMFLOAT3(1.5f, 2.0f, 1.2f));
        room->SetPosition(XMFLOAT3(-0.5f, 0.6f - 0.55f * i, 0.98f));
        room->SetUseTexture(true);
        m_SelectSceneObjects.push_back(room);
    }

    auto startBtn = make_shared<GameObject>(device);
    startBtn->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 0.5f, 0.15f, 0.98f));
    startBtn->SetTexture(m_textures["STARTBTN"]);
    startBtn->SetScale(XMFLOAT3(0.8f, 1.4f, 1.0f));
    startBtn->SetPosition(XMFLOAT3(0.85f, -0.65f, 0.98f));
    startBtn->SetUseTexture(true);
    m_SelectSceneObjects.push_back(startBtn);


    auto StartBar = make_shared<GameObject>(device);
    StartBar->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 0.5f, 0.15f, 0.98f));
    StartBar->SetTexture(m_textures["BAR"]);
    StartBar->SetScale(XMFLOAT3(0.8f, 1.1f, 1.0f));
    StartBar->SetPosition(XMFLOAT3(0.85f, -0.75f, 0.98f));
    StartBar->SetUseTexture(true);
    m_startBar.push_back(StartBar);

}
