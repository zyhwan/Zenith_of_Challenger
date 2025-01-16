#pragma once
#include "stdafx.h"

class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer() = default;

	void Tick();
	FLOAT GetElapsedTime() const;
	UINT GetFPS() const; // FPS 계산 함수 추가

private:
	LARGE_INTEGER	m_prev;
	LARGE_INTEGER	m_frequency;
	FLOAT			m_deltaTime;
	UINT            m_frameCount;  // 지난 1초 동안의 프레임 수
	FLOAT           m_elapsedTime; // 지난 시간 합계
	UINT            m_fps;         // 최종 계산된 FPS

};