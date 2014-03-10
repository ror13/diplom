// CreepingLineDoc.cpp : implementation of the CCreepingLineDoc class
//

#include "stdafx.h"
#include "CreepingLine.h"

#include "CreepingLineDoc.h"
#include "LinePropertySheet.h"
//#include "../CommonLib/CommonLibExt.h"
#include "../Libs/Translate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineDoc

IMPLEMENT_DYNCREATE(CCreepingLineDoc, CDocument)

BEGIN_MESSAGE_MAP(CCreepingLineDoc, CDocument)
	//{{AFX_MSG_MAP(CCreepingLineDoc)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_RUN_CREEPING_LINE, OnRunCreepingLine)
	ON_COMMAND(ID_STOP_CREEPING_LINE, OnStopCreepingLine)
	ON_UPDATE_COMMAND_UI(ID_RUN_CREEPING_LINE, OnUpdateRunCreepingLine)
	ON_UPDATE_COMMAND_UI(ID_STOP_CREEPING_LINE, OnUpdateStopCreepingLine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineDoc construction/destruction

CCreepingLineDoc::CCreepingLineDoc()
{
	m_rTextSize = CRect(0,0,0,0);
}

CCreepingLineDoc::~CCreepingLineDoc()
{
	OnStopCreepingLine();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCreepingLineDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_line.InitDefault();
	m_line_orig = m_line;
	SetTitle( m_line.m_strText );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCreepingLineDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	m_line.InitDefault();
	BOOL bLoaded = m_line.Load( lpszPathName );
	if( !bLoaded )
	{
		MessageBox(NULL,_T("Can't open file"), _T("Ok"), MB_OK);
		return FALSE;
	}

	m_line_orig = m_line;

	// due to error into MFC code during SetTitle of line more than _MAX_PATH we reduce it to _MAX_PATH
	CString smallTitle = m_line.m_strText;
	int smallTitleLength = smallTitle.GetLength();
	if (smallTitleLength > _MAX_PATH)
	{
		smallTitle.Delete(_MAX_PATH, smallTitleLength-_MAX_PATH);
	}
	SetTitle( smallTitle );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCreepingLineDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_line.Save(lpszPathName);
	m_line_orig = m_line;
	SetModifiedFlag( FALSE );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineDoc serialization

void CCreepingLineDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineDoc diagnostics

#ifdef _DEBUG
void CCreepingLineDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCreepingLineDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineDoc commands

void CCreepingLineDoc::OnOptions() 
{
	CLinePropertySheet propSheet;

	propSheet.m_line = m_line;

	int res = propSheet.DoModal();
	if( res == IDOK )
	{
		m_line = propSheet.m_line;

		BOOL bChanged = !(m_line_orig == m_line);
		this->SetModifiedFlag( bChanged );

		UpdateAllViews( NULL );

		if(m_CrippingLinePreview.IsPlay())
		{
			OnStopCreepingLine();
			OnRunCreepingLine();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineDoc::OnRunCreepingLine() 
{
	if(!m_CrippingLinePreview.IsPlay())
		m_CrippingLinePreview.BeginCreepingLine(m_line);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineDoc::OnStopCreepingLine() 
{
	if( m_CrippingLinePreview.IsPlay())
	{
		m_CrippingLinePreview.StopCreepingLine();
	}
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineDoc::OnUpdateRunCreepingLine(CCmdUI* pCmdUI) 
{
	BOOL bLineExists = m_CrippingLinePreview.IsPlay();
	pCmdUI->Enable( !bLineExists );
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineDoc::OnUpdateStopCreepingLine(CCmdUI* pCmdUI) 
{
	BOOL bLineExists = m_CrippingLinePreview.IsPlay();
	pCmdUI->Enable( bLineExists );
}

/////////////////////////////////////////////////////////////////////////////
