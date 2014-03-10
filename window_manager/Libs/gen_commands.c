/* This is generated file
   Do not EDIT
   Edit gen.py and Commands.h instead
*/

#ifdef _WIN32
#include <tchar.h>
typedef TCHAR	gchar;
typedef int	gint;
#else
#include <glib.h>
#endif


const gchar * get_message_string(gint m)
{
	if (m == 0) {
		return _T("MESSAGE_TYPE_UPDATE_FIRMWARE");
	}
	if (m == 1) {
		return _T("MESSAGE_TYPE_LOGIN_INFO");
	}
	if (m == 2) {
		return _T("MESSAGE_TYPE_LOGIN_ERROR");
	}
	if (m == 3) {
		return _T("MESSAGE_TYPE_LOGIN_SUCCESS");
	}
	if (m == 4) {
		return _T("MESSAGE_TYPE_GET_TASKS");
	}
	if (m == 5) {
		return _T("MESSAGE_TYPE_TASKS_ERROR");
	}
	if (m == 6) {
		return _T("MESSAGE_TYPE_TASKS");
	}
	if (m == 7) {
		return _T("MESSAGE_TYPE_GET_TASK_BY_FILENAME");
	}
	if (m == 8) {
		return _T("MESSAGE_TYPE_TASK_ERROR");
	}
	if (m == 9) {
		return _T("MESSAGE_TYPE_TASK");
	}
	if (m == 10) {
		return _T("MESSAGE_TYPE_SET_TASKS");
	}
	if (m == 11) {
		return _T("MESSAGE_TYPE_SET_TASKS_ERROR");
	}
	if (m == 12) {
		return _T("MESSAGE_TYPE_SET_TASK_SSUCCESS");
	}
	if (m == 13) {
		return _T("MESSAGE_TYPE_SET_TASK_BY_FILENAME");
	}
	if (m == 14) {
		return _T("MESSAGE_TYPE_SET_TASK_ERROR");
	}
	if (m == 15) {
		return _T("MESSAGE_TYPE_SET_TASK_SUCCESS");
	}
	if (m == 16) {
		return _T("MESSAGE_TYPE_GET_NEW_TASK_NAME");
	}
	if (m == 17) {
		return _T("MESSAGE_TYPE_NEW_TASK_NAME");
	}
	if (m == 18) {
		return _T("MESSAGE_TYPE_NEW_TASK");
	}
	if (m == 19) {
		return _T("MESSAGE_TYPE_NEW_TASK_ERROR");
	}
	if (m == 20) {
		return _T("MESSAGE_TYPE_NEW_TASK_SUCCESS");
	}
	if (m == 21) {
		return _T("MESSAGE_TYPE_GET_SCHEDULES");
	}
	if (m == 22) {
		return _T("MESSAGE_TYPE_SCHEDULES_ERROR");
	}
	if (m == 23) {
		return _T("MESSAGE_TYPE_SCHEDULES");
	}
	if (m == 24) {
		return _T("MESSAGE_TYPE_GET_SCHEDULE_BY_FILENAME");
	}
	if (m == 25) {
		return _T("MESSAGE_TYPE_SCHEDULE_ERROR");
	}
	if (m == 26) {
		return _T("MESSAGE_TYPE_SCHEDULE");
	}
	if (m == 27) {
		return _T("MESSAGE_TYPE_SET_SCHEDULES");
	}
	if (m == 28) {
		return _T("MESSAGE_TYPE_SET_SCHEDULES_ERROR");
	}
	if (m == 29) {
		return _T("MESSAGE_TYPE_SET_SCHEDULE_SSUCCESS");
	}
	if (m == 30) {
		return _T("MESSAGE_TYPE_SET_SCHEDULE_BY_FILENAME");
	}
	if (m == 31) {
		return _T("MESSAGE_TYPE_SET_SCHEDULE_ERROR");
	}
	if (m == 32) {
		return _T("MESSAGE_TYPE_SET_SCHEDULE_SUCCESS");
	}
	if (m == 33) {
		return _T("MESSAGE_TYPE_GET_NEW_SCHEDULE_NAME");
	}
	if (m == 34) {
		return _T("MESSAGE_TYPE_NEW_SCHEDULE_NAME");
	}
	if (m == 35) {
		return _T("MESSAGE_TYPE_NEW_SCHEDULE");
	}
	if (m == 36) {
		return _T("MESSAGE_TYPE_NEW_SCHEDULE_ERROR");
	}
	if (m == 37) {
		return _T("MESSAGE_TYPE_NEW_SCHEDULE_SUCCESS");
	}
	if (m == 38) {
		return _T("MESSAGE_TYPE_DELETE_TASK_BY_FILENAME");
	}
	if (m == 39) {
		return _T("MESSAGE_TYPE_DELETE_TASK_ERROR");
	}
	if (m == 40) {
		return _T("MESSAGE_TYPE_DELETE_TASK_SUCCESS");
	}
	if (m == 41) {
		return _T("MESSAGE_TYPE_DELETE_SCHEDULE_BY_FILENAME");
	}
	if (m == 42) {
		return _T("MESSAGE_TYPE_DELETE_SCHEDULE_ERROR");
	}
	if (m == 43) {
		return _T("MESSAGE_TYPE_DELETE_SCHEDULE_SUCCESS");
	}
	if (m == 44) {
		return _T("MESSAGE_TYPE_GET_FILE_SIZE");
	}
	if (m == 45) {
		return _T("MESSAGE_TYPE_FILE_SIZE_ERROR");
	}
	if (m == 46) {
		return _T("MESSAGE_TYPE_FILE_SIZE");
	}
	if (m == 47) {
		return _T("MESSAGE_TYPE_GET_FILE_BY_FILENAME");
	}
	if (m == 48) {
		return _T("MESSAGE_TYPE_FILE_ERROR");
	}
	if (m == 49) {
		return _T("MESSAGE_TYPE_FILE");
	}
	if (m == 50) {
		return _T("MESSAGE_TYPE_GET_FILE_CANCEL");
	}
	if (m == 51) {
		return _T("MESSAGE_TYPE_GET_SESSIONS");
	}
	if (m == 52) {
		return _T("MESSAGE_TYPE_SESSIONS_ERROR");
	}
	if (m == 53) {
		return _T("MESSAGE_TYPE_SESSIONS");
	}
	if (m == 54) {
		return _T("MESSAGE_TYPE_START_PROCESS");
	}
	if (m == 55) {
		return _T("MESSAGE_TYPE_START_PROCESS_ERROR");
	}
	if (m == 56) {
		return _T("MESSAGE_TYPE_START_PROCESS_SUCCESS");
	}
	if (m == 57) {
		return _T("MESSAGE_TYPE_STOP_PROCESS");
	}
	if (m == 58) {
		return _T("MESSAGE_TYPE_STOP_PROCESS_ERROR");
	}
	if (m == 59) {
		return _T("MESSAGE_TYPE_STOP_PROCESS_SUCCESS");
	}
	if (m == 60) {
		return _T("MESSAGE_TYPE_TERMINATE_PROCESS");
	}
	if (m == 61) {
		return _T("MESSAGE_TYPE_TERMINATE_PROCESS_ERROR");
	}
	if (m == 62) {
		return _T("MESSAGE_TYPE_TERMINATE_PROCESS_SUCCESS");
	}
	if (m == 63) {
		return _T("MESSAGE_TYPE_STOP_ALL_PROCESSES");
	}
	if (m == 64) {
		return _T("MESSAGE_TYPE_STOP_ALL_PROCESSES_ERROR");
	}
	if (m == 65) {
		return _T("MESSAGE_TYPE_STOP_ALL_PROCESSES_SUCCESS");
	}
	if (m == 66) {
		return _T("MESSAGE_TYPE_GET_PROCESS_LIST");
	}
	if (m == 67) {
		return _T("MESSAGE_TYPE_PROCESS_LIST_ERROR");
	}
	if (m == 68) {
		return _T("MESSAGE_TYPE_PROCESS_LIST");
	}
	if (m == 69) {
		return _T("MESSAGE_TYPE_GET_STARTED_PROCESSES");
	}
	if (m == 70) {
		return _T("MESSAGE_TYPE_STARTED_PROCESSES_ERROR");
	}
	if (m == 71) {
		return _T("MESSAGE_TYPE_STARTED_PROCESSES");
	}
	if (m == 72) {
		return _T("MESSAGE_TYPE_START_SCHEDULE");
	}
	if (m == 73) {
		return _T("MESSAGE_TYPE_START_SCHEDULE_ERROR");
	}
	if (m == 74) {
		return _T("MESSAGE_TYPE_START_SCHEDULE_SUCCESS");
	}
	if (m == 75) {
		return _T("MESSAGE_TYPE_SKIP_SCHEDULE_TASK");
	}
	if (m == 76) {
		return _T("MESSAGE_TYPE_SKIP_SCHEDULE_TASK_ERROR");
	}
	if (m == 77) {
		return _T("MESSAGE_TYPE_SKIP_SCHEDULE_TASK_SUCCESS");
	}
	if (m == 78) {
		return _T("MESSAGE_TYPE_STOP_SCHEDULE");
	}
	if (m == 79) {
		return _T("MESSAGE_TYPE_STOP_SCHEDULE_ERROR");
	}
	if (m == 80) {
		return _T("MESSAGE_TYPE_STOP_SCHEDULE_SUCCESS");
	}
	if (m == 81) {
		return _T("MESSAGE_TYPE_UPDATE_TASK_LIST");
	}
	if (m == 82) {
		return _T("MESSAGE_TYPE_UPDATE_SCHEDULE_LIST");
	}
	if (m == 83) {
		return _T("MESSAGE_TYPE_CLIENT_DISCONNECT");
	}
	if (m == 84) {
		return _T("MESSAGE_TYPE_GET_DISPLAY_MODE");
	}
	if (m == 85) {
		return _T("MESSAGE_TYPE_GET_DISPLAY_MODE_ERROR");
	}
	if (m == 86) {
		return _T("MESSAGE_TYPE_DISPLAY_MODE");
	}
	if (m == 87) {
		return _T("MESSAGE_TYPE_GET_DISPLAY_PROPERTIES");
	}
	if (m == 88) {
		return _T("MESSAGE_TYPE_GET_DISPLAY_PROPERTIES_ERROR");
	}
	if (m == 89) {
		return _T("MESSAGE_TYPE_DISPLAY_PROPERTIES");
	}
	if (m == 90) {
		return _T("MESSAGE_TYPE_SET_DISPLAY_PROPERTIES");
	}
	if (m == 91) {
		return _T("MESSAGE_TYPE_SET_DISPLAY_PROPERTIES_ERROR");
	}
	if (m == 92) {
		return _T("MESSAGE_TYPE_SET_DISPLAY_PROPERTIES_SUCCESS");
	}
	if (m == 93) {
		return _T("MESSAGE_TYPE_ADD_REMOTE_MONITOR");
	}
	if (m == 94) {
		return _T("MESSAGE_TYPE_ADD_REMOTE_MONITOR_ERROR");
	}
	if (m == 95) {
		return _T("MESSAGE_TYPE_ADD_REMOTE_MONITOR_SUCCESS");
	}
	if (m == 96) {
		return _T("MESSAGE_TYPE_GET_CLIENT_SCHEDULES");
	}
	if (m == 97) {
		return _T("MESSAGE_TYPE_GET_CLIENT_SCHEDULES_ERROR");
	}
	if (m == 98) {
		return _T("MESSAGE_TYPE_CLIENT_SCHEDULES");
	}
	if (m == 99) {
		return _T("MESSAGE_TYPE_SET_CLIENT_SCHEDULES");
	}
	if (m == 100) {
		return _T("MESSAGE_TYPE_SET_CLIENT_SCHEDULES_ERROR");
	}
	if (m == 101) {
		return _T("MESSAGE_TYPE_SET_CLIENT_SCHEDULES_SUCCESS");
	}
	if (m == 102) {
		return _T("MESSAGE_TYPE_PING");
	}
	if (m == 103) {
		return _T("MESSAGE_TYPE_PROTECTION_FAILED");
	}
	if (m == 104) {
		return _T("MESSAGE_TYPE_LOG");
	}
	if (m == 105) {
		return _T("MESSAGE_TYPE_SEND_INPUT");
	}
	if (m == 106) {
		return _T("MESSAGE_TYPE_UPDATE_REMOTE_SCREEN");
	}
	if (m == 107) {
		return _T("MESSAGE_TYPE_CAPTURE_SCREEN_TO_FILE");
	}
	if (m == 108) {
		return _T("MESSAGE_TYPE_CAPTURE_CURSOR_TO_FILE");
	}
	if (m == 109) {
		return _T("MESSAGE_TYPE_KEYLOCKSTATE");
	}
	if (m == 110) {
		return _T("MESSAGE_TYPE_ADD_REMOTE_INPUT");
	}
	if (m == 111) {
		return _T("MESSAGE_TYPE_REMOVE_REMOTE_INPUT");
	}
	if (m == 112) {
		return _T("MESSAGE_TYPE_GET_MONITOR_CONFIG");
	}
	if (m == 113) {
		return _T("MESSAGE_TYPE_GET_MONITOR_CONFIG_ERROR");
	}
	if (m == 114) {
		return _T("MESSAGE_TYPE_MONITOR_CONFIG");
	}
	if (m == 115) {
		return _T("MESSAGE_TYPE_SET_MONITOR_CONFIG");
	}
	if (m == 116) {
		return _T("MESSAGE_TYPE_SET_MONITOR_CONFIG_ERROR");
	}
	if (m == 117) {
		return _T("MESSAGE_TYPE_SET_MONITOR_CONFIG_SUCCESS");
	}
	if (m == 118) {
		return _T("MESSAGE_TYPE_GET_WALL_VIEWS");
	}
	if (m == 119) {
		return _T("MESSAGE_TYPE_GET_WALL_VIEWS_ERROR");
	}
	if (m == 120) {
		return _T("MESSAGE_TYPE_WALL_VIEWS");
	}
	if (m == 121) {
		return _T("MESSAGE_TYPE_SET_WALL_VIEWS");
	}
	if (m == 122) {
		return _T("MESSAGE_TYPE_SET_WALL_VIEWS_ERROR");
	}
	if (m == 123) {
		return _T("MESSAGE_TYPE_SET_WALL_VIEWS_SUCCESS");
	}
	if (m == 124) {
		return _T("MESSAGE_TYPE_SCAN_MEDIA_CONTENT");
	}
	if (m == 125) {
		return _T("MESSAGE_TYPE_CONFIG_CHANGE");
	}
	if (m == 126) {
		return _T("MESSAGE_TYPE_TOGGLE_STATUS_BAR");
	}
	if (m == 127) {
		return _T("MESSAGE_TYPE_TOGGLE_PLAY");
	}
	if (m == 128) {
		return _T("MESSAGE_TYPE_GET_LIST_MEDIA");
	}
	if (m == 129) {
		return _T("MESSAGE_TYPE_LIST_MEDIA");
	}
	if (m == 130) {
		return _T("MESSAGE_TYPE_CLIENT_GROUPS");
	}
	if (m == 131) {
		return _T("MESSAGE_TYPE_SET_CLIENT_GROUPS");
	}
	if (m == 132) {
		return _T("MESSAGE_TYPE_CLIENT_LIST");
	}
	if (m == 133) {
		return _T("MESSAGE_TYPE_SET_CLIENT_LIST");
	}
	if (m == 134) {
		return _T("MESSAGE_TYPE_WALL_VIRTUAL_VIEWS");
	}
	if (m == 135) {
		return _T("MESSAGE_TYPE_SET_WALL_VIRTUAL_VIEWS");
	}
	if (m == 136) {
		return _T("MESSAGE_TYPE_SET_WALL_VIRTUAL_VIEWS_ERROR");
	}
	if (m == 137) {
		return _T("MESSAGE_TYPE_SET_WALL_VIRTUAL_VIEWS_SUCCESS");
	}
	if (m == 138) {
		return _T("MESSAGE_TYPE_DEFAULT_SERVER_SETTINGS");
	}
	if (m == 139) {
		return _T("MESSAGE_TYPE_GET_CLIENT_SETTINGS");
	}
	if (m == 140) {
		return _T("MESSAGE_TYPE_GET_CLIENT_SETTINGS_ERROR");
	}
	if (m == 141) {
		return _T("MESSAGE_TYPE_CLIENT_SETTINGS");
	}
	if (m == 142) {
		return _T("MESSAGE_TYPE_SET_CLIENT_SETTINGS");
	}
	if (m == 143) {
		return _T("MESSAGE_TYPE_SET_CLIENT_SETTINGS_ERROR");
	}
	if (m == 144) {
		return _T("MESSAGE_TYPE_SET_CLIENT_SETTINGS_SUCCESS");
	}
	if (m == 145) {
		return _T("MESSAGE_TYPE_TIME_REQUEST");
	}
	if (m == 146) {
		return _T("MESSAGE_TYPE_TIME");
	}
	if (m == 147) {
		return _T("MESSAGE_TYPE_CLIENT_AUTOCONF_MODE_BEGIN");
	}
	if (m == 148) {
		return _T("MESSAGE_TYPE_CLIENT_AUTOCONF_MODE_END");
	}
	if (m == 149) {
		return _T("MESSAGE_TYPE_GET_USER_LIST");
	}
	if (m == 150) {
		return _T("MESSAGE_TYPE_GET_USER_LIST_ERROR");
	}
	if (m == 151) {
		return _T("MESSAGE_TYPE_USER_LIST");
	}
	if (m == 152) {
		return _T("MESSAGE_TYPE_GET_USER_SETTINGS");
	}
	if (m == 153) {
		return _T("MESSAGE_TYPE_GET_USER_SETTINGS_ERROR");
	}
	if (m == 154) {
		return _T("MESSAGE_TYPE_USER_SETTINGS");
	}
	if (m == 155) {
		return _T("MESSAGE_TYPE_SET_USER_SETTINGS");
	}
	if (m == 156) {
		return _T("MESSAGE_TYPE_SET_USER_SETTINGS_ERROR");
	}
	if (m == 157) {
		return _T("MESSAGE_TYPE_SET_USER_SETTINGS_SUCCESS");
	}
	if (m == 158) {
		return _T("MESSAGE_TYPE_ADD_USER");
	}
	if (m == 159) {
		return _T("MESSAGE_TYPE_ADD_USER_ERROR");
	}
	if (m == 160) {
		return _T("MESSAGE_TYPE_ADD_USER_SUCCESS");
	}
	if (m == 161) {
		return _T("MESSAGE_TYPE_DEL_USER");
	}
	if (m == 162) {
		return _T("MESSAGE_TYPE_DEL_USER_ERROR");
	}
	if (m == 163) {
		return _T("MESSAGE_TYPE_DEL_USER_SUCCESS");
	}
	if (m == 164) {
		return _T("MESSAGE_TYPE_REN_USER");
	}
	if (m == 165) {
		return _T("MESSAGE_TYPE_REN_USER_ERROR");
	}
	if (m == 166) {
		return _T("MESSAGE_TYPE_REN_USER_SUCCESS");
	}
	if (m == 167) {
		return _T("MESSAGE_TYPE_UPDATE_USER_LIST");
	}
	if (m == 168) {
		return _T("MESSAGE_TYPE_GET_NEW_USER_NAME");
	}
	if (m == 169) {
		return _T("MESSAGE_TYPE_NEW_USER_NAME");
	}
	if (m == 170) {
		return _T("MESSAGE_TYPE_GET_MEDIA_STATUS");
	}
	if (m == 171) {
		return _T("MESSAGE_TYPE_MEDIA_STATUS");
	}
	if (m == 172) {
		return _T("MESSAGE_TYPE_MEDIA_STATUS_ERROR");
	}
	if (m == 173) {
		return _T("MESSAGE_TYPE_LOGGED_USER");
	}
	if (m == 174) {
		return _T("MESSAGE_TYPE_SESSION_CHANGED");
	}
	if (m == 175) {
		return _T("MESSAGE_TYPE_GET_SCHEDULE_ARCHIVE");
	}
	if (m == 176) {
		return _T("MESSAGE_TYPE_SCHEDULE_ARCHIVE_ERROR");
	}
	if (m == 177) {
		return _T("MESSAGE_TYPE_SCHEDULE_ARCHIVE");
	}
	if (m == 178) {
		return _T("MESSAGE_TYPE_SEND_FILE_TO_SERVER");
	}
	if (m == 179) {
		return _T("MESSAGE_TYPE_SEND_FILE_TO_SERVER_ERROR");
	}
	if (m == 180) {
		return _T("MESSAGE_TYPE_SEND_FILE_TO_SERVER_SUCCESS");
	}
	if (m == 181) {
		return _T("MESSAGE_TYPE_CHECK_FILE_ON_SERVER");
	}
	if (m == 182) {
		return _T("MESSAGE_TYPE_FILE_ON_SERVER_EXIST");
	}
	if (m == 183) {
		return _T("MESSAGE_TYPE_FILE_ON_SERVER_NONE");
	}
	if (m == 184) {
		return _T("MESSAGE_TYPE_FILE_ON_SERVER_ERROR");
	}
	if (m == 185) {
		return _T("MESSAGE_TYPE_GET_SERVER_FILE_LIST");
	}
	if (m == 186) {
		return _T("MESSAGE_TYPE_SERVER_FILE_LIST");
	}
	if (m == 187) {
		return _T("MESSAGE_TYPE_SERVER_FILE_LIST_ERROR");
	}
	if (m == 188) {
		return _T("MESSAGE_TYPE_SERVER_FILE_LIST_SUCCESS");
	}
	if (m == 189) {
		return _T("MESSAGE_TYPE_SERVER_HAS_ALL_FILES");
	}
	if (m == 190) {
		return _T("MESSAGE_TYPE_SERVER_UPLOADING_FILES");
	}
	if (m == 191) {
		return _T("MESSAGE_TYPE_INITIALIZE");
	}
	if (m == 192) {
		return _T("MESSAGE_TYPE_INITIALIZE_SUCCESS");
	}
	if (m == 193) {
		return _T("MESSAGE_TYPE_INITIALIZE_ERROR");
	}
	if (m == 194) {
		return _T("MESSAGE_TYPE_GET_LIST_MEDIA_ERROR");
	}
	if (m == 195) {
		return _T("MESSAGE_TYPE_STOP_UPLOAD_FILES");
	}
	if (m == 196) {
		return _T("MESSAGE_TYPE_STOP_UPLOAD_FILES_SUCCESS");
	}
	if (m == 197) {
		return _T("MESSAGE_TYPE_STOP_UPLOAD_FILES_ERROR");
	}
	if (m == 198) {
		return _T("MESSAGE_TYPE_WAKE_UP_ON_LAN");
	}
	if (m == 199) {
		return _T("MESSAGE_TYPE_WAKE_UP_ON_LAN_SUCCESS");
	}
	if (m == 200) {
		return _T("MESSAGE_TYPE_WAKE_UP_ON_LAN_ERROR");
	}
	if (m == 201) {
		return _T("MESSAGE_TYPE_GET_SERVER_SETTINGS");
	}
	if (m == 202) {
		return _T("MESSAGE_TYPE_GET_SERVER_SETTINGS_ERROR");
	}
	if (m == 203) {
		return _T("MESSAGE_TYPE_SERVER_SETTINGS");
	}
	if (m == 204) {
		return _T("MESSAGE_TYPE_SET_SERVER_SETTINGS");
	}
	if (m == 205) {
		return _T("MESSAGE_TYPE_SET_SERVER_SETTINGS_ERROR");
	}
	if (m == 206) {
		return _T("MESSAGE_TYPE_SET_SERVER_SETTINGS_SUCCESS");
	}
	if (m == 207) {
		return _T("MESSAGE_TYPE_GET_CLIENT_REPORT");
	}
	if (m == 208) {
		return _T("MESSAGE_TYPE_GET_CLIENT_REPORT_ERROR");
	}
	if (m == 209) {
		return _T("MESSAGE_TYPE_CLIENT_REPORT");
	}
	if (m == 210) {
		return _T("MESSAGE_TYPE_TRIGGER_EVENT");
	}
	if (m == 211) {
		return _T("MESSAGE_TYPE_TRIGGER_EVENT_SUCCESS");
	}
	if (m == 212) {
		return _T("MESSAGE_TYPE_TRIGGER_EVENT_ERROR");
	}
	if (m == 213) {
		return _T("MESSAGE_TYPE_GET_PLAYER_STATUS");
	}
	if (m == 214) {
		return _T("MESSAGE_TYPE_PLAYER_STATUS");
	}
	if (m == 215) {
		return _T("MESSAGE_TYPE_GET_PLAYER_STATUS_ERROR");
	}
	if (m == 216) {
		return _T("MESSAGE_TYPE_RESTART_SCHEDULE");
	}
	if (m == 217) {
		return _T("MESSAGE_TYPE_ADD_LOGO");
	}
	if (m == 218) {
		return _T("MESSAGE_TYPE_ADD_LOGO_SUCCESS");
	}
	if (m == 219) {
		return _T("MESSAGE_TYPE_ADD_LOGO_ERROR");
	}
	if (m == 220) {
		return _T("MESSAGE_TYPE_REMOVE_LOGO");
	}
	if (m == 221) {
		return _T("MESSAGE_TYPE_REMOVE_LOGO_SUCCESS");
	}
	if (m == 222) {
		return _T("MESSAGE_TYPE_REMOVE_LOGO_ERROR");
	}
	if (m == 223) {
		return _T("MESSAGE_TYPE_GET_SERVER_TEMPLATE_LIST");
	}
	if (m == 224) {
		return _T("MESSAGE_TYPE_SERVER_TEMPLATE_LIST");
	}
	if (m == 225) {
		return _T("MESSAGE_TYPE_SERVER_TEMPLATE_LIST_ERROR");
	}
	if (m == 226) {
		return _T("MESSAGE_TYPE_SERVER_TEMPLATE_LIST_SUCCESS");
	}
	if (m == 227) {
		return _T("MESSAGE_TYPE_LOCK_KEYBOARD");
	}
	if (m == 228) {
		return _T("MESSAGE_TYPE_TASK_SCHEDULE_GROUPS");
	}
	if (m == 229) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUPS");
	}
	if (m == 230) {
		return _T("MESSAGE_TYPE_TOUCHSCREEN_CLICKED");
	}
	if (m == 231) {
		return _T("MESSAGE_TYPE_GET_USER_NAME_LIST");
	}
	if (m == 232) {
		return _T("MESSAGE_TYPE_GET_USER_NAME_LIST_ERROR");
	}
	if (m == 233) {
		return _T("MESSAGE_TYPE_USER_NAME_LIST");
	}
	if (m == 234) {
		return _T("MESSAGE_TYPE_URGENT_CREEPING_LINE");
	}
	if (m == 235) {
		return _T("MESSAGE_TYPE_URGENT_CREEPING_LINE_ERROR");
	}
	if (m == 236) {
		return _T("MESSAGE_TYPE_URGENT_CREEPING_LINE_SUCCESS");
	}
	if (m == 237) {
		return _T("MESSAGE_TYPE_STOP_URGENT_CREEPING_LINE");
	}
	if (m == 238) {
		return _T("MESSAGE_TYPE_STOP_URGENT_CREEPING_LINE_ERROR");
	}
	if (m == 239) {
		return _T("MESSAGE_TYPE_STOP_URGENT_CREEPING_LINE_SUCCESS");
	}
	if (m == 240) {
		return _T("MESSAGE_TYPE_GET_SERVER_EXE_PATH");
	}
	if (m == 241) {
		return _T("MESSAGE_TYPE_SERVER_EXE_PATH");
	}
	if (m == 242) {
		return _T("MESSAGE_TYPE_GET_SERVER_ROOT_PATH");
	}
	if (m == 243) {
		return _T("MESSAGE_TYPE_SERVER_ROOT_PATH");
	}
	if (m == 244) {
		return _T("MESSAGE_TYPE_GET_PLAYER_REPORT");
	}
	if (m == 245) {
		return _T("MESSAGE_TYPE_PLAYER_REPORT");
	}
	if (m == 246) {
		return _T("MESSAGE_TYPE_PLAYER_REPORT_ERROR");
	}
	if (m == 247) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUP");
	}
	if (m == 248) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUP_SUCCESS");
	}
	if (m == 249) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUP_ERROR");
	}
	if (m == 250) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUP_NAME");
	}
	if (m == 251) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUP_NAME_SUCCESS");
	}
	if (m == 252) {
		return _T("MESSAGE_TYPE_SET_TASK_SCHEDULE_GROUP_NAME_ERROR");
	}
	if (m == 253) {
		return _T("MESSAGE_TYPE_DELETE_TASK_SCHEDULE_GROUP");
	}
	if (m == 254) {
		return _T("MESSAGE_TYPE_DELETE_TASK_SCHEDULE_GROUP_SUCCESS");
	}
	if (m == 255) {
		return _T("MESSAGE_TYPE_DELETE_TASK_SCHEDULE_GROUP_ERROR");
	}
	if (m == 256) {
		return _T("MESSAGE_TYPE_TCPSTREAM_OPEN");
	}
	if (m == 257) {
		return _T("MESSAGE_TYPE_TCPSTREAM_OPEN_SUCCESS");
	}
	if (m == 258) {
		return _T("MESSAGE_TYPE_TCPSTREAM_OPEN_ERROR");
	}
	if (m == 259) {
		return _T("MESSAGE_TYPE_TCPSTREAM_DATA");
	}
	if (m == 260) {
		return _T("MESSAGE_TYPE_TCPSTREAM_DATA_SUCCESS");
	}
	if (m == 261) {
		return _T("MESSAGE_TYPE_TCPSTREAM_DATA_ERROR");
	}
	if (m == 262) {
		return _T("MESSAGE_TYPE_TCPSTREAM_CLOSE");
	}
	if (m == 263) {
		return _T("MESSAGE_TYPE_TCPSTREAM_CLOSE_SUCCESS");
	}
	if (m == 264) {
		return _T("MESSAGE_TYPE_TCPSTREAM_CLOSE_ERROR");
	}
	if (m == 265) {
		return _T("MESSAGE_TYPE_PLAYER_STATE");
	}
	if (m == 266) {
		return _T("MESSAGE_TYPE_UPLOAD_SITE");
	}
	if (m == 267) {
		return _T("MESSAGE_TYPE_UPLOAD_SITE_SUCCESS");
	}
	if (m == 268) {
		return _T("MESSAGE_TYPE_UPLOAD_SITE_ERROR");
	}
	if (m == 269) {
		return _T("MESSAGE_TYPE_UPDATE_SITE");
	}
	if (m == 270) {
		return _T("MESSAGE_TYPE_UPDATE_SITE_SUCCESS");
	}
	if (m == 271) {
		return _T("MESSAGE_TYPE_UPDATE_SITE_ERROR");
	}
	if (m == 272) {
		return _T("MESSAGE_TYPE_UPDATE_PLAYER_FIRMWARE");
	}
	if (m == 273) {
		return _T("MESSAGE_TYPE_UPDATE_PLAYER_FIRMWARE_SUCCESS");
	}
	if (m == 274) {
		return _T("MESSAGE_TYPE_UPDATE_PLAYER_FIRMWARE_ERROR");
	}
	if (m == 275) {
		return _T("MESSAGE_TYPE_GET_WALL_PROFILES");
	}
	if (m == 276) {
		return _T("MESSAGE_TYPE_GET_WALL_PROFILES_ERROR");
	}
	if (m == 277) {
		return _T("MESSAGE_TYPE_WALL_PROFILES");
	}
	if (m == 278) {
		return _T("MESSAGE_TYPE_GET_CURRENT_WALL_PROFILE");
	}
	if (m == 279) {
		return _T("MESSAGE_TYPE_GET_CURRENT_WALL_PROFILE_ERROR");
	}
	if (m == 280) {
		return _T("MESSAGE_TYPE_CURRENT_WALL_PROFILE");
	}
	if (m == 281) {
		return _T("MESSAGE_TYPE_GET_WALL_PROFILE_MONITOR_RESOLUTION");
	}
	if (m == 282) {
		return _T("MESSAGE_TYPE_GET_WALL_PROFILE_MONITOR_RESOLUTION_ERROR");
	}
	if (m == 283) {
		return _T("MESSAGE_TYPE_WALL_PROFILE_MONITOR_RESOLUTION");
	}
	if (m == 284) {
		return _T("MESSAGE_TYPE_SET_WALL_PROFILES");
	}
	if (m == 285) {
		return _T("MESSAGE_TYPE_SET_WALL_PROFILES_SUCCESS");
	}
	if (m == 286) {
		return _T("MESSAGE_TYPE_SET_WALL_PROFILES_ERROR");
	}
	if (m == 287) {
		return _T("MESSAGE_TYPE_SET_CURRENT_WALL_PROFILE");
	}
	if (m == 288) {
		return _T("MESSAGE_TYPE_SET_CURRENT_WALL_PROFILE_SUCCESS");
	}
	if (m == 289) {
		return _T("MESSAGE_TYPE_SET_CURRENT_WALL_PROFILE_ERROR");
	}
	if (m == 290) {
		return _T("MESSAGE_TYPE_SET_WALL_PROFILE_MONITOR_RESOLUTION");
	}
	if (m == 291) {
		return _T("MESSAGE_TYPE_SET_WALL_PROFILE_MONITOR_RESOLUTION_SUCCESS");
	}
	if (m == 292) {
		return _T("MESSAGE_TYPE_SET_WALL_PROFILE_MONITOR_RESOLUTION_ERROR");
	}
	if (m == 293) {
		return _T("MESSAGE_TYPE_SERVER_SITES");
	}
	if (m == 294) {
		return _T("MESSAGE_TYPE_SET_SERVER_SITES");
	}
	if (m == 295) {
		return _T("MESSAGE_TYPE_SET_SERVER_SITES_SUCCESS");
	}
	if (m == 296) {
		return _T("MESSAGE_TYPE_SET_SERVER_SITES_ERROR");
	}
	if (m == 297) {
		return _T("MESSAGE_TYPE_NETWORK_DISK");
	}
	if (m == 298) {
		return _T("MESSAGE_TYPE_GET_CAMERA_REPORT_LIST");
	}
	if (m == 299) {
		return _T("MESSAGE_TYPE_CAMERA_REPORT_LIST");
	}
	if (m == 300) {
		return _T("MESSAGE_TYPE_GET_CAMERA_REPORT_LIST_ERROR");
	}
	if (m == 301) {
		return _T("MESSAGE_TYPE_LIVE_CAMERA");
	}
	if (m == 302) {
		return _T("MESSAGE_TYPE_LIVE_CAMERA_SUCCESS");
	}
	if (m == 303) {
		return _T("MESSAGE_TYPE_LIVE_CAMERA_ERROR");
	}
	if (m == 304) {
		return _T("MESSAGE_TYPE_CAMERA_PHOTO");
	}
	if (m == 305) {
		return _T("MESSAGE_TYPE_CAMERA_PHOTO_SUCCESS");
	}
	if (m == 306) {
		return _T("MESSAGE_TYPE_CAMERA_PHOTO_ERROR");
	}
	if (m == 307) {
		return _T("MESSAGE_TYPE_PLAYER_SHUTDOWN");
	}
	if (m == 308) {
		return _T("MESSAGE_TYPE_PLAYER_SHUTDOWN_SUCCESS");
	}
	if (m == 309) {
		return _T("MESSAGE_TYPE_PLAYER_SHUTDOWN_ERROR");
	}
	if (m == 310) {
		return _T("MESSAGE_TYPE_PLAYER_RESTART");
	}
	if (m == 311) {
		return _T("MESSAGE_TYPE_PLAYER_RESTART_SUCCESS");
	}
	if (m == 312) {
		return _T("MESSAGE_TYPE_PLAYER_RESTART_ERROR");
	}
	if (m == 313) {
		return _T("MESSAGE_TYPE_PLAYER_SYNC_MODE");
	}
	if (m == 314) {
		return _T("MESSAGE_TYPE_PLAYER_IDENTIFY");
	}
	if (m == 315) {
		return _T("MESSAGE_TYPE_COUNT");
	}

	return _T("MESSAGE_TYPE_UNKNOWN_MESSAGE");
}
