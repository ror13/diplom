#pragma once

//Arrow drawing is ported from the FLATGUI version in Delphi.
//You are free to use and modify this as long as you don`t claim it .

class CToolTipCtrlEx : public CToolTipCtrl
{
	DECLARE_DYNAMIC(CToolTipCtrlEx)

public:
	CToolTipCtrlEx();
	virtual ~CToolTipCtrlEx();
	
	void RelayEvent(LPMSG lpMsg);
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
private:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
public:
	COLORREF	m_bkColor;
	COLORREF	m_leftColor;
	COLORREF	m_frameColor;
	COLORREF	m_textColor;
};
