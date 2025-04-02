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
    std::cout << "==== 로그인 화면 ====" << std::endl;
    std::cout << "아이디를 입력하세요: " << std::endl;

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
    // 스카이박스 렌더링
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
                obj->Render(commandList); // 배경 → 타이틀 순서대로 push_back된 상태
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

    // 화면 정규 좌표 [-1, 1]로 변환
    float ndcX = 2.0f * mouseX / gGameFramework->GetWindowWidth() - 1.0f;
    float ndcY = 1.0f - 2.0f * mouseY / gGameFramework->GetWindowHeight();

    // START 버튼의 위치 기준 계산
    XMFLOAT3 btnPos = m_SelectSceneObjects[4]->GetPosition();
    float btnW = 0.5f, btnH = 0.15f;

    // 버튼 충돌 체크
    if (ndcX >= 0.6f && ndcX <= 0.9 &&
        ndcY >= -0.25 && ndcY <= -0.18)
    {
        m_isMouseOnStartBtn = true;

        if (message == WM_LBUTTONDOWN) // 클릭 시 씬 전환
        {
            std::cout << "START 버튼 클릭됨 -> GameFramework에서 처리 예정\n";
            m_isStartButtonClicked = true; // 씬 전환 요청 플래그만 설정
        }
    }
    else
    {
        m_isMouseOnStartBtn = false;
    }
}

// 아이디와 비밀번호 입력 상태를 콘솔에 출력
void StartScene::DisplayLoginStatus()
{
    std::cout << "\r아이디: " << username << (isTypingUsername ? "_" : " ")
        << "  비밀번호: " << std::string(password.length(), '*') << (!isTypingUsername ? "_" : " ")
        << "    " << std::flush;
}

void StartScene::KeyboardEvent(UINT message, WPARAM wParam)
{
    if (message == WM_KEYDOWN)
    {
        if (wParam == VK_BACK) // 백스페이스 처리
        {
            if (isTypingUsername && !username.empty())
                username.pop_back();
            else if (!isTypingUsername && !password.empty())
                password.pop_back();
        }
        else if (wParam == VK_TAB) // 탭을 누르면 아이디 -> 비밀번호 입력 모드 전환
        {
            isTypingUsername = !isTypingUsername;
        }
        else if (wParam == VK_RETURN) // Enter 입력 시 로그인 검증
        {
            if (username.empty() || password.empty())  // 빈 값 입력 방지
            {
                std::cout << "\n[!] 아이디와 비밀번호를 모두 입력하세요.\n";
                return;
            }

            if (username == correctUsername && password == correctPassword)
            {
                isLoginSuccess = true;
                m_isRoomSelectionActive = true; // 씬 전환 대신 방 선택 UI 활성화
                std::cout << "\n[o] 로그인 성공! 방 선택 UI 표시.\n";
            }
            else
            {
                std::cout << "\n[x] 로그인 실패! 다시 입력하세요.\n";
                username.clear();
                password.clear();
                isTypingUsername = true;
            }
        }
        else if ((wParam >= 'A' && wParam <= 'Z') || (wParam >= '0' && wParam <= '9')) // 문자 및 숫자 입력 처리
        {
            if (isTypingUsername)
                username += static_cast<char>(wParam);
            else
                password += static_cast<char>(wParam);
        }

        // 입력될 때마다 화면을 업데이트하여 실시간으로 보이게 함
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

    // Room 선택 화면용
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

    // 화면 비율 계산
    float screenWidth = 2.0f;
    float screenAspect = static_cast<float>(FRAME_BUFFER_WIDTH) / FRAME_BUFFER_HEIGHT;
    float screenHeight = screenWidth / screenAspect; // 예: 2.0 / 1.8  1.11

    // StartScreen: 화면 가득 채우기
    auto screen = make_shared<GameObject>(device);

    // 화면에 약간 여유 있게 가득 채움
    float fullScreenWidth = 1.47f;  // 기존보다 넓게 (기본 2.0  2.4)
    float fullScreenHeight = fullScreenWidth / screenAspect;

    screen->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.1f, 0.9f, 0.99f));
    screen->SetTexture(m_textures["START"]);
    screen->SetUseTexture(true);
    screen->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
    m_StartSceneObjects.push_back(screen);

    // Title (로고 이미지): 앞에 출력, 살짝 위쪽
    auto title = make_shared<GameObject>(device);
    title->SetMesh(CreateScreenQuad(device, gGameFramework->GetCommandList(), 1.8f, 1.6f, 0.98f));
    title->SetTexture(m_textures["TITLE"]);
    title->SetUseTexture(true);
    title->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
    title->SetPosition(XMFLOAT3(0.f, 0.2f, 0.98f));
    m_StartSceneObjects.push_back(title);

    // Room 선택 UI 구성
    // 블러 오버레이
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
