// MMTimer.cpp: implementation of the CMMTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MMTimer.h"

#include <Mmsystem.h>

#pragma comment(lib, "Winmm.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMMTimer::CMMTimer()
{
	m_wPeriodMin = 10;
	m_wPeriodMax = 0;

	m_wResolution = 0;

	TIMECAPS tc;
	if(timeGetDevCaps(&tc, sizeof(tc)) == TIMERR_NOERROR)
	{
		m_wPeriodMin = tc.wPeriodMin;
		m_wPeriodMax = tc.wPeriodMax;

		m_wResolution = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);


		timeBeginPeriod(m_wPeriodMin);
	}
}

CMMTimer::~CMMTimer()
{
	if(m_wResolution)
		timeEndPeriod(m_wResolution);
}

BOOL CMMTimer::SetMMTimer(CMMTimerClient *pClient, UINT uPeriod, int MMTimerNum)
{
	m_MMTimerNum = MMTimerNum;
//	timeSetEvent();
	return FALSE;
}
