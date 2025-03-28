cbuffer GameObject : register(b0)
{
    matrix g_worldMatrix : packoffset(c0); // c0 ~ c3
    float4 g_baseColor : packoffset(c4); // c4
    int g_useTexture : packoffset(c5.x); // c5.x
    float3 padding : packoffset(c5.y); // c5.y ~ c5.w
};

cbuffer Camera : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
    float3 g_cameraPosition : packoffset(c8);
};

#include "Lighting.hlsl"

TextureCube g_textureCube : register(t0);
Texture2D g_texture[2] : register(t1);

struct InstanceData
{
    float4x4 worldMatrix;
    uint textureIndex;
    uint materialIndex;
};
StructuredBuffer<InstanceData> g_instanceData : register(t0, space1);

SamplerState g_sampler : register(s0);
