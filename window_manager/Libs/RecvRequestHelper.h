// RecvRequestHelper.h

#ifndef __RECVREQUESTHELPER_H__
#define __RECVREQUESTHELPER_H__

#include "Request.h"

#ifdef _DEBUG
void _hlpTRACERecvRequest(CString Title, CRequest req);
#define TRACERecvRequest(t, x)	_hlpTRACERecvRequest(t, x);
//#define TRACERecvRequest(t, x)
#else
#define TRACERecvRequest(t, x)
#endif

#endif // __RECVREQUESTHELPER_H__