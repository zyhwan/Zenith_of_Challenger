#include "StartScene.h"

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
}

void StartScene::Update(FLOAT timeElapsed)
{
    m_skybox->SetPosition(m_camera->GetEye());

    // 로그인 성공 시 씬 전환
    if (isLoginSuccess)
    {
        std::cout << "로그인 성공! GameScene으로 전환 중..." << std::endl;
        gGameFramework->GetSceneManager()->ChangeScene("GameScene",
            gGameFramework->GetDevice(), gGameFramework->GetCommandList(), gGameFramework->GetRootSignature());
    }
}

void StartScene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
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
        else if (wParam == VK_TAB) // 탭을 누르면 아이디 → 비밀번호 입력 모드 전환
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
                std::cout << "\n[o] 로그인 성공! GameScene으로 전환 중..." << std::endl;
                gGameFramework->GetSceneManager()->ChangeScene("GameScene",
                    gGameFramework->GetDevice(), gGameFramework->GetCommandList(), gGameFramework->GetRootSignature());
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
}

void StartScene::BuildObjects(const ComPtr<ID3D12Device>& device)
{
    m_camera = make_shared<ThirdPersonCamera>(device);
    m_camera->SetLens(0.25 * XM_PI, gGameFramework->GetAspectRatio(), 0.1f, 1000.f);

    m_skybox = make_shared<GameObject>(device);
    m_skybox->SetMesh(m_meshes["SKYBOX"]);
    m_skybox->SetTexture(m_textures["SKYBOX"]);
}
