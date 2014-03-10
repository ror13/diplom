//////////////////////////////////////////////////////////////
// File:		// PresenterRegistry.h
// File time:	// 28.01.2005	17:11
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _PRESENTERREGISTRY_H_UID0000003AC3C14FFE
#define _PRESENTERREGISTRY_H_UID0000003AC3C14FFE

/////////////////////////////////////////////////////////////////////////////

// {19E72666-6C1E-4666-83B5-B3F863639A8E}
//static const GUID g_guidPresenter =
//{ 19e72666, 6c1e, 4666, { 83, b5, b3, f8, 63, 63, 9a, 8e } };

#include "../Inc/CommonApi.h"
#include "../Inc/product.h"

#define PRESENTER_GUID_T PRODUCT_GUID //_T("{19E72666-6C1E-4666-83B5-B3F863639A8E}")

// ***** Base registry key: *************************************************************

//#define HKLM_CONTROL_REG_KEY_	"SYSTEM\\CurrentControlSet\\Control\\"

// SYSTEM\CurrentControlSet\Control\{19E72666-6C1E-4666-83B5-B3F863639A8E}
#define PRESENTER_REG_ROOT	PRODUCT_ROOT //HKEY_LOCAL_MACHINE
#define PRESENTER_REG_KEY_T PRODUCT_KEY //_T(HKLM_CONTROL_REG_KEY_) PRESENTER_GUID_T

#define REG_PRESENTER_INSTALLATION_FOLDER_VALUE_T		_T("InstallationFolder")
#define REG_PRESENTER_INSTALLER_DATE_STAMP_VALUE_T		_T("InstallerDateStamp")
#define REG_PRESENTER_INSTALLER_TIME_STAMP_VALUE_T		_T("InstallerTimeStamp")
#define REG_PRESENTER_REMOTE_ADMIN_DEBUG_VALUE_T		_T("RemoteAdminDebugInfo")
#define REG_PRESENTER_CLIENT_TRAY_FLASH_VALUE_T			_T("ClientTrayIconFlash")
#define REG_PRESENTER_PREVIEW_TRAY_FLASH_VALUE_T		_T("PreviewTrayIconFlash")
#define REG_PRESENTER_SERVER_LASTCOMMAND_VALUE_T		_T("LastCommand")
#define REG_WMHOOK_TRACE_MESSAGES_VALUE_T				_T("WmHookTraceMessages")
#define REG_PRESENTER_LOG_POSTED_COMMANDS_T				_T("LogPostedCommandsEnabled")
#define REG_PRESENTER_MAX_CREATED_TASK_VALUE_T			_T("MaxCreatedTask")
#define REG_PRESENTER_MAX_CREATED_SCHEDULE_VALUE_T		_T("MaxCreatedSchedule")
#define REG_PRESENTER_MAX_CREATED_WALL_SCHEDULE_VALUE_T	_T("MaxCreatedWallSchedule")
#define REG_PRESENTER_MAX_CREATED_TEMPLATE_VALUE_T		_T("MaxCreatedTemplate")
#define REG_PRESENTER_DEVELOPER_MODE_VALUE_T			_T("DeveloperMode")
#define REG_PRESENTER_SCHEDULE_SERVER_IP_ADDRESS_T		_T("PServerIP")
#define REG_PRESENTER_SCHEDULE_SERVER_USER_NAME_T		_T("PServerUser")
#define REG_PRESENTER_SCHEDULE_SERVER_PASSWORD_T		_T("PServerPass")

#define REG_PRESENTER_STATION_MANAGE_SERVER_IP_ADDRESS_T	_T("PMStationServerIP")
#define REG_PRESENTER_STATION_MANAGE_SERVER_USER_NAME_T		_T("PMStationServerUser")
#define REG_PRESENTER_STATION_MANAGE_SERVER_PASSWORD_T		_T("PMStationServerPass")
#define REG_PRESENTER_STATION_MANAGE_SERVER_SAVE_PASSWORD_T	_T("PMStationServerSavePass")

