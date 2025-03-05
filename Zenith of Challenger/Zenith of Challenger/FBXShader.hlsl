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

// Á¤Á¡ ¼ÎÀÌ´õ (FBX ¸ðµ¨¿ë)
PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.Position, 1.0f), g_worldMatrix);
    float4 viewPosition = mul(worldPosition, g_viewMatrix);
    output.Position = mul(viewPosition, g_projectionMatrix);
    
    // ³ë¸Ö º¯È¯ Àû¿ë
    output.Normal = normalize(mul(input.Normal, (float3x3) g_worldMatrix));

    // UV ÁÂÇ¥ º¯È¯ (DirectX ÁÂÇ¥°è º¸Á¤)
    output.TexCoord = float2(input.TexCoord.x, 1.0f - input.TexCoord.y);

    return output;
}

// ÇÈ¼¿ ¼ÎÀÌ´õ (ÅØ½ºÃ³ Àû¿ë)
float4 PSMain(PSInput input) : SV_TARGET
{
    float4 textureColor = g_texture[0].Sample(g_sampler, input.TexCoord);
    
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(float3(0.5f, -1.0f, 0.5f));
    float lightIntensity = max(dot(normal, lightDir), 0.2f);
    
    return float4(textureColor.rgb * lightIntensity, textureColor.a);
}
