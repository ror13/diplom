#include "StdAfx.h"
#include "InternalTask.h"

#include "FileOperation.h"
#include "PresenterGlobals.h"

#include "ExecuteProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInternalTaskVLC::CInternalTaskVLC()
{
	m_bVLCCaptureMode = FALSE;
	// vlc init
	m_inst = NULL;
	m_mp = NULL;
	m_m = NULL;
	
	m_bVLCCaptureMode = FALSE;
	
	m_length = 0;
	m_start_time = 0;
}

CInternalTaskVLC::~CInternalTaskVLC()
{
	DestroyPlayer();
}

void CInternalTaskVLC::WaitPlayer()
{
#ifdef PRESENTER_CLIENT_CODE
	libvlc_wait (m_inst);
#endif // PRESENTER_CLIENT_CODE	
}

UINT CInternalTaskVLC::CreatePlayerThreadFunc( LPVOID pParam )
//void CInternalTaskVLC::CreatePlayerThreadFunc( LPVOID pParam )
{
	CInternalTaskVLC* me = (CInternalTaskVLC*)pParam;

	me->CreatePlayer2(me->m_th_VLCOptions, me->m_th_VLCMedia, me->m_th_VLCCaptureMode);
	me->WaitPlayer();

	return 0;
}

BOOL CInternalTaskVLC::CreatePlayer(CString VLCOptions, CString VLCMedia, BOOL VLCCaptureMode)
{
	TRACE(_T("CInternalTaskVLC::CreatePlayer\n"));
//#ifdef _DEBUG
 	return CreatePlayer2(VLCOptions, VLCMedia, VLCCaptureMode);
// #else
	m_th_VLCOptions = VLCOptions;
	m_th_VLCMedia = VLCMedia;
	m_th_VLCCaptureMode = VLCCaptureMode;
	AfxBeginThread(CreatePlayerThreadFunc, this);
//	_beginthread( CreatePlayerThreadFunc, 0, this);

	return TRUE;
//#endif
}

