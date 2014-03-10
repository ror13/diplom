#ifndef _ReportCameraConfig_H_UID0000000BDC564B3A
#define _ReportCameraConfig_H_UID0000000BDC564B3A

/////////////////////////////////////////////////////////////

#include "product.h"

#include "SerializeStruct.h"
//////////////////////////////////////////////////////////////////////////

enum E_REPORT_CAMERA_ACTION
{
	REPORT_CAMERA_NONE,
	REPORT_CAMERA_SHOOT_PHOTO,
	REPORT_CAMERA_RECORD_VIDEO,
	
	REPORT_CAMERA_ACTION_COUNT
};

class CReportCameraConfig : public CSerializeStruct
{
public:

	int				m_Action;
	
	CString			m_CaptureDevicePath;
	
	int				m_VideoSizeWidth;
	int				m_VideoSizeHeight;
	
	int				m_iShootPhotoEvery;
	int				m_iReportsSizeLimit;
	

	CReportCameraConfig();
	CReportCameraConfig(const CReportCameraConfig& src);

	CReportCameraConfig& operator =(const CReportCameraConfig &src);
	
	BOOL operator !=(const CReportCameraConfig &src);
	BOOL operator ==(const CReportCameraConfig &src);
	
	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );
#ifndef COMPILE_DL_PRESENTER
	virtual void LuaSerialize(CLua &L);
#endif // !COMPILE_DL_PRESENTER
#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
	virtual BOOL PHPSerialize(BOOL GetData, zval *val);
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER
};

/////////////////////////////////////////////////////////////

#endif //ifndef _ReportCameraConfig_H_UID0000000BDC564B3A
