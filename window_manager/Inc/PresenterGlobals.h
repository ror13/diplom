// PresenterGlobals.h : global definitions for presenter
//

#if !defined(__PRESENTERGLOBALS_H__)
#define __PRESENTERGLOBALS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// duration of one video report duration
#define REPORT_CAMERA_VIDEO_DURATION		2
#define REPORT_CAMERA_CONFIG_FILE_NAME		_T("report_camera_config.ini")

// SyncConfig data
#define SYNC_CLIENT_CONFIG_FILE_NAME		_T("sync_client_config.ini")

#define LIVE_VIDEO_ONCLIENT_SERVER_PORT		7100
#define LOCAL_PORT							7000


#define MAX_CLIENT_COLS		4
#define MAX_CLIENT_ROWS		4
#if APPLICA_PRODUCT == PRODUCT_PRESENTER_SIGNAGE || APPLICA_PRODUCT == PRODUCT_PRESENTER_SIGNWARE || APPLICA_PRODUCT == PRODUCT_SIGNIA
#define MAX_CLIENT_VIEWS	1
#else
#define MAX_CLIENT_VIEWS	(MAX_CLIENT_COLS*MAX_CLIENT_ROWS)
#endif

#define FIRST_VIEW_ID		1
#define FIRST_SET_ID		1

#define MON_RES_X		800
#define MON_RES_Y		600

#define MAX_DIMENSION	10

#define WM_FILE_INFO	(WM_USER+10)


#define WALL_VIEWS_FILE_NAME		_T("wall_views.ini")

typedef struct tagDataFileSendInfo
{
	LARGE_INTEGER	FileSize;
	LARGE_INTEGER	SendedBytes;
	LONG			FileCount;
} DataFileSendInfo, *PDataFileSendInfo; 

#define MIME_TYPES_FILE	_T("mime.types")

#define WWW_PORT	10080

#define APPWEB_WWW_PORT	8888

#define UDP_TASK_START_MASTER_SYNC_PORT		20001
//#define UDP_TASK_START_SLAVE_SYNC_PORT	20002
#define UDP_TASK_START_SYNC_MASTER_MESSAGE	_T("MasterSync1")
#define UDP_TASK_START_SYNC_SLAVE_MESSAGE	_T("SlaveSync1")

#define	REALM_A	"Presenter"
#define	REALM	_T(REALM_A)

// CaptureVideoSizes
static struct 
{ 
	TCHAR *psz_size; 
	int  i_width; 
	int  i_height;
} size_table[] =
{ 
	{ _T("Auto"), 0, 0 },
	{ _T("subqcif"), 128, 96 }, 
	{ _T("qsif"), 160, 120 }, 
	{ _T("qcif"), 176, 144 },
	{ _T("sif"), 320, 240 }, 
	{ _T("cif"), 352, 288 }, 
	{ _T("d1"), 640, 480 },
	{ _T("720 x 480"), 720, 480 },
	{ _T("1920 x 1080"), 1920, 1080 },
	{ 0, 0, 0 },
};

#define MOVE_CONTROL_TO_CONTROL(_ORIG_, _TO_) { \
	CRect rect; \
	_TO_.GetWindowRect(rect); \
	ScreenToClient(&rect); \
	_ORIG_.SetWindowPos( &CWnd::wndTop, \
		rect.left, rect.top, 0, 0, \
		SWP_NOACTIVATE | SWP_NOSIZE ); \
	}


//#define USE_PRVLC	1
#define PRVLC_EXE_FILE_NAME			_T("PRVLC.exe")
//#define USE_VLC	1
#define VLC_EXE_FILE_NAME			_T("vlc.exe")
#define USE_MPC	1
#define MPC_EXE_FILE_NAME			_T("mpc-hc.exe")

#define PRVLC_EVENT_NAME_PREFIX		_T("PRVLC_EVENT_")
#define PRVLC_EVENT_NAME_POSTFIX	_T("XXXXXXXX")

#define PRVLC_MEDIA_NAME_OPTION		_T("--input-media")
#define PRVLC_MEDIA_LOOP_OPTION		_T("--loop-media")

#define CUSTOM_RESOLUTIONS_FILE_NAME	_T("custom_res.ini")

#endif // __PRESENTERGLOBALS_H__
