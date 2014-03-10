#if !defined(AFX_DRAGSOURCESTATIC_H__7B0841D1_82C0_4486_9DE0_CEE0FEADC507__INCLUDED_)
#define AFX_DRAGSOURCESTATIC_H__7B0841D1_82C0_4486_9DE0_CEE0FEADC507__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dragsourcestatic.h : header file
//

struct STATIC_OBJ_DESC
{
	int		m_Param;
	SIZE	m_Size;
	POINT	m_Offset;
};

/////////////////////////////////////////////////////////////////////////////
// CDragSourceStatic window

class CDragSourceStatic : public CStatic
{
// Construction
public:
	CDragSourceStatic();

// Attributes
public:
	UINT			m_ClipFormat;
	int				m_Param;

	COleDataSource	m_DataSrc;

// Operations
public:
	void SetData(UINT ClipFormat, int Param);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragSourceStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragSourceStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragSourceStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGSOURCESTATIC_H__7B0841D1_82C0_4486_9DE0_CEE0FEADC507__INCLUDED_)
