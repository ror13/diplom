#ifndef __SIGNWALLHELPER_H__
#define __SIGNWALLHELPER_H__

// Check if this monitor supported for SignWALL
BOOL IsMonitorSupportBySignWall(DWORD iDevNum);
// if no need to leave host monitor unchanged
BOOL IsWallWithoutHost();
// set if no need to leave host monitor unchanged
BOOL SetWallWithoutHost(BOOL UseWallWithoutHost);
// if need to reset monitors on each client run
BOOL IsAlwaysResetMonitors();

#endif // __SIGNWALLHELPER_H__