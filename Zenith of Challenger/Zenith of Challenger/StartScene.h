#pragma once
#include "Scene.h"
#include "GameFramework.h"

class StartScene : public Scene
{
public:
    StartScene();
    ~StartScene() override = default;

    void BuildObjects(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList,
        const ComPtr<ID3D12RootSignature>& rootSignature) override;

    void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;
    virtual void Update(FLOAT timeElapsed);

    virtual void MouseEvent(HWND hWnd, FLOAT timeElapsed);
    virtual void MouseEvent(UINT message, LPARAM lParam);


    virtual void KeyboardEvent(UINT message, WPARAM wParam); 

    virtual void BuildShaders(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList,
        const ComPtr<ID3D12RootSignature>& rootSignature);
    virtual void BuildMeshes(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList);
    virtual void BuildTextures(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList);
    virtual void BuildObjects(const ComPtr<ID3D12Device>& device);

    bool IsStartButtonClicked() const { return m_isStartButtonClicked; }
    void ResetStartButtonClicked() { m_isStartButtonClicked = false; }

private:
    void DisplayLoginStatus(); // 콘솔 창에 아이디 및 비밀번호 표시하는 함수 추가

    string username;  // 입력된 아이디 저장
    string password;  // 입력된 비밀번호 저장
    bool isTypingUsername; // 현재 아이디 입력 중인지 여부
    bool isLoginSuccess;   // 로그인 성공 여부
    bool m_isRoomSelectionActive = false; //방 선택 여부
    bool m_isMouseOnStartBtn = false;

    bool m_isStartButtonClicked = false;
};

shared_ptr<Mesh<TextureVertex>> CreateScreenQuad(
    const ComPtr<ID3D12Device>& device,
    const ComPtr<ID3D12GraphicsCommandList>& commandList,
    float width, float height, float z = 0.f);