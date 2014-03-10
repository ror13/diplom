// ApplicaServer.h
// Stuff common for ApplicaServer product
// It should not conflict with all other Applica products.

// Load this file only once
//#pragma once

#ifndef _APPLICASERVER_H_
#define _APPLICASERVER_H_

// define for Unicode formatting
#ifndef _L
#define __L(x) L ## x
#define _L(x) __L(x)
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

//#include "types.h"


#define APPLICA_SERVER_GUID_A    "{ED70158B-B24A-4d93-A125-EE9E144B811A}"
#define APPLICA_SERVER_GUID_T _T(APPLICA_SERVER_GUID_A)
#define APPLICA_SERVER_GUID_L _L(APPLICA_SERVER_GUID_A)
#if APPLICA_PRODUCT==PRODUCT_APPLICASERVER
#define APPLICA_SERVER_USERGROUP_A	"Applica Server Users"
#else
#if APPLICA_PRODUCT==PRODUCT_ZSERVER
#define APPLICA_SERVER_USERGROUP_A	"ZServer Users"
#endif
#endif

#define APPLICA_SERVER_USERGROUP_T	_T(APPLICA_SERVER_USERGROUP_A)
#define APPLICA_SERVER_USERGROUP_L	_L(APPLICA_SERVER_USERGROUP_A)

// ***** Base registry key: *************************************************************

#ifndef HKLM_CONTROL_REG_KEY_
#define HKLM_CONTROL_REG_KEY_	"SYSTEM\\CurrentControlSet\\Control\\"
#endif

#define HKLM_APPLICASERVER_INFO_REG_KEY_T _T(HKLM_CONTROL_REG_KEY_) APPLICA_SERVER_GUID_T
#define HKLM_APPLICASERVER_INFO_REG_KEY_L _L(HKLM_CONTROL_REG_KEY_) APPLICA_SERVER_GUID_L


// Just some warning

//#if !(APPLICA_PRODUCT==PRODUCT_APPLICASERVER)
//#pragma message (WARNING "APPLICA_PRODUCT <> PRODUCT_APPLICASERVER")
//#endif

#define APPLICASERVER_IPSTATION_SETUP_L			HKLM_APPLICASERVER_INFO_REG_KEY_L L"\\Setup"
#define APPLICASERVER_STATION_INFORMATION_FMT_A	"IP_%03d.%03d.%03d.%03d"
#define APPLICASERVER_STATION_INFORMATION_FMT_T	_T(APPLICASERVER_STATION_INFORMATION_FMT_A)
#define APPLICASERVER_STATION_INFORMATION_FMT_L	_L(APPLICASERVER_STATION_INFORMATION_FMT_A)


#define APPLICASERVER_AUTOLOGON_VALUE_A			"AutoLogon"
#define APPLICASERVER_DOMAIN_VALUE_A			"Default Domain"
#define APPLICASERVER_LOGIN_VALUE_A				"Default User"
#define APPLICASERVER_PASSWORD_VALUE_A			"Default Password"
#define APPLICASERVER_INITIAL_PROGRAM_VALUE_A	"Initial Program"
#define APPLICASERVER_USE_INITIAL_VALUE_A		"UseInitial"
#define APPLICASERVER_OLD_REMOTE_USERS_A		"OldRemoteUsers"
#define APPLICASERVER_AUTOLOGON_VALUE_T			_T(APPLICASERVER_AUTOLOGON_VALUE_A)
#define APPLICASERVER_DOMAIN_VALUE_T			_T(APPLICASERVER_DOMAIN_VALUE_A)
#define APPLICASERVER_LOGIN_VALUE_T				_T(APPLICASERVER_LOGIN_VALUE_A)
#define APPLICASERVER_PASSWORD_VALUE_T			_T(APPLICASERVER_PASSWORD_VALUE_A)
#define APPLICASERVER_INITIAL_PROGRAM_VALUE_T	_T(APPLICASERVER_INITIAL_PROGRAM_VALUE_A)
#define APPLICASERVER_USE_INITIAL_VALUE_T		_T(APPLICASERVER_USE_INITIAL_VALUE_A)
#define APPLICASERVER_OLD_REMOTE_USERS_T		_T(APPLICASERVER_OLD_REMOTE_USERS_A)
#define APPLICASERVER_AUTOLOGON_VALUE_L			_L(APPLICASERVER_AUTOLOGON_VALUE_A)
#define APPLICASERVER_DOMAIN_VALUE_L			_L(APPLICASERVER_DOMAIN_VALUE_A)
#define APPLICASERVER_LOGIN_VALUE_L				_L(APPLICASERVER_LOGIN_VALUE_A)
#define APPLICASERVER_PASSWORD_VALUE_L			_L(APPLICASERVER_PASSWORD_VALUE_A)
#define APPLICASERVER_INITIAL_PROGRAM_VALUE_L	_L(APPLICASERVER_INITIAL_PROGRAM_VALUE_A)
#define APPLICASERVER_USE_INITIAL_VALUE_L		_L(APPLICASERVER_USE_INITIAL_VALUE_A)
#define APPLICASERVER_OLD_REMOTE_USERS_L		_L(APPLICASERVER_OLD_REMOTE_USERS_A)


