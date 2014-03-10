#include "StdAfx.h"
#include "PServiceData.h"
#include "AppWebServer.h"
#include "appweb/php5Handler.h"

#include "Utils.h"

#include "FileOperation.h"
#include "PresenterGlobals.h"

#include "PresenterUser.h"
#include "PresenterUserArray.h"

MaUploadHandlerService *uploadHandlerService;

CAppWebServer::CAppWebServer(CPServiceData *pServiceData):
	m_mpr("PService2WebServer")
{
	m_pServiceData = pServiceData;
	
/*	m_TaskManagerDir = NULL;
	m_UserManagerDir = NULL;
	m_StationManagerDir = NULL;
	m_bIsUsersUpdated = FALSE;*/
	
	m_Thread.SetName(_T("CAppWebServer"));
	m_Thread.Create(this, 0);

	// Create PHPFiles, ServerFiles, GetServerUploadFiles Directories
	/*CreateServerPHPFilesDirectory();
	CreateServerFilesDirectory();
	CreateServerUploadFilesDirectory();*/

	// To correct reading of mime.types files we set CWD - running folder
	SetCurrentDirectory(GetExeFolder());
	
#if BLD_FEATURE_LOG
	m_mpr.addListener(new MprLogToFile());
	m_mpr.setLogSpec("http_error.log:9");
#endif 

	//
	//	Start the Mbedthis Portable Runtime with 10 pool threads
	// 
	m_mpr.start(MPR_SERVICE_THREAD);
	
#if BLD_FEATURE_MULTITHREAD
	m_mpr.poolService->setMaxPoolThreads(10);
#endif 
	
	//
	//	Create Http and Server objects for this application. We set the
	//	ServerName to be "default" and the initial serverRoot to be ".".
	//	This will be overridden in simpleServer.conf.
	//
	m_http = new MaHttp();
	if (configure(m_http) < 0) {
		mprFprintf(MPR_STDERR, "Can't configure the server\n");
		//exit(2);
		return;
	}

	// set to update users for dirs
	/*m_bIsUsersUpdated = TRUE;*/
 	
	//
	//	Start the server
	//
	if (m_http->start() < 0) {
		mprFprintf(MPR_STDERR, "Can't start the server\n");
		//exit(2);
		return;
	}
}

CAppWebServer::~CAppWebServer()
{
	//
	//	Orderly shutdown
	//
	m_http->stop();
	if (m_server)
	{
		delete m_server;
	}
	delete m_http;
	
	//
	//	MPR run-time will automatically stop and be cleaned up
	// 
}

