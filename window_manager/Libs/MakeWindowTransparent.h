/**************************************************************
 * MakewWindowTransparent.h
 */

#ifndef __MAKEWINDOWTRANSPARENT_H
#define __MAKEWINDOWTRANSPARENT_H

#define MWT_MIN_FACTOR (0)
#define MWT_MAX_FACTOR (0xFF)

BOOL MakeWindowTransparent(HWND hWnd, BYTE factor, COLORREF color = RGB(255,255,255) );
//BOOL MakeWindowTransparent(CWnd *w, BYTE factor, COLORREF color = RGB(255,255,255));
BOOL UnMakeWindowTransparent(HWND hWnd);

#endif // __MAKEWINDOWTRANSPARENT_H