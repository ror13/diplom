#ifndef __KEYLOK_H__
#define __KEYLOK_H__

#include "client.h"             /* KEY-LOK defs for demo device */
#include "keyerror.h"           /* KEY-LOK error codes */

/* COMMAND CODES */ 
#define TERMINATE      -1
#define KLCHECK         1
#define READAUTH        2
#define GETSN           3
#define GETVARWORD      4
#define WRITEAUTH       5
#define WRITEVARWORD    6
#define DECREMENTMEM    7
#define GETEXPDATE      8
#define CKLEASEDATE     9
#define SETEXPDATE     10
#define SETMAXUSERS    11
#define GETMAXUSERS    12
#define REMOTEUPDUPT1  13
#define REMOTEUPDUPT2  14
#define REMOTEUPDUPT3  15
#define REMOTEUPDCPT1  16
#define REMOTEUPDCPT2  17
#define REMOTEUPDCPT3  18
#define DOREMOTEUPDATE 21

/* ERROR FLAGS USED DURING LEASE EXPIRATION TEST */
#define LEASEOK            0xFFFF /* Lease active - not expired */
#define LEASEEXPIRED       0xFFFE /* Clock date Beyond Lease Expiration date */
#define SYSDATESETBACK     0xFFFD /* User has set back his system clock */
#define NOLEASEDATE        0xFFFC /* Lease expiration date address contains 0 */
#define LEASEDATEBAD       0xFFFB /* Invalid lease expiration date */
#define LASTSYSDATECORRUPT 0xFFFA /* Last System Date - contents corrupt */

/* ERROR FLAGS USED DURING DECREMENT OF MEMORY */
#define NOCOUNTSLEFT    1   /* Counter already counted down to zero */
#define INVALIDADDRESS  2   /* Invalid memory address requested */
#define NOWRITEAUTH     3   /* No write authorization */

/* KEYBD Function Arguments */
#define LAUNCHANTIDEBUGGER 0  /* Launch anti-debugger */

/* LEASE EXPIRATION & REMOTE UPDATE CONSTANTS */
#define BASEYEAR         1990  // Reference for expiration dates

/* REMOTE UPDATE TASK CODES */
#define REMOTEADD           0
#define REMOTEDATEEXTEND    1
#define REMOTEOR            2
#define REMOTEREPLACE       3
#define REMOTEGETMEMORY     4
#define REMOTESETUSERCT     5
#define REMOTEGETUSERCT     6
#define REMOTEGETDATE       7
#define REMOTEINVALID       8

#define RotateLeft(a, n) (((a >> (16 - n)) & 0xFFFF) | ((a & 0xFFFF) << n))

/* OPTION SELECTIONS */
#define NOTPASSED         0
#define PASSED         1



/* FUNCTION PROTOTYPES */

#ifdef __cplusplus
#ifdef KL_DLL
	extern "C" unsigned long int __stdcall KFUNC(int, int, int, int);
	extern "C" unsigned int __stdcall far GETLASTKEYERROR(void);
	extern "C" unsigned __stdcall KEYBD(int);
#else
	extern "C" unsigned long int __cdecl KFUNC(int, int, int, int);
	extern "C" unsigned int __cdecl far GETLASTKEYERROR(void);
	extern "C" unsigned __cdecl KEYBD(int);
#endif

#else

#ifdef KL_DLL
	extern unsigned long int __stdcall KFUNC(int, int, int, int);
    extern unsigned int __stdcall GETLASTKEYERROR(void);
	extern unsigned __stdcall KEYBD(int);
#else
	extern unsigned long int __cdecl KFUNC(int, int, int, int);
    extern unsigned int __cdecl GETLASTKEYERROR(void);
	extern unsigned __cdecl KEYBD(int);
#endif

#endif

/********************************* KTASK **********************************/
extern unsigned ReturnValue1, ReturnValue2, Value;

inline unsigned long int KTASK(int CommandCode, int Argument2, int Argument3, int Argument4, HWND hWnd)
{
  unsigned long int ReturnValue;

  // Only enable the next line of code when you are through debugging from within
  // the 32 bit IDE and are ready to test your program in a compiled .EXE format from
  // outside of the IDE.
  //KEYBD(LAUNCHANTIDEBUGGER); // Launch anti-debugging utility PPMON.EXE
  
  ReturnValue =  KFUNC(CommandCode, Argument2, Argument3, Argument4);
  ReturnValue1 = (int)ReturnValue & 0XFFFF;
  ReturnValue2 = (int)(ReturnValue >> 16);
 
//   if (CommandCode != TERMINATE)
//     ShowLastKeyError(hWnd);          /* Display any errors from KTASK call */
  return ReturnValue;
}

#endif // __KEYLOK_H__
