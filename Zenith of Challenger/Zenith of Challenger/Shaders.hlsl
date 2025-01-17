cbuffer GameObject : register(b0)
{
    matrix g_worldMatrix : packoffset(c0);
};

cbuffer Camera : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
    float3 g_cameraPosition : packoffset(c8);
};

TextureCube g_textureCube : register(t0);
Texture2D g_texture[2] : register(t1);

SamplerState g_sampler : register(s0);