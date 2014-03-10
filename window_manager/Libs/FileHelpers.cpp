#include "stdafx.h"


#include <algorithm>
#include <set>

#include "FileHelpers.h"
#include "FileOperation.h"


static void staticPathList(std::list<CString>& paths, std::set<CString>& folders, const CString& startDir, const CString& folder, BOOL recursive)
{
	CString strDirectory = PathJoin(startDir, folder);

	CFileFind finder;	
	BOOL bWorking = finder.FindFile(strDirectory + _T("\\*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		CString filename = finder.GetFileName();

		if (filename == "." || filename == "..")
			continue;

		if (finder.IsDirectory())
		{
			if (recursive)
			{
				staticPathList(paths, folders, startDir, PathJoin(folder, filename), recursive);
			}
			folders.insert(PathJoin(folder, filename));
		}
		else
		{
			paths.push_back(PathJoin(folder, filename));
		}
	}
}


std::list<CString> GetFileList(const CString& folder, BOOL recursive)
{
	CString strFolder = folder;

	std::list<CString> ret;
	std::set<CString> folders;

	staticPathList(ret, folders, folder, _T("."), recursive);
	return ret;
}

std::list<CString> GetDirectoryList(const CString& folder, BOOL recursive)
{
	CString strFolder = folder;

	std::list<CString> ret;
	std::set<CString> folders;

	staticPathList(ret, folders, folder, _T("."), recursive);
	ret.clear();
	std::copy(folders.begin(), folders.end(), std::back_inserter(ret));
	return ret;
}
