#if !defined(AFX_ERRORMESSAGEDLG_H__85559AEC_B4FB_4F58_889E_F13CE7288E81__INCLUDED_)
#define AFX_ERRORMESSAGEDLG_H__85559AEC_B4FB_4F58_889E_F13CE7288E81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrorMessageDlg.h : header file
//

#include "resource.h"

#include "thread.h"

struct ERROR_MESSAGE
{
	CWnd	*m_pParent;
	DWORD	m_dwFlags;
	DWORD	m_dwTimeOut; // milliseconds
	CString	m_strCaption;
	CString	m_strText;

	ERROR_MESSAGE()
	{
		m_dwFlags = 0;
		m_dwTimeOut = 0;
	}
};

class CDisplayError : public CThreadClient
{
public:
	CDisplayError();
	~CDisplayError();

	void AddDlg(ERROR_MESSAGE *pMsg);
	void Stop();

protected:
	CCriticalSection	m_CritSect;

	CArray<ERROR_MESSAGE*, ERROR_MESSAGE*>	m_Array;

	CThread				m_Thread;

	virtual UINT OnThread(int ThreadNum);
};

/////////////////////////////////////////////////////////////////////////////
// CErrorMessageDlg dialog

class CErrorMessageDlg : public CDialog
{
// Construction
public:
	CErrorMessageDlg(ERROR_MESSAGE *pMsg);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CErrorMessageDlg)
	enum { IDD = IDD_DISPLAY_ERROR_DIALOG };
	CButton	m_wndCancel;
	CButton	m_wndOK;
	CStatic	m_wndRemind;
	CStatic	m_wndText;
	//}}AFX_DATA

	ERROR_MESSAGE	m_Msg;
//	DWORD	m_dwFlags;
//	DWORD	m_dwTimeOut; // milliseconds
//	CString	m_strCaption;
//	CString	m_strText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorMessageDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	BOOL	m_bAutoStop;
	DWORD	m_dwTimerDelta;
	DWORD	m_dwRemained; // milliseconds

	CFont	m_font;

	void Resize();
	void UpdateStatus();

	// Generated message map functions
	//{{AFX_MSG(CErrorMessageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	//}}AFX_MSG
	virtual HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
};


class CErrorDlgThread : public CWinThread
{
protected:

	virtual BOOL InitInstance();

	ERROR_MESSAGE *m_pMsg;

public:
	~CErrorDlgThread();

	void SetMsg(ERROR_MESSAGE *pMsg);
	
	DECLARE_DYNCREATE(CErrorDlgThread)
};



int DisplayError ( HWND hWndParent, DWORD dwFlags, DWORD dwTimeOut,
					LPCTSTR szCaption, LPCTSTR szTextFmt, ... );
int DisplayError1( HWND hWndParent, DWORD dwFlags, DWORD dwTimeOut,
					LPCTSTR szCaption, LPCTSTR szTextFmt, ... );

#define		DEFAULT_DISPLAY_LENGTH		15000
#define		DEFAULT_DISPLAY_FOREVER		0

inline int MessageBox4( HWND hWndParent, LPCTSTR szText, LPCTSTR szCaption, DWORD dwFlags )
{
	int res = DisplayError( hWndParent, 0, DEFAULT_DISPLAY_LENGTH,
		szCaption, _T("%s"), szText );
	return res;
}

inline int MessageBox5( LPCTSTR szText )
{
	int res = DisplayError1( NULL, 0, DEFAULT_DISPLAY_LENGTH,
		NULL, _T("%s"), szText );
	return res;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRORMESSAGEDLG_H__85559AEC_B4FB_4F58_889E_F13CE7288E81__INCLUDED_)
