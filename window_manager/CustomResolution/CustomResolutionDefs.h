// CustomResolution.h : main header file for the CUSTOMRESOLUTION DLL
//

#if !defined(AFX_CUSTOMRESOLUTIONDEFS_H__0B2D3D75_EDF5_4936_8CE7_3D370E09142B__INCLUDED_)
#define AFX_CUSTOMRESOLUTIONDEFS_H__0B2D3D75_EDF5_4936_8CE7_3D370E09142B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifdef CUSTOMRESOLUTION_EXPORTS
#ifdef _USRDLL
#define CUSTOMRESOLUTION_API __declspec(dllexport)
#else
#define CUSTOMRESOLUTION_API __declspec(dllimport)
#endif

#endif // !defined(AFX_CUSTOMRESOLUTIONDEFS_H__0B2D3D75_EDF5_4936_8CE7_3D370E09142B__INCLUDED_)
