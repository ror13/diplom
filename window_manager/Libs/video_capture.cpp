// video_capture.cpp

#include "stdafx.h"
#include "video_capture.h"

#pragma comment(lib, "Strmiids.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void TYPE_DEVICE_LIST::AddDevice(TYPE_DEVICE_NAME Device)
{
	push_back(Device);
}

TYPE_DEVICE_NAME TYPE_DEVICE_LIST::GetDevice(int Index)
{
	iterator i = begin();
	while(i != end() && Index)
	{
		Index--;
		i++;
	}
	if(i != end())
		return *i;
	else
		return TYPE_DEVICE_NAME();
}


IBaseFilter *
FindCaptureDevice(std::string *p_devicepath,
				  TYPE_DEVICE_LIST *p_listdevices, BOOL b_audio)
{
    IBaseFilter *p_base_filter = NULL;
    IMoniker *p_moniker = NULL;
    ULONG i_fetched;
    HRESULT hr;

    /* Create the system device enumerator */
    ICreateDevEnum *p_dev_enum = NULL;

    hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **)&p_dev_enum );
    if( FAILED(hr) )
    {
        TRACE( _T("failed to create the device enumerator (0x%lx)"), hr);
        return NULL;
    }

    /* Create an enumerator for the video capture devices */
    IEnumMoniker *p_class_enum = NULL;
    if( !b_audio )
        hr = p_dev_enum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory,
                                                &p_class_enum, 0 );
    else
        hr = p_dev_enum->CreateClassEnumerator( CLSID_AudioInputDeviceCategory,
                                                &p_class_enum, 0 );
    p_dev_enum->Release();
    if( FAILED(hr) )
    {
        TRACE( _T("failed to create the class enumerator (0x%lx)"), hr );
        return NULL;
    }

    /* If there are no enumerators for the requested type, then
     * CreateClassEnumerator will succeed, but p_class_enum will be NULL */
    if( p_class_enum == NULL )
    {
        TRACE( _T("no capture device was detected") );
        return NULL;
    }

    /* Enumerate the devices */

    /* Note that if the Next() call succeeds but there are no monikers,
     * it will return S_FALSE (which is not a failure). Therefore, we check
     * that the return code is S_OK instead of using SUCCEEDED() macro. */

    while( p_class_enum->Next( 1, &p_moniker, &i_fetched ) == S_OK )
    {
        /* Getting the property page to get the device name */
        IPropertyBag *p_bag;
        hr = p_moniker->BindToStorage( 0, 0, IID_IPropertyBag,
                                       (void **)&p_bag );
        if( SUCCEEDED(hr) )
        {
            VARIANT var_frendly_name;
            var_frendly_name.vt = VT_BSTR;
            HRESULT hr1 = p_bag->Read( L"FriendlyName", &var_frendly_name, NULL );

            VARIANT var_device_path;
            var_device_path.vt = VT_BSTR;
            HRESULT hr2 = p_bag->Read( L"DevicePath", &var_device_path, NULL );
            
			p_bag->Release();

            if( SUCCEEDED(hr1) && SUCCEEDED(hr2) )
            {
                int i_convert = WideCharToMultiByte(CP_ACP, 0, var_frendly_name.bstrVal,
                        SysStringLen(var_frendly_name.bstrVal), NULL, 0, NULL, NULL);

                char *p_buf_frendly_name = (char *)alloca( i_convert+1 );
				p_buf_frendly_name[0] = 0;

                WideCharToMultiByte( CP_ACP, 0, var_frendly_name.bstrVal,
                        SysStringLen(var_frendly_name.bstrVal), p_buf_frendly_name,
						i_convert, NULL, NULL );
                SysFreeString(var_frendly_name.bstrVal);

                p_buf_frendly_name[i_convert] = '\0';

                i_convert = WideCharToMultiByte(CP_ACP, 0, var_device_path.bstrVal,
                        SysStringLen(var_device_path.bstrVal), NULL, 0, NULL, NULL);

                char *p_buf_device_path = (char *)alloca( i_convert+1 );
				p_buf_device_path[0] = 0;

                WideCharToMultiByte( CP_ACP, 0, var_device_path.bstrVal,
                        SysStringLen(var_device_path.bstrVal), p_buf_device_path,
						i_convert, NULL, NULL );
                SysFreeString(var_device_path.bstrVal);

                p_buf_device_path[i_convert] = '\0';

                if( p_listdevices )
				{
					TYPE_DEVICE_NAME device_name;
					device_name.FrendlyName = p_buf_frendly_name;
					device_name.DevicePath = p_buf_device_path;
					p_listdevices->AddDevice( device_name );
				}

                if( p_devicepath && *p_devicepath == std::string(p_buf_device_path) )
                {
                    /* Bind Moniker to a filter object */
                    hr = p_moniker->BindToObject( 0, 0, IID_IBaseFilter,
                                                  (void **)&p_base_filter );
                    if( FAILED(hr) )
                    {
                        TRACE( _T("couldn't bind moniker to filter ")
                                 _T("object (0x%lx)"), hr );
                        p_moniker->Release();
                        p_class_enum->Release();
                        return NULL;
                    }
                    p_moniker->Release();
                    p_class_enum->Release();
                    return p_base_filter;
                }
            }
        }

        p_moniker->Release();
    }

    p_class_enum->Release();
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
// unit tests

#ifdef _DEBUG
/*
using namespace std;

class CFindCaptureDeviceTest: public CppUnit::TestFixture
{
private:
public:
	void setUp()
	{
	}
	
	void testFindCaptureDevice()
	{
		CoInitialize(0);

		list<string> device_list;
		FindCaptureDevice(NULL, &device_list, FALSE);

		CoUninitialize();

		int c = device_list.size();
		printf("\nCapture Devices count=%d\n", c);

		list<string>::iterator i;
		for(i = device_list.begin(); i != device_list.end(); i++)
			printf("Capture Device: %s\n", (*i).c_str());
	}

	void test()
	{
		CPPUNIT_ASSERT_MESSAGE("Important Message", 1);
		CPPUNIT_ASSERT(1);
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CFindCaptureDeviceTest);
	CPPUNIT_TEST(testFindCaptureDevice);
	CPPUNIT_TEST_SUITE_END();
	
};
CPPUNIT_TEST_SUITE_REGISTRATION(CFindCaptureDeviceTest);
*/
#endif // _DEBUG
