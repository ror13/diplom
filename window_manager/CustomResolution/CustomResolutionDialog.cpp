// CustomResolutionDialog.cpp : implementation file
//

#include "stdafx.h"
#
//#include "..\presenter\presenter.h"
#include "CustomResolutionDialog.h"

#include "MonCustomRes.h"

#include "FileOperation.h"

#include "PresenterGlobals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditCustomResolutionDialog dialog


CEditCustomResolutionDialog::CEditCustomResolutionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEditCustomResolutionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditCustomResolutionDialog)
	m_dwHeight = 0;
	m_dwWidth = 0;
	//}}AFX_DATA_INIT
}


void CEditCustomResolutionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditCustomResolutionDialog)
	DDX_Text(pDX, IDC_CR_HEIGHT, m_dwHeight);
	DDV_MinMaxDWord(pDX, m_dwHeight, 1, 99999);
	DDX_Text(pDX, IDC_CR_WIDTH, m_dwWidth);
	DDV_MinMaxDWord(pDX, m_dwWidth, 1, 99999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditCustomResolutionDialog, CDialog)
	//{{AFX_MSG_MAP(CEditCustomResolutionDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCustomResolutionDialog message handlers
/////////////////////////////////////////////////////////////////////////////
// CCustomResolutionsDialog dialog


CCustomResolutionsDialog::CCustomResolutionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomResolutionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomResolutionsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCustomResolutionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomResolutionsDialog)
	DDX_Control(pDX, IDC_CR_DEL, m_wndBtCrDel);
	DDX_Control(pDX, IDC_CR_EDIT, m_wndBtCrEdit);
	DDX_Control(pDX, IDC_CUSTOM_RESOLUTIONS_LIST, m_wndCustomResolutionsList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomResolutionsDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomResolutionsDialog)
	ON_BN_CLICKED(IDC_CR_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CR_DEL, OnDel)
	ON_BN_CLICKED(IDC_CR_EDIT, OnEdit)
	ON_LBN_SELCHANGE(IDC_CUSTOM_RESOLUTIONS_LIST, OnSelchangeCustomResolutionsList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomResolutionsDialog message handlers

BOOL CCustomResolutionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strCustomResolutionList;

	m_CustomRes.LoadFromFile();

	UpdateControls();

	OnSelchangeCustomResolutionsList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCustomResolutionsDialog::UpdateControls()
{
	int CurSel = m_wndCustomResolutionsList.GetCurSel();
	m_wndCustomResolutionsList.ResetContent();

	for (int i = 0; i < m_CustomRes.Size(); i++)
	{
		m_wndCustomResolutionsList.AddString(m_CustomRes.GetStringFromIndex(i));
	}

	m_wndCustomResolutionsList.SetCurSel(CurSel);

	return TRUE;
}

void CCustomResolutionsDialog::OnAdd() 
{
	CEditCustomResolutionDialog EditCustomResolutionDialog;

	if (EditCustomResolutionDialog.DoModal() == IDOK)
	{
		m_CustomRes.Add(CCustomResolution(EditCustomResolutionDialog.m_dwWidth, EditCustomResolutionDialog.m_dwHeight, TRUE));

		UpdateControls();
	}
}

void CCustomResolutionsDialog::OnDel() 
{
	int CurSel = m_wndCustomResolutionsList.GetCurSel();

	if (CurSel == -1)
	{
		return;
	}
	

	CCustomResolution& CustomResolution = m_CustomRes.Get(CurSel);

	CString s;
	s.Format(
		_T("Are you sure to delete resolution %s (%d x %d)?\n"),
		CustomResolution.m_Txt, CustomResolution.m_W, CustomResolution.m_H);
	
	int res = AfxMessageBox( s, MB_OKCANCEL | MB_ICONWARNING );
	if( res == IDOK )
	{
		m_CustomRes.Remove(CustomResolution.m_W, CustomResolution.m_H);
		UpdateControls();
	}
}

void CCustomResolutionsDialog::OnEdit() 
{
	int CurSel = m_wndCustomResolutionsList.GetCurSel();

	if (CurSel == -1)
	{
		return;
	}
	
	CCustomResolution& CustomResolution = m_CustomRes.Get(CurSel);
	CCustomResolution CustomResolutionNew; 
	
	CEditCustomResolutionDialog EditCustomResolutionDialog;

	EditCustomResolutionDialog.m_dwWidth = CustomResolution.m_W;
	EditCustomResolutionDialog.m_dwHeight = CustomResolution.m_H;
	
	if (EditCustomResolutionDialog.DoModal() == IDOK)
	{
		m_CustomRes.Edit(CustomResolution, CCustomResolution(EditCustomResolutionDialog.m_dwWidth, EditCustomResolutionDialog.m_dwHeight, TRUE));
		
		UpdateControls();
	}
	
}

void CCustomResolutionsDialog::OnOK() 
{
	// TODO: Add extra validation here
	g_CustomRes.RemoveAll();

	for (int i=0; i < m_CustomRes.Size(); i++)
	{
		g_CustomRes.Add(m_CustomRes.Get(i));

		g_CustomRes.SaveToFile();
	}
	
	CDialog::OnOK();
}

int CUSTOMRESOLUTION_API CustomResolutionsDialogDoModal()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCustomResolutionsDialog dlg;
	return dlg.DoModal();
}


void CCustomResolutionsDialog::OnSelchangeCustomResolutionsList() 
{
	int CurSel = m_wndCustomResolutionsList.GetCurSel();

	m_wndBtCrEdit.EnableWindow(CurSel != -1);
	m_wndBtCrDel.EnableWindow(CurSel != -1);
}
