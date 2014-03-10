#pragma once


#include <list>


std::list<CString> GetFileList(const CString& folder, BOOL recursive);
std::list<CString> GetDirectoryList(const CString& folder, BOOL recursive);