int CAppWebServer::configure(MaHttp *http)
{
	MaHost			*host;
	MaDir			*dir;
//	MaAlias			*ap;
//	MaLocation		*loc;
	char			*documentRoot;
//	char			pathBuf[MPR_MAX_FNAME];
	char			serverAddr[MPR_MAX_FNAME];

	CString			strRoot;
	char			*Root;

	CString			strMimeTypes;
	char			*MimeTypes;

	/*CString			strTaskManagerDir;
	char			*TaskManagerDir;
	
	CString			strUserManagerDir;
	char			*UserManagerDir;
	
	CString			strStationManagerDir;
	char			*StationManagerDir;*/
	
	strRoot = GetServerPHPFilesDirectory();
	Root = GetAnsiString(strRoot);

	documentRoot = Root;

	//
	// Set new limits
	//
	MaLimits *limits = http->getLimits();
	limits->maxBody = INT_MAX;

	//
	//	Create a server and set the server root directory to "."
	//
	m_server = new MaServer(http, "default");

	//
	//	Create a new host and define the document root
	//
	mprSprintf(serverAddr, sizeof(serverAddr), "%s:%d", "0.0.0.0", APPWEB_WWW_PORT);
	host = m_server->newHost(documentRoot, serverAddr);
	if (host == 0) {
		delete Root;
		return MPR_ERR_CANT_OPEN;
	}

	strMimeTypes= GetExeFolder() + MIME_TYPES_FILE;
	MimeTypes = GetAnsiString(strMimeTypes);
	host->openMimeTypes(MimeTypes);

	//
	//	Load modules that are to be statically loaded
	//
	new MaDirModule(0);
	new MaCopyModule(0);
	new MaAuthModule(0);
	new MaPutModule(0);
	new MaPhp5Module(0);

#if IF_YOU_WISH
	new MaEspModule(0);
	new MaUploadModule(0);
	new MaAdminModule(0);
	new MaCgiModule(0);
	new MaCompatModule(0);
	new MaCapiModule(0);
	new MaEgiModule(0);
	new MaSslModule(0);
	new MaMatrixSslModule(0);
	new MaOpenSslModule(0);
	new MaPhp4Module(0);
#endif

	//
	//	Now load the dynamically loadable modules and activate the statically
	//	linked modules from above. loadModule will do this for us.
	//

	if (m_server->loadModule("auth") == 0) {
		host->addHandler("authHandler", "");
	}
	
	if (m_server->loadModule("dir") == 0) {
		host->addHandler("dirHandler", "");
	}
	
	if (m_server->loadModule("put") == 0) {
		host->addHandler("putHandler", "");
	}
	
	if (m_server->loadModule("php5") == 0) {
		host->addHandler("php5Handler", ".php");
	}
	
#if IF_YOU_WISH
	//	
	//	This handler must be added first to authorize all requests.
	//
	if (m_server->loadModule("esp") == 0) {
		host->addHandler("espHandler", ".esp");
	}
	
	if (m_server->loadModule("upload") == 0) {
		host->addHandler("uploadHandler", "");
	}
	
	if (m_server->loadModule("cgi") == 0) {
		host->addHandler("cgiHandler", ".cgi .cgi-nph .bat .cmd .pl .py");
	}

	if (m_server->loadModule("egi") == 0) {
		host->addHandler("egiHandler", ".egi");
	}

	if (m_server->loadModule("esp") == 0) {
		host->addHandler("espHandler", ".esp .asp");
	}

	m_server->loadModule("ssl");
	m_server->loadModule("matrixSsl");
#endif

	// Must be the LAST module!!!
	if (m_server->loadModule("copy") == 0) {
		host->addHandler("copyHandler", "");
	}
	
	//
	//	Create the top level directory
	//
 	dir = new MaDir(host);
 	dir->setPath(documentRoot);
 	dir->setIndex("index.php");
 	host->insertDir(dir);

	CString strTmplDir = GetClientTemplatesFilesDirectory();
	strTmplDir.Replace(_T('\\'), _T('/'));
	char *tmplDir = GetAnsiString(strTmplDir);
	MaAlias *ap = new MaAlias("/templates/", tmplDir);
	host->insertAlias(ap);
	MaDir *tmpl = new MaDir(host);
	tmpl->setPath(tmplDir);
	host->insertDir(tmpl);
	delete tmplDir;

	//
	// Create Task Manager level directory
	//
	/*strTaskManagerDir = GetServerPHPTaskManagerFilesDirectory();
	TaskManagerDir = GetAnsiString(strTaskManagerDir);

	m_TaskManagerDir = new MaDir(host);
	m_TaskManagerDir->setPath(TaskManagerDir);
	m_TaskManagerDir->setIndex("index.php");
	m_TaskManagerDir->setType(MPR_HTTP_AUTH_BASIC);
	m_TaskManagerDir->setRealm(REALM_A);
	m_TaskManagerDir->setAnyValidUser();
	host->insertDir(m_TaskManagerDir);

	host->enableAuth();

	delete TaskManagerDir;

	//
	// Create User Manager level directory
	//
	strUserManagerDir = GetServerPHPUserManagerFilesDirectory();
	UserManagerDir = GetAnsiString(strUserManagerDir);
	
	delete UserManagerDir;

	//
	// Create Client Manager level directory
	//
	strStationManagerDir = GetServerPHPStationManagerFilesDirectory();
	StationManagerDir = GetAnsiString(strStationManagerDir);
	
	delete StationManagerDir;*/

	/*
	//
	//	Add cgi-bin with a location block for the /cgi-bin URL prefix.
	//
	mprSprintf(pathBuf, sizeof(pathBuf), "%s/cgi-bin", serverRoot);
	ap = new MaAlias("/cgi-bin/", pathBuf);
	mprLog(4, "ScriptAlias \"/cgi-bin/\":\n\t\t\t\"%s\"\n", pathBuf);
	host->insertAlias(ap);
	loc = new MaLocation(dir->getAuth());
	loc->setPrefix("/cgi-bin/");
	loc->setHandler("cgiHandler");
	host->insertLocation(loc);
	*/

	/*uploadHandlerService = 	(MaUploadHandlerService*)http->lookupHandlerService("uploadHandler");

	if (uploadHandlerService != 0) {
		strUploadDir = GetRootFolder() + WEB_SERVER_UPLOAD_FILES_DIRECTORY;
		UploadDir = GetAnsiString(strUploadDir);
		uploadHandlerService->setUploadDir(UploadDir);
		delete UploadDir;
	}
	else
	{
		mprError(MPR_L, MPR_LOG, "Can't find handler %s", "uploadHandler"); 
		//return MPR_ERR_NOT_FOUND;
	}*/


	delete Root;
	delete MimeTypes;

	return 0; 
}

