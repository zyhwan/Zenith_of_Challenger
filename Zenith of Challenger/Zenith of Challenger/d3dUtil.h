// d3dUtil.h
#pragma once
#include "stdafx.h"


namespace d3dUtil
{
    // DefaultBuffer ���� + UploadBuffer�� �ʱ� ������ ����
    void CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* commandList,
        const void* initData,
        UINT64 byteSize,
        ComPtr<ID3D12Resource>& defaultBuffer,
        ComPtr<ID3D12Resource>& uploadBuffer,
        D3D12_RESOURCE_STATES finalState); // �߰��� �κ�
}