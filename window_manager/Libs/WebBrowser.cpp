//////////////////////////////////////////////////////////////
// File:		// WebBrowser.cpp
// File time:	// 14.02.2006	18:53
// Description: //////////////////////////////////////////////
// 
//

#include "stdafx.h"
//#include <windows.h>
#include "WebBrowser.h"

//////////////////////////////////////////////////////////////

CString ResourceToURL(LPCTSTR lpszResourcePath)
{
	// This functions shows how to convert an URL to point
	// within the application
	// I only use it once to get the startup page

	CString strURL;
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	TCHAR lpszModule[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strURL.Format(_T("res://%s/%s"), lpszModule, lpszResourcePath);
	}

	return strURL;
}

//////////////////////////////////////////////////////////////

CWebBrowserExtensions::CWebBrowserExtensions( CWebBrowser2* pWebBrowser )
	:m_Browser(NULL), m_BrowserDispatch(NULL)
{
//	TRACE( _T("CWebBrowserExtensions created from CWebBrowser2\n") );

	if( pWebBrowser != NULL )
	{
		SetBrowser( pWebBrowser );
	}
}

//////////////////////////////////////////////////////////////

CWebBrowserExtensions::CWebBrowserExtensions( CHtmlView* pHtmlView )
	:m_Browser(NULL), m_BrowserDispatch(NULL)
{
//	TRACE( _T("CWebBrowserExtensions created from CHtmlView\n") );

	if( pHtmlView != NULL )
	{
		SetBrowser( pHtmlView );
	}
}

//////////////////////////////////////////////////////////////

CWebBrowserExtensions::CWebBrowserExtensions( IWebBrowser2* pBrowser )
	:m_Browser(NULL), m_BrowserDispatch(NULL)
{
	if( pBrowser == NULL )
	{
//		TRACE( _T("CWebBrowserExtensions created\n") );
	}
	else
	{
//		TRACE( _T("CWebBrowserExtensions created from IWebBrowser2\n") );
	}

	if( pBrowser != NULL )
	{
		SetBrowser( pBrowser );
	}
}

//////////////////////////////////////////////////////////////

CWebBrowserExtensions::~CWebBrowserExtensions()
{
//	TRACE( _T("CWebBrowserExtensions destroyed\n") );

	if (m_Browser != NULL)
	{
		m_Browser->Release();
		m_Browser = NULL;
	}

	if (m_BrowserDispatch != NULL)
	{
		m_BrowserDispatch->Release();
		m_BrowserDispatch = NULL;
	}
}

//////////////////////////////////////////////////////////////

BOOL CWebBrowserExtensions::SetBrowser( CWebBrowser2* pWebBrowser )
{
	BOOL res = SetBrowserWnd( pWebBrowser );
	return res;
}

//////////////////////////////////////////////////////////////

BOOL CWebBrowserExtensions::SetBrowser( CHtmlView* pHtmlView )
{
	BOOL res = SetBrowserWnd( &pHtmlView->m_wndBrowser );
	return res;
}

//////////////////////////////////////////////////////////////

