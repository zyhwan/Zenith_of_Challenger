#include "Lighting.h"

void Lighting::SetLight(const XMFLOAT3& pos, const XMFLOAT3& color, float intensity)
{
    m_light.position = pos;
    m_light.color = color;
    m_light.intensity = intensity;
}

const Light& Lighting::GetLight() const
{
    return m_light;
}
