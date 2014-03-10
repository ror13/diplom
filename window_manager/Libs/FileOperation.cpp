// FileOpearation.cpp

#include "stdafx.h"
#include "FileOperation.h"

#include "..\Inc\CommonApi.h"

#include "ExecuteProcess.h"

#ifdef DEBUG_NEW
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#include <atlbase.h>
#include <atlconv.h>


#define UPLOAD_EXT _T(".upload85f67742ce76687d4cd576b9a5ce9f40")

LPTSTR g_replaceExeFolder = NULL;
LPTSTR g_replaceRootFolder = NULL;

/////////////////////////////////////////////////////////////////////////////

DWORD GetBinFileSize( LPCTSTR szFileName)
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return (DWORD)-1;

	DWORD size = GetFileSize( hFile, NULL );

	CloseHandle(hFile);
	return size;
}

LONGLONG GetBinFileSizeEx( LPCTSTR szFileName)
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return -1;

	LARGE_INTEGER size;
	size.QuadPart = 0;
	GetFileSizeEx( hFile, &size);

	CloseHandle(hFile);
	return size.QuadPart;
}

LONGLONG GetBinFileSizeEx(FILE* fl)
{
	fpos_t pos = 0;
	fpos_t fsize = 0;
	fgetpos(fl, &pos);
	fseek(fl, 0, SEEK_END);
	fgetpos(fl, &fsize);
	fsetpos(fl, &pos);
	return fsize;
}

DWORD LoadBinFileToBuf( LPCTSTR szFileName, PBYTE& pBuffer )
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return 0;

	DWORD size = GetFileSize( hFile, NULL );
	pBuffer = new BYTE[size+1];

	DWORD read_bytes = 0;
	ReadFile( hFile, pBuffer, size, &read_bytes, NULL );
	pBuffer[size] = 0;
	ASSERT( read_bytes == size );

	CloseHandle( hFile );

	return size;
}

/////////////////////////////////////////////////////////////////////////////

BOOL SaveBinFileFromBuf(LPCTSTR szFileName, PBYTE pBuffer, DWORD BufSize)
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;
	
	DWORD written = 0;
	WriteFile( hFile, pBuffer, BufSize, &written, NULL );
	
	CloseHandle( hFile );
	
	return written == BufSize;
}

/////////////////////////////////////////////////////////////////////////////

