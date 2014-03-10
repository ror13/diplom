// CreepingLine.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CreepingLine.h"

#include "MainFrm.h"
#include "CreepingLineDoc.h"
#include "CreepingLineView.h"

#include "../Libs/FileOperation.h"
#include "../Libs/Translate.h"
#include "LinePropertySheet.h"
//#include "../CommonLib/CommonLibExt.h"

#include "CommandLineParser.h"

#include "RunTests.h"

#define NO_SPECIALIZED_PROTECTION
#include "..\Inc\CommonApi.h"

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineApp

BEGIN_MESSAGE_MAP(CCreepingLineApp, CWinApp)
	//{{AFX_MSG_MAP(CCreepingLineApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineApp construction

CCreepingLineApp::CCreepingLineApp()
{
	m_bRunCreepingLine = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCreepingLineApp object

CCreepingLineApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CCreepingLineApp initialization

BOOL CCreepingLineApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("CreepingLine"));

	LoadStdProfileSettings(12);  // Load standard INI file options (including MRU)

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineParser cmdInfo;
	ParseCommandLine(cmdInfo);

#ifdef _DEBUG
	if(cmdInfo.m_bTestMode)
	{
		RunTests();
		return FALSE;
	}
#endif

	BOOL bOpenFile = cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen; // Edit mode

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo )
	{
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
		m_bRunCreepingLine = TRUE;
	}

	if( !m_bRunCreepingLine )
	{
		// Register document templates

		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CCreepingLineDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CCreepingLineView));
		AddDocTemplate(pDocTemplate);
	}

	// Enable DDE Execute open
//	EnableShellOpen();
//	RegisterShellFileTypes(TRUE);
	CString strExe = GetExeFolder() + _T("CreepingLine.exe");
	CString strOpenCmd;
	CString strEditCmd;
	CString strIconCmd;
	strOpenCmd.Format( _T("\"%s\" /pt \"%%1\""), strExe );
	strEditCmd.Format( _T("\"%s\" \"%%1\""), strExe );
	strIconCmd.Format( _T("\"%s\",1"), strExe );

	regWriteRegString( REG_HKCR, _T(".clf"), NULL, _T("CreepingLine.Document") );
	regWriteRegString( REG_HKCR, _T("CreepingLine.Document"),
		NULL, _T("Creeping Line Document") );
	regWriteRegString( REG_HKCR, _T("CreepingLine.Document\\DefaultIcon"),
		NULL, strIconCmd );
	regWriteRegString( REG_HKCR, _T("CreepingLine.Document\\shell\\open\\command"),
		NULL, strOpenCmd );
	regWriteRegString( REG_HKCR, _T("CreepingLine.Document\\shell\\edit\\command"),
		NULL, strEditCmd );

/*
HKEY_CLASSES_ROOT\.clf = CreepingLine.Document
HKEY_CLASSES_ROOT\CreepingLine.Document\DefaultIcon = CREEPINGLINE.EXE, 1
HKEY_CLASSES_ROOT\CreepingLine.Document\shell\open\command = CREEPINGLINE.EXE pt %1
HKEY_CLASSES_ROOT\CreepingLine.Document\shell\edit\command = CREEPINGLINE.EXE %1
HKEY_CLASSES_ROOT\CreepingLine.Document = Creeping Line Document
*/

	if( m_bRunCreepingLine )
	{
		//AfxMessageBox2( _T("Running creeping line...") );
		CCreepingLineInfo	line;
		line.InitDefault();

		BOOL bLoaded = line.Load( cmdInfo.m_strFileName );
		if( !bLoaded )
		{
			MessageBox(NULL,_T("Can't open file"), _T("Ok"), MB_OK);
			return FALSE;
		}


		if(cmdInfo.m_bSetRect)
			m_CrippingLineViewer.BeginCreepingLine(line, TRUE, cmdInfo.m_FullViewRect);
		else
			m_CrippingLineViewer.BeginCreepingLine(line, TRUE);

		m_CrippingLineViewer.WaitForCreepingLineEnd();

		return FALSE;

//		m_CrippingLine.ShowWindow(SW_SHOW);
//		m_CrippingLine.UpdateWindow();
//		m_pMainWnd = &m_CrippingLine;
	}

	if( bOpenFile )
	{
		CLinePropertySheet propSheet;

		propSheet.m_line.InitDefault();
		BOOL bLoaded = propSheet.m_line.Load(cmdInfo.m_strFileName);
		if( !bLoaded )
		{
			MessageBox(NULL,_T("Can't open file"), _T("Ok"), MB_OK);
			return FALSE;
		}

		int res = propSheet.DoModal();
		if( res == IDOK )
		{
			propSheet.m_line.Save(cmdInfo.m_strFileName);
		}

		return FALSE;
	}

	// Dispatch commands specified on the command line
	if( !m_bRunCreepingLine && !bOpenFile )
	{
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;

		m_pMainWnd->SetWindowPos( NULL,
			0, 0,		// position
			500, 200,	// size
			SWP_NOZORDER | SWP_NOMOVE );

		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
	}

	if( !m_bRunCreepingLine )
	{
		// Enable drag/drop open
		m_pMainWnd->DragAcceptFiles();
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCreepingLineApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineApp message handlers