#define REG_PRESENTER_TASK_MANAGE_SERVER_IP_ADDRESS_T		_T("PMTaskServerIP")
#define REG_PRESENTER_TASK_MANAGE_SERVER_USER_NAME_T		_T("PMTaskServerUser")
#define REG_PRESENTER_TASK_MANAGE_SERVER_PASSWORD_T			_T("PMTaskServerPass")
#define REG_PRESENTER_TASK_MANAGE_SERVER_SAVE_PASSWORD_T	_T("PMTaskServerSavePass")

#define REG_PRESENTER_USER_MANAGE_SERVER_IP_ADDRESS_T		_T("PMUserServerIP")
#define REG_PRESENTER_USER_MANAGE_SERVER_USER_NAME_T		_T("PMUserServerUser")
#define REG_PRESENTER_USER_MANAGE_SERVER_PASSWORD_T			_T("PMUserServerPass")
#define REG_PRESENTER_USER_MANAGE_SERVER_SAVE_PASSWORD_T	_T("PMUserServerSavePass")

#define REG_PRESENTER_SMART_GATEWAY_ENABLED_T			_T("SmartGatewayEnabled")
#define REG_PRESENTER_SMART_GATEWAY_URL_T				_T("SmartGatewayURL")
#define REG_PRESENTER_SMART_GATEWAY_USER_T				_T("SmartGatewayUser")
#define REG_PRESENTER_SMART_GATEWAY_PASSWORD_T			_T("SmartGatewayPassword")

#define REG_PRESENTER_NETWORK_DISK_ENABLED_T			_T("NetworkDiskEnabled")
#define REG_PRESENTER_NETWORK_DISK_PATH_T				_T("NetworkDiskPath")
#define REG_PRESENTER_NETWORK_DISK_USER_T				_T("NetworkDiskUser")
#define REG_PRESENTER_NETWORK_DISK_PASSWORD_T			_T("NetworkDiskPassword")

#define REG_PRESENTER_REPORTS_KEEP_T					_T("ReportsKeepTime")

#define REG_PRESENTER_SERVER_IP_ADDRESS_LIST_SIZE_T		_T("PServerIPListSize")

#define REG_PRESENTER_BUILD_DATE_T				_T("BuildDate")

// Indicate if no need to run Host monitor
#define REG_WALL_WITHOUT_HOST_VALUE_T	_T("WallWithoutHost")
// Indicate if need to reset monitors on each client run
#define REG_WALL_RESET_MONITORS_VALUE_T	_T("WallResetMonitors")
// Save current output video mode for vlc (see OutputVideSettings.cpp)
#define REG_OUTPUT_VIDEO_SETTING_VALUE_T	_T("OutputVideoSetting")
// Save current output video player name (see OutputVidePlayerSettings.cpp)
#define REG_OUTPUT_VIDEO_PLAYER_SETTING_VALUE_T	_T("OutputVideoPlayerSetting")

/////////////////////////////////////////////////////////////////////////////
#define REG_IMAGE_VIEWER_PRODUCT_GUID_T				_T("{5FDF3B56-0C85-4f66-BFBC-6EC7E183A3C7}")
#define REG_IMAGE_VIEWER_PRODUCT_ROOT_T				_T("SYSTEM\\CurrentControlSet\\Control\\") REG_IMAGE_VIEWER_PRODUCT_GUID_T
#define REG_PRESENTER_IMAGE_VIEWER_KEEP_ASPECT_T		_T("ImageViewerKeepAspect")

/////////////////////////////////////////////////////////////////////////////

#define PRESENTER_AUTORUN_ROOT	REG_HKLM
#define PRESENTER_AUTORUN_KEY	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")
#define PRESENTER_AUTORUN_VALUE	_T(PRODUCT_NAME) _T(" Client")

/////////////////////////////////////////////////////////////////////////

#endif //ifndef _PRESENTERREGISTRY_H_UID0000003AC3C14FFE
