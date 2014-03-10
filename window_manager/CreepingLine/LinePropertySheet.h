// LinePropertySheet.h : header file
//
// This class defines custom modal property sheet 
// CLinePropertySheet.
 
#ifndef __LINEPROPERTYSHEET_H__
#define __LINEPROPERTYSHEET_H__

#include "LinePropertyPageGeneral.h"
#include "LinePropertyPageFont.h"
#include "LinePropertyPageWindow.h"
#include "LinePropertyPagePosition.h"

#include "CreepingLineInfo.h"
#include "CreepingLineWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CLinePropertySheet

class CLinePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CLinePropertySheet)

// Construction
public:
	CLinePropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CLinePropertyPageGeneral	m_PageGeneral;
	CLinePropertyPageFont		m_PageFont;
	CLinePropertyPageWindow		m_PageWindow;
	CLinePropertyPagePosition	m_PagePosition;

	CCreepingLineInfo			m_line;

	CCreepingLineViewer			m_PreviewWnd;

	void Load();
	void Save();

	void OnPreview();
	void OnStopPreview();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinePropertySheet)
	public:
	virtual int DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLinePropertySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CLinePropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __LINEPROPERTYSHEET_H__
