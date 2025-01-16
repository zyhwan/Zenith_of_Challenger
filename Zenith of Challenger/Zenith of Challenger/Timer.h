#pragma once
#include "stdafx.h"

class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer() = default;

	void Tick();
	FLOAT GetElapsedTime() const;
	UINT GetFPS() const; // FPS ��� �Լ� �߰�

private:
	LARGE_INTEGER	m_prev;
	LARGE_INTEGER	m_frequency;
	FLOAT			m_deltaTime;
	UINT            m_frameCount;  // ���� 1�� ������ ������ ��
	FLOAT           m_elapsedTime; // ���� �ð� �հ�
	UINT            m_fps;         // ���� ���� FPS

};