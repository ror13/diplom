// ErrorMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Product.h"
#include "resource.h"
#include "ErrorMessageDlg.h"
#include "../Libs/Translate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FONT_FAMILY		_T("MS Sans Serif")

CDisplayError g_DisplayError;


CDisplayError::CDisplayError()
{
	m_Thread.SetName(_T("CDisplayError"));
	m_Thread.Create(this);
}

CDisplayError::~CDisplayError()
{
	Stop();
}

void CDisplayError::AddDlg(ERROR_MESSAGE *pMsg)
{
	CSingleLock lock(&m_CritSect);
	lock.Lock();
	m_Array.Add(pMsg);
	lock.Unlock();

	m_Thread.SetEvent();
}

void CDisplayError::Stop()
{
	m_Thread.WaitStopThread(2000);

	CSingleLock lock(&m_CritSect);
	lock.Lock();

	for(int i = 0; i < m_Array.GetSize(); i++)
		delete m_Array[i];
	m_Array.RemoveAll();

	lock.Unlock();
}

UINT CDisplayError::OnThread(int ThreadNum)
{
	CSingleLock lock(&m_CritSect);

	while(!m_Thread.IsStopped())
	{
		if(m_Thread.WaitEvent(1000) == -1)
			break;

		lock.Lock();
		while(m_Array.GetSize())
		{
			ERROR_MESSAGE *pMsg = (ERROR_MESSAGE*)m_Array[0];
			m_Array.RemoveAt(0);
			

			CErrorDlgThread *pThread = (CErrorDlgThread*)AfxBeginThread(RUNTIME_CLASS(CErrorDlgThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			pThread->SetMsg(pMsg);
			pThread->m_bAutoDelete = FALSE;

			pThread->ResumeThread();

			lock.Unlock();

			DWORD ExitCode = 0;
			do
			{
				if(m_Thread.WaitEvent(100) == -1)
					break;

				GetExitCodeThread(*pThread, &ExitCode);
			} while(ExitCode == STILL_ACTIVE);

			if(ExitCode == STILL_ACTIVE)
			{
				pThread->m_pMainWnd->PostMessage(WM_CLOSE, 0, 0);
				WaitForSingleObject(*pThread, 10000);
			}

			lock.Lock();

			delete pThread;

			delete pMsg;

			if(m_Thread.IsStopped())
				break;
		}
		lock.Unlock();
	}

	return 0;
}

//////////////////////////////////////////////////////////////

int DisplayErrorText (BOOL bWaitDone, CWnd* pParent, DWORD dwFlags, DWORD dwTimeOut,
					LPCTSTR szCaption, LPCTSTR szText )
{
	ERROR_MESSAGE *pMsg = new ERROR_MESSAGE();

	pMsg->m_pParent = pParent;
	pMsg->m_dwFlags = dwFlags;
	pMsg->m_dwTimeOut = dwTimeOut;
	pMsg->m_strCaption = szCaption == NULL ? _T(PRODUCT_NAME) : szCaption;
	pMsg->m_strText = szText;

	{
		CString s;
		s.Format( _T("DisplayErrorText: \"%s\" - %d - \"%s\"\n"),
			pMsg->m_strCaption, pMsg->m_dwTimeOut, pMsg->m_strText );
		OutputDebugString( s );
	}
	
	if(bWaitDone)
	{
		CErrorMessageDlg dlg(pMsg);
		
		dlg.DoModal();

		delete pMsg;
	} else
		g_DisplayError.AddDlg(pMsg);

	return IDOK;
}

//////////////////////////////////////////////////////////////

int DisplayError ( HWND hWndParent, DWORD dwFlags, DWORD dwTimeOut,
					LPCTSTR szCaption, LPCTSTR szTextFmt, ... )
{
	CString s;
	{
		va_list marker;
		va_start( marker, szTextFmt );
		s.FormatV( szTextFmt, marker );
		va_end( marker );
	}

	CWnd* pWnd = hWndParent == NULL ? NULL : CWnd::FromHandle( hWndParent );

	int res = DisplayErrorText(FALSE, pWnd, dwFlags, dwTimeOut, szCaption, s );
	return res;
}

int DisplayError1 ( HWND hWndParent, DWORD dwFlags, DWORD dwTimeOut,
					LPCTSTR szCaption, LPCTSTR szTextFmt, ... )
{
	CString s;
	{
		va_list marker;
		va_start( marker, szTextFmt );
		s.FormatV( szTextFmt, marker );
		va_end( marker );
	}

	CWnd* pWnd = hWndParent == NULL ? NULL : CWnd::FromHandle( hWndParent );

	int res = DisplayErrorText(TRUE, pWnd, dwFlags, dwTimeOut, szCaption, s );
	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CErrorMessageDlg dialog


CErrorMessageDlg::CErrorMessageDlg(ERROR_MESSAGE *pMsg)
	: CDialog(CErrorMessageDlg::IDD, pMsg->m_pParent)
{
	//{{AFX_DATA_INIT(CErrorMessageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ERROR_MESSAGE);

	m_Msg = *pMsg;

	m_bAutoStop = FALSE;
	m_dwTimerDelta = 1000;
}


void CErrorMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorMessageDlg)
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_REMIND_TIME_STATIC, m_wndRemind);
	DDX_Control(pDX, IDC_TEXT_START, m_wndText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrorMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CErrorMessageDlg)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrorMessageDlg message handlers

BOOL CErrorMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowText( m_Msg.m_strCaption );
	m_wndText.SetWindowText( m_Msg.m_strText );

	ASSERT( m_Msg.m_dwFlags == 0 );

	m_bAutoStop = m_Msg.m_dwTimeOut != DEFAULT_DISPLAY_FOREVER;
	m_dwRemained = m_Msg.m_dwTimeOut;

	if( m_bAutoStop )
	{
		SetTimer( 1, m_dwTimerDelta, NULL );
	}

	LPCTSTR szFont = FONT_FAMILY;
	int PointSize = 8;

	CDC* pDC = GetWindowDC();
	int nHeight = -MulDiv(PointSize, GetDeviceCaps(*pDC, LOGPIXELSY), 72);
	ReleaseDC( pDC );
	pDC = NULL;

	m_font.CreateFont( nHeight, 0,
		0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, szFont );

	UpdateStatus();
	Resize();

	SetForegroundWindow();

	FlashWindow(TRUE); // invert the title bar 
//	Sleep(5000);              // wait a bit 
//	FlashWindow(TRUE); // invert again 

	MessageBeep( MB_ICONHAND ); // m_dwFlags

	// Shaman with a buben: not to allow maximize,
	// when CreateProcess(MAXIMIZE) & this is a first window:
	ShowWindow( SW_HIDE );
	CenterWindow();
	ShowWindow( SW_SHOWNORMAL );	// TWICE!!! see. STARTUPINFO.wShowWindow in MSDN!
	ShowWindow( SW_SHOWNORMAL );	// TWICE!!! see. STARTUPINFO.wShowWindow in MSDN!
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CErrorMessageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CErrorMessageDlg::OnClose() 
{
	CDialog::OnClose();
}

void CErrorMessageDlg::OnDestroy() 
{
	m_font.DeleteObject();
	CDialog::OnDestroy();
}

void CErrorMessageDlg::OnOK() 
{
	CDialog::OnOK();
}

void CErrorMessageDlg::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL CErrorMessageDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

void CErrorMessageDlg::OnTimer(UINT nIDEvent) 
{
	if( m_dwRemained <= m_dwTimerDelta )
	{
		EndDialog( IDOK );
	}
	else
	{
		m_dwRemained -= m_dwTimerDelta;
		UpdateStatus();
	}

	//CDialog::OnTimer(nIDEvent);
}

void CErrorMessageDlg::UpdateStatus()
{
	CString s;
	if( m_bAutoStop )
	{
		s.Format( _TRANS(_T("%d seconds before close")),
			m_dwRemained/1000 );
	}
	m_wndRemind.SetWindowText( s );
}

void CErrorMessageDlg::Resize()
{
	CRect rTextOrig, rTextNew;
	m_wndText.GetWindowRect( &rTextOrig );
	ScreenToClient( &rTextOrig );
	CDC* pDC = GetWindowDC();
	if( pDC == NULL )
		return;

	rTextNew = rTextOrig;
	int len = m_Msg.m_strText.GetLength();

	pDC->SetBkMode( TRANSPARENT );
	CFont* pOldFont = pDC->SelectObject( &m_font );

	DrawText( *pDC,
		m_Msg.m_strText,
		len, &rTextNew,
		DT_CALCRECT | DT_EXPANDTABS | DT_LEFT | DT_TOP );

	pDC->SelectObject( pOldFont );
	ReleaseDC( pDC );
	pDC = NULL;

	int dx = rTextNew.Width() - rTextOrig.Width();
	int dy = rTextNew.Height() - rTextOrig.Height();

	if( dx < 0 )
		dx = 0;
	if( dy < 0 )
		dy = 0;

	CRect r;

	m_wndText.GetWindowRect( &r );
	ScreenToClient( &r );
	r.right += dx;
	r.bottom += dy;
	m_wndText.MoveWindow( &r, TRUE );

	m_wndRemind.GetWindowRect( &r );
	ScreenToClient( &r );
	r.right += dx;
	r.top += dy;
	r.bottom += dy;
	m_wndRemind.MoveWindow( &r, TRUE );

	m_wndOK.GetWindowRect( &r );
	ScreenToClient( &r );
	r.left += dx/2;
	r.right += dx/2;
	r.top += dy;
	r.bottom += dy;
	m_wndOK.MoveWindow( &r, TRUE );

	m_wndCancel.GetWindowRect( &r );
	ScreenToClient( &r );
	r.left += dx/2;
	r.right += dx/2;
	r.top += dy;
	r.bottom += dy;
	m_wndCancel.MoveWindow( &r, TRUE );

	GetWindowRect( &r );
	r.right += dx;
	r.bottom += dy;
	MoveWindow( &r, TRUE );
}

void CErrorMessageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		return;
	}

	CRect rTextNew;
	m_wndText.GetWindowRect( &rTextNew );
	ScreenToClient( &rTextNew );
	dc.SetBkMode( TRANSPARENT );
	CFont* pOldFont = dc.SelectObject( &m_font );

	dc.DrawText( m_Msg.m_strText, &rTextNew, DT_EXPANDTABS | DT_LEFT | DT_TOP );

	dc.SelectObject( pOldFont );
}

IMPLEMENT_DYNCREATE(CErrorDlgThread, CWinThread)

void CErrorDlgThread::SetMsg(ERROR_MESSAGE *pMsg)
{
	m_pMsg = pMsg;
}

CErrorDlgThread::~CErrorDlgThread()
{
}

BOOL CErrorDlgThread::InitInstance()
{
	CErrorMessageDlg dlg(m_pMsg);
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}