BOOL LoadAnsiFileToString( LPCTSTR szFileName, CString& strString )
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD size = GetFileSize( hFile, NULL ), read_bytes = 0;
	PBYTE pBuffer = new BYTE[size+1];

	ReadFile( hFile, pBuffer, size, &read_bytes, NULL );
	pBuffer[size] = 0;
	ASSERT( read_bytes == size );

	USES_CONVERSION;
	strString = A2CT( (LPCSTR)pBuffer );

	delete[] pBuffer;
	CloseHandle( hFile );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL SaveAnsiFileFromString( LPCTSTR szFileName, const CString& strString )
{
	BOOL res;
	DWORD attr = GetFileAttributes( szFileName );
	if( attr != 0xFFFFFFFF )
	{
		if( (attr&FILE_ATTRIBUTE_READONLY)!=0 )
		{
			SetFileAttributes( szFileName, (attr&(~FILE_ATTRIBUTE_READONLY)) );
		}
	}

	HANDLE hFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	res = hFile != INVALID_HANDLE_VALUE;
	if( !res ) return res;

	USES_CONVERSION;
	LPCSTR p = T2CA( (LPCTSTR)strString );
	DWORD written = 0;
	WriteFile( hFile, p, strlen(p), &written, NULL );

	CloseHandle( hFile );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL LoadTcharFileToString( LPCTSTR szFileName, CString& strString )
{
	BOOL res;
	strString.Empty();

	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, // | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	res = hFile != INVALID_HANDLE_VALUE;
	if( !res ) return res;
	DWORD size = GetFileSize( hFile, NULL ); //f.GetLength();

	LPTSTR buf = strString.GetBuffer( size+1 );
	DWORD read = 0;
	ReadFile( hFile, buf, size, &read, NULL );
	buf[size] = _T('\0');
	strString.ReleaseBuffer();
	CloseHandle( hFile );
	if( read != size ) return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL SaveTcharFileFromString( LPCTSTR szFileName, const CString& strString )
{
	BOOL res;
	DWORD attr = GetFileAttributes( szFileName );
	if( attr != 0xFFFFFFFF )
	{
		if( (attr&FILE_ATTRIBUTE_READONLY)!=0 )
		{
			SetFileAttributes( szFileName, (attr&(~FILE_ATTRIBUTE_READONLY)) );
		}
	}

	HANDLE hFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	res = hFile != INVALID_HANDLE_VALUE;
	if( !res ) return res;

	DWORD written = 0;
	WriteFile( hFile, (LPCTSTR)strString, strString.GetLength(), &written, NULL );
	CloseHandle( hFile );
	return TRUE;
}
//////////////////////////////////////////////////////////////

BOOL LoadUTF8FileToString( LPCTSTR szFileName, CString& strString )
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;
	
	DWORD size = GetFileSize( hFile, NULL ), read_bytes = 0;
	LPSTR pBuffer = new char[size+1];
	
	ReadFile( hFile, pBuffer, size, &read_bytes, NULL );
	pBuffer[size] = 0;
	
	if(read_bytes != size )
	{
		CloseHandle( hFile );
		delete[] pBuffer;
		return FALSE;
	}
	
	int wSize = MultiByteToWideChar(CP_UTF8, 0, pBuffer, size, NULL, 0);
	
	LPWSTR wszBuffer = new WCHAR[wSize+1];
	int Count = MultiByteToWideChar(CP_UTF8, 0, pBuffer, size, wszBuffer, wSize+1);
	wszBuffer[Count] = 0;
	
	USES_CONVERSION;
	strString = W2T(wszBuffer);
	
	delete[] wszBuffer;
	delete[] pBuffer;
	CloseHandle( hFile );
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////

BOOL SaveUTF8FileFromString(LPCTSTR szFileName, const CString &strString)
{
	USES_CONVERSION;
	int buf_size = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strString), -1,
		NULL, 0, NULL, NULL );
	
	LPSTR pBuf = new char[buf_size+1];
	
	DWORD size = 0;
	size = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strString), -1,
		pBuf, buf_size, NULL, NULL );
	
	if(!size)
	{
		delete[] pBuf;
		return FALSE;
	}
	
	BOOL res;
	DWORD attr = GetFileAttributes( szFileName );
	if( attr != 0xFFFFFFFF )
	{
		if( (attr&FILE_ATTRIBUTE_READONLY)!=0 )
		{
			SetFileAttributes( szFileName, (attr&(~FILE_ATTRIBUTE_READONLY)) );
		}
	}
	
	HANDLE hFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	res = hFile != INVALID_HANDLE_VALUE;
	if(!res)
	{
		delete[] pBuf;
		return res;
	}
	
	DWORD written = 0;
	res = WriteFile(hFile, pBuf, size - 1, &written, NULL);
	delete[] pBuf;
	
	CloseHandle( hFile );
	return res && written == (size-1);
}

CString UTF8ToString(LPCSTR strUTF8)
{
	CString strResult;

	if(strUTF8 == NULL)
		return strResult;

	int len = strlen(strUTF8);
	if(len)
	{
		USES_CONVERSION;
		LPWSTR wszBuffer = new WCHAR[len+1];
		int Count = MultiByteToWideChar(CP_UTF8, 0, strUTF8, len, wszBuffer, len);
		wszBuffer[Count] = 0;
		strResult = W2T(wszBuffer);
		delete []wszBuffer;
	}
	return strResult;
}

/////////////////////////////////////////////////////////////////////////////

CString GetExeFileName()
{
	TCHAR szExe[MAX_PATH] = _T("");
	GetModuleFileName( NULL, szExe, MAX_PATH );

	return szExe;
}

CString GetExeFolder(LPCTSTR szDirectory) // With trailing '\'
{
	TCHAR szExe[MAX_PATH] = _T("");
	GetModuleFileName( NULL, szExe, MAX_PATH );
	LPTSTR p = _tcsrchr( szExe, _T('\\') );
	if( p != NULL )
	{
		p[1] = _T('\0');
	}

	// if no szDirectory and set global new exe folder we use global
	// if set szDirectory we use it
	if (szDirectory == NULL &&
		g_replaceExeFolder != NULL)
	{
		szDirectory = g_replaceExeFolder;
	}

	CString folder = szExe;
	if (szDirectory)
	{
		folder = szDirectory;
		if (szDirectory[_tcslen(szDirectory)-1] != '\\')
		{
			folder += _T("\\");
		}
	}

	return folder;
}

