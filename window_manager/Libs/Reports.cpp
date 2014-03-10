#include "stdafx.h"

#include <time.h>
#include <io.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#include "Thread.h"
#include "Reports.h"
#include "FileOperation.h"
#include "../zlib/zlib.h"
#include "Translate.h"
#include "PresenterRegistry.h"

#include <map>
#include <string>

/*
static void* getGZIPData(void *buffer)
{
    unsigned char* b = (unsigned char*)buffer;
    if (*b++ != 31)
		return NULL;
    if (*b++ != 139)
		return NULL;
    if (*b++ != 8)
		return NULL;
    unsigned char flags = *b++;
	
    // skip mtime
    *b++;*b++;*b++;*b++;
    // skip xfl
    *b++;
    // skip os
    *b++;
	
    if (flags & 1) 
	{
		// skip extra
		unsigned short skip = *((unsigned short*)b);
		b = b + skip + 2;
    }
    if (flags&(1<<3)) 
	{
		b = b + strlen((const char*)b) + 1;
    }
    if (flags & (1<<4)) 
	{
		b = b + strlen((const char*)b) + 1;
    }
    if (flags & (1<<1)) 
	{
		// skip extra
		unsigned short c16 = *((unsigned short*)b);
		b = b + 2;
    }
    return b;
}

BOOL Reports::CompressGZIPBuffer(void* inp, int len, void** outb, int*outLen)
{
    FILE* f = tmpfile();
    int fd = fileno(f);
    gzFile gzF = gzdopen(fd, (TCHAR*)"wb");
    if (gzF < 0) {
		//fclose(f);
		return FALSE;
	}
	int written = 0;
	int ret = 0;
	do 
	{
		ret = gzwrite(gzF, (char*)inp + written, len - written);
		if (ret <= 0) {
			*outLen = ret;
			return FALSE;
		}
		written += ret;
	} while (written != len);
	gz_stream *s = (gz_stream*)gzF;
	FILE* hack = s->file;
	s->file = 0;
	gzclose(gzF);
	*outLen = (int)s->file;
	//FILE* f = fopen(name, "rb");
	//if ()

	*outLen = lseek(fd, 0, SEEK_END);
	*outb = new char[*outLen];
	lseek(fd, 0, SEEK_SET);
	read(fd, *outb, *outLen);
	//gzclose(gzF);
	fclose(f);
	return TRUE;
}


BOOL Reports::UncompressGZIPBuffer(void* inp, int len, void** outb, int*outLen)
{
	// FIXME: It would be much better to don't use temporary files for uncompress
	BOOL ret = TRUE;
    if (len < 8 || getGZIPData(inp) == 0)
		return FALSE;
	
    FILE* f = tmpfile();
    fwrite(inp, 1, len, f);
    rewind(f);
    int fd = fileno(f);
    *outLen = *((unsigned int*)((char*)inp + len - 4));
	
    gzFile gzF = gzdopen(fd, _T("rb"));
    if (gzF < 0) {
		fclose(f);
		return FALSE;
	}
	*outb = malloc(*outLen + 1);
    if (gzread(gzF, *outb, *outLen) != *outLen)
    {
		delete[] *outb;
		ret = FALSE;
    }
    gzclose(gzF);
    fclose(f);
    return ret;
}
*/

#define MIN(a,b) (((a)<(b))?(a):(b))

static int parse_header(void* ibuf, int ilen, void** data, int* clen, int* len)
{
	if(ilen < 18)
		return 0;
	unsigned char* p = (unsigned char*)ibuf;
	int plen = ilen;
	
	if (p[0] != 0x1f || p[1] != 0x8b || p[2] != 8)
		return 0;
	unsigned char flags = p[3];
	p = p + 10;	
	plen = plen - 10;
	
	if (flags & 1) {
		short extlen = *((unsigned short*)p);
		p = p + extlen;
		plen = plen - extlen;
	}
	if (flags & 4) {
		p = p + strlen((char*)p) + 1;
	}
	if (flags & 8) {
		p = p + strlen((char*)p) + 1;
	}
	if (flags & 2) {
		p = p + 2;
	}
	*data = p;
	*clen = ilen - (p - (unsigned char*)ibuf) - 8;
	*len = *((uint32_t*)((char*)ibuf + ilen - 4));
	return 1;
}

BOOL Reports::UncompressGZIPBuffer(void* ibuf, int ilen, void** obuf, int* olen)
{
	void* cbuf;
	int clen;
	int ret;
    unsigned have;
    z_stream strm;

	if (ilen == sizeof(nullData) && memcmp(ibuf, nullData, sizeof(nullData)) == 0)
	{
		*olen = 0;
		*obuf = new char[1];
		return TRUE;
	}

	if (parse_header(ibuf, ilen, &cbuf, &clen, olen) == 0)
		return 0;    
	*olen = *olen + 32768;
    *obuf = new char[*olen];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = clen;
    strm.next_in = (unsigned char*)cbuf;
    
    ret = inflateInit2(&strm, -15);
    if (ret != Z_OK)
        return FALSE;
        
	int readed = 0;
	int written = 0;
    do {
		strm.avail_in = clen - readed;
		strm.next_in = (unsigned char*)cbuf + readed;
        strm.avail_out = *olen - written;
        strm.next_out = (unsigned char*)*obuf + written;
        
        ret = inflate(&strm, Z_NO_FLUSH);
        
        if (ret != Z_STREAM_END && strm.avail_out == 0 && strm.avail_in) {
			ret = Z_DATA_ERROR;
			(void)inflateEnd(&strm);
			delete[] *obuf;
            return FALSE;
		}
        switch (ret) {
            case Z_NEED_DICT:
				printf("Z_NEED_DICT\n");
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
			case Z_STREAM_ERROR:
				printf("Z_MEM_ERROR\n");
                (void)inflateEnd(&strm);
				delete[] *obuf;
                return FALSE;
             case Z_BUF_ERROR:
				printf("Z_BUF_ERROR: %d %d\n", strm.avail_in, strm.avail_out);				
        }
        have = *olen - written - strm.avail_out;
        written += have;
        have = ilen - readed - strm.avail_in;
        readed += have;
    } while (ret != Z_STREAM_END);
    (void)inflateEnd(&strm);
    *olen = written;
	return TRUE;
//#endif
}

