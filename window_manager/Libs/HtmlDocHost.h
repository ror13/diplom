//////////////////////////////////////////////////////////////
// File:		// HtmlDocHost.h
// File time:	// 3.05.2006	15:28
// Description: //////////////////////////////////////////////
// 
//

#ifndef _HTMLDOCHOST_H_UID00354dgf0838B20740
#define _HTMLDOCHOST_H_UID00354dgf0838B20740

//////////////////////////////////////////////////////////////

class CHtmlDocHost
{
public:
	CHtmlDocHost (void);
	virtual ~CHtmlDocHost (void);

	BOOL EnableControlContainer(
		IN	BOOL	b3DBorder = FALSE,
		IN	BOOL	bScroll = FALSE,
		IN	BOOL	bTheme = TRUE
		);
	BOOL DisableControlContainer();

protected:

	class CDocHostOccManager *		m_pOccManager;
	class CHtml_Host_Handlers *		m_pHandlers;
	COccManager *					m_pOldOccManager;

private:

};

//////////////////////////////////////////////////////////////

#endif //ifndef _HTMLDOCHOST_H_UID00354dgf0838B20740
