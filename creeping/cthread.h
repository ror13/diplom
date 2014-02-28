#include <pthread.h>
#include <unistd.h>
class CThread
{
	public:
		CThread(void *(*funct) (void *), void * data);
		virtual ~CThread();
		void start();
		void stop();
		void join();
		bool is_run();
	private:
		void * funct_param;
		void *(*funct_ptr) (void *);
		pthread_t pid;
		bool runing;

};

class CMutex
{
	public:
		CMutex();
		~CMutex();
		void lock();
		void unlock();
	private:
		void * mut;

};
