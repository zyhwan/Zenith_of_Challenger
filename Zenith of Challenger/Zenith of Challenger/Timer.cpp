#include "timer.h"

CGameTimer::CGameTimer() : m_prev{ 0 }, m_deltaTime{ 0.f }
{
	QueryPerformanceFrequency(&m_frequency);	// Ÿ�̸��� ���ļ� ȹ��
}

void CGameTimer::Tick()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	m_deltaTime = static_cast<FLOAT>((now.QuadPart - m_prev.QuadPart) / static_cast<FLOAT>(m_frequency.QuadPart));
	m_prev = now;
}

FLOAT CGameTimer::GetElapsedTime() const
{
	return m_deltaTime;
}