BOOL Reports::CompressGZIPBuffer(void* ibuf, int ilen, void** obuf, int* olen)
{
	int ret;
    unsigned have;
    z_stream strm;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = (unsigned char*)ibuf;
    strm.avail_in = ilen;
            
    *olen = 32768 + ilen;
    *obuf = new char[*olen];
    
    ret = deflateInit2(&strm, 9, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        return FALSE;
	}
	int readed = 0;
	int written = 0;

    do {
            strm.avail_out = *olen - written;
            strm.next_out = (unsigned char*)*obuf + written;
            if (strm.avail_out == 0) {
				*olen = 2* *olen;
				void* tmp = new char[*olen];
				memcpy(tmp, *obuf, written);
				delete[] *obuf;
				*obuf = tmp;
				strm.avail_out = *olen - written;
	            strm.next_out = (unsigned char*)*obuf + written;
			}            
            ret = deflate(&strm, Z_FINISH);
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
			case Z_STREAM_ERROR:
                (void)deflateEnd(&strm);
				delete[] *obuf;
                return FALSE;
            case Z_BUF_ERROR:
				printf("Z_BUF_ERROR\n");
            }
            have = *olen - written - strm.avail_out;
            written += have;            
    } while (ret != Z_STREAM_END);
    /* clean up and return */
    (void)deflateEnd(&strm);
    *((uint32_t*)((unsigned char*)*obuf + written - 8)) = crc32(0, (unsigned char*)ibuf, ilen);
    *((uint32_t*)((unsigned char*)*obuf + written - 4)) = ilen;
    *olen = written;
	return TRUE;	
}


BOOL Reports::UncompressGZIPFile(const CString& name, void** outb, int*outLen)
{
	USES_CONVERSION;
	int step = 8192;
	int readed = 0;;
	int alloced = 0;

	gzFile gzF = gzopen(T2A(name), "rb");	
    if (!gzF) {
		return FALSE;
	}

	unsigned char* buf = (unsigned char*)malloc(step);
	alloced = alloced + step;	
	while(1)
	{
		int len = gzread(gzF, buf + readed, step);
		if (len < 0)
		{
			gzclose(gzF);
			free(buf);
			return FALSE;
		}
		if (len == 0)
			break;
		readed += len;
		alloced = alloced + step;
		buf = (unsigned char*)realloc(buf, alloced);
	}

	*outb = (void*)buf;
	*outLen = readed;
    return TRUE;
}

BOOL Reports::CompressGZIPFile(const CString& name, void* inb, int inLen)
{
	USES_CONVERSION;
	gzFile gzF = gzopen(T2A(name), "wb");	
    if (!gzF) {
		return FALSE;
	}


	int len = gzwrite(gzF, inb, inLen);
	gzclose(gzF);
// 	if (len < 0)
// 	{
// 		return FALSE;
// 	}

    return TRUE;
}

BOOL Reports::CompressGZIPFile(const CString& name_gzip, const CString& in_name)
{
	USES_CONVERSION;
	int step = 8192;
	
	gzFile gzF = gzopen(T2A(name_gzip), "wb");	
    if (!gzF) {
		return FALSE;
	}

	CFile in_file;

	if (in_file.Open(in_name, CFile::modeRead | CFile::typeBinary))
	{
		BYTE buffer[4096];
		DWORD dwRead;
		
		// Read in 4096-byte blocks,
		// remember how many bytes were actually read,
		// and try to write that many out. This loop ends
		// when there are no more bytes to read.
		
		do
		{
			dwRead = in_file.Read(buffer, 4096);
			int len = gzwrite(gzF, buffer, dwRead);
		}
		while (dwRead > 0);
		
		// Close both files
		
		in_file.Close();
	}
	gzclose(gzF);
	
    return TRUE;
}

static CString GetStringTime(struct tm* tm)
{
	CString ret;
	ret.Format(_T("%d%02d%02d"), 
		tm->tm_year + 1900, 
		tm->tm_mon + 1,
		tm->tm_mday);
	return ret;
}

