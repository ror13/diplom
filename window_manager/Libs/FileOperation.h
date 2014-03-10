#if !defined(FileOperation__INCLUDED_)
#define FileOperation__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


extern LPTSTR g_replaceExeFolder;
extern LPTSTR g_replaceRootFolder;

CString GetServerLocalFilePath(CString FileName, CString MD5Signature);
CString GetServerLocalTemplatePath(CString FileName, CString MD5Signature);
CString GetClientFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetClientDataDirectory(LPCTSTR szDirectory = NULL);
CString GetClientLogFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetClientCameraReportsFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetClientTemplatesFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetServerTemplatesFilesDirectory(LPCTSTR szDirectory = NULL);
#define CreateServerFilesDirectory			GetServerFilesDirectory
CString GetServerFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetServerReportsDirectory(LPCTSTR szDirectory = NULL);
CString GetServerCameraReportsDirectory(LPCTSTR szDirectory = NULL);
CString GetServerHTMLFilesDirectory(LPCTSTR szDirectory = NULL);
#define CreateServerPHPFilesDirectory		GetServerPHPFilesDirectory
CString GetServerPHPFilesDirectory(LPCTSTR szDirectory = NULL);
/*#define CreateServerPHPTaskManagerFilesDirectory	GetServerPHPTaskManagerFilesDirectory
CString GetServerPHPTaskManagerFilesDirectory();
#define CreateServerPHPUserManagerFilesDirectory	GetServerPHPUserManagerFilesDirectory
CString GetServerPHPUserManagerFilesDirectory();
#define CreateServerPHPStationManagerFilesDirectory	GetServerPHPStationManagerFilesDirectory
CString GetServerPHPStationManagerFilesDirectory();*/
#define CreateServerUploadFilesDirectory	GetServerUploadFilesDirectory
CString GetServerUploadFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetTempDirectory();
CString GetLocalServerFileName(LPCTSTR szMediaFileName, LPCTSTR szMediaMD5);
BOOL IsUploadFileExists(CString FileName, CString MD5Signature);
BOOL IsServerFileExists(CString FileName, CString MD5Signature);
CString GetServerTemplateFilePath(CString FileName, CString MD5Signature);
CString GetServerMD5FileName(CString FileName, CString MD5Signature);
CString GetUploadMD5FileName(CString FileName, CString MD5Signature);

DWORD GetBinFileSize( LPCTSTR szFileName);
LONGLONG GetBinFileSizeEx( LPCTSTR szFileName);
LONGLONG GetBinFileSizeEx( FILE* fl);

DWORD LoadBinFileToBuf( LPCTSTR szFileName, PBYTE& pBuffer );
BOOL SaveBinFileFromBuf(LPCTSTR szFileName, PBYTE pBuffer, DWORD BufSize);

BOOL LoadAnsiFileToString( LPCTSTR szFileName, CString& strString );
BOOL SaveAnsiFileFromString( LPCTSTR szFileName, const CString& strString );

BOOL LoadTcharFileToString( LPCTSTR szFileName, CString& strString );
BOOL SaveTcharFileFromString( LPCTSTR szFileName, const CString& strString );

BOOL LoadUTF8FileToString( LPCTSTR szFileName, CString& strString );
BOOL SaveUTF8FileFromString(LPCTSTR szFileName, const CString &strString);

CString UTF8ToString(LPCSTR strUTF8);

CString GetSchedulesIniFilesDirectory(LPCTSTR szDirectory = NULL);
CString GetTasksIniFilesDirectory(LPCTSTR szDirectory = NULL);

CString GetExeFileName();
CString GetExeFolder(LPCTSTR szDirectory = NULL); // With trailing '\'
CString GetRootFolder(LPCTSTR szDirectory = NULL); // With trailing '\'
CString GetLine( CString& strString );
BOOL IsValue( CString line, CString& name, CString& value );

BOOL IsExternalLink(LPCTSTR szFileName);
BOOL IsFileExists( LPCTSTR szFileName );
BOOL IsDirectoryExists( LPCTSTR szFileName );

CString GetUploadFileName(LPCTSTR FileName);

CString GetFileDriveLetter( LPCTSTR szFile ); // Without trailing '\'
CString GetFileFolder( LPCTSTR szFile ); // Without trailing '\'
CString GetFileName( LPCTSTR szFile );
CString GetFileNameOnly( LPCTSTR szFile);
CString GetFileExtension(LPCTSTR szFile);

BOOL GetFileModificationTime(LPCTSTR szFileName, LPFILETIME mTime);
CString PathJoin(LPCTSTR szFile1, LPCTSTR szFile2);

LPSTR StringToUTF8(CString strString);
CString UTF8ToString(LPCSTR strUTF8);


// class for conversion CString (ansi/unicode) to UTF8 string
// and automatic control allocated buf for string
class CUTF8String
{
public:
	CUTF8String(CString str);
	~CUTF8String();

	operator LPSTR();

protected:
	LPSTR m_pStrUTF8;
};

LONG RemoveDirectoryWithItsContent(CString Directory);

CString GetMediaFilesDirectory(LPCTSTR TaskFileName, LPCTSTR szDirectory = NULL);
CString GetLocalMediaFileName(LPCTSTR szMediaFileName, LPCTSTR szMediaMD5);

CString GetLocalMediaPath(CString FileName, CString FileMD5, LPCTSTR TaskFileName, LPCTSTR szDirectory = NULL);
CString GetAdditionalLocalMediaPath(CString FileName, LPCTSTR TaskFileName, LPCTSTR szDirectory = NULL);
BOOL IsMediaFilePresent(CString FileName, CString FileMD5, CString TaskFileName, LPCTSTR szDirectory = NULL);
BOOL IsAdditionalMediaFilePresent(CString FileName, CString TaskFileName, LPCTSTR szDirectory = NULL);

BOOL IsPreviewerExist();
BOOL GetPathForPreviewer(CString &PreviewerPath);
BOOL GetPathForProgram(CString ProgramName, CString &ProgramPath);
BOOL GetExecutableProgramForFile(CString DocumentName, CString &ProgramPath);
BOOL GetMediaFileByHash(const CString &mediaFolder, const CString &mediaHash, CString &filePath);
BOOL GetMediaFileByHashSkipMd5(const CString &mediaFolder, const CString &mediaHash, CString &filePath);


CString GetServerSitePath(LPCTSTR szDirectory = NULL);

BOOL AdjustSystemShutdownPrivileges();

BOOL EnablePrivilege(IN LPTSTR lpName);

BOOL WindowsSystemShutdown();
BOOL WindowsSystemRestart();

                                      
/////////////////////////////////////////////////////////////

#define TASKS_SUBDIR	_T("tasks\\")
#define TASK_EXT		_T("ini")

/////////////////////////////////////////////////////////////

#define SCHEDULES_SUBDIR	_T("schedules\\")
#define SCHEDULE_EXT		_T("ini")

/////////////////////////////////////////////////////////////

#define CLIENT_TASKS_FILE_NAME			_T("client_tasks.ini")
#define CLIENT_SCHEDULES_FILE_NAME		_T("client_schedules.ini")

/////////////////////////////////////////////////////////////



#endif // !defined(FileOperation__INCLUDED_)