BOOL CWebBrowserExtensions::SetBrowserWnd( CWnd* pWnd )
{
    LPUNKNOWN unknown = pWnd->GetControlUnknown();
	if( unknown == NULL )
	{
		return FALSE;
	}

	IWebBrowser2* pBrowser = NULL;
	HRESULT hr = unknown->QueryInterface(IID_IWebBrowser2,(void **)&pBrowser);
	if (SUCCEEDED(hr))
	{
		BOOL res = SetBrowser( pBrowser );
		return res;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////

BOOL CWebBrowserExtensions::SetBrowser( IWebBrowser2* pBrowser )
{
	if( pBrowser == NULL )
	{
		return FALSE;
	}

	HRESULT hr;
	m_Browser = pBrowser;
	hr = pBrowser->QueryInterface(IID_IDispatch,(void **)&m_BrowserDispatch);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////

IHTMLDocument2 *CWebBrowserExtensions::GetDocument()
{
	IHTMLDocument2 *document = NULL;
	
	if (m_Browser != NULL) {
	
		// get browser document's dispatch interface

		IDispatch *document_dispatch = NULL;

		HRESULT hr = m_Browser->get_Document(&document_dispatch);

		if (SUCCEEDED(hr) && (document_dispatch != NULL)) {

			// get the actual document interface

			hr = document_dispatch->QueryInterface(IID_IHTMLDocument2,
			                                       (void **)&document);

			// release dispatch interface
					
			document_dispatch->Release();
		
		}
		
	}
	
	return document;
}

//////////////////////////////////////////////////////////////

// Append string to current document

void CWebBrowserExtensions::Write(LPCTSTR string)
{
	if (m_Browser != NULL) {

		// get document interface

		IHTMLDocument2 *document = GetDocument();
		
		if (document != NULL) {

			// construct text to be written to browser as SAFEARRAY

			SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT,0,1);
			
			VARIANT	*variant;
			
			SafeArrayAccessData(safe_array,(LPVOID *)&variant);
			
			variant->vt      = VT_BSTR;
			variant->bstrVal = CString(string).AllocSysString();
			
			SafeArrayUnaccessData(safe_array);

			// write SAFEARRAY to browser document

			document->write(safe_array);
			
			document->Release();
			document = NULL;

		}

	}
}

//////////////////////////////////////////////////////////////

BOOL CWebBrowserExtensions::insertAdjacentHTML(
			LPCTSTR where, LPCTSTR tagid, LPCTSTR string)
{
	MSHTML::IHTMLDocument2Ptr ptrHtmlDoc = GetDocumentPtr();
	if( ptrHtmlDoc == NULL )
	{
		return FALSE;
	}
	MSHTML::IHTMLElementCollectionPtr ptrColl = ptrHtmlDoc->all;
	if( ptrColl == NULL )
	{
		return FALSE;
	}
	MSHTML::IHTMLElementPtr ptrElement = ptrColl->item( tagid, (long)0 );
	if( ptrElement == NULL )
	{
		return FALSE;
	}

	HRESULT hr = ptrElement->insertAdjacentHTML( where, string );
	if( FAILED(hr) )
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////

BOOL CWebBrowserExtensions::replaceInnerHTML( LPCTSTR tagid, LPCTSTR string)
{
	MSHTML::IHTMLDocument2Ptr ptrHtmlDoc = GetDocumentPtr();
	if( ptrHtmlDoc == NULL )
	{
		return FALSE;
	}
	MSHTML::IHTMLElementCollectionPtr ptrColl = ptrHtmlDoc->all;
	if( ptrColl == NULL )
	{
		return FALSE;
	}
	MSHTML::IHTMLElementPtr ptrElement = ptrColl->item( tagid, (long)0 );
	if( ptrElement == NULL )
	{
		return FALSE;
	}

	ptrElement->innerHTML = string;

	return TRUE;
}

//////////////////////////////////////////////////////////////

_bstr_t CWebBrowserExtensions::getInnerHTML( LPCTSTR tagid )
{
	MSHTML::IHTMLDocument2Ptr ptrHtmlDoc = GetDocumentPtr();
	if( ptrHtmlDoc == NULL )
	{
		return _T("");
	}
	MSHTML::IHTMLElementCollectionPtr ptrColl = ptrHtmlDoc->all;
	if( ptrColl == NULL )
	{
		return _T("");
	}
	MSHTML::IHTMLElementPtr ptrElement = ptrColl->item( tagid, (long)0 );
	if( ptrElement == NULL )
	{
		return _T("");
	}

	return ptrElement->innerHTML;
}

//////////////////////////////////////////////////////////////

void CWebBrowserExtensions::Clear()
{
	if (m_Browser != NULL) {

		// if document interface available, close/re-open document to clear display

		IHTMLDocument2	*document	= GetDocument();
		HRESULT			hr			= S_OK;
		
		if (document != NULL) {

			// close and re-open document to empty contents

			Close();
			//hr = document->close();
			
			VARIANT		open_name;
			VARIANT		open_features;
			VARIANT		open_replace;
			IDispatch	*open_window	= NULL;

			::VariantInit(&open_name);

			BSTR bstrUrl = ::SysAllocString(L"text/html");

			open_name.vt      = VT_BSTR;
			open_name.bstrVal = ::SysAllocString(L"_self");
//			open_name.bstrVal = ::SysAllocString(L"replace");

			::VariantInit(&open_features);
			::VariantInit(&open_replace);

			hr = document->open(bstrUrl,
			                    open_name,
			                    open_features,
			                    open_replace,
			                    &open_window);

			if (hr == S_OK)
			{
				Refresh();
			}

			if (open_window != NULL) {
				open_window->Release();
			}

		}

		// otherwise, navigate to about:blank and wait for document ready

		else {

			Navigate(_T("about:blank"));

			IHTMLDocument2 *document = NULL;
			HRESULT			hr       = S_OK;

			while ((document == NULL) && (hr == S_OK)) {

				Sleep(0);

				IDispatch *document_dispatch = NULL;

				hr = m_Browser->get_Document(&document_dispatch);

				// if dispatch interface available, retrieve document interface
				
				if (SUCCEEDED(hr) && (document_dispatch != NULL)) {

					// retrieve document interface
					
					hr = document_dispatch->QueryInterface(IID_IHTMLDocument2,(void **)&document);

					document_dispatch->Release();

				}
				
			}
			
			if (document != NULL) {
				document->Release();	
			}

		}
		
	}
}

//////////////////////////////////////////////////////////////

void CWebBrowserExtensions::Close()
{
	if (m_Browser != NULL)
	{
		IHTMLDocument2	*document	= GetDocument();
		HRESULT			hr			= S_OK;
		
		if (document != NULL)
		{
			// close document

			hr = document->close();
		}	
	}
}

//////////////////////////////////////////////////////////////

void CWebBrowserExtensions::Refresh()
{
	if (m_Browser != NULL)
	{
		m_Browser->Refresh();
	}
}

//////////////////////////////////////////////////////////////
// Navigate to URL

void CWebBrowserExtensions::Navigate(LPCTSTR URL)
{
	TRACE( _T("CWebBrowserExtensions::Navigate( \"%s\" )\n"), URL );

	if (m_Browser != NULL)
	{
		CString		url(URL);

		_variant_t	flags(0L,VT_I4);
		_variant_t	target_frame_name("");
		_variant_t	post_data("");
		_variant_t	headers("");

		m_Browser->Navigate(url.AllocSysString(),
						   &flags,
						   &target_frame_name,
						   &post_data,
						   &headers);

	}
}

//////////////////////////////////////////////////////////////

void CWebBrowserExtensions::NavigateResource(LPCTSTR szResourceName, LPCTSTR szResurceType )
{
	CString sResource, sResourceName, sResourceType;

	if( szResurceType == NULL )
	{
		sResourceType.Empty();
	}
	else if( HIWORD(szResurceType) == 0 )
	{
		sResourceType.Format( _T("#%d/"), szResurceType );
	}
	else
	{
		sResourceType = szResurceType + _T('/');
	}

	if( HIWORD(szResourceName) == 0 )
	{
		sResourceName.Format( _T("#%d"), szResourceName );
	}
	else
	{
		sResourceName = szResourceName;
	}

	sResource = sResourceType + sResourceName;

	CString sUrl = ResourceToURL( szResourceName );

	TRACE( _T("CWebBrowserExtensions::Navigate -> %s\n"), sUrl );

	Navigate( sUrl );
}

//////////////////////////////////////////////////////////////

CString GetSystemErrorMessage(DWORD dwError)
{
	CString strError;
	LPTSTR lpBuffer;

	if(!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,  dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTSTR) &lpBuffer, 0, NULL))

	{
		strError = "FormatMessage Native Error" ;
	}
	else
	{
		strError = lpBuffer;
		LocalFree(lpBuffer);
	}
	return strError;
}

//////////////////////////////////////////////////////////////

void ShowError(LPCTSTR lpszText)
{
	CString m_strError = "Applicaion script error:\n" + CString(lpszText);
	AfxMessageBox( lpszText, MB_ICONSTOP );
}

//////////////////////////////////////////////////////////////

#include <dispex.h>

//////////////////////////////////////////////////////////////

void TraceDispatch(IDispatchPtr spDispatch)
{
	DISPID dispid = NULL;
	HRESULT hr;
	IDispatchExPtr spDispatchEx = spDispatch;

	BSTR bstrName;

	// Assign to pdex
	hr = spDispatchEx->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
	while (hr == NOERROR)
	{
		hr = spDispatchEx->GetMemberName(dispid, &bstrName);
		TRACE( _T("member (%11d%s): %s\n"),
			(0x7fffffff&dispid),
			( (0x80000000&dispid) ? _T("x") : _T(" ") ),
			CString( bstrName ) );

		SysFreeString(bstrName);
		hr = spDispatchEx->GetNextDispID(fdexEnumAll, dispid, &dispid);
	}
}

//////////////////////////////////////////////////////////////

bool CWebBrowserExtensions::CallJScript(const CString strFunc, const CStringArray& paramArray,CString *sResult)
{
	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	HRESULT hr;
	int i;

	IDispatchPtr spScript = GetDocumentPtr()->Script;

	hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
								LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
		//ShowError(GetSystemErrorMessage(hr));
		TRACE( _T("ERROR in CallJScript(%s), line: %d: %s\n"),
			strFunc, __LINE__, GetSystemErrorMessage(hr) );
		return false;
	}

	const int arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];

	for( i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg
     
	hr = spScript->Invoke(dispid,IID_NULL,0,
							DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
//		ShowError(GetSystemErrorMessage(hr));
		TRACE( _T("ERROR in CallJScript(%s), line: %d: %s\n"),
			strFunc, __LINE__, GetSystemErrorMessage(hr) );
		return false;
	}

	if(sResult)
	{
		if (vaResult.vt == VT_BSTR )
			//sResult->SetSysString(&vaResult.bstrVal);
			*sResult = vaResult.bstrVal;
		else
			sResult->Empty();
	}
	return true;
}

