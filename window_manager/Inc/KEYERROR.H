//**************************************************************************
// KEYERROR.H   -  $Revision: 1.3 $  -  $Date: 1996/01/03 17:48:39 $
//**************************************************************************
//
// KEY-LOK (TM) Security System
// (C) Copyright 1986-1995 - by Microcomputer Applications - All rights reserved
//
// NOTICE:  THIS DOCUMENT CONTAINS CONFIDENTIAL AND PROPRIETARY DATA
// PROTECTED UNDER THE TRADE SECRET PROVISIONS OF FEDERAL AND STATE LAW.
//
//**************************************************************************
//                         MODULE  DESCRIPTION
// Module Name:
// 
//     keyerror.h
// 
// Abstract:
//
//     User-defined error codes set into calling app's
//     GetLastError function.  These can be distinguished from regular
//     windows error codes because bit 29 is set.
//
// Environment:
// 
//     Kernel mode and User mode
//***************************************************************************

#ifndef KEYERROR_H_
#define KEYERROR_H_


//********************************************************************
//************* S T A T U S  and  E R R O R   C O D E S  *************
//********************************************************************
//
// The driver can return various "status and error codes" to the calling
// application by setting the Return DWORD.  (Remember that these status
// returns are not the same as the NT I/O Manager's status returns for
// an IRP.) In may cases, the error return is in the high WORD of reply.
//

// ReturnWord Status Codes specific to KEYTAG KeyLok commands:

#define KEY_STATUS_NOERROR        0
#define KEY_STATUS_BUNOTUPDATED   1
#define KEY_STATUS_KTFLAGPRBLM    2
#define KEY_STATUS_WRONGKT        3
#define KEY_STATUS_WRONGKTSN      4
#define KEY_STATUS_KTNOTERASED    5
#define KEY_STATUS_KTUSED         6
#define KEY_STATUS_WRITETIMEOUT   7        // Not necessarily write failure

// ReturnWord Status codes for Lease Expiration Date:

#ifdef KL2


#ifdef KL1RTC  // 5/22/00 addition for Tracer
 #define KEY_STATUS_LEASEOVER      1
 #define KEY_STATUS_SYSDATESETBK   2
 #define KEY_STATUS_NOLEASEDATE    3
 #define KEY_STATUS_LEASEDATEBAD   4
 #define KEY_STATUS_FSDATEBAD      5
#else  // Standard KL2
 #define KEY_STATUS_LEASEOVER      0xfffe
 #define KEY_STATUS_SYSDATESETBK   0xfffd
 #define KEY_STATUS_NOLEASEDATE    0xfffc
 #define KEY_STATUS_LEASEDATEBAD   0xfffb
 #define KEY_STATUS_FSDATEBAD      0xfffa
#endif // KL1RTC

#else // KL1
#define KEY_STATUS_LEASEOVER      1
#define KEY_STATUS_NORTC          2
#define KEY_STATUS_NOLEASEDATE    3
#define KEY_STATUS_LEASEDATEBAD   4
#define KEY_STATUS_FSDATEBAD      5
#endif //KL2

// ReturnWord Decrement Counter status codes:

#define KEY_STATUS_NOCOUNTSLEFT   1
#define KEY_STATUS_INVALIDADDRESS 2
#define KEY_STATUS_NOWRITEAUTH    3


// Starting with the Win95 version, the 32 bit API can return the error
// code to caller using GetLastError.  System-generated error codes
// are defined in error.h;  User error codes are flagged by setting
// bit 29 in the status word, and the user must parse their meaning.

// GetLastError() returns codes so 32bit api caller can see results:

#define KEY_ERROR_NOERROR         0x00000000
#define KEY_ERROR_NOKEYLOK_ALSO   0x00000002
#define KEY_ERROR_NO_SESSIONS     0x00000005  // Same as ERROR_ACCESS_DENIED
#define KEY_ERROR_NOKEYLOK        0x20000001  // No keylok found on port/lan.
#define KEY_ERROR_BADVERSION      0x20000002  // Driver/kfunc version mismatch
#define KEY_ERROR_BADFUNC         0x20000003  // Illegal kfunc command 
#define KEY_ERROR_WRONGKEYLOK     0x20000004  // Authenticate doesn't match
#define KEY_ERROR_NOREADAUTH      0x20000005  // Read Auth wrong or not done
#define KEY_ERROR_NOWRITEAUTH     0x20000006  // Write Auth wrong or not done
#define KEY_ERROR_INVALIDADDRESS  0x20000007  // Bad KeyLok Device Address
#define KEY_ERROR_NOCOUNTSLEFT    0x20000008  // Decrement Counter already 0
#define KEY_ERROR_WRITETIMEOUT    0x20000009  // D0 did not go high in time
#define KEY_ERROR_NOKEYTAG        0x2000000a  // No keytag present
#define KEY_ERROR_WRONGKT         0x2000000b  // Wrong KeyTag
#define KEY_ERROR_WRONGKTSN       0x2000000c  // Wrong KeyTag Serial Number
#define KEY_ERROR_KTNOTERASED     0x2000000d  // KeyTag not erased
#define KEY_ERROR_KTUSED          0x2000000e  // KeyTag already used once
#define KEY_ERROR_BUNOTUPDATED    0x2000000f  // KeyTag did not update Keylok
#define KEY_ERROR_LEASEOVER       0x20000010  // KL2 Lease Expiration
#define KEY_ERROR_SYSDATESETBK    0x20000011  // KL2 System Date Setback Error
#define KEY_ERROR_NOLEASEDATE     0x20000012  // KL2 No Lease Date   
#define KEY_ERROR_LEASEDATEBAD    0x20000013  // KL2 Lease Date Bad  
#define KEY_ERROR_FSDATEBAD       0x20000014  // KL2 FS Date Bad ??????
// Attempting to perform some other task before doing authentication or after 
// having done TERMINATE will return junk from netbios call
#define KEY_ERROR_LANRETURNSJUNK  0x20000015 
#define KEY_ERROR_DONGLEREMOVED	  0x20000016  // Dongle removed and then replaced

#define KEY_ERROR_COMMTIMEOUT	  0x20000017  // Elapsed time since last dongle check exceeds SessionTimeOutValue
#define KEY_ERROR_NOAUTHENTICATE  0x20000018 

// Error codes related to Real-Time-Clock module within KEYLOK FORTRESS dongle
#define KEY_ERROR_REALTIMECLOCK	  0x20000019	// Error reading RTC module
#define KEY_ERROR_RTCPOWER		  0x20000020	// RTC module powered down

#define KEY_ERROR_WRONGSESSIONID  0x20000207  // Fortress dongle - wrong session ID to dongle

// Remote Update Error Codes
#define KEY_ERROR_NOAUTHORIZEFILE		0x20000101  // Missing AUTHORIZE.DAT file
#define KEY_ERROR_CORRUPTAUTHORIZEFILE	0x20000102	// Corrupt AUTHORIZE.DAT file
#define	KEY_ERROR_WRONGDONGLE			0x20000103  // Dongle SN doesn't match AUTHORIZE.DAT
#define	KEY_ERROR_WRONGRANDOMVALUE		0x20000104  // Dongle Random Value doesn't match AUTHORIZE.DAT
#define	KEY_ERROR_WRONGFILEVERSION		0x20000105  // Last remote update file version processed
													// and stored within the dongle is not 
													// compatible with the file version identifier
                                                    // stored within the ACTIVATE.DAT file

#endif // KEYERROR_H_

