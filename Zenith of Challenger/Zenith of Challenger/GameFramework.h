#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "StartScene.h"

class CGameFramework
{
public:
	CGameFramework(UINT windowWidth, UINT windowHeight);
	~CGameFramework();

	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void FrameAdvance();

	void MouseEvent(HWND hWnd, FLOAT timeElapsed);
	void KeyboardEvent(FLOAT timeElapsed);
	void MouseEvent(UINT message, LPARAM lParam);
	void KeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void SetActive(BOOL isActive);

	FLOAT GetAspectRatio();
	UINT GetWindowWidth();
	UINT GetWindowHeight();

	ComPtr<ID3D12Device> GetDevice() { return m_device; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return m_commandList; } 

	// Player 객체 가져오기
	shared_ptr<Player> GetPlayer() { return m_player; } 

	// Player 객체 설정 (GameScene에서 호출)
	void SetPlayer(shared_ptr<Player> player) { m_player = player; }
private:
	void InitDirect3D();

	void CreateDevice();
	void CreateFence();
	void Check4xMSAAMultiSampleQuality();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvDsvDescriptorHeap();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateRootSignature();

	void BuildObjects();
	void WaitForGpuComplete();

	void Update();
	void Render();

	void ProcessInput(); //키 입력 체크

private:
	static const UINT					m_nSwapChainBuffers = 2;

	BOOL								m_activate;

	HINSTANCE							m_hInstance;
	HWND								m_hWnd; 
	UINT								m_nWndClientWidth;
	UINT								m_nWndClientHeight;
	FLOAT								m_aspectRatio;
	
	D3D12_VIEWPORT						m_viewport;
	D3D12_RECT							m_scissorRect;
	ComPtr<IDXGIFactory4>				m_factory;
	ComPtr<IDXGISwapChain3>				m_swapChain;
	ComPtr<ID3D12Device>				m_device;

	INT									m_MSAA4xQualityLevel;
	ComPtr<ID3D12CommandAllocator>		m_commandAllocator;
	ComPtr<ID3D12CommandQueue>			m_commandQueue;
	ComPtr<ID3D12GraphicsCommandList>	m_commandList;
	ComPtr<ID3D12Resource>				m_renderTargets[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap>		m_rtvHeap;
	UINT								m_rtvDescriptorSize;
	ComPtr<ID3D12Resource>				m_depthStencil;
	ComPtr<ID3D12DescriptorHeap>		m_dsvHeap;
	ComPtr<ID3D12RootSignature>			m_rootSignature;

	ComPtr<ID3D12Fence>					m_fence;
	UINT								m_frameIndex;
	UINT64								m_fenceValue;
	HANDLE								m_fenceEvent;

	CGameTimer							m_GameTimer;
	POINT								m_ptOldCursorPos;

	_TCHAR								m_pszBaseTitle[256];// 원래 창 제목
	_TCHAR								m_pszFrameRate[50];// FPS 표시용

	unique_ptr<SceneManager>			m_sceneManager;
	shared_ptr<Player>					m_player;  // 플레이어 객체 추가 GameFramework가 Player를 관리
};

