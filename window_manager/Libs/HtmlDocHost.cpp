//////////////////////////////////////////////////////////////
// File:		// HtmlDocHost.cpp
// File time:	// 3.05.2006	15:28
// Description: //////////////////////////////////////////////
// 
//

#include "stdafx.h"
//#include <windows.h>
#include "HtmlDocHost.h"

//////////////////////////////////////////////////////////////

#include "../Libs/Html_Host_Handlers.h"

#pragma pack(push)
#pragma pack(8)

#if _MFC_VER >= 0x0700
#include <afxocc.h>
#else
#include <..\src\occimpl.h>
#endif

#pragma pack(push)

#include "../Libs/DocHostSite.h"

//////////////////////////////////////////////////////////////

CHtmlDocHost::CHtmlDocHost (void)
{
	m_pOccManager = NULL;
	m_pHandlers = NULL;
	m_pOldOccManager = NULL;
}

CHtmlDocHost::~CHtmlDocHost (void)
{
	DisableControlContainer();
	delete m_pHandlers;
	delete m_pOccManager;
}

//////////////////////////////////////////////////////////////

BOOL CHtmlDocHost::EnableControlContainer(
		IN	BOOL	b3DBorder,
		IN	BOOL	bScroll,
		IN	BOOL	bTheme
		)
{
	if( m_pHandlers == NULL )
	{
		m_pHandlers = new CHtml_Host_Handlers(
			b3DBorder, bScroll, bTheme );
	}

	if( m_pOccManager == NULL )
	{
		m_pOccManager = new CDocHostOccManager;
		ASSERT(m_pOccManager);
		if (!m_pOccManager)
			return FALSE;

		m_pOccManager->SetView( m_pHandlers );
	}

	DisableControlContainer();

	m_pOldOccManager = AfxGetModuleState()->m_pOccManager;
	AfxEnableControlContainer( m_pOccManager );

	return TRUE;
}

//////////////////////////////////////////////////////////////

BOOL CHtmlDocHost::DisableControlContainer()
{
	if( m_pOldOccManager != NULL )
	{
		AfxGetModuleState()->m_pOccManager = m_pOldOccManager;
		m_pOldOccManager = NULL;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
