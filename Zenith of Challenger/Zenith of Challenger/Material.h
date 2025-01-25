#pragma once
#include "stdafx.h"
#include "buffer.h"

//������ � ��ü�� ���� �󸶳� �ݻ��ϰų� �������, ��ü�� ǥ�� ��ĥ��� �����, ��ü�� �������� ���� ���� ���� ����
//�ʴ��� �⺻������ �󸶳� ���� �޵��� ����� ���� ����.

struct MaterialData : public BufferBase
{
	XMFLOAT3 fresnelR0; //�ش� ������ �Ի��� ���� �󸶳� �ݻ��ϴ��� ���� (�ݻ� ����)
	FLOAT roughness; //������ ��ĥ��
	XMFLOAT3 ambient; //��ü�� ���� ���� ���� �Ѱ� �ֺ� ��ü�� �ݻ�� ���� ������ ���� ������ �ִ� ��
};

class Material
{
public:
	Material() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetMaterial(MaterialData material);
	void SetMaterial(XMFLOAT3 fresnelR0, FLOAT roughness, XMFLOAT3 ambient);
	void CreateShaderVariable(const ComPtr<ID3D12Device>& device);

private:
	vector<MaterialData> m_material;
	unique_ptr<UploadBuffer<MaterialData>> m_constantBuffer;
};