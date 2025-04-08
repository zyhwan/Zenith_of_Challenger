//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "shader.h"

void Shader::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->SetPipelineState(m_pipelineState.Get());
}

ObjectShader::ObjectShader(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCE_ID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> mvsByteCode, mpsByteCode;
	D3DCompileFromFile(TEXT("Object.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "VERTEX_MAIN", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr);
	D3DCompileFromFile(TEXT("Object.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "PIXEL_MAIN", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
}

SkyboxShader::SkyboxShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } };

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> mvsByteCode, mpsByteCode;
	D3DCompileFromFile(TEXT("SkyBox.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "SKYBOX_VERTEX", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr);
	D3DCompileFromFile(TEXT("SkyBox.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "SKYBOX_PIXEL", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
}

DetailShader::DetailShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } };

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> mvsByteCode, mpsByteCode;
	D3DCompileFromFile(TEXT("Terrain.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "DETAIL_VERTEX", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr);
	D3DCompileFromFile(TEXT("Terrain.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "DETAIL_PIXEL", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize() };
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
}

FBXShader::FBXShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> mvsByteCode, mpsByteCode;
	ComPtr<ID3DBlob> errorMsgs;

	HRESULT hr1 = D3DCompileFromFile(TEXT("FBXShader.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_1", compileFlags, 0, &mvsByteCode, &errorMsgs);

	if (FAILED(hr1))
	{
		if (errorMsgs)
		{
			std::cerr << "FBXShader Vertex Shader ������ ����: " << (char*)errorMsgs->GetBufferPointer() << std::endl;
		}
		else
		{
			std::cerr << "FBXShader Vertex Shader ������ ã�� �� �����ϴ�!" << std::endl;
		}
		return;
	}

	HRESULT hr2 = D3DCompileFromFile(TEXT("FBXShader.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_1", compileFlags, 0, &mpsByteCode, &errorMsgs);

	if (FAILED(hr2))
	{
		if (errorMsgs)
		{
			std::cerr << "FBXShader Pixel Shader ������ ����: " << (char*)errorMsgs->GetBufferPointer() << std::endl;
		}
		else
		{
			std::cerr << "FBXShader Pixel Shader ������ ã�� �� �����ϴ�!" << std::endl;
		}
		return;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()), mvsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()), mpsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �ø� ��Ȱ��ȭ
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hr3 = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr3))
	{
		std::cerr << "FBXShader ���������� ���� ����!" << std::endl;
	}

	if (!m_pipelineState)
		std::cout << "�� PSO ���� ����" << std::endl;
	else
		cout << "�� PSO ���� ����!!" << endl;

}

UIScreenShader::UIScreenShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
	   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	   { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	   { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> vsByteCode, psByteCode;
	D3DCompileFromFile(TEXT("UIScreenShader.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSMain", "vs_5_1", compileFlags, 0, &vsByteCode, nullptr);
	D3DCompileFromFile(TEXT("UIScreenShader.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSMain", "ps_5_1", compileFlags, 0, &psByteCode, nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(vsByteCode->GetBufferPointer()), vsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(psByteCode->GetBufferPointer()), psByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE; // UI���� ���� ���� ��
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;

	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));

}



CharacterShader::CharacterShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> vs, ps, errorMsgs;

	HRESULT hr1 = D3DCompileFromFile(TEXT("Character.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_1", compileFlags, 0, &vs, &errorMsgs);
	if (FAILED(hr1) && errorMsgs)
		std::cerr << "[CharacterShader] VS Error: " << (char*)errorMsgs->GetBufferPointer() << std::endl;

	HRESULT hr2 = D3DCompileFromFile(TEXT("Character.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_1", compileFlags, 0, &ps, &errorMsgs);
	if (FAILED(hr2) && errorMsgs)
		std::cerr << "[CharacterShader] PS Error: " << (char*)errorMsgs->GetBufferPointer() << std::endl;
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(vs->GetBufferPointer()), vs->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(ps->GetBufferPointer()), ps->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;

	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (!m_pipelineState)
		std::cout << "ĳ���� PSO ���� ����" << std::endl;
	else
		cout << "ĳ���� PSO ���� ����!!" << endl;
}
