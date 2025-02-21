#include "Shaders.hlsl"

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

// ���� ���̴� (���� �޽� ���)
PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.Position, 1.0f), g_worldMatrix);
    float4 viewPosition = mul(worldPosition, g_viewMatrix);
    output.Position = mul(viewPosition, g_projectionMatrix);
    
    // ��� ��ȯ ����
    output.Normal = mul(input.Normal, (float3x3) g_worldMatrix);
    output.TexCoord = input.TexCoord;

    return output;
}

// �ȼ� ���̴� (�ܻ� ���)
float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 0.5f, 0.2f, 1.0f); // �ؽ�ó ���� �ܻ� ���
}
