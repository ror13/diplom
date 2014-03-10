// template_parser.cpp
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FileOperation.h"
#include "md5.h"

#include "TemplateFile.h"
#include "template_parser.h"

#include "md5_util.h"

//////////////////////////////////////////////////////////////////////////
// util function for html parser

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])

#define START_COMMENT_BLOCK		_T("<!--")
#define END_COMMENT_BLOCK		_T("-->")

// remove comments
void StripComment(LPTSTR Buffer)
{
	LPTSTR StartComment = Buffer;
	while((StartComment = _tcsstr(StartComment, START_COMMENT_BLOCK)) != NULL)
	{
		// find end of comment block
		LPTSTR EndComment = _tcsstr(StartComment, END_COMMENT_BLOCK);
		if(EndComment != NULL)
		{
			EndComment += lstrlen(END_COMMENT_BLOCK);
			
			lstrcpy(StartComment, EndComment);
		} else
		{
			*StartComment = _T('\0');
			break;
		}
	}
}

void StripLeft(LPTSTR Buffer)
{
	// strip begin white spaces and tabs
	int iLen = lstrlen(Buffer);
	int iPos = 0;
	while(iLen-- > 0)
	{
		if(Buffer[iPos] != _T(' ') && Buffer[iPos] != _T('\t'))
			break;
		
		iPos++;
	}
	
	if(iPos > 0)
		lstrcpy(Buffer, Buffer + iPos);
}

void StripRight(LPTSTR Buffer)
{
	// strip end white spaces, tabs and \r
	int iLen = lstrlen(Buffer);
	while(iLen-- > 0)
	{
		if( Buffer[iLen] != _T(' ') &&
			Buffer[iLen] != _T('\t') &&
			Buffer[iLen] != _T('\r')
			)
			break;
	}
	Buffer[iLen + 1] = _T('\0');
}

void StripLine(LPTSTR Buffer)
{
	StripLeft(Buffer);
	StripRight(Buffer);
}

void StripHtml(LPTSTR Buffer)
{
	LPTSTR BeginLine = Buffer;
	LPTSTR EndLine;
	while((EndLine = _tcsstr(BeginLine, _T("\n"))) != NULL)
	{
		LPTSTR NextLine = CharNext(EndLine);
		
		*EndLine = _T('\0');
		
		StripLine(BeginLine);
		
		int StrippedLineSize = lstrlen(BeginLine);
		
		if(StrippedLineSize > 0)
		{
			EndLine = BeginLine + StrippedLineSize + 1;
			
			if(EndLine < NextLine)
			{
				lstrcat(BeginLine, _T(" "));
				lstrcat(BeginLine, NextLine);
			} else
				BeginLine[StrippedLineSize] = _T(' ');
		} else
		{
			EndLine = BeginLine;
			lstrcat(BeginLine, NextLine);
		}
		
		BeginLine = EndLine;
	}
	
	if(lstrlen(BeginLine) > 0)
		StripLine(BeginLine);
}

//////////////////////////////////////////////////////////////////////////
// CHtmlTagParser

#define START_TAG			_T("<")
#define END_TAG				_T(">")

#define START_SCRIPT_TAG	_T("SCRIPT")
#define END_SCRIPT_TAG		_T("/SCRIPT")

