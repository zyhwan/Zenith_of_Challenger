#include "Shaders.hlsl"

struct OBJECT_VERTEX_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct OBJECT_PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

OBJECT_PIXEL_INPUT OBJECT_VERTEX(OBJECT_VERTEX_INPUT input)
{
    OBJECT_PIXEL_INPUT output;
    output.position = mul(float4(input.position, 1.0f), g_worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.uv = input.uv;
    
    return output;
}

float4 OBJECT_PIXEL(OBJECT_PIXEL_INPUT input) : SV_TARGET
{
    return g_texture[0].Sample(g_sampler, input.uv);
}