BOOL CInternalTaskVLC::CreatePlayer2(CString VLCOptions, CString VLCMedia, BOOL VLCCaptureMode)
{
	TRACE(_T("CInternalTaskVLC::CreatePlayer2\n"));
#ifdef PRESENTER_CLIENT_CODE
//////////////////////////////////////////////////////////////////////////
	USES_CONVERSION;

	TRACE(_T("CInternalTaskVLC::CreatePlayer(VLCOptions = [%s], VLCMedia = [%s])\n"), VLCOptions, VLCMedia);

	BOOL bVLCFullScreen = TRUE;
	BOOL bVLCDirectXOutput = FALSE;

	char * vlc_args[99];

	char* vlc_args_buffer;
	int vlc_args_count = 0;

	vlc_args_buffer = (char *)malloc(VLCOptions.GetLength()+1);
	strcpy(vlc_args_buffer, T2A(VLCOptions.GetBuffer(0)));

	char seps[]   = " \t\n";
	char *token;

	BOOL bVLCFNowGetAspectRatio = FALSE;
	int iAspectRatio = -1;

	/* Establish string and get the first token: */
	token = strtok( vlc_args_buffer, seps );
	while( token != NULL )
	{
		/* While there are tokens in "string" */
		//printf( " %s\n", token );
		vlc_args[vlc_args_count] = strdup(token);

		if (!strcmp(token, "--no-fullscreen"))
		{
			bVLCFullScreen = FALSE;
		}

		if (!strcmp(token, "--wall-monitors-assoc"))
		{
			bVLCDirectXOutput = TRUE;
		}
		
		// read it value
		if (bVLCFNowGetAspectRatio)
		{
			iAspectRatio = vlc_args_count;
			bVLCFNowGetAspectRatio = FALSE;
			
			TRACE(_T("--aspect-ratio with [%S]\n"), vlc_args[iAspectRatio]);
		}
		//if we get --aspect-ratio command next we...
		if (!strcmp(token, "--aspect-ratio"))
		{
			bVLCFNowGetAspectRatio = TRUE;
		}

		vlc_args_count++;
		/* Get next token: */
		token = strtok( NULL, seps );
	}

// 	if (bVLCDirectXOutput)
// 	{
// 		vlc_args[vlc_args_count++] = strdup("--vout");
// 		vlc_args[vlc_args_count++] = strdup("directx");
// 	}
// 	else
// 	{
// 		vlc_args[vlc_args_count++] = strdup("--vout");
// 		vlc_args[vlc_args_count++] = strdup("direct3d");
// 	}

    vlc_args[vlc_args_count++] = strdup("--ignore-config");
    vlc_args[vlc_args_count++] = strdup("--no-plugins-cache");
#ifdef _DEBUG
#if 0
	vlc_args[vlc_args_count++] = strdup("--extraintf=logger"); //log anything
	vlc_args[vlc_args_count++] = strdup("--verbose=3"); //be much more verbose then normal for debugging purpose
	vlc_args[vlc_args_count++] = strdup("--dshow-adev=none");

	char szLogOption[256];
	sprintf(szLogOption, "--logfile=VLCfoo%.8d.log", GetCurrentThreadId());

	vlc_args[vlc_args_count++] = strdup("-vvv");
	vlc_args[vlc_args_count++] = strdup("--file-logging");
//	vlc_args[vlc_args_count++] = strdup("--logfile=VLCfoo.log");
	vlc_args[vlc_args_count++] = strdup(szLogOption);
	vlc_args[vlc_args_count++] = strdup("--logmode=text");

//	vlc_args_count+= 3;
#endif
#endif //_DEBUG

	
// 	const char * const vlc_args[] = {
// 		"-I", "dummy", /* Don't use any interface */
// 		"--ignore-config", /* Don't use VLC's config */
// //		"--plugin-path=E:\\TEMP\\VLC-1.0.5\\plugins\\",
// 		T2A(VLCOptions.GetBuffer(0)),
// //		T2A(VLCMedia.GetBuffer(0)),
// 
// 	};
// 	int vlc_args_count = sizeof(vlc_args) / sizeof(vlc_args[0]);

#if 0
	//for (int i = 0; i < sizeof(vlc_args) / sizeof(vlc_args[0]); i++)
	for (int i = 0; i < vlc_args_count; i++)
	{
		TRACE( _T("%d - %s\n"), i, A2T(vlc_args[i]));
	}
#endif
	
	/* init vlc modules, should be done only once */
	//m_inst = libvlc_new (sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	m_inst = libvlc_new (vlc_args_count, vlc_args);
	
	if (m_inst == NULL)
	{
		return FALSE;
	}
	
	/* Create a new item */
	//m_m = libvlc_media_new (m_inst, "E:/Media/2/1.avi", &m_ex);
	m_m = libvlc_media_new_path (m_inst, T2A(VLCMedia.GetBuffer(0)));
	
	if (m_m == NULL)
	{
		libvlc_release(m_inst);
		m_inst = NULL;
		TRACE(_T("CInternalTaskVLC::CreatePlayer m_m == NULL\n"));
		return FALSE;
	}
	
	/* Create a media player playing environment */
	m_mp = libvlc_media_player_new_from_media (m_m);
	
	if (m_mp == NULL)
    {
		libvlc_media_release(m_m);
		m_m = NULL;
		
		libvlc_release(m_inst);
		m_inst = NULL;
		TRACE(_T("CInternalTaskVLC::CreatePlayer m_mp == NULL\n"));
		return FALSE;
	}
	
	/* No need to keep the media now */
	libvlc_media_release (m_m);
	m_m = NULL;
	
//!!!	libvlc_media_player_set_hwnd(m_mp, m_wndCaptureWindow.GetSafeHwnd());
	libvlc_set_fullscreen(m_mp, bVLCFullScreen);
	if (iAspectRatio != -1)
	{
		libvlc_video_set_aspect_ratio( m_mp, vlc_args[iAspectRatio] );
	}
	
	TRACE(_T("BEFORE libvlc_media_player_play\n"));
	/* play the media_player */
	int iplay = libvlc_media_player_play (m_mp);
	TRACE(_T("AFTER libvlc_media_player_play (%d)\n"), iplay);
	
	if (iplay == -1)
    {
		libvlc_media_player_release (m_mp);
		m_mp = NULL;
		
		libvlc_release(m_inst);
		m_inst = NULL;
		TRACE(_T("CInternalTaskVLC::CreatePlayer iplay == -1\n"));
		return FALSE;
	}

	for (int i = 0; i < vlc_args_count; i++)
	{
		if (vlc_args[i])
		{
			free(vlc_args[i]);
			vlc_args[i] = NULL;
		}
	}

	if (vlc_args_buffer)
	{
		free(vlc_args_buffer);
		vlc_args_buffer = NULL;
	}

	m_bVLCCaptureMode = VLCCaptureMode;
	
	TRACE(_T("CInternalTaskVLC::CreatePlayer success started\n"));

	if (!VLCCaptureMode)
	{
		m_length = libvlc_media_player_get_length( m_mp );
		TRACE(_T("m_length %I64d\n"), m_length);
		time(&m_start_time);
		TRACE(_T("m_start_time %Ld\n"), m_start_time);

		while (!IsActive())
		{
		}
		time_t curent_time;
		time(&curent_time);
		
		double elapsed_time;
		elapsed_time = difftime(curent_time, m_start_time);
		TRACE(_T("elapsed_time %6.0f\n"), elapsed_time);
	}
#endif // PRESENTER_CLIENT_CODE	
	return TRUE;
}

