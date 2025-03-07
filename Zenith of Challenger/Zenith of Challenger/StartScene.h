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

    virtual void KeyboardEvent(UINT message, WPARAM wParam); 

    virtual void BuildShaders(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList,
        const ComPtr<ID3D12RootSignature>& rootSignature);
    virtual void BuildMeshes(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList);
    virtual void BuildTextures(const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12GraphicsCommandList>& commandList);
    virtual void BuildObjects(const ComPtr<ID3D12Device>& device);

private:
    void DisplayLoginStatus(); // �ܼ� â�� ���̵� �� ��й�ȣ ǥ���ϴ� �Լ� �߰�

    std::string username;  // �Էµ� ���̵� ����
    std::string password;  // �Էµ� ��й�ȣ ����
    bool isTypingUsername; // ���� ���̵� �Է� ������ ����
    bool isLoginSuccess;   // �α��� ���� ����
};