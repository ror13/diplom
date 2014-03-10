// MMTimer.h: interface for the CMMTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMTIMER_H__7E1FF4CD_6F97_48E0_92A4_12DDC9FECDC6__INCLUDED_)
#define AFX_MMTIMER_H__7E1FF4CD_6F97_48E0_92A4_12DDC9FECDC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TARGET_RESOLUTION 1

class CMMTimerClient
{
public:
	virtual void OnMMTimer(int MMTimerNum) = 0;
};

class CMMTimer
{
public:
	CMMTimer();
	virtual ~CMMTimer();

	BOOL SetMMTimer(CMMTimerClient *pClient, UINT uPeriod, int MMTimerNum = 0);

protected:
	int		m_MMTimerNum;

    UINT    m_wPeriodMin;     /* minimum period supported  */
    UINT    m_wPeriodMax;     /* maximum period supported  */

	UINT	m_wResolution;
};

#endif // !defined(AFX_MMTIMER_H__7E1FF4CD_6F97_48E0_92A4_12DDC9FECDC6__INCLUDED_)