UINT CInternalTaskVLC::DestroyPlayerThreadFunc( LPVOID pParam )
//void CInternalTaskVLC::DestroyPlayerThreadFunc( LPVOID pParam )
{
	TRACE(_T("CInternalTaskVLC::DestroyPlayer2\n"));
	CInternalTaskVLC* me = (CInternalTaskVLC*)pParam;

	me->DestroyPlayer2();
	return 0;
}

void CInternalTaskVLC::DestroyPlayer()
{
	TRACE(_T("CInternalTaskVLC::DestroyPlayer\n"));
//#ifdef _DEBUG
	DestroyPlayer2();
//#else
//	AfxBeginThread(DestroyPlayerThreadFunc, this);
//	_beginthread( DestroyPlayerThreadFunc, 0, this);
//#endif
}

void CInternalTaskVLC::DestroyPlayer2()
{
	TRACE(_T("CInternalTaskVLC::DestroyPlayer\n"));
#ifdef PRESENTER_CLIENT_CODE
	// clear vlc
	if (m_m)
	{
		libvlc_media_release (m_m);
		m_m = NULL;
	}
	
	if (m_mp)
	{
		libvlc_media_player_stop (m_mp);
		
		libvlc_media_player_release (m_mp);
		m_mp = NULL;
	}
	
	if (m_inst)
	{
		libvlc_release(m_inst);
		m_inst = NULL;
	}
	TRACE(_T("CInternalTaskVLC::DestroyPlayer Done\n"));
#endif //PRESENTER_CLIENT_CODE
}

void CInternalTaskVLC::CloseTaskHandle()
{
	TRACE(_T("CInternalTaskVLC::CloseTaskHandle\n"));
}

#ifdef PRESENTER_CLIENT_CODE
int libvlc_media_player_is_playing_or_opening( libvlc_media_player_t *p_mi )
{
	if (p_mi == NULL)
	{
		return FALSE;
	}
    libvlc_state_t state = libvlc_media_player_get_state( p_mi );
    return (libvlc_Opening == state) || (libvlc_Playing == state) || (libvlc_Buffering == state);
}
#endif // PRESENTER_CLIENT_CODE

BOOL CInternalTaskVLC::IsActive()
{
#ifdef PRESENTER_CLIENT_CODE
	TRACE(_T("CInternalTaskVLC::IsActive\n"));
	if (m_mp)
	{
		if (m_bVLCCaptureMode)
		{
			return TRUE;
		}

		// at start we can get some pause
// 		time_t curent_time;
// 		time(&curent_time);
// 
// 		double elapsed_time;
// 		elapsed_time = difftime(curent_time, m_start_time);
// 		if (m_length != -1 && 
// 			(min(m_length/1000, 10) < elapsed_time) )
// 		{
// 			return TRUE;
// 		}

//		if (libvlc_media_player_is_playing(m_mp))
		if (libvlc_media_player_is_playing_or_opening(m_mp))
		{
			return TRUE;
		}
	}
#endif //PRESENTER_CLIENT_CODE
	return FALSE;
}

BOOL CInternalTaskVLC::StopProcess()
{
#ifdef PRESENTER_CLIENT_CODE
	TRACE(_T("CInternalTaskVLC::StopProcess\n"));
	if (m_mp)
	{
		libvlc_media_player_stop(m_mp);
	}

	DestroyPlayer();
#endif //PRESENTER_CLIENT_CODE

	return TRUE;
}