/*
void CAppWebServer::SetUsersChanged()
{
	m_bIsUsersUpdated = TRUE;
}

BOOL CAppWebServer::UpdateUsers()
{
	mprLog(9, "UpdateUsers:\n");
	if (m_pServiceData)
	{
		CPresenterUserArray* pPresenterUserArray = m_pServiceData->GetUserArray();

		if (pPresenterUserArray)
		{
			m_bIsUsersUpdated = FALSE;
			
			// reset all users
			if (m_TaskManagerDir)
				m_TaskManagerDir->reset();
// 			if (m_UserManagerDir)
// 				m_UserManagerDir->reset();
// 			if (m_StationManagerDir)
// 				m_StationManagerDir->reset();

			for (int i = 0; i < pPresenterUserArray->GetUserCount(); i++)
			{
				CPresenterUser User;
				pPresenterUserArray->GetUser(i, User);

				CString strbuf;
				char *buf;

				strbuf.Format(_T("%s:%s:%s"), User.m_UserName, REALM, User.m_Password);

				buf = GetAnsiString(strbuf);

				char	*encodedPassword;

				encodedPassword = maMD5(buf);

				char	*userName, *realm;

				userName = GetAnsiString(User.m_UserName);
				realm = GetAnsiString(REALM);

				mprLog(9, "UpdateUsers: buf = %s [%s] (%s)\n", buf, realm, encodedPassword);

				if (m_TaskManagerDir && User.IsRightExists(PRESENTER_USERS_TASK_MANAGER_RIGHT))
				{
					m_TaskManagerDir->addUser(userName, realm, encodedPassword, TRUE);
				}
// 				if (m_UserManagerDir && User.IsRightExists(PRESENTER_USERS_USER_MANAGER_RIGHT))
// 				{
// 					m_UserManagerDir->addUser(userName, realm, encodedPassword, TRUE);
// 				}
// 				if (m_StationManagerDir && User.IsRightExists(PRESENTER_USERS_STATION_MANAGER_RIGHT))
// 				{
// 					m_StationManagerDir->addUser(userName, realm, encodedPassword, TRUE);
// 				}

				if (encodedPassword) {
					mprFree(encodedPassword);
				}

				if (buf)
					delete buf;
				if (userName)
					delete userName;
				if (realm)
					delete realm;
			}
		}
	}

	return TRUE;
}
*/

UINT CAppWebServer::OnThread(int ThreadNum)
{
	while(!m_Thread.IsStopped())
	{
		if(m_Thread.WaitEvent() == CThread::WAIT_STATUS_ON_EVENT)
		{
			if (m_pServiceData)
			{
				//
				//	Tell the MPR to loop servicing incoming requests. We can 
				//	replace this call with a variety of event servicing 
				//	mechanisms offered by AppWeb.
				//
				m_mpr.serviceEvents(0, -1);
 			}
		}
	}
	
	return 0;
}
