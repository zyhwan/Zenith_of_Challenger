#pragma once
#include "stdafx.h"

struct Light
{
    XMFLOAT3 position;
    float intensity;
    XMFLOAT3 color;
    float padding; // 16바이트 정렬
};

class Lighting
{
public:
    Lighting() = default;
    ~Lighting() = default;

    void SetLight(const XMFLOAT3& pos, const XMFLOAT3& color, float intensity);
    const Light& GetLight() const;

private:
    Light m_light;
};