// **** Something from Applica.h **********************************************
#ifndef REG_BASE_REG_VALUE_T
#define REG_BASE_REG_VALUE_T	_T("ProductBaseRegKey")
#endif
#ifndef REG_BASE_FOLDER_VALUE_T
#define REG_BASE_FOLDER_VALUE_T	_T("ProductBaseFolder")
#endif
#ifndef REG_PRODUCT_NAME_VALUE_T
#define REG_PRODUCT_NAME_VALUE_T	_T("ProductName")
#endif
// ****************************************************************************

// ****** Texts ***************************************************************

#if APPLICA_PRODUCT==PRODUCT_APPLICASERVER
// --- main MMC ------------ ApplicaIpMMC -------------------------------------
#define CLIENT_MMC_CTRL_ADD_TO_GROUP_TEXT "Add to ""Applica Server Users"" group (Allow logon using RDP)"
#define CLIENT_MMC_CTRL_SERVER_CONFIG_TEXT "Applica Server configuration"
#define CLIENT_MMC_NAME_CONFIGURATION "ApplicaServer Configuration"
#define CLIENT_MMC_NAME_CONSOLE "ApplicaServer Management Console"
#define CLIENT_MMC_NAME_CLIENT "ApplicaServer Client"
#define CLIENT_MMC_NAME_FULL_VERSION "ApplicaServer Management Console v1.1"
// --- logon MMC ----------- LogonMMC -----------------------------------------
#define LOGON_MMC_CONFIG_DIALOG "Applica Server Autologon Configuration Dialog"
#define LOGON_MMC_CONFIGURATION "ApplicaServer Configuration"
#define LOGON_MMC_NAME_CONSOLE "ApplicaServer Autologon Console"
#define LOGON_MMC_NAME_CLIENT "ApplicaServer Autologon"
#define LOGON_MMC_NAME_FULL_VERSION "ApplicaServer Autologon Console v1.0a"
// --- Patch Driver -------- HLPK Driver---------------------------------------
#define HLPR_DRIVER_NAME_TEXT "Server Helper Driver"
#else
#if APPLICA_PRODUCT==PRODUCT_ZSERVER
// --- main MMC ------------ ApplicaIpMMC -------------------------------------
#define CLIENT_MMC_CTRL_ADD_TO_GROUP_TEXT "Add to ""ZServer Users"" group (Allow logon using RDP)"
#define CLIENT_MMC_CTRL_SERVER_CONFIG_TEXT "ZServer configuration"
#define CLIENT_MMC_NAME_CONFIGURATION "ZServer Configuration"
#define CLIENT_MMC_NAME_CONSOLE "ZServer Management Console"
#define CLIENT_MMC_NAME_CLIENT "ZServer Client"
#define CLIENT_MMC_NAME_FULL_VERSION "ZServer Management Console v1.1"
// --- logon MMC ----------- LogonMMC -----------------------------------------
#define LOGON_MMC_CONFIG_DIALOG "ZServer Autologon Configuration Dialog"
#define LOGON_MMC_CONFIGURATION "ZServer Configuration"
#define LOGON_MMC_NAME_CONSOLE "ZServer Autologon Console"
#define LOGON_MMC_NAME_CLIENT "ZServer Autologon"
#define LOGON_MMC_NAME_FULL_VERSION "ZServer Autologon Console v1.0a"
// --- Patch Driver -------- HLPK Driver---------------------------------------
#define HLPR_DRIVER_NAME_TEXT "Server Helper Driver"
#endif
#endif
/////////////////////////////////////////////////////////////////

#define REG_HKLM HKEY_LOCAL_MACHINE
#define REG_HKCU HKEY_CURRENT_USER
#define REG_HKCR HKEY_CLASSES_ROOT

 
#endif // _APPLICASERVER_H_