//////////////////////////////////////////////////////////////

bool CWebBrowserExtensions::CallJScript2(const CString strFunc, const CStringArray& paramArray, CString *sResult)
{
	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	IDispatchPtr spScript = GetDocumentPtr()->Script;

	HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
											LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
//		ShowError(GetSystemErrorMessage(hr));
		TRACE( _T("ERROR in CallJScript(%s), line: %d: %s\n"),
			strFunc, __LINE__, GetSystemErrorMessage(hr) );
		return false;
	}

	const int arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	SAFEARRAY * psa;
	VARIANT var;
	long idx[1];        

	memset(&dispparams, 0, sizeof dispparams);
	dispparams.rgvarg = new VARIANT;

	// Creating a SafeArray with 1 element.
   psa = SafeArrayCreateVector( VT_VARIANT, 0, arraySize);
   if(psa == NULL){
      return false;
   }   
	
	for( int i = 0; i < arraySize; i++)
	{
		VariantInit(&var);
		V_VT(&var) = VT_BSTR;
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&var.bstrVal);		
		idx[0]=i;
		hr = SafeArrayPutElement(psa, idx, &var);
		hr = VariantClear(&var); 
		
	}
	dispparams.cNamedArgs = 0;
	dispparams.cArgs = 1;
	dispparams.rgvarg->parray = psa;
    dispparams.rgvarg->vt = VT_ARRAY | VT_VARIANT;   

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
   	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg
         
	hr = spScript->Invoke(dispid,IID_NULL,0,
							DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;
	SafeArrayDestroy(psa);

	if(FAILED(hr))	{
//		ShowError(GetSystemErrorMessage(hr));
		TRACE( _T("ERROR in CallJScript(%s), line: %d: %s\n"),
			strFunc, __LINE__, GetSystemErrorMessage(hr) );
		return false;
	}
	
	if(sResult)	{		
		if (vaResult.vt == VT_BSTR )
			sResult->SetSysString(&vaResult.bstrVal);
		else
			sResult->Empty();
	}
	return true;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
