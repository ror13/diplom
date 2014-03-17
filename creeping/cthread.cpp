#include <cthread.h>
#ifndef _WIN32
#include <sys/signal.h>
#endif

#include "utils.h"
#include "debug_utils.h"

CThread::
CThread(void *(*funct) (void *), void * data)
{
	DEBUG_PRINT_LINE;
	funct_ptr = funct;
	funct_param = data;
	runing = false;	
	DEBUG_PRINT_LINE;
}

void CThread::
start()
{
	DEBUG_PRINT_LINE;
	stop();
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create( &pid, &attr, funct_ptr, funct_param);
	pthread_attr_destroy(&attr);
	runing = true;
	DEBUG_PRINT_LINE;
}

void CThread::
join()
{
	DEBUG_PRINT_LINE;
	if(!runing)
		return;
	while(pthread_kill(pid, 0) == 0)
		sleep(1);
	pthread_join(pid, NULL);
	runing = false;
	DEBUG_PRINT_LINE;
}

void CThread::
stop()
{
	DEBUG_PRINT_LINE;
	if(!runing)
		return;
	pthread_cancel(pid);
	join();
	DEBUG_PRINT_LINE;
}

bool CThread::
is_run()
{
	DEBUG_PRINT_LINE;
	return runing;
}

CThread::
~CThread()
{
	DEBUG_PRINT_LINE;
	stop();
	DEBUG_PRINT_LINE;
}





CMutex::
CMutex()
{
	mut = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*) mut, NULL);
}

CMutex::
~CMutex()
{
	pthread_mutex_destroy((pthread_mutex_t*) mut);
	delete mut;
}
void CMutex::
lock()
{
	pthread_mutex_lock((pthread_mutex_t*) mut);
}
void CMutex::
unlock()
{
	pthread_mutex_unlock((pthread_mutex_t*) mut);
}