static BOOL IsValidDate(const CString& date)
{
	int i;
	if (date.GetLength() != 8)
		return FALSE;
	for ( i=0; i< 8; i++) 
	{
		if (!(date[i] == '0' || date[i] == '1' ||
			date[i] == '2' || date[i] == '3' ||
			date[i] == '4' || date[i] == '5' ||
			date[i] == '6' || date[i] == '7' || 
			date[i] == '8' || date[i] == '9'))
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*Reports* Reports::m_reports = NULL;
Reports* Reports::GetInstance()
{
	if(m_reports)
		return m_reports;
	m_reports = new Reports();
	return m_reports;
}
*/

CString Reports::GetOldestDate(BOOL reg)
{
	DWORD keepTime;
	if (reg)
	{
		BOOL res = regReadRegDword(PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
			REG_PRESENTER_REPORTS_KEEP_T, &keepTime);
		if(!res)
			keepTime = 365;

		time_t today;
		time(&today);
		CTime t(today);
		CTimeSpan day(keepTime, 0, 0, 0);
		t -= day;
		return t.Format(_T("%Y%m%d"));
	}
	return _T("20100101");
}

unsigned char Reports::nullData[] = {
	0x1F, 0x8B, 0x08, 0x08, 0x54, 0xA6, 0xC5, 0x4C, 0x00, 0x03, 
	0x61, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00
};

CString Reports::GetCurrentDate()
{
	time_t long_time;
	time( &long_time );
	return GetStringTime(localtime( &long_time ));
}


CCriticalSection Reports::m_gStoreMutex;

BOOL Reports::GetReportArchive(const CString& client, 
									 const CString date, 
									 void** data, int* len)
{
	CSingleLock singleLock(&m_gStoreMutex, TRUE);
	*len = 0;
	CString name = GetServerReportsDirectory(NULL) + client + _T("\\") + date + _T(".gz");
	if (IsFileExists(name))
	{
		PBYTE tmp;
		*len = LoadBinFileToBuf(name, tmp);
		*data = tmp;
		return TRUE;
	}
	return FALSE;
}

BOOL Reports::SetReportArchive(const CString& client, 
					  const CString date, 
					  void* data, int len)
{
	CString oldestDate = GetOldestDate(TRUE);
	if(date < oldestDate || date == GetCurrentDate())
		return FALSE;

	CleanServerReports(client);
	CSingleLock singleLock(&m_gStoreMutex, TRUE);
	CString name = GetServerReportsDirectory(NULL) + client + _T("\\") + date + _T(".gz");
	PBYTE tmp = (PBYTE)data;
	CreateDirectory(GetServerReportsDirectory(NULL) + client, NULL);
	BOOL ret = SaveBinFileFromBuf(name, tmp, len);
	return ret;
}

std::list<CString> Reports::ListReports(const CString& client)
{
	CSingleLock singleLock(&m_gStoreMutex, TRUE);
	CString name = GetServerReportsDirectory(NULL) + client + _T("\\*.gz");
	struct _tfinddata_t c_file;
	long hFile;
	std::list<CString> lst;

	if ( (hFile = _tfindfirst((LPTSTR)(LPCTSTR)name, &c_file)) == -1L ) {
		return lst;
	} else {
		do {
			if (c_file.attrib & _A_SUBDIR)
				continue;
			CString date = c_file.name;
			date = date.Left(8);
			if (IsValidDate(date))
				lst.push_back(date);
		} while ( _tfindnext(hFile, &c_file) == 0 );
		_findclose(hFile);
	}
	return lst;
}

void Reports::CleanServerReports(const CString& client)
{
	CSingleLock singleLock(&m_gStoreMutex, TRUE);

	CString reportDir = GetServerReportsDirectory(NULL);
	CString oldestDate = GetOldestDate(TRUE);
	
	CFileFind finderReports;
	BOOL bReportFound = finderReports.FindFile(reportDir + client + _T("\\*.gz"));
	while(bReportFound)
	{
		bReportFound = finderReports.FindNextFile();
		CString fileName = finderReports.GetFileName();
		fileName = fileName.Mid(0, fileName.GetLength() - 3);
		if(fileName < oldestDate)
			DeleteFile(finderReports.GetFilePath());
	}
}

static int staticGetYear(const CString& date)
{
	CString tmp = date.Left(4);
	return _ttoi(tmp);
}

static int staticGetMonth(const CString& date)
{
	CString tmp = date.Mid(4, 2);
	return _ttoi(tmp);
}

static int staticGetDay(const CString& date)
{
	CString tmp = date.Mid(6, 2);
	return _ttoi(tmp);
}

static CTime staticDateToCTime(const CString& date)
{
	return CTime(staticGetYear(date), staticGetMonth(date), 
		staticGetDay(date), 0, 0, 0);
}

CString Reports::GetNextDay(const CString& date)
{
	CTime time = staticDateToCTime(date);		
	CTimeSpan day(1, 0, 0, 0);
	time += day;
	if (time.Format(_T("%Y%m%d")) == date)
		time += day;
	return time.Format(_T("%Y%m%d"));
}

CString Reports::GetPrevDay(const CString& date)
{
	CTime time = staticDateToCTime(date);		
	CTimeSpan day(1, 0, 0, 0);
	time -= day;
	if (time.Format(_T("%Y%m%d")) == date)
		time -= day;
	return time.Format(_T("%Y%m%d"));
}

BOOL Reports::ParseRange(const CString& range, CString& first, CString& second)
{
	if (range.GetLength() != 17)
		return FALSE;
	first = range.Left(8);
	second = range.Mid(9);
	return TRUE;
}

BOOL Reports::MakeDateRange(const CString& first, const CString& second, CString& range)
{
	CTime begin = staticDateToCTime(first);
	CTime end = staticDateToCTime(second);
	if (begin > end)
		return FALSE;
	else if (begin == end) {
		range = first;
		return TRUE;
	}
	range = first + _T("-") + second;
	return TRUE;
}


std::list<CString>::iterator Reports::FindFirstDay(std::list<CString>& dates, const CString& date)
{
	CTime t = staticDateToCTime(date);
	std::list<CString>::iterator i;
	for( i = dates.begin(); i != dates.end(); i++)
	{
		if (staticDateToCTime(*i) >= t)
			break;
	}
	return i;
}

std::list<CString> Reports::SplitRange(CString range)
{
	std::list<CString> tmpRanges, ret;
	int pos;
	CString tmp;

	if (range.GetLength() == 0)
		return ret;

	while(1)
	{
		pos = range.Find(_T(","));
		if (pos == -1)
			break;
		tmp = range.Left(pos);
		tmpRanges.push_back(tmp);
		range = range.Mid(pos + 1);
	}
	tmpRanges.push_back(range);

	std::list<CString>::iterator i;
	for( i = tmpRanges.begin(); i != tmpRanges.end(); i++)
	{
		if((*i).Find(_T("-")) != -1)
		{
			CString begin, end;
			if(!Reports::ParseRange((*i), begin, end))
				continue;

			while(begin <= end)
			{
				ret.push_back(begin);
				begin = Reports::GetNextDay(begin);
			}
		}
		else
		{
			ret.push_back((*i));
		}
	}

	return ret;
}

static CString MakeRanges(std::vector<CString>::iterator begin,
						  std::vector<CString>::iterator end)
{
	CString ret;
	if(begin == end)
		return _T("");

	ret = *begin;
	CString last = *begin;
	BOOL rangeCount = FALSE;
	begin++;

	for (std::vector<CString>::iterator i = begin; 
			i != end; i++)
	{
		if (rangeCount)
		{
			if(Reports::GetNextDay(last) == *i)
			{
				last = *i;
				continue;
			}
			ret = ret + last;
			rangeCount = FALSE;
			last = *i;
			ret = ret + _T(",") + *i;
		} 
		else 
		{
			if(Reports::GetNextDay(last) == *i)
			{
				ret = ret + _T("-");
				rangeCount = TRUE;
				last = *i;
				continue;
			}

			ret = ret + _T(",") + *i;
			last = *i;
		}
	}
	if(rangeCount)
		ret = ret + *(--end);

	return ret;
}

CString Reports::IntersectRanges(const std::list<CString>& range1, const std::list<CString>&  range2)
{
	std::vector<CString>::iterator res;
	std::vector<CString> rng3(range1.size());
	
	res = std::set_intersection(range1.begin(), range1.end(), range2.begin(), range2.end(), rng3.begin());
	return MakeRanges(rng3.begin(), res);
}


CString Reports::IntersectRanges(const CString range1, const CString range2)
{
	std::list<CString> rng1, rng2;
	
	rng1 = SplitRange(range1);
	rng2 = SplitRange(range2);	
	return IntersectRanges(rng1, rng2);
}

CString Reports::RelativeComplementRanges(const std::list<CString>& range1, const std::list<CString>&  range2)
{
	std::vector<CString>::iterator res;	
	std::vector<CString> rng3(range1.size());
	res = std::set_difference(range1.begin(), range1.end(), range2.begin(), range2.end(), rng3.begin());
	return MakeRanges(rng3.begin(), res);
}


CString Reports::RelativeComplementRanges(const CString range1, const CString range2)
{
	std::list<CString> rng1, rng2;	
	rng1 = SplitRange(range1);
	rng2 = SplitRange(range2);
	return RelativeComplementRanges(rng1, rng2);
}

static CMutex gCacheMutex;
typedef std::map<CString, std::map<CString, std::pair<void*,int> > > ReportCacheMap;
static ReportCacheMap gReportCache;


std::list<CString> Reports::ListCache(const CString& client)
{
	gCacheMutex.Lock();
	std::list<CString> lst;
	ReportCacheMap::iterator i = gReportCache.find(client);
	if (i == gReportCache.end())
	{
		gCacheMutex.Unlock();
		return lst;
	}
	std::map<CString, std::pair<void*,int> >::iterator j;
	for (j = (*i).second.begin(); j != (*i).second.end(); j++)
	{
		lst.push_back((*j).first);
	}
	gCacheMutex.Unlock();
	return lst;

}


BOOL Reports::GetCacheDay(const CString& client, const CString& day, void** data, int* len)
{
	gCacheMutex.Lock();

	ReportCacheMap::iterator i = gReportCache.find(client);
	if (i == gReportCache.end())
	{
		gCacheMutex.Unlock();
		return FALSE;
	}

	std::map<CString, std::pair<void*,int> >::iterator j = (*i).second.find(day);
	if (j != (*i).second.end())
	{
		*len = (*j).second.second;
		*data = new char[*len];
		memcpy(*data, (*j).second.first, *len);
		gCacheMutex.Unlock();
		return TRUE;
	}
	gCacheMutex.Unlock();
	return FALSE;
}

BOOL Reports::SetCacheDay(const CString& client, const CString& day, void* data, int len)
{
	gCacheMutex.Lock();
	void *d = new char[len];
	memcpy(d, data, len);

	ReportCacheMap::iterator i = gReportCache.find(client);
	if (i == gReportCache.end())
	{
		gReportCache[client] = std::map<CString, std::pair<void*,int> >();
		gReportCache[client][day] = std::pair<void*,int>(d, len);
		gCacheMutex.Unlock();
		return TRUE;
	}

	std::map<CString, std::pair<void*,int> >::iterator j = (*i).second.find(day);
	if (j != (*i).second.end())
	{
		delete [] (*j).second.first;
		(*j).second.first = d;
		(*j).second.second = len;
		gCacheMutex.Unlock();
		return TRUE;
	}
	gReportCache[client][day] = std::pair<void*,int>(d, len);
	gCacheMutex.Unlock();
	return TRUE;
}

void Reports::CleanCache()
{
	gCacheMutex.Lock();

	ReportCacheMap::iterator i;
	for (i = gReportCache.begin(); i != gReportCache.end(); i++)
	{
		std::map<CString, std::pair<void*,int> >::iterator j;
		for (j = (*i).second.begin(); j != (*i).second.end(); j++)
		{
			delete[] (*j).second.first;
		}
	}
	gReportCache.clear();

	gCacheMutex.Unlock();
}

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define	LEGAL_ALT(x)		{ if (alt_format & ~(x)) return (0); }

static char* Month[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static int s_conv_num(const char **buf, int *dest, int llim, int ulim)
{
	int result = 0;
	
	/* The limit also determines the number of valid digits. */
	int rulim = ulim;
	
	if (**buf < '0' || **buf > '9')
		return (0);
	
	do {
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} 
	while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');
	
	if (result < llim || result > ulim)
		return (0);
	
	*dest = result;
	return (1);
}

static char *ech_strptime_p(const char *buf, const char *fmt, struct tm *tim)
{
	char c;
	const char *bp;
	int alt_format, i;
	int match;
    
	bp = buf;

	while ((c = *fmt) != '\0') {
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/* Eat up white-space. */
		if (isspace(c)) {
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}
				
		if ((c = *fmt++) != '%')
			goto literal;


again:		switch (c = *fmt++) {
		case '%':	/* "%%" is converted to "%". */
literal:
			if (c != *bp++)
				return (0);
			break;

		/*
		 * "Alternative" modifiers. Just set the appropriate flag
		 * and start over again.
		 */
		case 'E':	/* "%E?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_E;
			goto again;

		case 'O':	/* "%O?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_O;
			goto again;
			
		/*
		 * "Complex" conversion rules, implemented through recursion.
		 */
		case 'R':	/* The time as "%H:%M". */
			LEGAL_ALT(0);
			if (!(bp = ech_strptime_p(bp, "%H:%M", tim)))
				return (0);
			break;

		case 'T':	/* The time as "%H:%M:%S". */
			LEGAL_ALT(0);
			if (!(bp = ech_strptime_p(bp, "%H:%M:%S", tim)))
				return (0);
			break;

		case 'd':	/* The days. */
		case 'e':
		case 'j':
			LEGAL_ALT(ALT_O);
			if (!(s_conv_num(&bp, &tim->tm_mday, 0, (INT_MAX-1)/86400)))
				return (0);
			break;

		case 'k':	/* The hours. */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'H':
			LEGAL_ALT(ALT_O);
			if (!(s_conv_num(&bp, &tim->tm_hour, 0, (INT_MAX-1)/3600)))
				return (0);
			break;

		case 'M':	/* The minute. */
			LEGAL_ALT(ALT_O);
			if (!(s_conv_num(&bp, &tim->tm_min, 0, (INT_MAX-1)/60)))
				return (0);
			break;

		case 'S':	/* The seconds. */
			LEGAL_ALT(ALT_O);
			if (!(s_conv_num(&bp, &tim->tm_sec, 0, INT_MAX-1)))
				return (0);
			break;

		case '-':	/* 0 seconds (thx freebsd "w") */
			if (*bp++ != '-')
				return (0);
            tim->tm_sec = 0;
			break;

		case 'Y':	/* The year. */
			LEGAL_ALT(ALT_E);
			if (!(s_conv_num(&bp, &i, 0, 3000))) 
				return (0);

			tim->tm_year = i - 1900;
			break;
		case 'y':
			LEGAL_ALT(ALT_E);
			if (!(s_conv_num(&bp, &i, 0, 100))) // reasonal?
				return (0);

			tim->tm_year = i;
			break;

		/*
		 * Miscellaneous conversions.
		 */
		case 'n':	/* Any kind of white-space. */
		case 't':
			LEGAL_ALT(0);
			while (isspace(*bp))
				bp++;
			break;

		case 'B':
		case 'b':
			LEGAL_ALT(0);
			if (bp[0] == 0 || bp[1] == 0 || bp[2] == 0)
				return (0);
			match = 0;
			for (i = 0; i< sizeof(Month)/sizeof(Month[0]); i++)
			{
				if (memcmp(bp, Month[i], 3) == 0)
				{
					tim->tm_mon = i;
					match = 1;
					bp += 3;
					break;
				}
			}
			if (!match)
				return (0);
			break;

		default:	/* Unknown/unsupported conversion. */
			return (0);
		}
	}

	/* LINTED functional specification */
	return const_cast<char *>(bp);
} // end of ech_strptime_p()

static CString ConvertDate(char* str)
{
	struct tm tim;
	TCHAR output[9];

	memset(&tim, 0, sizeof(struct tm));

	if (str[0] == 0 || 
			str[1] == 0 ||
			str[2] == 0 ||
			str[3] == 0 ||
			str[4] == 0
			)
		return _T("");

	if (ech_strptime_p(str + 4, "%b %d %H:%M:%S %Y", &tim))
	{
		_tcsftime(output, sizeof(output), _T("%Y%m%d"), &tim);
		return output;
	}
	return _T("");
}

CString Reports::GetDateFromReportString(char* str)
{
	return ConvertDate(str);
}

std::list< std::pair<const char*, int> >  Reports::ParseReportsString(const char* txt)
{
	std::list< std::pair<const char*, int> > lst;
	CString date = GetDateFromReportString((char*)txt);
	const char* pos = txt;
	const char* secionPos = txt;

	if (date.GetLength() == 0)
		return lst;

	while(1)
	{
		CString curDate = GetDateFromReportString((char*)pos);
		if (curDate.GetLength() && curDate != date)
		{
			lst.push_back(std::pair<const char*,int>(secionPos, pos - secionPos));
			date = curDate;
			secionPos = pos;
		}
		pos = strchr(pos, '\n');
		if (!pos)
		{
			lst.push_back(std::pair<const char*,int>(secionPos, strlen(secionPos)));
			return lst;
		} 
		else
			pos++;
	}
	return lst;
}

BOOL Reports::CameraStringParser(const char* str, REPORT_CAMERA_LIST& ClientListReport)
{
	CameraLine line;

	REPORT_CAMERA_LIST lst;
	
	char* s = strdup(str);
	char* tmp = s;
	char* tmp2 = s;
	
	while(*tmp) 
	{
		if (*tmp == '\r')
		{
			tmp++;
			continue;
		}
		*tmp2++ = *tmp++;
	}
	*tmp2 = 0;
	tmp = s;
	
	while(*tmp) 
	{
		char* end = strchr(tmp, '\n');
		if (end) 
			*end = 0;
		
		char* comma1 = strchr(tmp, ',');
		if (!comma1)
		{
			if (end) 
			{
				tmp = ++end;
				continue;
			} 
			else 
				break;
		}
		*comma1 = 0;
		char* comma2 = strchr(comma1 + 1, ',');
		if (!comma2)
		{
			if (end) 
			{
				tmp = ++end;
				continue;
			}
			else 
				break;
		}
		*comma2 = 0;
		char* comma3 = strchr(comma2 + 1, ',');
		if (!comma3)
		{
			if (end) 
			{
				tmp = ++end;
				continue;
			}
			else 
				break;
		}
		*comma3 = 0;

		line.DateTime = tmp;
		line.FileName = comma1 + 1;
		line.size = atoi(comma2 + 1);
		line.md5 = comma3 + 1;

		lst.push_front(line);
		if (end)
			tmp = ++end;
		else 
			break;
	}
	free(s);
	lst.reverse();
	ClientListReport.splice(ClientListReport.end(), lst);
	return TRUE;
}

BOOL Reports::StringParser(const char* str, REPORT_LIST& ClientListReport)
{
	ReportLine line;

	REPORT_LIST lst;
	
	char* s = strdup(str);
	char* tmp = s;
	char* tmp2 = s;
	
	while(*tmp) 
	{
		if (*tmp == '\r')
		{
			tmp++;
			continue;
		}
		*tmp2++ = *tmp++;
	}
	*tmp2 = 0;
	tmp = s;
	
	while(*tmp) 
	{
		char* end = strchr(tmp, '\n');
		if (end) 
			*end = 0;
		
		char* comma1 = strchr(tmp, ',');
		if (!comma1)
		{
			if (end) 
			{
				tmp = ++end;
				continue;
			} 
			else 
				break;
		}
		*comma1 = 0;
		char* comma2 = strchr(comma1 + 1, ',');
		if (!comma2)
		{
			if (end) 
			{
				tmp = ++end;
				continue;
			}
			else 
				break;
		}
		*comma2 = 0;
		line.DateTime = tmp;
		line.Duration = atoi(comma1 + 1);
		line.FileName = comma2 + 1;
		
		lst.push_front(line);
		if (end)
			tmp = ++end;
		else 
			break;
	}
	free(s);
	lst.reverse();
	ClientListReport.splice(ClientListReport.end(), lst);
	return TRUE;
}

CString Reports::HumanReadableSize(__int64 size)
{
	double tmp;
	CString suffix;
	CString ret;

	if (size > 1024 * 1024 * 1024) {
		tmp = (double)size / (1024 * 1024 * 1024);
		suffix = _T("GB");
	} else if (size > 1024 * 1024) {
		tmp = (double)size / (1024 * 1024);
		suffix = _T("MB");
	} else if (size > 1024) {
		tmp = (double)size / 1024;
		suffix = _T("KB");
	} else {
		tmp = (double)size;
		suffix = _T("");
	}
	ret.Format(_T("%.2f %s"), tmp, suffix);
	return ret;
}



REPORT_LIST gTrashReportList;
CMutex gTrashReportListMutex;

class CReportTrashThread: public CThreadClient
{
	virtual UINT OnThread(int ThreadNum);
};

UINT CReportTrashThread::OnThread(int ThreadNum)
{
	while(1)
	{
		gTrashReportListMutex.Lock();
		if (gTrashReportList.size())
			gTrashReportList.erase(gTrashReportList.begin());
		else
			break;
		gTrashReportListMutex.Unlock();
	}
	gTrashReportListMutex.Unlock();
	return 0;
}

CReportTrashThread gTrashReportThreadClient;
CThread gTrashReportThread;

void Reports::TrashReport(REPORT_LIST& lst)
{
	gTrashReportListMutex.Lock();
	gTrashReportList.splice(gTrashReportList.end(), lst);
	gTrashReportListMutex.Unlock();

	if (!gTrashReportThread.IsActive())
	{
		gTrashReportThread.WaitStopThread();
		gTrashReportThread.Create(&gTrashReportThreadClient);
	}
}

void Reports::SkipOutdated(std::list<CString>& lst)
{
	CString oldest = Reports::GetOldestDate(TRUE);

	while(lst.size())
	{
		if(lst.front() >= oldest)
			break;

		lst.erase(lst.begin());
	}
}

BOOL Reports::GetReports(const CString& client, const CString& range, 
						 int page, int pageSize, 
						 void* todayData, int todayLen, 
						 REPORT_LIST& lst, int& pagesCount)
{
	REPORT_LIST lstRet;
	std::list<CString> rng = Reports::SplitRange(range);
	std::list<CString>::iterator i;
	int count = 0;
	CString curDay = Reports::GetCurrentDate();

	pagesCount = 0;

	static CMutex cacheMutex;
	static CString cacheRange;
	static CString cacheClient;
	static REPORT_LIST cacheList;

	int end = page * pageSize;
	int start = end - pageSize;

	if (rng.size() == 0)
		return TRUE;


	cacheMutex.Lock();
	if (cacheRange == range && cacheClient == client)
	{
		if (rng.back() == curDay && todayData == NULL &&
			cacheList.size() < end)
		{
			cacheMutex.Unlock();
			return FALSE;
		}

		int nWithoutToday = cacheList.size();
		if (todayData && rng.back() == curDay)
		{
			void* data;
			int len;

			if(!UncompressGZIPBuffer(todayData, todayLen, &data, &len))
			{
				cacheMutex.Unlock();
				return FALSE;
			}			
			StringParser((const char *)data, cacheList);
			delete [] data;
		}
		int tmp = start;
		REPORT_LIST::iterator newbegin = cacheList.begin();
		while(tmp-- && newbegin != cacheList.end())
			newbegin++;
		count = pageSize;
		while(count--)
		{
			if (newbegin == cacheList.end())
				break;
			lst.push_back(*newbegin);
			newbegin++;
		}
		
		if(cacheList.size() % pageSize != 0)
			pagesCount = cacheList.size() / pageSize + 1;
		else
			pagesCount = cacheList.size() / pageSize;

		if (cacheList.size() != nWithoutToday)
			cacheList.resize(nWithoutToday);
		cacheMutex.Unlock();
		return TRUE;
	}
	TrashReport(cacheList);

	for(i = rng.begin(); i != rng.end(); i++)
	{
		void* adata;
		int alen;
		void* data;
		int len;

		REPORT_LIST tmp;
		if((*i) == curDay)
		{
			continue;
		}
		else 
		{
			if (!GetReportArchive(client, *i, &adata, &alen))
			{
				cacheMutex.Unlock();
				return FALSE;
			}
		}
		if(!UncompressGZIPBuffer(adata, alen, &data, &len))
		{
			if(alen && (*i) != curDay)
				delete[] adata;
			cacheMutex.Unlock();
			return FALSE;
		}
		if(alen && (*i) != curDay)
			delete[] adata;

		StringParser((const char *)data, cacheList);
		delete [] data;		
	}
	cacheRange = range;
	cacheClient = client;
	BOOL ret = GetReports(client, range, page, pageSize, todayData, todayLen, lst, pagesCount);
	cacheMutex.Unlock();
	return ret;
}

static CString my_itoa(int n)
{
	CString tmp;
	tmp.Format(_T("%d"), n);
	return tmp;
}

CString Reports::SecondsToString(int seconds)
{	
	int day = seconds / (24 * 60 * 60);
	int rest = seconds % (24 * 60 * 60);
	int hour = rest / (60 * 60);
	rest = rest % (60 * 60);
	int minutes = rest / 60;
	int sec = rest % 60;
	
	CString res = "";
	if (day) {
		if (day == 1)
			res = res + my_itoa(day) + _TRANS(_T(" day, "));
		else
			res = res + my_itoa(day) + _TRANS(_T(" days, "));
	} 
	
	if(hour) {
		if(hour == 1)
			res = res + my_itoa(hour) + _TRANS(_T(" hour, "));
		else
			res = res + my_itoa(hour) + _TRANS(_T(" hours, "));
	}
	
	if(minutes) {
		if(minutes == 1)
			res = res + my_itoa(minutes) + _TRANS(_T(" minute, "));
		else
			res = res + my_itoa(minutes) + _TRANS(_T(" minutes, "));
	} 
	
	if(sec) {
		if(sec == 1)
			res = res + my_itoa(sec) + _TRANS(_T(" second"));
		else
			res = res + my_itoa(sec) + _TRANS(_T(" seconds"));
	} 
	
	return res;
}

struct ReportStatSt
{
	int duration;
	int times;
};

typedef std::map< std::string, struct ReportStatSt > REPORT_STATISTIC_MAP;

BOOL Reports::GetReportsStatistics(REPORT_LIST &lst, REPORT_STATISTICS_LIST& ret, CThread* watchThread)
{
	REPORT_STATISTIC_MAP retMap;
	for (REPORT_LIST_ITERATOR i = lst.begin(); 
		i!= lst.end(); i++)
	{
		if(watchThread && watchThread->IsStopped())
			return FALSE;

		REPORT_STATISTIC_MAP::iterator mi = retMap.find((*i).FileName);

		if(mi == retMap.end())
		{
			struct ReportStatSt tmp;
			tmp.duration = (*i).Duration;
			tmp.times = 1;			
			retMap[(*i).FileName] = tmp;
		}
		else
		{
			(*mi).second.duration += (*i).Duration;
			(*mi).second.times++;
		}
	}

	REPORT_STATISTICS_LIST tmpLst;	
	for(REPORT_STATISTIC_MAP::iterator j = retMap.begin(); j != retMap.end(); j++)
	{
		StatisticsLine tmpLine;
		tmpLine.FileName = (*j).first;
		char* conv_sec = StringToUTF8(SecondsToString((*j).second.duration));
		tmpLine.WholeDuration = conv_sec;
		delete[] conv_sec;
		tmpLine.TimesPlayed = (*j).second.times;
		
		tmpLst.push_front(tmpLine);
	}
	tmpLst.reverse();
		
	ret.splice(ret.end(), tmpLst);
	return TRUE;
}


std::list< std::pair<const char*, int> >::iterator Reports::staticFindDay(std::list< std::pair<const char*, int> >& lst, const CString& day)
{
	std::list< std::pair<const char*, int> >::iterator i;
	for (i = lst.begin(); i != lst.end(); i++)
	{
		if (Reports::GetDateFromReportString((char*)(*i).first) == day)
			return i;
	}
	return i;
}


#ifdef _DEBUG

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

class TestReports: public CppUnit::TestFixture
{
public:
	void setUp()
	{
		DeleteFile(GetServerReportsDirectory(NULL) + _T("xxx\\20090101.gz"));
		DeleteFile(GetServerReportsDirectory(NULL) + _T("xxx\\") + Reports::GetPrevDay(Reports::GetCurrentDate()) + _T(".gz"));
		DeleteFile(GetServerReportsDirectory(NULL) + _T("xxx\\20101027.gz"));
	}
	void tearDown()
	{
		DeleteFile(GetServerReportsDirectory(NULL) + _T("xxx\\20090101.gz"));
		DeleteFile(GetServerReportsDirectory(NULL) + _T("xxx\\") + Reports::GetPrevDay(Reports::GetCurrentDate()) + _T(".gz"));
		DeleteFile(GetServerReportsDirectory(NULL) + _T("xxx\\20101027.gz"));
	}

	void testDate()
	{
		std::list<CString> l = Reports::ListReports(_T("xxx"));
		CPPUNIT_ASSERT_EQUAL(0, (int)l.size());
		Reports::SetReportArchive(_T("xxx"), _T("20090101"), _T("12345"), 5);
		l = Reports::ListReports(_T("xxx"));
		CPPUNIT_ASSERT_EQUAL(0, (int)l.size());
		Reports::SetReportArchive(_T("xxx"), Reports::GetPrevDay(Reports::GetCurrentDate()), _T("12345"), 5);
		l = Reports::ListReports(_T("xxx"));
		CPPUNIT_ASSERT_EQUAL(1, (int)l.size());
	}

	void testNextDay()
	{
		CPPUNIT_ASSERT(CString(_T("20010102")) == Reports::GetNextDay(_T("20010101")));
	}

	void testRanges()
	{
		CString first;
		CString second;
		Reports::ParseRange(_T("20010101-20010103"), first, second);
		CPPUNIT_ASSERT(first == _T("20010101"));
		CPPUNIT_ASSERT(second == _T("20010103"));
	}

	void testFindFirstDay()
	{
		std::list<CString> lst;
		std::list<CString>::iterator i;

		lst.push_back(_T("20010101"));
		lst.push_back(_T("20010102"));
		lst.push_back(_T("20010103"));
		lst.push_back(_T("20010105"));

		i = Reports::FindFirstDay(lst, _T("20010102"));
		CPPUNIT_ASSERT(CString(_T("20010102")) == *i);
		i = Reports::FindFirstDay(lst, _T("20010104"));
		CPPUNIT_ASSERT(CString(_T("20010105")) == *i);
		i = Reports::FindFirstDay(lst, _T("20000104"));
		CPPUNIT_ASSERT(CString(_T("20010101")) == *i);
		i = Reports::FindFirstDay(lst, _T("20010106"));
		CPPUNIT_ASSERT(lst.end() == i);
	}

	void testArchiveBuffer()
	{
		char test[] = "this is test!!!!!";
		void* data;
		void* data2;
		int len = 0;
		Reports::CompressGZIPBuffer(test, strlen(test) + 1, &data, &len);
		CPPUNIT_ASSERT(len != strlen(test));
		Reports::UncompressGZIPBuffer(data, len, &data2, &len);
		CPPUNIT_ASSERT(strcmp(test, (const char*)data2) == 0);
	}

	void testCache()
	{
		char x1[] = "abc";
		void *data;
		int len;
		CPPUNIT_ASSERT_EQUAL(FALSE, Reports::GetCacheDay(_T("myclient"), 
							_T("20010101"), &data, &len));
		Reports::SetCacheDay(_T("myclient"), 
							_T("20010101"), x1, 3);
		CPPUNIT_ASSERT_EQUAL(TRUE, Reports::GetCacheDay(_T("myclient"), 
							_T("20010101"), &data, &len));
		CPPUNIT_ASSERT(memcmp(x1, data, 3) == 0);
		delete[] data;
		Reports::SetCacheDay(_T("myclient"), 
							_T("20010102"), x1, 3);
		CPPUNIT_ASSERT_EQUAL(TRUE, Reports::GetCacheDay(_T("myclient"), 
							_T("20010102"), &data, &len));
		CPPUNIT_ASSERT(memcmp(x1, data, 3) == 0);
	}

	void testIntersectRelativeComplementRanges()
	{
		static std::list<CString> rangeLst = Reports::SplitRange(_T("20090101-20090106,20090108,20090110-20090111"));

		CPPUNIT_ASSERT_EQUAL(9, (int)rangeLst.size());
		CPPUNIT_ASSERT_EQUAL(std::string("20090101"), 
			std::string(StringToUTF8(rangeLst.front())));
		CPPUNIT_ASSERT_EQUAL(std::string("20090111"), 
			std::string(StringToUTF8(rangeLst.back())));


		CPPUNIT_ASSERT_EQUAL(std::string("20000102"), 
				std::string(StringToUTF8(
		Reports::IntersectRanges(_T("20000101-20000105"), _T("20000102-20000102")))));
		
		CPPUNIT_ASSERT_EQUAL(std::string("20000102-20000103"), 
			std::string(StringToUTF8(
			Reports::IntersectRanges(_T("20000101-20000105"), _T("20000102-20000103")))));
		CPPUNIT_ASSERT_EQUAL(std::string("20000104-20000105,20000107"), 
			std::string(StringToUTF8(
			Reports::IntersectRanges(_T("20000101-20000105,20000107"), _T("20000104-20000108")))));
		CPPUNIT_ASSERT_EQUAL(std::string("20000101-20000103"), 
			std::string(StringToUTF8(
			Reports::RelativeComplementRanges(_T("20000101-20000105,20000107"), _T("20000104-20000108")))));
		CPPUNIT_ASSERT_EQUAL(std::string(""), 
			std::string(StringToUTF8(
			Reports::RelativeComplementRanges(_T("20000101-20000105,20000107"), _T("20000101-20000105,20000107")))));
	}

	void testGetNextDay()
	{
		CPPUNIT_ASSERT_EQUAL(std::string("20081027"), 
			std::string(StringToUTF8(
			Reports::GetNextDay(_T("20081026")))));
		CPPUNIT_ASSERT_EQUAL(std::string("20081026"), 
			std::string(StringToUTF8(
			Reports::GetPrevDay(_T("20081027")))));
	}

	void testGetDateFromReportString()
	{
		CPPUNIT_ASSERT_EQUAL(std::string("20101029"),
			std::string(StringToUTF8(
			Reports::GetDateFromReportString("Fri Oct 29 11:54:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n"
			))));
		CPPUNIT_ASSERT_EQUAL(std::string(""),
			std::string(StringToUTF8(
			Reports::GetDateFromReportString(""
			))));
	}

	void testParseReportString()
	{
		std::list< std::pair<const char*, int> > lst;
		lst = Reports::ParseReportsString("Fri Oct 29 11:54:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n");
		CPPUNIT_ASSERT_EQUAL(1, (int)lst.size());
		lst = Reports::ParseReportsString("Fri Oct 29 11:54:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n"
			"Fri Oct 29 11:56:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n");
		CPPUNIT_ASSERT_EQUAL(1, (int)lst.size());
		lst = Reports::ParseReportsString("Fri Oct 29 11:54:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n"
			"Fri Oct 29 11:56:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n"
			"Sat Oct 30 11:56:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n");
		CPPUNIT_ASSERT_EQUAL(2, (int)lst.size());
		CPPUNIT_ASSERT(memcmp("Sat Oct 30 11:56:03 2010", lst.back().first, 20) == 0);
		lst = Reports::ParseReportsString("");
		CPPUNIT_ASSERT_EQUAL(0, (int)lst.size());
		lst = Reports::ParseReportsString("\n");
		CPPUNIT_ASSERT_EQUAL(0, (int)lst.size());
		lst = Reports::ParseReportsString("Fri Oct 29 11:54:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n\n"
			"Fri Oct 29 11:56:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n"
			"Sat Oct 30 11:56:03 2010,14,F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi\n");
		CPPUNIT_ASSERT_EQUAL(2, (int)lst.size());
	}

	void testGetReports()
	{
		REPORT_LIST lst;
		unsigned char bindata_20101027[] = {
0x1f, 0x8b,0x08, 0x08, 0xf4, 0x33, 0xcb, 0x4c, 0x02, 0xff, 0x2f, 0x6d, 0x65, 0x64, 0x69, 
0x61, 0x2f,0x53, 0x69, 0x67, 0x6e, 0x61, 0x67, 0x65, 0x50, 0x72, 0x6f, 0x2f, 0x72, 0x65, 
0x70, 0x6f,0x72, 0x74, 0x73, 0x2f, 0x72, 0x65, 0x70, 0x6f, 0x72, 0x74, 0x2e, 0x32, 0x30, 
0x31, 0x30,0x31, 0x30, 0x32, 0x37, 0x00, 0xed, 0xd0, 0xdd, 0x4a, 0xc2, 0x60, 0x1c, 0xc0, 
0xe1, 0xf3,0xa0, 0x7b, 0x78, 0x2f, 0x40, 0xe5, 0xdd, 0x94, 0xa9, 0x3b, 0xcd, 0xec, 0xa4, 
0x28, 0x28,0xb0, 0x83, 0xc1, 0x98, 0x3a, 0x6a, 0xa5, 0x26, 0x29, 0xa3, 0xcb, 0xaf, 0x20, 
0xe8, 0x20,0xa2, 0xe3, 0xc5, 0x73, 0x01, 0xff, 0x8f, 0xe7, 0xb7, 0xa8, 0xd7, 0xe1, 0x7a, 
0x75, 0x0c,0xe9, 0x38, 0x24, 0xd3, 0x7c, 0x94, 0xe4, 0x31, 0x86, 0x34, 0x26, 0xb1, 0x37, 
0x8c, 0x59,0x6f, 0x9e, 0x17, 0x6d, 0xb3, 0xae, 0x5f, 0x8a, 0x45, 0xb3, 0x5b, 0x6f, 0x9b, 
0xcd, 0x66,0x5e, 0xbd, 0x6e, 0xcb, 0xfb, 0xb6, 0x99, 0x95, 0x57, 0x37, 0xe7, 0x17, 0x65, 
0x16, 0x63,0x7c, 0x5e, 0xee, 0x0f, 0x65, 0x92, 0x4e, 0xe2, 0x5b, 0x12, 0xd3, 0xd1, 0xa0, 
0x6a, 0x9b,0xd3, 0x93, 0xc5, 0x6f, 0x4b, 0x3f, 0x06, 0x7a, 0x67, 0x79, 0x51, 0xed, 0x8f, 
0x6d, 0x5a,0x8c, 0xfa, 0xc3, 0x7e, 0xcc, 0x42, 0x4c, 0x27, 0x83, 0xa7, 0xfd, 0xc3, 0x8f, 
0xa9, 0x2c,0x8f, 0xe3, 0xaf, 0x57, 0x92, 0xe9, 0xf7, 0x2b, 0xb7, 0x55, 0x5b, 0x87, 0xbb, 
0xc7, 0x3a,0x5c, 0x56, 0x87, 0x63, 0x98, 0x55, 0xbb, 0x55, 0xfd, 0x79, 0xf4, 0x8f, 0xc3, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 
0x34, 0x34,0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0xff, 
0x44, 0xa3,0x32, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x4d, 0xd7, 0x35, 0x2a, 0xd3, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0x74,0x5d, 0xa3, 0x32, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x4d, 0xd7, 0x35, 0x2a, 0xd3, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0x74, 0x5d, 0xa3, 0x32, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x4d, 0xd7, 0x35, 0x2a, 
0xd3, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 
0xd0, 0xd0,0xd0, 0xd0, 0xd0, 0x74, 0x5d, 0xa3, 0x32, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 
0x0d, 0x0d,0x0d, 0x0d, 0x0d, 0x4d, 0xf7, 0x34, 0xef, 0x72, 0xb2, 0x6b, 0xfc, 0x24, 0xb6, 
0x06, 0x00}; 
		
		int pagesCount;
		Reports::GetReports(_T("xxx"), _T("20101027"), 1, 30, (void *)bindata_20101027, sizeof(bindata_20101027), lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(0, (int)lst.size());
		Reports::GetReports(_T("xxx"), Reports::GetCurrentDate(), 1, 30, (void *)bindata_20101027, sizeof(bindata_20101027), lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(30, (int)lst.size());
		CPPUNIT_ASSERT_EQUAL(std::string("F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi"),
			lst.front().FileName);
		lst.clear();
		Reports::GetReports(_T("xxx"), Reports::GetCurrentDate(), 2, 5, (void *)bindata_20101027, sizeof(bindata_20101027), lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(std::string("F:\\video\\Save The Last Dance.avi.avi"),
			lst.front().FileName);
		CPPUNIT_ASSERT_EQUAL(5, (int)lst.size());
		lst.clear();
		

		char data[] = "Wed Oct 27 19:41:00 2010,306,A\n"
		"Wed Oct 27 19:41:00 2010,600,A.jpg\n"
		"Wed Oct 27 19:46:07 2010,319,The Last Dance.avi.avi\n"
		"Wed Oct 27 19:41:00 2010,306,F:\\VIDEO4.avi\n"
		"Wed Oct 27 19:41:00 2010,600,4-3-06 028.jpg\n";

		void* adata;
		int alen;
		CString yesterday = Reports::GetPrevDay(Reports::GetCurrentDate());
		Reports::CompressGZIPBuffer(data, strlen(data), &adata, &alen);
		Reports::SetReportArchive(_T("xxx"), yesterday, 
			adata, alen);
		Reports::GetReports(_T("xxx"), yesterday, 1, 30, NULL, 0, lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(5, (int)lst.size());
		lst.clear();

		BOOL ret = Reports::GetReports(_T("xxx"), yesterday + _T("-") + Reports::GetCurrentDate(), 1, 30, NULL, 0, lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(FALSE, ret);
		CPPUNIT_ASSERT_EQUAL(0, (int)lst.size());

		ret = Reports::GetReports(_T("xxx"), yesterday + _T("-") + Reports::GetCurrentDate(), 1, 3, NULL, 0, lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(TRUE, ret);
		CPPUNIT_ASSERT_EQUAL(3, (int)lst.size());

		lst.clear();
		ret = Reports::GetReports(_T("xxx"), yesterday + _T("-") + Reports::GetCurrentDate(), 2, 3, NULL, 0, lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(FALSE, ret);
		CPPUNIT_ASSERT_EQUAL(0, (int)lst.size());

		ret = Reports::GetReports(_T("xxx"), yesterday + _T("-") + Reports::GetCurrentDate(), 2, 3, (void *)bindata_20101027, sizeof(bindata_20101027), lst, pagesCount);
		CPPUNIT_ASSERT_EQUAL(TRUE, ret);
		CPPUNIT_ASSERT_EQUAL(3, (int)lst.size());

		CPPUNIT_ASSERT_EQUAL(std::string("F:\\VIDEO4.avi"),
			lst.front().FileName);
		CPPUNIT_ASSERT_EQUAL(std::string("F:\\video\\WindmillFarm_XviD_MPEG_6000kbps_1280x1024.avi"),
			lst.back().FileName);

		REPORT_LIST::iterator x = lst.begin();
		x++;
		CPPUNIT_ASSERT_EQUAL(std::string("4-3-06 028.jpg"),
			(*x).FileName);
	}

	
	CPPUNIT_TEST_SUITE(TestReports);
	CPPUNIT_TEST(testDate);
	CPPUNIT_TEST(testNextDay);
	CPPUNIT_TEST(testArchiveBuffer);
	CPPUNIT_TEST(testFindFirstDay);
	CPPUNIT_TEST(testRanges);
	CPPUNIT_TEST(testIntersectRelativeComplementRanges);
	CPPUNIT_TEST(testCache);
	CPPUNIT_TEST(testGetNextDay);
	CPPUNIT_TEST(testGetDateFromReportString);
	CPPUNIT_TEST(testParseReportString);
	CPPUNIT_TEST(testGetReports);
	CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestReports);

#endif // _DEBUG