BOOL CHtmlTagParser::Parse(LPTSTR Buffer)
{
	// remove comments
	StripComment(Buffer);

	// remove spaces
	StripHtml(Buffer);

	BOOL bFindScriptEnd = FALSE;
	
	LPTSTR TagEnd = Buffer;
	LPTSTR TagBegin;
	do
	{
		while((TagBegin = _tcsstr(TagEnd, START_TAG)) != NULL)
		{
			LPTSTR TagName = CharNext(TagBegin);
			StripLeft(TagName);
			if( !(
				(TagName[0]>=_T('a') && TagName[0]<=_T('z')) ||
				(TagName[0]>=_T('A') && TagName[0]<=_T('Z')) ||
				(TagName[0]==_T('!')) ||
				(TagName[0]==_T('/')) )
				)
			{
				TagEnd = TagName;
				continue;
			}

			if(bFindScriptEnd)
			{
				TCHAR TagNameBuf[20];
				lstrcpyn(TagNameBuf, TagName, lstrlen(END_SCRIPT_TAG)+1);
				if(lstrcmpi(TagNameBuf, END_SCRIPT_TAG) == 0)
					bFindScriptEnd = FALSE;
				else
				{
					TagEnd = TagName;
					continue;
				}
			}
			
			TagEnd = _tcsstr(TagBegin, END_TAG);
			if(TagEnd == NULL)
				break;
			
			LPTSTR TagParams = _tcsstr(TagBegin, _T(" "));
			
			TCHAR Name[256];
			TCHAR Params[1024];
			if(TagParams != NULL && TagParams < TagEnd)
			{
				lstrcpyn(Name, TagName, TagParams - TagName + 1);
				lstrcpyn(Params, TagParams, TagEnd - TagParams + 1);
			} else
			{
				lstrcpyn(Name, TagName, TagEnd - TagName + 1);
				lstrcpy(Params, _T(""));
			}
			
			StripLine(Name);
			StripLine(Params);
			
			OnHtmlTag(Name, Params);

			if(lstrcmpi(Name, START_SCRIPT_TAG) == 0)
				bFindScriptEnd = TRUE;
		}
	} while(TagBegin != NULL);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CParamList

CParamList::CParamList()
{
	m_ParamListSize = 0;
}

LPTSTR CParamList::GetParam(LPCTSTR ParamName)
{
	for(int i = 0; i < m_ParamListSize; i++)
	{
		if(_tcsicmp(m_ParamList[i].Name, ParamName) == 0)
			return m_ParamList[i].Value;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// CTagParamList

void CTagParamList::FromString(LPTSTR TagParams)
{
	StripLine(TagParams);

	m_ParamListSize = 0;

	LPTSTR NextParam = TagParams;
	LPTSTR ParamVal;
	while((ParamVal = _tcsstr(NextParam, _T("="))) != NULL)
	{
		LPTSTR Name = NextParam;
		LPTSTR Val = CharNext(ParamVal);
		
		*ParamVal = _T('\0');
		
		StripLine(Name);
		StripLeft(Val);
		
		BOOL bQuoted = (Val[0] == _T('\"'));
		
		LPTSTR EndParamVal;
		if(bQuoted)
		{
			Val = CharNext(Val);
			EndParamVal = _tcsstr(Val, _T("\""));
		}else
			EndParamVal = _tcsstr(Val, _T(" "));

		if(EndParamVal)
		{
			NextParam = CharNext(EndParamVal);
			*EndParamVal = _T('\0');
		} else
			NextParam = Val + lstrlen(Val);

		PARAM param;
		lstrcpy(param.Name, Name);
		lstrcpy(param.Value, Val);

		m_ParamList[m_ParamListSize++] = param;
	}
}

//////////////////////////////////////////////////////////////////////////
// CStyleParamList

void CStyleParamList::FromString(LPTSTR Style)
{
	StripLine(Style);
	
	m_ParamListSize = 0;
	
	LPTSTR NextParam = Style;
	LPTSTR ParamVal;
	while((ParamVal = _tcsstr(NextParam, _T(":"))) != NULL)
	{
		LPTSTR Name = NextParam;
		LPTSTR Val = CharNext(ParamVal);
		
		*ParamVal = _T('\0');
		
		StripLine(Name);
		StripLeft(Val);
		
		LPTSTR EndParamVal;
		EndParamVal = _tcsstr(Val, _T(";"));
		
		if(EndParamVal)
		{
			NextParam = CharNext(EndParamVal);
			*EndParamVal = _T('\0');
		} else
			NextParam = Val + lstrlen(Val);
		
		PARAM param;
		lstrcpy(param.Name, Name);
		lstrcpy(param.Value, Val);
		
		m_ParamList[m_ParamListSize++] = param;
	}
}


//////////////////////////////////////////////////////////////////////////
// CViewsTemplate

CViewsTemplate::CViewsTemplate()
{
	m_bIsScript = FALSE;
	m_Width = 0;
	m_Height = 0;
}

int CViewsTemplate::GetViewsCount()
{
	return m_ViewList.size();
}

BOOL CViewsTemplate::LoadViewsFromHtml(CString HtmlFileFile)
{
	CString strHtmlTemplate;
	if (!LoadUTF8FileToString(HtmlFileFile, strHtmlTemplate))
	{
		return FALSE;
	}
	
	m_ViewList.clear();
	m_strBackgroungImageFilePath.Empty();
	m_Width = 0;
	m_Height = 0;
	
	Parse(strHtmlTemplate.GetBuffer(0));
	
	return TRUE;
}

BOOL CViewsTemplate::LoadViews(CString TemplateFile)
{
	CTemplateFile templ;
	templ.Open(TemplateFile);
	
	int ItemCount = 0;
	templ.GetItemCount(&ItemCount);
	if(ItemCount == 0)
		return FALSE;

	templ.SetPointerToFirstItem();

	TCHAR ItemName[256];
	DWORD ItemSize = 0;
	templ.GetCurItemInfo(ItemName, SIZE_OF_ARRAY(ItemName), &ItemSize);

	m_strHtmlName = ItemName;

	LPBYTE pBuf = new BYTE[ItemSize+1];
	templ.GetCurItem(pBuf, ItemSize);
	pBuf[ItemSize] = 0;
	
	USES_CONVERSION;
	CString sHtml = A2T((LPSTR)pBuf);
	
	m_ViewList.clear();
	m_strBackgroungImageFilePath.Empty();
	m_Width = 0;
	m_Height = 0;
	
	Parse(sHtml.GetBuffer(0));

	delete [] pBuf;
	
	return TRUE;
}

BOOL CViewsTemplate::LoadViews(CString TemplateFile, CString &sHtmlFileName)
{
	CString FolderForTemplates = GetClientTemplatesFilesDirectory();
	CreateDirectory(FolderForTemplates, NULL);

	CString FolderName = PathJoin(FolderForTemplates, GetFileNameOnly(TemplateFile));
	CreateDirectory(FolderName, NULL);

	if (!ExpandScheduleTemplateFile(TemplateFile, FolderName, sHtmlFileName, NULL))
		return FALSE;

	return LoadViews(TemplateFile);
}

BOOL CViewsTemplate::GetViewInfo(DWORD ViewID, LPRECT pRect)
{
	*pRect = CRect(0,0,0,0);
	for(VIEW_ITERATOR i = m_ViewList.begin(); i != m_ViewList.end(); i++)
	{
		if((*i).ViewID == ViewID)
		{
			*pRect = (*i).Rect;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CViewsTemplate::GetTemplateResolution(int *pWidth, int *pHeight)
{
	if (m_Width == 0 && m_Height == 0)
	{
		CRect Rect(0,0,0,0);
		for(VIEW_ITERATOR i = m_ViewList.begin(); i != m_ViewList.end(); i++)
		{
			//CRect RectRes;
			//CRect RectView = (*i).Rect;
			//RectRes.UnionRect(Rect, &((*i).Rect));
			//RectRes.UnionRect(&Rect, &RectView);
			//Rect = RectRes;
			Rect.UnionRect(&Rect, &((*i).Rect));
		}

		if (Rect.left > 0 && Rect.bottom > 0)
		{
			m_Width = Rect.right;
			m_Height = Rect.bottom;
// 			m_Width = Rect.Width();
// 			m_Height = Rect.Height();
		}
	}

	if(pWidth)
		*pWidth = m_Width;
	if(pHeight)
		*pHeight = m_Height;
	
	return m_Width > 0 && m_Height > 0;
}

#define START_BLOCK_DIV		_T("DIV")
#define END_BLOCK_DIV		_T("/DIV")

#define BLOCK_IMG			_T("IMG")
#define ATTR_BACKGROUND		_T("backgroung_img")

#define BLOCK_BODY			_T("BODY")
#define BODY_STYLE_WIDTH	_T("width")
#define BODY_STYLE_HEIGHT	_T("height")

void CViewsTemplate::OnHtmlTag(LPCTSTR TagName, LPTSTR TagParam)
{
	TRACE(_T("OnHtmlTag: Name=%s Param=%s\n"), TagName, TagParam);

	if(lstrcmpi(TagName, START_BLOCK_DIV) == 0)
		m_DivParams = TagParam;
	else if(lstrcmpi(TagName, END_BLOCK_DIV) == 0 && !m_DivParams.IsEmpty())
	{
		CTagParamList tag_params;
		tag_params.FromString(m_DivParams.GetBuffer(0));
		m_DivParams.Empty();

		VIEW view;

		// get view id
		view.ViewID = _ttoi(tag_params.GetParam(_T("id")));
		if(view.ViewID == 0)
			return;

		LPTSTR style = tag_params.GetParam(_T("style"));
		
		if(style == NULL)
			return;

		CStyleParamList style_params;
		style_params.FromString(style);

		view.Rect.left = _ttoi(style_params.GetParam(_T("left")));
		view.Rect.top = _ttoi(style_params.GetParam(_T("top")));

		int width = _ttoi(style_params.GetParam(BODY_STYLE_WIDTH));
		int height = _ttoi(style_params.GetParam(BODY_STYLE_HEIGHT));

		view.Rect.right = view.Rect.left + width;
		view.Rect.bottom = view.Rect.top + height;

		m_ViewList.push_back(view);
	} else if(lstrcmpi(TagName, BLOCK_IMG) == 0)
	{
		CTagParamList tag_params;
		tag_params.FromString(TagParam);

		if(lstrcmpi(tag_params.GetParam(_T("ID")), ATTR_BACKGROUND) == 0)
		{
			m_strBackgroungImageFilePath = tag_params.GetParam(_T("SRC"));
			m_strBackgroungImageFilePath.Replace(_T("%20"), _T(" "));
		}
	} else if(lstrcmpi(TagName, BLOCK_BODY) == 0)
	{
		CTagParamList tag_params;
		tag_params.FromString(TagParam);

		LPTSTR style = tag_params.GetParam(_T("style"));
		if(style == NULL)
			return;

		CStyleParamList style_params;
		style_params.FromString(style);
		
		m_Width = _ttoi(style_params.GetParam(BODY_STYLE_WIDTH));
		m_Height = _ttoi(style_params.GetParam(BODY_STYLE_HEIGHT));

	}
}

#define BLOCK_IMG			_T("IMG")
#define IMG_PARAM			_T("SRC")
#define BLOCK_LINK			_T("LINK")
#define LINK_PARAM			_T("HREF")
#define BLOCK_SCRIPT		_T("SCRIPT")
#define SCRIPT_PARAM		_T("SRC")

BOOL CViewsTemplate::CreateScheduleTemplateFile(
		const CString& SourceTemplateFile, const CString& outputFolder, CString &NewTemplateFile)
{
	CString strHtml;
	LoadAnsiFileToString(SourceTemplateFile, strHtml);

	// get source template file dir for expand local relative paths
	CString SourceTemplateFileFolder = GetFileFolder(SourceTemplateFile);
	
	// construct new html template file with links to local img files
	CString strNewHtml;

	int BeginPos = 0;
	int ImgTagSrcValBeginPos = 0;
	int ImgTagSrcValEndPos = 0;
	
	m_bIsScript = FALSE;

	IMG_LIST	ImgList;

	FIND_TAG TagsForFind[] = {
			{BLOCK_IMG, IMG_PARAM},
			{BLOCK_LINK, LINK_PARAM},
			{BLOCK_SCRIPT, SCRIPT_PARAM},
			{NULL, NULL}
	};

	// find all image files
	while(FindTagParamVal(strHtml, BeginPos,
		TagsForFind, &ImgTagSrcValBeginPos, &ImgTagSrcValEndPos))
	{
		strNewHtml += strHtml.Mid(BeginPos, ImgTagSrcValBeginPos - BeginPos);

		CString ImgFileNameForReplace =	strHtml.Mid(ImgTagSrcValBeginPos,
			ImgTagSrcValEndPos - ImgTagSrcValBeginPos);
		// check external link

		// ptr: fix %20 to space 
		ImgFileNameForReplace.Replace(_T("%20"), _T(" "));

		BOOL bIsExternalLink = IsExternalLink(ImgFileNameForReplace);

		IMG img;
		lstrcpy(img.ImgSource, ImgFileNameForReplace);
		img.bExternalLink = bIsExternalLink;

		if(!bIsExternalLink)
		{
			CString ImgPath = CreateAbsFilePath(SourceTemplateFileFolder,
				ImgFileNameForReplace);

			CString Signature = GetMD5Signature(ImgPath);
			if(Signature.IsEmpty())
			{
				CString err;
				err.Format(_T("Error GetMD5Signature: %s"), ImgPath);
				AfxMessageBox(err);
				return FALSE;
			}
			lstrcpy(img.ImgMD5, Signature);
		}

		CString ImgNewFileName = CreateNewFileName(img);

		strNewHtml += ImgNewFileName;

		BeginPos = ImgTagSrcValEndPos;

		ImgList.push_back(img);
	}

	strNewHtml += strHtml.Mid(BeginPos);

	// create list of unique files
	IMG_LIST	UniqueImgList;
	
	IMG_ITERATOR img = ImgList.begin();
	while(img != ImgList.end())
	{
		LPCTSTR cur_file_md5 = (*img).ImgMD5;
		
		// find cur img file in unique list
		BOOL bFound = FALSE;
		IMG_ITERATOR unique_img = UniqueImgList.begin();
		while(unique_img != UniqueImgList.end())
		{
			if(lstrcmp(cur_file_md5, (*unique_img).ImgMD5) == 0)
			{
				bFound = TRUE;
				break;
			}
			unique_img++;
		}
		
		if(bFound == FALSE)
			UniqueImgList.push_back(*img);
		
		img++;
	}

	// create one integrated template file

	CString NewHtmlSignature = GetMD5SignatureFromString(strNewHtml);

	// build file name for new template
	CreateDirectory(outputFolder, NULL);

	NewTemplateFile = NewHtmlSignature + TEMPLATE_EXT;

	CTemplateFile file;
	CString NewTemplateFilePath = outputFolder + NewTemplateFile;
	file.Create(NewTemplateFilePath);

	// first add html
//	CString NewHtmlFileName =
//		NewHtmlSignature + GetFileExtension(SourceTemplateFile);
	CString NewHtmlFileName;
	int index = SourceTemplateFile.ReverseFind('\\');
	if(index != -1)
		NewHtmlFileName = SourceTemplateFile.Right(SourceTemplateFile.GetLength() - index - 1);
	else
		NewHtmlFileName = SourceTemplateFile;
	USES_CONVERSION;
	LPSTR AnsiHtml = T2A(strNewHtml);
	file.AddItem(NewHtmlFileName, AnsiHtml, strlen(AnsiHtml));

	// add needed image files
	IMG_ITERATOR unique_img = UniqueImgList.begin();
	while(unique_img != UniqueImgList.end())
	{
		if((*unique_img).bExternalLink)
		{
			unique_img++;
			continue;
		}
		
		CString ImgSourceFileName = (*unique_img).ImgSource;
		CString ImgFilePath = CreateAbsFilePath(SourceTemplateFileFolder,
				ImgSourceFileName);
		CString NewImgFileName = CreateNewFileName(*unique_img);
		
		LPBYTE pImgBuf = NULL;
		DWORD ImgFileSize = LoadBinFileToBuf(ImgFilePath, pImgBuf);

		if(pImgBuf)
		{
			file.AddItem(NewImgFileName, pImgBuf, ImgFileSize);

			delete pImgBuf;
		} else
		{
			CString err;
			err.Format(_T("Can't copy media file: %s"), ImgFilePath);
			AfxMessageBox(err);
			return FALSE;
		}
			
		unique_img++;
	}

	file.Close();
	NewTemplateFile = GetMD5Signature(NewTemplateFilePath, NULL) + TEMPLATE_EXT;
	if(MoveFileEx(NewTemplateFilePath, outputFolder + NewTemplateFile, MOVEFILE_REPLACE_EXISTING) == 0)
	{
		CString err;
		err.Format(_T("Can't create template file: %s"), NewTemplateFile);
		AfxMessageBox(err);
		return FALSE;
	}

	return TRUE;
}

CString CViewsTemplate::CreateNewFileName(IMG Img)
{
	if(Img.bExternalLink)
		return Img.ImgSource;
	else
		return Img.ImgMD5 + GetFileExtension(Img.ImgSource);
}

CString CViewsTemplate::CreateAbsFilePath(CString strHtmlDir, CString strItemPath)
{
	if(strHtmlDir.GetLength() > 1 &&
		strHtmlDir[strHtmlDir.GetLength()-1] != _T('/') &&
		strHtmlDir[strHtmlDir.GetLength()-1] != _T('\\'))
	{
		strHtmlDir += _T("/");
	}

	CString AbsPath;
	if(strItemPath.Mid(0, 7).CompareNoCase(_T("file://")) != 0)
		AbsPath = strHtmlDir + strItemPath;
	else
		AbsPath = strItemPath.Mid(7);
	return AbsPath;
}

BOOL CViewsTemplate::FindTagParamVal(CString Html,
									 int BeginPos,
									 FIND_TAG Tags[],
									 int *pTagParamValBeginPos,
									 int *pTagParamValEndPos)
{
	int HtmlSize = Html.GetLength();

	while(BeginPos < HtmlSize)
	{
		int BeginTagPos = Html.Find(_T('<'), BeginPos);
		if(BeginTagPos == -1)
			break;

		int EndTagPos = Html.Find(_T(">"), BeginTagPos);
		if(EndTagPos == -1)
			break;

		int EndTagNamePos = Html.Find(_T(' '), BeginTagPos);
		if(EndTagNamePos == -1)
		{
			BeginPos = EndTagPos+1;
			continue;
		}

		BOOL bIsParamPresent = EndTagNamePos < EndTagPos;

		int BeginTagNamePos = BeginTagPos+1;
		
		int TagNameSize = bIsParamPresent ?
			EndTagNamePos-BeginTagPos-1 : EndTagPos-BeginTagPos-1;

		CString FindedTagName = 
			Html.Mid(BeginTagNamePos, TagNameSize);

		if(FindedTagName.CompareNoCase(END_SCRIPT_TAG) == 0)
			m_bIsScript = FALSE;

		if(!bIsParamPresent || m_bIsScript)
		{
			BeginPos = EndTagPos+1;
			continue;
		}

		if(FindedTagName.CompareNoCase(START_SCRIPT_TAG) == 0)
			m_bIsScript = TRUE;

		int FindedTagNum = 0;
		while(Tags[FindedTagNum].TagName != NULL)
		{
			if(FindedTagName.CompareNoCase(Tags[FindedTagNum].TagName) == 0)
				break;
			FindedTagNum++;
		}

		if(Tags[FindedTagNum].TagName == NULL)
		{
			BeginPos = EndTagPos+1;
			continue;
		}

		int TagParamsBegin = EndTagNamePos+1;

		CString localParams = Html.Mid(TagParamsBegin, EndTagPos-TagParamsBegin);
		localParams.MakeUpper();
		int localParamPos = localParams.Find(Tags[FindedTagNum].ParamName);
		if(localParamPos == -1)
		{
			BeginPos = EndTagPos+1;
			continue;
		}
		int localParamPosEnd = localParams.Find(_T('='), localParamPos);
		int localParamValPosBegin = localParamPosEnd+1;
		
		// skip white spaces
		while(localParams[localParamValPosBegin] == _T(' '))
			localParamValPosBegin++;

		BOOL bQuoted = FALSE;
		if(localParams[localParamValPosBegin] == _T('"'))
		{
			bQuoted = TRUE;
			localParamValPosBegin++;
		}

		int localParamValPosEnd = 0;
		if(bQuoted)
			localParamValPosEnd = localParams.Find(_T('"'), localParamValPosBegin);
		else
		{
			localParamValPosEnd = localParams.Find(_T(' '), localParamValPosBegin);
			if(localParamValPosEnd == -1)
				localParamValPosEnd = localParams.GetLength();
		}

		if(localParamValPosEnd == -1)
		{
			BeginPos = EndTagPos+1;
			continue;
		}

		if(pTagParamValBeginPos)
			*pTagParamValBeginPos = TagParamsBegin + localParamValPosBegin;
		if(pTagParamValEndPos)
			*pTagParamValEndPos = TagParamsBegin + localParamValPosEnd;

		return TRUE;
	}
	return FALSE;
}

BOOL CViewsTemplate::ExpandScheduleTemplateFile(CString TemplateFileName,
		CString FolderForTemplates,	CString &strHtmlFileName, LPCTSTR wantedHtmlName)
{
	CTemplateFile templ;
	BOOL res = templ.Open(TemplateFileName);
	if(!res)
		return FALSE;

	int ItemCount = 0;
	templ.GetItemCount(&ItemCount);
	if(ItemCount == 0)
		return FALSE;

	CString FolderName = FolderForTemplates;
	CreateDirectory(FolderName, NULL);

	templ.SetPointerToFirstItem();
	for(int i = 0; i < ItemCount; i++)
	{
		TCHAR ItemName[256];
		DWORD ItemSize = 0;
		templ.GetCurItemInfo(ItemName, SIZE_OF_ARRAY(ItemName), &ItemSize);

		LPBYTE pBuf = new BYTE[ItemSize+1];
		templ.GetCurItem(pBuf, ItemSize);

		if(i == 0 && wantedHtmlName)
		{
			_tcscpy(ItemName, wantedHtmlName);
		}
		CString FilePath = FolderName + _T("\\") + ItemName;
		SaveBinFileFromBuf(FilePath, pBuf, ItemSize);

		if(i == 0)
			strHtmlFileName = FilePath;
		
		delete pBuf;

		templ.MoveToNextItem();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CViewsTemplateTest

#ifdef _DEBUG

class CViewsTemplateTest: public CppUnit::TestFixture
{
public:
	void setUp()
	{
		SetCurrentDirectory(GetRootFolder());
	}

	void testHtmlStrip()
	{
		CString strHtmlStrip;
		LoadAnsiFileToString(_T("../../TestFiles/test_html_strip.htm"), strHtmlStrip);
		CString strHtmlStripRes;
		LoadAnsiFileToString(_T("../../TestFiles/test_html_strip_res.htm"), strHtmlStripRes);
		
		StripHtml(strHtmlStrip.GetBuffer(0));
		
		CPPUNIT_ASSERT(strHtmlStrip.GetLength());
		CPPUNIT_ASSERT(strHtmlStrip == strHtmlStripRes);
	}

	void testStripComment()
	{
		CString strHtmlWithComment;
		LoadAnsiFileToString(_T("../../TestFiles/test_html_with_comments.htm"), strHtmlWithComment);
		CString strHtml;
		LoadAnsiFileToString(_T("../../TestFiles/test_html_strip.htm"), strHtml);
		
		StripComment(strHtmlWithComment.GetBuffer(0));
		StripHtml(strHtmlWithComment.GetBuffer(0));

		StripHtml(strHtml.GetBuffer(0));
		
		CPPUNIT_ASSERT(strHtmlWithComment == strHtml);
	}
	
	void testGetViews()
	{
		CViewsTemplate templ;

		CString strHtml;
		LoadAnsiFileToString(_T("../../TestFiles/test_template1.htm"), strHtml);

		StripComment(strHtml.GetBuffer(0));
		StripHtml(strHtml.GetBuffer(0));

		CPPUNIT_ASSERT(templ.Parse(strHtml.GetBuffer(0)));
		
		CPPUNIT_ASSERT_EQUAL(3, (int)templ.m_ViewList.size());

		CViewsTemplate::VIEW_ITERATOR view;

		view = templ.m_ViewList.begin();
		DWORD id1 = 1;
		CRect rect1(20,50,620,250);
		CPPUNIT_ASSERT_EQUAL(id1, (*view).ViewID);
		CPPUNIT_ASSERT(rect1 == (*view).Rect);

		view++;
		DWORD id2 = 2;
		CRect rect2(650,50,1000,250);
		CPPUNIT_ASSERT_EQUAL(id2, (*view).ViewID);
		CPPUNIT_ASSERT(rect2 == (*view).Rect);
	
		view++;
		DWORD id3 = 3;
		CRect rect3(20,270,1000,370);
		CPPUNIT_ASSERT_EQUAL(id3, (*view).ViewID);
		CPPUNIT_ASSERT(rect3 == (*view).Rect);
	}

	void testCreateTemplate()
	{
		CViewsTemplate templ;
		CString newTemplate;
		CPPUNIT_ASSERT(templ.CreateScheduleTemplateFile(_T("../../TestFiles/test_template2.htm"),
			GetClientTemplatesFilesDirectory(), newTemplate));
	}

	void testCreateAndExpandHardTemplate()
	{
		CViewsTemplate templ;
		CString newTemplate;
		CPPUNIT_ASSERT(templ.CreateScheduleTemplateFile(_T("../../TestFiles/test_html/test.htm"),
			GetClientTemplatesFilesDirectory(), newTemplate));

		CString htmlFileName;
		CString folder = GetClientTemplatesFilesDirectory();
		CPPUNIT_ASSERT(templ.ExpandScheduleTemplateFile(folder+newTemplate,
			folder + newTemplate + _T("_dir"), htmlFileName, NULL));
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CViewsTemplateTest);
	CPPUNIT_TEST(testHtmlStrip);
	CPPUNIT_TEST(testStripComment);
	CPPUNIT_TEST(testGetViews);
	CPPUNIT_TEST(testCreateTemplate);
	CPPUNIT_TEST(testCreateAndExpandHardTemplate);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CViewsTemplateTest);

#endif // _DEBUG