BOOL CInternalTaskVLC::StopChildProcesses()
{
	TRACE(_T("CInternalTaskVLC::StopChildProcesses\n"));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInternalTaskPRVLC::CInternalTaskPRVLC()
{
	m_bVLCCaptureMode = FALSE;

	m_hPRVLCProcess = INVALID_HANDLE_VALUE;
	m_dwPRVLCProcessId = -1;
	
	m_start_time = 0;
}

CInternalTaskPRVLC::~CInternalTaskPRVLC()
{
	DestroyPlayer();
}

BOOL CInternalTaskPRVLC::CreatePlayerWithPlayList(CString VLCOptions, CStringArray& VLCMedias)
{
	CString strExeFolder = GetExeFolder();
	CString dir = GetExeFolder();
	CString PlayerExecutable = PRVLC_EXE_FILE_NAME;
	CString command;
	
	VLCOptions += ADDITIONAL_PRVLC_OPTIONS;
	
// 	command.Format( _T("\"%s%s\" %s %s \"%s\""), 
// 		strExeFolder, PlayerExecutable, VLCOptions, PRVLC_MEDIA_NAME_OPTION, VLCMedia);
	command.Format( _T("\"%s%s\" %s"), 
		strExeFolder, PlayerExecutable, VLCOptions);

	for (int i = 0; i < VLCMedias.GetSize(); i++)
	{
		CString smallCommand;

		smallCommand.Format(_T(" %s \"%s\""), PRVLC_MEDIA_NAME_OPTION, VLCMedias[i]);
		command += smallCommand;
	}

	TRACE(_T("CInternalTaskPRVLC::CreatePlayer command [%s]"), command);
	
	HANDLE	hProcess = NULL;
	
	BOOL bStartedOK = FALSE;
	int nShowWindow = SW_SHOWNORMAL;
	
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nShowWindow;
	
	DWORD dwCreationFlags = 0;
	
	LPTSTR lszCommand = command.IsEmpty() ? NULL : (LPTSTR)(LPCTSTR)command;
	LPCTSTR lszDir = dir.IsEmpty() ? NULL : (LPCTSTR)dir;
	
	SetLastError( ERROR_SUCCESS );
	
	bStartedOK = CreateProcess( NULL, lszCommand,
		NULL, NULL, FALSE, dwCreationFlags, NULL, lszDir, &si, &pi );
	LONG err = GetLastError();
	
	TRACE( _T("after CreateProcess bStartedOK %d, %.8X\n"), bStartedOK, err);
	
	if(bStartedOK)
	{
		CloseHandle(pi.hThread);
		hProcess = pi.hProcess;
		TRACE( _T("pi.hProcess = %.8X\n"), pi.hProcess);
		TRACE( _T("pi.dwProcessId = %.8X (%d)\n"), pi.dwProcessId, pi.dwProcessId);
		
		m_dwPRVLCProcessId = pi.dwProcessId;
	}
	
	TRACE(_T("CInternalTaskPRVLC::CreatePlayer hProcess = %.8X\n"), hProcess);
	m_hPRVLCProcess = hProcess;
	
	return TRUE;
}

BOOL CInternalTaskPRVLC::CreatePlayer(CString VLCOptions, CString VLCMedia, BOOL VLCCaptureMode)
{
	CString strExeFolder = GetExeFolder();
	CString dir = GetExeFolder();
	CString PlayerExecutable = PRVLC_EXE_FILE_NAME;
	CString command;

	VLCOptions += ADDITIONAL_PRVLC_OPTIONS;

	command.Format( _T("\"%s%s\" %s %s \"%s\""), 
		strExeFolder, PlayerExecutable, VLCOptions, PRVLC_MEDIA_NAME_OPTION, VLCMedia);

	TRACE(_T("CInternalTaskPRVLC::CreatePlayer command [%s]"), command);

	HANDLE	hProcess = NULL;

	BOOL bStartedOK = FALSE;
	int nShowWindow = SW_SHOWNORMAL;

	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nShowWindow;
	
	DWORD dwCreationFlags = 0;
	
	LPTSTR lszCommand = command.IsEmpty() ? NULL : (LPTSTR)(LPCTSTR)command;
	LPCTSTR lszDir = dir.IsEmpty() ? NULL : (LPCTSTR)dir;
	
	SetLastError( ERROR_SUCCESS );
	
	bStartedOK = CreateProcess( NULL, lszCommand,
		NULL, NULL, FALSE, dwCreationFlags, NULL, lszDir, &si, &pi );
	LONG err = GetLastError();
	
	TRACE( _T("after CreateProcess bStartedOK %d, %.8X\n"), bStartedOK, err);
	
	if(bStartedOK)
	{
		CloseHandle(pi.hThread);
		hProcess = pi.hProcess;
		TRACE( _T("pi.hProcess = %.8X\n"), pi.hProcess);
		TRACE( _T("pi.dwProcessId = %.8X (%d)\n"), pi.dwProcessId, pi.dwProcessId);

		m_dwPRVLCProcessId = pi.dwProcessId;
	}

	TRACE(_T("CInternalTaskPRVLC::CreatePlayer hProcess = %.8X\n"), hProcess);
	m_hPRVLCProcess = hProcess;
	
	return TRUE;
}

void CInternalTaskPRVLC::DestroyPlayer()
{
	TRACE(_T("CInternalTaskPRVLC::DestroyPlayer\n"));

	if (m_hPRVLCProcess != INVALID_HANDLE_VALUE && m_hPRVLCProcess != NULL)
	{
		TCHAR* szEventName[sizeof(PRVLC_EVENT_NAME_PREFIX)+sizeof(PRVLC_EVENT_NAME_POSTFIX)+sizeof(TCHAR)];
		
		_stprintf((TCHAR*)szEventName, _T("%s%.8X"), PRVLC_EVENT_NAME_PREFIX, m_dwPRVLCProcessId);
		CEvent event(FALSE, FALSE, (TCHAR*)szEventName);
		
		event.SetEvent();
		
		WaitForSingleObject(m_hPRVLCProcess, 2000);
		
		//StopProcess();
		
		//m_hPRVLCProcess = INVALID_HANDLE_VALUE;
		CloseTaskHandle();
	}
}

void CInternalTaskPRVLC::CloseTaskHandle()
{
	TRACE(_T("CInternalTaskPRVLC::CloseTaskHandle\n"));
	if(m_hPRVLCProcess != INVALID_HANDLE_VALUE && m_hPRVLCProcess != NULL)
	{
		CloseHandle(m_hPRVLCProcess);
		m_hPRVLCProcess = INVALID_HANDLE_VALUE;
	}
}

BOOL CInternalTaskPRVLC::IsActive()
{
	TRACE( _T("CInternalTaskPRVLC::IsActive"));
	if (m_hPRVLCProcess == INVALID_HANDLE_VALUE || m_hPRVLCProcess == NULL)
	{
		return FALSE;
	}

	BOOL retval = FALSE;
	DWORD code = 0;
	BOOL res = GetExitCodeProcess(m_hPRVLCProcess, &code);
	
	if(res == 0 )
	{
		TRACE(_T(" function fails, errcode = %d\n"),GetLastError());
		retval = FALSE;
		TRACE (_T("(%d) \n"),retval);
		return retval;
		
	}
	if(code == STILL_ACTIVE)
	{
		retval = TRUE;
	}
	else
	{
		retval = FALSE;
	}
	
	TRACE (_T(" (%d)\n"),retval);	
	return retval;
}

BOOL CInternalTaskPRVLC::StopProcess()
{
	BOOL bRet = TRUE;

	TRACE(_T("CInternalTaskPRVLC::StopProcess\n"));
	StopChildProcesses();

	DestroyPlayer();
	CloseTaskHandle();

	TRACE( _T("CInternalTaskPRVLC TerminateProcess TaskProcess  = %.8X\n"), m_hPRVLCProcess);

	if (m_hPRVLCProcess != INVALID_HANDLE_VALUE && m_hPRVLCProcess != NULL)
	{
		//bRet = TerminateProcess(m_hPRVLCProcess, 0);
		bRet = SafeTerminateProcess(m_hPRVLCProcess, 0);
		m_hPRVLCProcess = INVALID_HANDLE_VALUE;
	}

	return bRet;
}

BOOL CInternalTaskPRVLC::StopChildProcesses()
{
	TRACE(_T("CInternalTaskPRVLC::StopChildProcesses\n"));
	return TRUE;
}

