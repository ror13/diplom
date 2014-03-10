// global_mutex.h
//////////////////////////////////////////////////////////////////////////

class CGlobalMutex
{
public:
	CGlobalMutex();
	~CGlobalMutex();

	BOOL OpenMutex(LPCTSTR Name = NULL);
	BOOL CloseMutex();

	BOOL Lock(DWORD Timeout = INFINITE);
	BOOL Unlock();

protected:
	HANDLE	m_hMutex;
};