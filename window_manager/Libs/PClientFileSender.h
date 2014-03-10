// PClientFileSender.h: interface for the CPClientFileSender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCLIENTFILESENDER_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
#define AFX_PCLIENTFILESENDER_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Request.h"
#include "RequestListBuf.h"


#include "FileOperation.h"
#include <map>
#include "LinkPoint.h"
#include "CritSect.h"
#include "Thread.h"
#include "OptionsMap.h"


class CPClientFileSender : public CThreadClient
{
	friend class CFileSenderTest;
public:
	//CPClientFileSender(CLinkPoint *pLink, BOOL UseLoPriority = TRUE, BOOL bIsInteractiveInfo = FALSE);
	CPClientFileSender(CLinkPoint *pLink, BOOL UseLoPriority = TRUE, CWnd *InteractiveWnd = NULL);
	virtual ~CPClientFileSender();

	BOOL StopUploadFiles();

	BOOL checkingRequestValide(CRequest &req);
	void addFile(const CString& hash, FILE * pTmpFileDiff);
	void addFile(const CString& hash, const CString& path);

	BOOL AddGetFileRequest(CRequest &req);
	void RemoveGetFileRequest(CRequest &req);
	
	void SetStop();
	BOOL IsWorking();
	
protected:
	CCritSect		m_CritSect;
	CLinkPoint		*m_pLink;

	//BOOL			m_bIsInteractiveInfo;
	CWnd			*m_InteractiveWnd;

	CRequestListBuf	m_GetFileRequestBuf;	
	CRequestListBuf	m_RemFileRequestBuf;

	BOOL			m_bSkipFile;
	CString			m_FileNameForSkip;

	CThread			m_Thread;

	BOOL			m_UseLoPriority;

	volatile BOOL			m_bSending;
	
	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	CCritSect m_UploadMediaMapCritSect;

	std::map< CString, std::pair<CString, LONGLONG> > m_UploadMediaMap;
	std::map< CString, std::pair<FILE *, LONGLONG> > m_UploadMediaMapFile;

	void closeFile(FILE * pFile, const CString& strHash);
	FILE * createFileToSend(const CString & strHash, const CString& strFilename);
	
	BOOL ReadFileBuf(FILE * hFile, DWORD DataSize, COptionsMap &Map, CRequest &Req);
	BOOL ReadFileBuf(HANDLE hFile, DWORD DataSize, COptionsMap &Map, CRequest &Req);
	BOOL IsSkipedFile(CString FileName);
};

#endif // !defined(AFX_PCLIENTFILESENDER_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
