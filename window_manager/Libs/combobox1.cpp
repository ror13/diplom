// combobox1.cpp : implementation file
//

#include "stdafx.h"
#include "..\Presenter\presenter.h"
#include "combobox1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBox1

CComboBox1::CComboBox1()
{
}

CComboBox1::~CComboBox1()
{
}


BEGIN_MESSAGE_MAP(CComboBox1, CComboBox)
	//{{AFX_MSG_MAP(CComboBox1)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBox1 message handlers

int CComboBox1::AddString(LPCTSTR lpszString, unsigned long Data)
{
	int ind = CComboBox::AddString(lpszString);
	SetItemData(ind, Data);
	return ind;
}

int CComboBox1::GetCurSelData()
{
	int CurSel = GetCurSel();
	int Data = GetItemData(CurSel);
	return Data;
}

int CComboBox1::SetCurSelData(unsigned long Data)
{
	for(int i = 0; i < GetCount(); i++)
	{
		if(Data == GetItemData(i))
		{
			return SetCurSel(i);
		}
	}
	return -1;
}
