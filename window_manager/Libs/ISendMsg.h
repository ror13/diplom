#pragma once

#include "Request.h"

class ISendMsg
{
	public:
		virtual BOOL SendMsg(CRequest &reqOut) = 0;
};

