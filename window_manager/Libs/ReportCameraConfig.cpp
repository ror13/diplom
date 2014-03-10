#include "stdafx.h"
//#include <windows.h>

#include "ReportCameraConfig.h"

//////////////////////////////////////////////////////////////////////////

CReportCameraConfig::CReportCameraConfig()
{
	m_Action = REPORT_CAMERA_SHOOT_PHOTO;
	
	m_CaptureDevicePath = _T("");
	
	m_VideoSizeWidth = 0;
	m_VideoSizeHeight = 0;
	
	m_iShootPhotoEvery = 10;
	m_iReportsSizeLimit = 30;
	
}

CReportCameraConfig::CReportCameraConfig(const CReportCameraConfig &src)
{
	*this = src;
}

CReportCameraConfig& CReportCameraConfig::operator =(const CReportCameraConfig &src)
{
	m_Action = src.m_Action;
	
	m_CaptureDevicePath = src.m_CaptureDevicePath;
	
	m_VideoSizeWidth = src.m_VideoSizeWidth;
	m_VideoSizeHeight = src.m_VideoSizeHeight;
	
	m_iShootPhotoEvery = src.m_iShootPhotoEvery;
	m_iReportsSizeLimit = src.m_iReportsSizeLimit;
	
	return *this;
}

BOOL CReportCameraConfig::operator !=(const CReportCameraConfig &src)
{
	BOOL res = ((*this) == src);
	return !res;
}

BOOL CReportCameraConfig::operator ==(const CReportCameraConfig &src)
{
	BOOL res = 
		m_Action == src.m_Action &&
	
		m_CaptureDevicePath == src.m_CaptureDevicePath &&
		
		m_VideoSizeWidth == src.m_VideoSizeWidth && 
		m_VideoSizeHeight == src.m_VideoSizeHeight &&
		
		m_iShootPhotoEvery == src.m_iShootPhotoEvery &&
		m_iReportsSizeLimit == src.m_iReportsSizeLimit;
		
	return res;
}

BOOL CReportCameraConfig::Serialize(SERIALIZE_CONTEXT& context)
{
	SERIALIZE_START
		SERIALIZE_INT( int, m_Action,					_T("Action"))

		SERIALIZE_STRING(m_CaptureDevicePath,	_T("CaptureDevicePath"))

		SERIALIZE_INT( int, m_VideoSizeWidth,			_T("VideoSizeWidth"))
		SERIALIZE_INT( int, m_VideoSizeHeight,		_T("VideoSizeHeight"))

		SERIALIZE_INT( int, m_iShootPhotoEvery,		_T("ShootPhotoEvery"))
		SERIALIZE_INT( int, m_iReportsSizeLimit,		_T("ReportsSizeLimit"))
	SERIALIZE_END
}

#ifndef COMPILE_DL_PRESENTER
void CReportCameraConfig::LuaSerialize(CLua &L)
{
	LUA_SERIALIZE(L, _T("m_Action"),	m_Action);
	LUA_SERIALIZE(L, _T("m_CaptureDevicePath"), m_CaptureDevicePath);
	LUA_SERIALIZE(L, _T("m_VideoSizeWidth"),	m_VideoSizeWidth);
	LUA_SERIALIZE(L, _T("m_VideoSizeHeight"),	m_VideoSizeHeight);
	LUA_SERIALIZE(L, _T("m_iShootPhotoEvery"),	m_iShootPhotoEvery);
	LUA_SERIALIZE(L, _T("m_iReportsSizeLimit"),	m_iReportsSizeLimit);
/*	L.AddKey(_T("FileName"),	m_MediaFileName);
	L.AddKey(_T("MD5Signature"),m_MD5Signature);
	L.AddKey(_T("FileSize"),	m_MediaFileSize);	*/	
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CReportCameraConfig::PHPSerialize(BOOL GetData, zval *val)
{
	PHP_SERIALIZE_START
		PHP_SERIALIZE_INT(m_Action,					_T("Action"))
		
		PHP_SERIALIZE_STRING(m_CaptureDevicePath,	_T("CaptureDevicePath"))
		
		PHP_SERIALIZE_INT(m_VideoSizeWidth,			_T("VideoSizeWidth"))
		PHP_SERIALIZE_INT(m_VideoSizeHeight,		_T("VideoSizeHeight"))
		
		PHP_SERIALIZE_INT(m_iShootPhotoEvery,		_T("ShootPhotoEvery"))
		PHP_SERIALIZE_INT(m_iReportsSizeLimit,		_T("ReportsSizeLimit"))
	PHP_SERIALIZE_END
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

