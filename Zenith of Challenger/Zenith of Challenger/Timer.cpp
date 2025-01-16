#include "timer.h"

CGameTimer::CGameTimer() : m_prev{ 0 }, m_deltaTime{ 0.f }, m_frameCount{ 0 }, m_elapsedTime{ 0.f }, m_fps{ 0 }
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_prev); // 초기화 시 현재 시간 저장
}

void CGameTimer::Tick()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	// 프레임 시간 계산
	m_deltaTime = static_cast<FLOAT>((now.QuadPart - m_prev.QuadPart) / static_cast<FLOAT>(m_frequency.QuadPart));
	m_prev = now;

	// FPS 계산
	m_frameCount++;
	m_elapsedTime += m_deltaTime;

	if (m_elapsedTime >= 1.0f) // 1초 경과 시
	{
		m_fps = m_frameCount;      // FPS 계산
		m_frameCount = 0;          // 프레임 카운터 초기화
		m_elapsedTime -= 1.0f;     // 초 단위 시간 초기화 (초과 시간 보존)
	}
}

FLOAT CGameTimer::GetElapsedTime() const
{
	return m_deltaTime; // 지난 프레임 시간 반환
}

UINT CGameTimer::GetFPS() const
{
	return m_fps; // 계산된 FPS 반환
}