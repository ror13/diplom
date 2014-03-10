// SyncConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "pnetclient.h"
#include "SyncConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyncConfigDialog dialog


CSyncConfigDialog::CSyncConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSyncConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyncConfigDialog)
	m_iSyncMode = -1;
	//}}AFX_DATA_INIT
}


void CSyncConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncConfigDialog)
	DDX_Radio(pDX, IDC_SYNC_NONE_RADIO, m_iSyncMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyncConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CSyncConfigDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncConfigDialog message handlers
