#include "timer.h"

CGameTimer::CGameTimer() : m_prev{ 0 }, m_deltaTime{ 0.f }, m_frameCount{ 0 }, m_elapsedTime{ 0.f }, m_fps{ 0 }
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_prev); // �ʱ�ȭ �� ���� �ð� ����
}

void CGameTimer::Tick()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	// ������ �ð� ���
	m_deltaTime = static_cast<FLOAT>((now.QuadPart - m_prev.QuadPart) / static_cast<FLOAT>(m_frequency.QuadPart));
	m_prev = now;

	// FPS ���
	m_frameCount++;
	m_elapsedTime += m_deltaTime;

	if (m_elapsedTime >= 1.0f) // 1�� ��� ��
	{
		m_fps = m_frameCount;      // FPS ���
		m_frameCount = 0;          // ������ ī���� �ʱ�ȭ
		m_elapsedTime -= 1.0f;     // �� ���� �ð� �ʱ�ȭ (�ʰ� �ð� ����)
	}
}

FLOAT CGameTimer::GetElapsedTime() const
{
	return m_deltaTime; // ���� ������ �ð� ��ȯ
}

UINT CGameTimer::GetFPS() const
{
	return m_fps; // ���� FPS ��ȯ
}