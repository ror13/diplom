
// Permission to distribute this example by
// Copyright (C) 2007 Ralf Junker
// Ralf Junker <delphi@yunqa.de>
// http://www.yunqa.de/delphi/

//---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

extern "C" {
#include "xdelta3/xdelta3.h"
}

#include "md5_util.h"

#include "FileOperation.h"
#include "XDelta.h"

//---------------------------------------------------------------------------

static int codeOld (
  int encode,
  const CString& strInFile,
  const CString& strSrcFile ,
  const CString& strOutFile,
  int BufSize)
{
	HANDLE InFile;
	HANDLE SrcFile;
	HANDLE OutFile;
	
	DWORD readed;
	int ret;
	xd3_stream stream;
	xd3_config config;
	xd3_source source;
	void* Input_Buf;
	int Input_Buf_Read;

	InFile = CreateFile( strInFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( InFile == INVALID_HANDLE_VALUE )
		return 1;

	SrcFile = CreateFile( strSrcFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( SrcFile == INVALID_HANDLE_VALUE ) {
		CloseHandle(InFile);
		return 1;
	}
	
	OutFile = CreateFile( strOutFile, GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( OutFile == INVALID_HANDLE_VALUE )
	{
		CloseHandle(InFile);
		CloseHandle(SrcFile);
		return 1;
	}

	if (BufSize < XD3_ALLOCSIZE)
		BufSize = XD3_ALLOCSIZE;

	memset (&stream, 0, sizeof (stream));
	memset (&source, 0, sizeof (source));

	xd3_init_config(&config, XD3_ADLER32);
	config.winsize = BufSize;
	xd3_config_stream(&stream, &config);

	source.blksize = BufSize;
	source.curblk = (const unsigned char*)malloc(source.blksize);

	/* Load 1st block of stream. */
	ReadFile(SrcFile, (void*)source.curblk, source.blksize, &readed, NULL);
	source.onblk = readed;
    source.curblkno = 0;
    xd3_set_source(&stream, &source);

	Input_Buf = malloc(BufSize);

	SetFilePointer (InFile, 0, NULL, FILE_BEGIN);

	do
	{
		ReadFile(InFile, (void*)Input_Buf, BufSize, &readed, NULL);
		Input_Buf_Read = readed;

		if (Input_Buf_Read < BufSize)
		{
			xd3_set_flags(&stream, XD3_FLUSH | stream.flags);
		}
		xd3_avail_input(&stream, (const unsigned char*)Input_Buf, Input_Buf_Read);

process:
		if (encode)
			ret = xd3_encode_input(&stream);
		else
			ret = xd3_decode_input(&stream);

		switch (ret)
		{
			case XD3_INPUT:
			{
				continue;
			}
			case XD3_OUTPUT:
			{
				WriteFile( OutFile, stream.next_out, stream.avail_out, &readed, NULL );
				xd3_consume_output(&stream);
				goto process;
			}
			case XD3_GETSRCBLK:
			{
				SetFilePointer (SrcFile, source.blksize * source.getblkno, NULL, FILE_BEGIN);
				ReadFile(SrcFile, (void*)source.curblk, source.blksize, &readed, NULL);
				source.onblk = readed;
				source.curblkno = source.getblkno;
				goto process;
			}
			case XD3_GOTHEADER:
			{
				goto process;
			}
			case XD3_WINSTART:
			{
				goto process;
			}
			case XD3_WINFINISH:
			{
				goto process;
			}

			default:
			{
				fprintf (stderr,"!!! INVALID %s %d !!!\n",
					stream.msg, ret);
				free(Input_Buf);
				free((void*)source.curblk);
				xd3_close_stream(&stream);
				xd3_free_stream(&stream);

				CloseHandle(InFile);
				CloseHandle(SrcFile);
				CloseHandle(OutFile);
				return ret;
			}
		}
	}
	while (Input_Buf_Read == BufSize);
	
	free(Input_Buf);
	free((void*)source.curblk);
	xd3_close_stream(&stream);
	xd3_free_stream(&stream);

	CloseHandle(InFile);
	CloseHandle(SrcFile);
	CloseHandle(OutFile);

	return 0;
};


static int codeResToFile (
  int encode,
  const CString& strInFile,
  const CString& strSrcFile ,
  FILE* OutFile,
  int BufSize,
  CMD5ProgressDlg *pProgressDlg = NULL)
{
	HANDLE InFile;
	HANDLE SrcFile;
	
	DWORD readed;
	int ret;
	xd3_stream stream;
	xd3_config config;
	xd3_source source;
	void* Input_Buf;
	int Input_Buf_Read;
	
	CString strInMd5;
	CString strSrcMd5;
	LONGLONG in_size = 0;

	int progress = 0;

	in_size = GetBinFileSizeEx(strInFile);

	if (pProgressDlg)
	{
		pProgressDlg->ShowProgress(in_size);
		pProgressDlg->UpdateProgress(0);
	}
	if (encode)
	{
		strInMd5 = GetMD5Signature(strInFile);
		strSrcMd5 = GetMD5Signature(strSrcFile);
	}

	InFile = CreateFile( strInFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( InFile == INVALID_HANDLE_VALUE )
	{
		if(pProgressDlg)
			pProgressDlg->HideProgress();
		TRACE(_T("XDelta InFile open failed"));
		return 1;
	}

	SrcFile = CreateFile( strSrcFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( SrcFile == INVALID_HANDLE_VALUE ) {
		CloseHandle(InFile);
		if(pProgressDlg)
			pProgressDlg->HideProgress();
		TRACE(_T("XDelta SrcFile open failed"));
		return 1;
	}

	CString strTmp;
	if (encode)
	{
		strTmp.Format(_T("SRDIFF\nsrc_md5=%s\ndst_md5=%s\nsrc_size=%I64d\ndst_size=%I64d\n"), strSrcMd5, strInMd5, 
			GetBinFileSizeEx(strSrcFile), GetBinFileSizeEx(strInFile));
		char* tmp = StringToUTF8(strTmp);
		readed = fwrite(tmp, 1, strlen(tmp)+1, OutFile);
		delete[] tmp;
	}

	if (BufSize < XD3_ALLOCSIZE)
		BufSize = XD3_ALLOCSIZE;

	memset (&stream, 0, sizeof (stream));
	memset (&source, 0, sizeof (source));

	xd3_init_config(&config, XD3_ADLER32);
	config.winsize = BufSize;
	xd3_config_stream(&stream, &config);

	source.blksize = BufSize;
	source.curblk = (const unsigned char*)malloc(source.blksize);

	/* Load 1st block of stream. */
	ReadFile(SrcFile, (void*)source.curblk, source.blksize, &readed, NULL);
	source.onblk = readed;
    source.curblkno = 0;
    xd3_set_source(&stream, &source);

	Input_Buf = malloc(BufSize);

	SetFilePointer (InFile, 0, NULL, FILE_BEGIN);
	if (encode == 0)
	{
		int character = 0;
		while(ReadFile(InFile, &character, 1, &readed, NULL) && readed == 1)
		{
			if (character == 0)
				break;
		}
	}

	do
	{
		ReadFile(InFile, (void*)Input_Buf, BufSize, &readed, NULL);
		Input_Buf_Read = readed;

		if (Input_Buf_Read < BufSize)
		{
			xd3_set_flags(&stream, XD3_FLUSH | stream.flags);
		}
		xd3_avail_input(&stream, (const unsigned char*)Input_Buf, Input_Buf_Read);
		if (pProgressDlg)
			pProgressDlg->UpdateProgress(readed);

process:
		if (encode)
			ret = xd3_encode_input(&stream);
		else
			ret = xd3_decode_input(&stream);

		if(pProgressDlg)
		{
			if (pProgressDlg->IsCancel())
			{
				free(Input_Buf);
				free((void*)source.curblk);
				xd3_close_stream(&stream);
				xd3_free_stream(&stream);

				CloseHandle(InFile);
				CloseHandle(SrcFile);
				if(pProgressDlg)
					pProgressDlg->HideProgress();
				TRACE(_T("XDelta Cancel"));
				return 1;
			}
		}

		switch (ret)
		{
			case XD3_INPUT:
			{
				continue;
			}
			case XD3_OUTPUT:
			{
				//WriteFile( OutFile, stream.next_out, stream.avail_out, &readed, NULL );
				readed = fwrite(stream.next_out, 1, stream.avail_out, OutFile);
				xd3_consume_output(&stream);
				goto process;
			}
			case XD3_GETSRCBLK:
			{
				SetFilePointer (SrcFile, source.blksize * source.getblkno, NULL, FILE_BEGIN);
				ReadFile(SrcFile, (void*)source.curblk, source.blksize, &readed, NULL);
				source.onblk = readed;
				source.curblkno = source.getblkno;
				goto process;
			}
			case XD3_GOTHEADER:
			{
				goto process;
			}
			case XD3_WINSTART:
			{
				goto process;
			}
			case XD3_WINFINISH:
			{
				goto process;
			}

			default:
			{
				fprintf (stderr,"!!! INVALID %s %d !!!\n",
					stream.msg, ret);
				TRACE(_T("XDelta INVALID %s %d !!!"), UTF8ToString(stream.msg), ret);

				free(Input_Buf);
				free((void*)source.curblk);
				xd3_close_stream(&stream);
				xd3_free_stream(&stream);

				CloseHandle(InFile);
				CloseHandle(SrcFile);
				if(pProgressDlg)
					pProgressDlg->HideProgress();
				return ret;
			}
		}
	}
	while (Input_Buf_Read == BufSize);

	if(pProgressDlg)
		pProgressDlg->HideProgress();

	free(Input_Buf);
	free((void*)source.curblk);
	xd3_close_stream(&stream);
	xd3_free_stream(&stream);

	CloseHandle(InFile);
	CloseHandle(SrcFile);

	return 0;
};

static int code (
  int encode,
  const CString& strInFile,
  const CString& strSrcFile ,
  const CString& strOutFile,
  int BufSize,
  CMD5ProgressDlg *pProgressDlg = NULL)
{
	FILE* outFile = _tfopen(strOutFile, _T("wb"));
	if (!outFile)
		return 1;
	int ret = codeResToFile(encode, strInFile, strSrcFile, outFile, BufSize, pProgressDlg);
	fclose(outFile);
	if (ret != 0)
		DeleteFile(strOutFile);
	return ret;
};


BOOL XDeltaDiffOld(const CString& strFrom, const CString& strTo, const CString& strDiff)
{

  int r = codeOld(1, strTo, strFrom, strDiff, 0x1000);

  if (r == 0)
	return TRUE;
  return FALSE;
}

BOOL XDeltaPatchOld(const CString& strFrom, const CString& strPatch, const CString& strDest)
{

  int r = codeOld (0, strPatch, strFrom, strDest, 0x1000);

  if (r == 0)
	return TRUE;
  return FALSE;
}

BOOL XDeltaDiff(const CString& strFrom, const CString& strTo, const CString& strDiff, CMD5ProgressDlg *pProgressDlg)
{

  int r = code (1, strTo, strFrom, strDiff, 0x1000, pProgressDlg);

  if (r == 0)
	return TRUE;
  return FALSE;
}

BOOL XDeltaDiff(const CString& strFrom, const CString& strTo, FILE* diff, CMD5ProgressDlg *pProgressDlg)
{

  int r = codeResToFile (1, strTo, strFrom, diff, 0x1000, pProgressDlg);

  if (r == 0)
	return TRUE;
  return FALSE;
}

BOOL XDeltaPatch(const CString& strFrom, const CString& strPatch, const CString& strDest)
{

  int r = code (0,strPatch, strFrom, strDest, 0x1000);

  if (r == 0)
	return TRUE;
  return FALSE;
}

static BOOL staticXDeltaGetKey(FILE * fpatch, const CString& key, CString& ret)
{
	CString strData;
	int posStart, posEnd;
	char buffer[32000];

	fpos_t pos = 0;
	fgetpos(fpatch, &pos);
	rewind(fpatch);

	DWORD read_bytes = fread(buffer, 1, 6, fpatch);

	if (read_bytes != 6 || memcmp("SRDIFF", buffer, 6) != 0)
	{
		fsetpos(fpatch, &pos);
		return FALSE;
	}

	pos = 0;
	while (pos < sizeof(buffer)) {
		read_bytes = fread(buffer + pos, 1, 1, fpatch);
		if (*(buffer+pos) == 0) {
			break;
		}
		if (read_bytes == 0)
		{
			fsetpos(fpatch, &pos);
			return FALSE;
		}
		pos++;
	}
	fsetpos(fpatch, &pos);

	if (pos == sizeof(buffer))
	{
		return FALSE;
	}
	strData = UTF8ToString(buffer);

	posStart = strData.Find(key);
	if (posStart == -1)
		return FALSE;
	posStart = posStart + key.GetLength();

	posEnd = strData.Find(_T("\n"), posStart);
	if (posEnd == -1)
		return FALSE;
	ret = strData.Mid(posStart, posEnd - posStart);
	return TRUE;
}

static BOOL staticXDeltaGetKey(const CString& patch, const CString& key, CString& ret)
{
	FILE* fpatch = _tfopen(patch, _T("rb"));
	if (!fpatch)
		return FALSE;
	BOOL res = staticXDeltaGetKey(fpatch, key, ret);
	fclose(fpatch);
	return res;
}


BOOL XDeltaGetSrcMd5(const CString& patch, CString& ret)
{
	return staticXDeltaGetKey(patch, _T("src_md5="), ret);
}

BOOL XDeltaGetSrcMd5(FILE * patch, CString& ret)
{
	return staticXDeltaGetKey(patch, _T("src_md5="), ret);
}



BOOL XDeltaGetDstMd5(const CString& patch, CString& ret)
{
	return staticXDeltaGetKey(patch, _T("dst_md5="), ret);
}

BOOL XDeltaGetDstMd5(FILE * patch, CString& ret)
{
	return staticXDeltaGetKey(patch, _T("dst_md5="), ret);
}


BOOL XDeltaGetSrcSize(const CString& patch, LONGLONG& ret)
{
	CString tmp;
	if (!staticXDeltaGetKey(patch, _T("src_size="), tmp))
		return FALSE;
	char* szTmp = StringToUTF8(tmp);
	ret = _atoi64(szTmp);
	delete[] szTmp;
	return TRUE;
}

BOOL XDeltaGetSrcSize(FILE * patch, LONGLONG& ret)
{
	CString tmp;
	if (!staticXDeltaGetKey(patch, _T("src_size="), tmp))
		return FALSE;
	char* szTmp = StringToUTF8(tmp);
	ret = _atoi64(szTmp);
	delete[] szTmp;
	return TRUE;
}

BOOL XDeltaGetDstSize(const CString& patch, LONGLONG& ret)
{
	CString tmp;
	if (!staticXDeltaGetKey(patch, _T("dst_size="), tmp))
		return FALSE;
	char* szTmp = StringToUTF8(tmp);
	ret = _atoi64(szTmp);
	delete[] szTmp;
	return TRUE;
}

BOOL XDeltaGetDstSize(FILE * patch, LONGLONG& ret)
{
	CString tmp;
	if (!staticXDeltaGetKey(patch, _T("dst_size="), tmp))
		return FALSE;
	char* szTmp = StringToUTF8(tmp);
	ret = _atoi64(szTmp);
	delete[] szTmp;
	return TRUE;
}
