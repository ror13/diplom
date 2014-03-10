//////////////////////////////////////////////////////////////
// File:		// CreepingLineInfo.h
// File time:	// 27.05.2005	14:36
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _CREEPINGLINEINFO_H_UID00000039156C0387
#define _CREEPINGLINEINFO_H_UID00000039156C0387

#include "../Libs/SerializeStruct.h"

enum eCREEPING_LINE_TEXT_LAYOUT
{
	CREEPING_LINE_TEXT_LAYOUT_LEFT_TO_RIGHT,
	CREEPING_LINE_TEXT_LAYOUT_RIGHT_TO_LEFT,
};

class CCreepingLineInfo: public CSerializeStruct
{
public:

	enum DOCKING_POS
	{
		DOCKING_POS_NONE, DOCKING_POS_TOP, DOCKING_POS_BOTTOM
	};

	CCreepingLineInfo (void);
	virtual ~CCreepingLineInfo (void);

	// General:
	BOOL		m_bUseRSS;
	CString		m_strText;
	double		m_ScrollingSpeed;	// pixels per second
	BOOL		m_bResizing;

	// Font:
	LOGFONT		m_font;
	COLORREF	m_clTextColor;
	LONG		m_clTextSize;		// in pt

	// Window:
	COLORREF	m_clBackground;
	BOOL		m_bTransparent;
	COLORREF	m_clTransparentColor;
	BYTE		m_TransparencyFactor;
	CRect		m_Rect;
	int			m_Position;

	// Direction
	int			m_iTextLayout;

	BOOL operator== (const CCreepingLineInfo& a);
	
	void InitDefault();

protected:
	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );

private:
};

#endif //ifndef _CREEPINGLINEINFO_H_UID00000039156C0387