/////////////////////////////////////////////////////////////////////////////

CString GetRootFolder(LPCTSTR szDirectory) // With trailing '\'
{
	TCHAR szExe[MAX_PATH] = _T("");
	GetModuleFileName( NULL, szExe, MAX_PATH );
	LPTSTR p = _tcsrchr( szExe, _T('\\') );
	if( p != NULL )
	{
		p[1] = _T('\0');
	}

	// if no szDirectory and set global new exe folder we use global
	// if set szDirectory we use it
	if (szDirectory == NULL &&
		g_replaceRootFolder != NULL)
	{
		szDirectory = g_replaceRootFolder;
	}

	CString folder = szExe;
	if (szDirectory)
	{
		folder = szDirectory;
		if (szDirectory[_tcslen(szDirectory)-1] != '\\')
		{
			folder += _T("\\");
		}
	}

	return folder;
}

/////////////////////////////////////////////////////////////////////////////

CString GetLine( CString& strString )
{
	CString s;
	int pos = strString.Find( _T('\n') );
	if( pos==-1 )
	{
		s = strString;
		strString.Empty();
	}
	else
	{
		s = strString.Left( pos );
		strString = strString.Mid( pos+1 );
	}

	s.Remove( _T('\r') );
	return s;
}

/////////////////////////////////////////////////////////////////////////////

