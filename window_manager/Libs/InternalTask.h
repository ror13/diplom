#ifndef __InternalTask_H__
#define __InternalTask_H__

#include "video_capture.h"

#include "vlc/vlc.h"
#include "vlc/vlc_additions.h"

#define ADDITIONAL_PRVLC_OPTIONS	_T(" --ignore-config ")

class CInternalTask
{
public:
	virtual ~CInternalTask() {};
	virtual void CloseTaskHandle() = 0;
	virtual BOOL IsActive() = 0;
	virtual BOOL StopProcess() = 0;
	virtual BOOL StopChildProcesses() = 0;
};

class CInternalTaskVLC : public CInternalTask
{
public:
	CInternalTaskVLC();
	virtual ~CInternalTaskVLC();
	
	virtual void CloseTaskHandle();
	virtual BOOL IsActive();
	virtual BOOL StopProcess();
	virtual BOOL StopChildProcesses();

	static UINT CreatePlayerThreadFunc( LPVOID pParam );
//	static void CreatePlayerThreadFunc( LPVOID pParam );
	BOOL CreatePlayer(CString VLCOptions, CString VLCMedia, BOOL VLCCaptureMode = FALSE);
	BOOL CreatePlayer2(CString VLCOptions, CString VLCMedia, BOOL VLCCaptureMode = FALSE);
	static UINT DestroyPlayerThreadFunc( LPVOID pParam );
//	static void DestroyPlayerThreadFunc( LPVOID pParam );
	void DestroyPlayer();
	void DestroyPlayer2();

	void WaitPlayer();

private:
	CString m_th_VLCOptions;
	CString m_th_VLCMedia;
	BOOL m_th_VLCCaptureMode;
	// VLC usage
	BOOL m_bVLCCaptureMode;
	libvlc_instance_t * m_inst;
	libvlc_media_player_t *m_mp;
	libvlc_media_t *m_m;

	libvlc_time_t m_length;

	time_t m_start_time;

	
//	CString	m_CaptureWndClass;
//	CWnd	m_wndCaptureWindow;
	
};

class CInternalTaskPRVLC : public CInternalTask
{
public:
	CInternalTaskPRVLC();
	virtual ~CInternalTaskPRVLC();
	
	virtual void CloseTaskHandle();
	virtual BOOL IsActive();
	virtual BOOL StopProcess();
	virtual BOOL StopChildProcesses();
	
	BOOL CreatePlayer(CString VLCOptions, CString VLCMedia, BOOL VLCCaptureMode = FALSE);
	BOOL CreatePlayerWithPlayList(CString VLCOptions, CStringArray& VLCMedias);
	void DestroyPlayer();
	
private:
	// VLC usage
	BOOL m_bVLCCaptureMode;

	HANDLE	m_hPRVLCProcess;
	DWORD	m_dwPRVLCProcessId;

	time_t m_start_time;
};

#endif // __InternalTask_H__