cbuffer GameObject : register(b0)
{
    matrix g_worldMatrix : packoffset(c0);
};

cbuffer Camera : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
};

Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
TextureCube g_textureCube : register(t2);

SamplerState g_sampler : register(s0);