BOOL IsValue( CString line, CString& name, CString& value )
{
	int pos = line.Find( _T('=') );
	if( pos == -1 )
		return FALSE;

	name = line.Left( pos );
	name.TrimLeft();
	name.TrimRight();
	value = line.Mid( pos + 1 );
	value.TrimLeft();
	value.TrimRight();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

TCHAR *g_PrefixForExternalLinks[] =
	{_T("http:"), _T("ftp:"), NULL};

BOOL IsExternalLink(LPCTSTR szFileName)
{
	int i = 0;
	while(g_PrefixForExternalLinks[i] != NULL)
	{
		TCHAR Buf[20];
		lstrcpyn(Buf, szFileName, lstrlen(g_PrefixForExternalLinks[i])+1);
		if(lstrcmpi(Buf, g_PrefixForExternalLinks[i]) == 0)
			return TRUE;
		i++;
	}
	return FALSE;
}

CString GetServerLocalFilePath(CString FileName, CString MD5Signature)
{
	CString sLocalFilePath = GetServerFilesDirectory() +
		GetLocalServerFileName(FileName, MD5Signature);

	return sLocalFilePath;
}

CString GetServerLocalTemplatePath(CString FileName, CString MD5Signature)
{
	CString sLocalFilePath = GetServerTemplatesFilesDirectory() +
		GetLocalServerFileName(FileName, MD5Signature);
	
	return sLocalFilePath;
}

CString GetSchedulesIniFilesDirectory(LPCTSTR szDirectory)
{
	CString sSchedulesIniFolder = GetRootFolder(szDirectory) + SCHEDULES_SUBDIR;
	CreateDirectory(sSchedulesIniFolder, NULL);
	return sSchedulesIniFolder;
}

CString GetTasksIniFilesDirectory(LPCTSTR szDirectory)
{
	CString sTasksIniFolder = GetRootFolder(szDirectory) + TASKS_SUBDIR;
	CreateDirectory(sTasksIniFolder, NULL);
	return sTasksIniFolder;
}

CString GetClientFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("media\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetClientDataDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("data\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetClientLogFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("client_logs\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetClientCameraReportsFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("client_camera_reports\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetClientTemplatesFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("client_templates\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerTemplatesFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("server_templates\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("serverfiles\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetTempDirectory()
{
	CString sMediaFolder = GetRootFolder(NULL) + _T("temp\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerSitePath(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("sites.ini");
	return sMediaFolder;
}

CString GetServerReportsDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("reports\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerCameraReportsDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("camera_reports\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerHTMLFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("HTML\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerPHPFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("HTML\\PHP\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

/*CString GetServerPHPTaskManagerFilesDirectory()
{
	CString sMediaFolder = GetServerPHPFilesDirectory() + _T("TM\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerPHPUserManagerFilesDirectory()
{
	CString sMediaFolder = GetServerPHPFilesDirectory() + _T("UM\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerPHPStationManagerFilesDirectory()
{
	CString sMediaFolder = GetServerPHPFilesDirectory() + _T("SM\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}*/

CString GetServerUploadFilesDirectory(LPCTSTR szDirectory)
{
	CString sMediaFolder = GetRootFolder(szDirectory) + _T("uploadfiles\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

CString GetServerMD5FileName(CString FileName, CString MD5Signature)
{
	CString strUploadFile = 
		GetServerFilesDirectory() + /*_T("\\") +*/
		GetLocalServerFileName(FileName, MD5Signature);

	return strUploadFile;
}

CString GetUploadMD5FileName(CString FileName, CString MD5Signature)
{
	CString strUploadFile = 
		GetServerUploadFilesDirectory() + /*_T("\\") +*/
		GetLocalServerFileName(FileName, MD5Signature);
	
	return strUploadFile;
}

BOOL IsUploadFileExists(CString FileName, CString MD5Signature)
{
	CString strUploadFile = 
		GetUploadMD5FileName(FileName, MD5Signature);
	
	return IsFileExists(strUploadFile);
}

BOOL IsServerFileExists(CString FileName, CString MD5Signature)
{
	CString strUploadFile = 
		GetServerMD5FileName(FileName, MD5Signature);
	
	return IsFileExists(strUploadFile);
}

CString GetServerTemplateFilePath(CString FileName, CString MD5Signature)
{
	CString strFile = GetServerTemplatesFilesDirectory() + GetLocalServerFileName(FileName, MD5Signature);
	if (IsFileExists(strFile))
		return strFile;
	return _T("");
}


CString GetLocalServerFileName(LPCTSTR szMediaFileName, LPCTSTR szMediaMD5)
{
	return szMediaMD5 + GetFileExtension(szMediaFileName);
}

/////////////////////////////////////////////////////////////////////////////

BOOL IsFileExists( LPCTSTR szFileName )
{
	DWORD attr = GetFileAttributes( szFileName );
	TRACE(_T("IsFileExists(%s) %d\n"), szFileName, attr != 0xFFFFFFFF && !( attr&FILE_ATTRIBUTE_DIRECTORY));
	return attr != 0xFFFFFFFF && !( attr&FILE_ATTRIBUTE_DIRECTORY );
}

BOOL IsDirectoryExists( LPCTSTR szFileName )
{
	DWORD attr = GetFileAttributes( szFileName );
	return attr != 0xFFFFFFFF && ( attr&FILE_ATTRIBUTE_DIRECTORY );
}

BOOL GetFileModificationTime(LPCTSTR szFileName, LPFILETIME mTime)
{
	BOOL ret;

	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;
	
	ret = GetFileTime(hFile, NULL, NULL, mTime);

	CloseHandle(hFile);
	return ret;
}


CString GetUploadFileName(LPCTSTR FileName)
{
	return GetFileFolder(FileName) + _T("\\") + GetFileNameOnly(FileName) + UPLOAD_EXT;
}

/////////////////////////////////////////////////////////////////////////////

CString GetFileDriveLetter( LPCTSTR szFile ) // Without trailing '\'
{
	CString s = szFile;
	int pos = s.Find( _T('\\') );
	if( pos != -1 )
		return s.Left( pos + 1 );
	else
		return _T("");
}

/////////////////////////////////////////////////////////////////////////////

CString GetFileFolder( LPCTSTR szFile ) // Without trailing '\'
{
	CString s = szFile;
	int pos1 = s.ReverseFind( _T('\\') );
	int pos2 = s.ReverseFind( _T('/') );
	int pos = max(pos1, pos2);
	if( pos != -1 )
		return s.Left( pos );
	else
		return _T("");
}

/////////////////////////////////////////////////////////////////////////////

CString GetFileName( LPCTSTR szFile )
{
	CString s = szFile;
	int pos = s.ReverseFind( _T('\\') );
	return s.Mid(pos + 1);
}

CString GetFileNameOnly( LPCTSTR szFile )
{
	CString s = GetFileName(szFile);
	int pos = s.ReverseFind( _T('.') );
	return pos == -1 ? s : s.Mid(0, pos);
}

/////////////////////////////////////////////////////////////////////////////

CString GetFileExtension(LPCTSTR szFile)
{
	CString sExt = szFile;
	int pos = sExt.ReverseFind(_T('.'));
	sExt = pos == -1 ? _T("") : sExt.Mid(pos);
	return sExt;
}

/////////////////////////////////////////////////////////////////////////////
CString PathJoin(LPCTSTR szFile1, LPCTSTR szFile2)
{
	CString s1 = szFile1;
	CString s2 = szFile2;

	if (s2 == ".")
		return s1;
	if (s1 == ".")
		return s2;
	
	if (s1.GetLength() && (s1[s1.GetLength()-1] == _T('\\') || s1[s1.GetLength()-1] == _T('/')))
	{
		s1 = s1.Left(s1.GetLength()-1);
	}
	return s1 + _T("\\") + s2;
}


//////////////////////////////////////////////////////////////////////////

LPSTR StringToUTF8(CString strString)
{
	USES_CONVERSION;
	int buf_size = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strString), -1,
		NULL, 0, NULL, NULL );;
	
	LPSTR pBuf = new char[buf_size+1];
	
	DWORD size = 0;
	size = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strString), -1,
		pBuf, buf_size, NULL, NULL );
	
	if(!size)
	{
		delete[] pBuf;
		return NULL;
	}

	return pBuf;
}


CUTF8String::CUTF8String(CString str)
{
	m_pStrUTF8 = StringToUTF8(str);
}

CUTF8String::~CUTF8String()
{
	delete[] m_pStrUTF8;
}

CUTF8String::operator LPSTR()
{
	return m_pStrUTF8;
}

LONG RemoveDirectoryWithItsContent(CString Directory)
{
	LONG DeletedFiles = 0;
	{
		CFileFind finder;
		
		while (Directory.Right(1) == _T("\\"))
		{
			Directory.Delete(Directory.ReverseFind(_T('\\')));
		}

		BOOL bWorking = finder.FindFile(Directory + _T("\\*.*"));
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			CString filename = finder.GetFileName();

			if (finder.IsDirectory())
			{
				if (filename != _T(".") &&
					filename != _T(".."))
				{
					DeletedFiles += RemoveDirectoryWithItsContent(Directory + _T("\\") + filename);
				}
			}
			else
			{
				DeleteFile(Directory + _T("\\") + filename);
				DeletedFiles++;
			}
		}
	}

	RemoveDirectory(Directory);

	return DeletedFiles + 1;
}

CString GetLocalMediaPath(CString FileName, CString FileMD5, LPCTSTR TaskFileName, LPCTSTR szDirectory)
{
	return GetMediaFilesDirectory(TaskFileName, szDirectory) + GetLocalMediaFileName(FileName, FileMD5);
}

CString GetAdditionalLocalMediaPath(CString FileName, LPCTSTR TaskFileName, LPCTSTR szDirectory)
{
	return GetMediaFilesDirectory(TaskFileName, szDirectory) + GetFileName(FileName);
}

BOOL IsMediaFilePresent(CString FileName, CString FileMD5, CString TaskFileName, LPCTSTR szDirectory)
{
	CString sLocalMediaPath = GetLocalMediaPath(FileName, FileMD5, TaskFileName, szDirectory);
	return IsFileExists(sLocalMediaPath);
}

BOOL IsAdditionalMediaFilePresent(CString FileName, CString TaskFileName, LPCTSTR szDirectory)
{
	CString sLocalMediaPath = GetAdditionalLocalMediaPath(FileName, TaskFileName, szDirectory);
	return IsFileExists(sLocalMediaPath);
}

//////////////////////////////////////////////////////////////////////////
// global client function

CString GetMediaFilesDirectory(LPCTSTR TaskFileName, LPCTSTR szDirectory)
{
	
	CString sMediaFolder = GetClientFilesDirectory(szDirectory);
	if(TaskFileName)
		sMediaFolder += GetFileNameOnly(TaskFileName) + _T("\\");
	CreateDirectory(sMediaFolder, NULL);
	return sMediaFolder;
}

// CString GetAdditionalMediaFilesDirectory(LPCTSTR TaskFileName)
// {
// 	CString sMediaFolder = GetMediaFilesDirectory() + GetFileNameOnly(TaskFileName) +
// 		_T("\\");
// 	CreateDirectory(sMediaFolder, NULL);
// 	return sMediaFolder;
// }

CString GetLocalMediaFileName(LPCTSTR szMediaFileName, LPCTSTR szMediaMD5)
{
	
	return szMediaMD5 + GetFileExtension(szMediaFileName);
}

BOOL IsPreviewerExist()
{
	CString strExeFolder;
	BOOL res = GetPathForPreviewer(strExeFolder);

	return res;
}

BOOL GetPathForPreviewer(CString &PreviewerPath)
{
	PreviewerPath = GetExeFolder();
	CString PlayerExecutable = PRESENTER_PREVIEWER;
	
	BOOL res = GetPathForProgram(PlayerExecutable, PreviewerPath);

	return res;
}

BOOL GetPathForProgram(CString ProgramName, CString &ProgramPath)
{
	
#define APP_PATHS_KEY	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\")
	
	TRACE( _T("GetPathForProgram."));
	BOOL resval = FALSE;
	
	CString ExePath;
	BOOL res = regReadRegString(REG_HKLM, CString(APP_PATHS_KEY + ProgramName), _T("Path"),
		ExePath.GetBuffer(MAX_PATH), MAX_PATH);
	ExePath.ReleaseBuffer();
	
	resval = res;
	if(res == FALSE)
	{
		// if we not found exe file in registry we try to find it in our exe directory
		if (IsFileExists(GetExeFolder() + ProgramName))
		{
			ExePath = GetExeFolder();
			res = TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	ExePath.TrimRight(_T(";\\ "));
	ExePath += _T("\\");
	
	ProgramPath = ExePath;
	
	TRACE( _T("GetPathForProgram: %s, result = %i \n"),ProgramPath, resval);
	return TRUE;
}

BOOL GetExecutableProgramForFile(CString DocumentName, CString &ProgramPath)
{
	
	TRACE( _T("GetExecutableProgramForFile."));
	CString Executable;
	
	HINSTANCE res;
	
	res = FindExecutable( DocumentName, _T(""), 
		Executable.GetBuffer(MAX_PATH));
	Executable.ReleaseBuffer();
	
	if((DWORD)(res) <= 32)
	{
		return FALSE;
	}
	
	ProgramPath = Executable;
	
	TRACE( _T("GetExecutableProgramForFile: %s open by %s \n"),DocumentName, ProgramPath);
	return TRUE;
}


BOOL GetMediaFileByHash(const CString &mediaFolder, const CString &mediaHash, CString &filePath)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(mediaFolder + _T("\\") + mediaHash + _T(".*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString filename = finder.GetFileName();
		if(GetFileExtension(filename) == UPLOAD_EXT)
			continue;
		
		filePath = finder.GetFilePath();
		return TRUE;
	}
	return FALSE;
}

BOOL GetMediaFileByHashSkipMd5(const CString &mediaFolder, const CString &mediaHash, CString &filePath)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(mediaFolder + _T("\\") + mediaHash + _T(".*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString filename = finder.GetFileName();
		if(GetFileExtension(filename) == _T(".md5"))
			continue;
		if(GetFileExtension(filename) == UPLOAD_EXT)
			continue;
		
		filePath = finder.GetFilePath();
		return TRUE;
	}
	return FALSE;
}

BOOL AdjustSystemShutdownPrivileges()
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	CloseHandle(hToken);

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	return TRUE;
}

BOOL EnablePrivilege(IN LPTSTR lpName)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, lpName, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	CloseHandle(hToken);

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	return TRUE;
}

BOOL WindowsSystemShutdown()
{
	DWORD dwShutdownReason =
		SHTDN_REASON_MAJOR_APPLICATION |
		SHTDN_REASON_MINOR_OTHER |
		SHTDN_REASON_FLAG_PLANNED;

	AdjustSystemShutdownPrivileges();
	
	return ExitWindowsEx( EWX_POWEROFF, dwShutdownReason );
}

BOOL WindowsSystemRestart()
{
	DWORD dwShutdownReason =
		SHTDN_REASON_MAJOR_APPLICATION |
		SHTDN_REASON_MINOR_OTHER |
		SHTDN_REASON_FLAG_PLANNED;
	
	AdjustSystemShutdownPrivileges();
	
	return ExitWindowsEx( EWX_REBOOT, dwShutdownReason );
}
