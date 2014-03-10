// video_capture.h

#include <dshow.h>
#include <string>
#include <list> 

#ifndef _VIDEO_CAPTURE_H_
#define _VIDEO_CAPTURE_H_


struct TYPE_DEVICE_NAME
{
	std::string			FrendlyName;
	std::string			DevicePath;
};

class TYPE_DEVICE_LIST : public std::list<TYPE_DEVICE_NAME>
{
public:
	void AddDevice(TYPE_DEVICE_NAME Device);
	TYPE_DEVICE_NAME GetDevice(int Index);

protected:
	BOOL CheckDeviceName();
};

IBaseFilter *
FindCaptureDevice(std::string *p_devicepath,
				  TYPE_DEVICE_LIST *p_listdevices, BOOL b_audio);

#endif //_VIDEO_CAPTURE_H_
