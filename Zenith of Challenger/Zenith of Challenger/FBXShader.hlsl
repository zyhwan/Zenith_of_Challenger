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
    float3 WorldPos : WORLDPOS;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.Position, 1.0f), g_worldMatrix);
    float4 viewPosition = mul(worldPosition, g_viewMatrix);
    output.Position = mul(viewPosition, g_projectionMatrix);

    output.WorldPos = worldPosition.xyz;
    output.Normal = normalize(mul(input.Normal, (float3x3) g_worldMatrix));
    output.TexCoord = float2(input.TexCoord.x, 1.0f - input.TexCoord.y); // UV ������

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = g_texture[0].Sample(g_sampler, input.TexCoord);

    // ������ �ʹ� ��ο�� fallback ����
    if (texColor.r + texColor.g + texColor.b < 0.01f)
    {
        texColor.rgb = float3(1.0f, 0.0f, 1.0f); // ������Ÿ
    }
    
    float3 normal = normalize(input.Normal);
    float3 toEye = normalize(g_cameraPosition - input.WorldPos);
    MaterialData matData = g_material[0];

    return Lighting(input.WorldPos, normal, toEye, texColor, matData);
}
