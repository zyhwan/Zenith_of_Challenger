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
    nointerpolation uint textureIndex : TEXINDEX;
};

OBJECT_PIXEL_INPUT OBJECT_VERTEX(OBJECT_VERTEX_INPUT input, uint instanceID : SV_InstanceID)
{
    OBJECT_PIXEL_INPUT output;
    InstanceData instData = g_instanceData[instanceID];
    output.position = mul(float4(input.position, 1.0f), instData.worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.uv = input.uv;
    output.textureIndex = instData.textureIndex;
    
    return output;
}

float4 OBJECT_PIXEL(OBJECT_PIXEL_INPUT input) : SV_TARGET
{
    return g_texture[input.textureIndex].Sample(g_sampler, input.uv);
}