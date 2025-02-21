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

// 정점 셰이더 (개별 메시 기반)
PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.Position, 1.0f), g_worldMatrix);
    float4 viewPosition = mul(worldPosition, g_viewMatrix);
    output.Position = mul(viewPosition, g_projectionMatrix);
    
    // 노멀 변환 적용
    output.Normal = mul(input.Normal, (float3x3) g_worldMatrix);
    output.TexCoord = input.TexCoord;

    return output;
}

// 픽셀 셰이더 (단색 출력)
float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 0.5f, 0.2f, 1.0f); // 텍스처 없이 단색 출력
}
