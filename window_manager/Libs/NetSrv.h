// ------------------------------------------------------------
// NetSrv.h - network server for Applica Server project - TV...
// - Header file: 
// -- Find Server functionality --
// ------------------------------------------------------------

#pragma once

#pragma comment (lib, "ws2_32.lib")

DWORD WINAPI asServerMainThread(LPVOID lpParameter);
char * find_signage_server_local(void);
