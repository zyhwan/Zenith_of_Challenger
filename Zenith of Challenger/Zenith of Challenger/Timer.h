#pragma once
#include "stdafx.h"

class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer() = default;

	void Tick();
	FLOAT GetElapsedTime() const;

private:
	LARGE_INTEGER	m_prev;
	LARGE_INTEGER	m_frequency;
	FLOAT			m_deltaTime;
};