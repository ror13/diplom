#ifndef __APPLICA_REPORTS_H__
#define __APPLICA_REPORTS_H__

#include <list>

#include "Thread.h"

struct ReportLine
{
	std::string DateTime;
	int Duration;
	std::string FileName;
};

struct StatisticsLine
{
	std::string FileName;
	std::string WholeDuration;
	int TimesPlayed;
};

struct CameraLine
{
	std::string DateTime;
	std::string FileName;
	int size;
	std::string md5;
};

typedef std::list<ReportLine> REPORT_LIST;
typedef std::list<CameraLine> REPORT_CAMERA_LIST;
typedef REPORT_LIST::iterator REPORT_LIST_ITERATOR;
typedef std::list<StatisticsLine> REPORT_STATISTICS_LIST;

class Reports
{
	public:
		static CCriticalSection m_gStoreMutex;
		static unsigned char nullData[22];
		//static Reports* m_reports;

		//static Reports* GetInstance();
		static void CleanServerReports(const CString& client);
		static CString GetOldestDate(BOOL reg);
		static CString GetCurrentDate();
		static CString GetNextDay(const CString& date);
		static CString GetPrevDay(const CString& date);
		static BOOL ParseRange(const CString& range, CString& first, CString& second);
		static BOOL MakeDateRange(const CString& first, const CString& second, CString& range);

		static BOOL GetReportArchive(const CString& client, 
									 const CString date, 
									 void** data, int* len);
		static BOOL SetReportArchive(const CString& client, 
									 const CString date, 
									 void* data, int len);
		static std::list<CString> ListReports(const CString& client);
		static std::list<CString>::iterator FindFirstDay(std::list<CString>& dates, const CString& date);
		static BOOL CompressGZIPBuffer(void* inp, int len, void** outb, int*outLen);
		static BOOL UncompressGZIPBuffer(void* inp, int len, void** outb, int*outLen);
		static BOOL UncompressGZIPFile(const CString& name, void** outb, int*outLen);
		static BOOL CompressGZIPFile(const CString& name, void* inb, int inLen);
		static BOOL CompressGZIPFile(const CString& name_gzip, const CString& in_name);

		static std::list<CString> SplitRange(CString range);
		static CString IntersectRanges(const std::list<CString>& range1, const std::list<CString>&  range2);
		static CString IntersectRanges(const CString range1, const CString range2);
		static CString RelativeComplementRanges(const std::list<CString>& range1, const std::list<CString>&  range2);
		static CString RelativeComplementRanges(const CString range1, const CString range2);

		static CString HumanReadableSize(__int64 size);


		static std::list<CString> ListCache(const CString& client);
		static BOOL GetCacheDay(const CString& client, const CString& day, void** data, int* len);
		static BOOL SetCacheDay(const CString& client, const CString& day, void* data, int len);
		static void CleanCache();

		static CString GetDateFromReportString(char* str);
		static std::list< std::pair<const char*, int> >  ParseReportsString(const char* txt);
		static BOOL StringParser(const char* str, REPORT_LIST& ClientListReport);
		static BOOL CameraStringParser(const char* str, REPORT_CAMERA_LIST& ClientListReport);

		static void TrashReport(REPORT_LIST& lst);
		static void SkipOutdated(std::list<CString>& lst);
		static BOOL GetReports(const CString& client, const CString& range, 
			int page, int pageSize, void* todayData, int todayLen, REPORT_LIST& lst, int& pagesCount);
		static BOOL GetReportsStatistics(REPORT_LIST &lst, REPORT_STATISTICS_LIST& ret, CThread* watchThread);
		static CString SecondsToString(int seconds);
		static std::list< std::pair<const char*, int> >::iterator staticFindDay(std::list< std::pair<const char*, int> >& lst, const CString& day);
};

#endif
