#ifndef _CREEPING_H_
#define _CREEPING_H_

#include <vector>
#include <string>

#include "base_window.h"
#include "file_reader.h"
#include "font_render.h"
#include "message_reader.h"

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


// work with miliseconds/pixels
class CTimer
{
	public:
		CTimer();
		void set_interval(int microsec_interval);
		int get_val();
		void start();
		void set_val(int v);
		void offset(int v);
	private:
		int interval;
		timeval start_time;
};

class Creeping : public CThread
{
	public:
		Creeping();
		~Creeping();
		int open_conf(const char *filename);
		void play_once();
	private:
		static void * redraw_window(void * _this);
		void init();
		void deinit();
		
		bool is_once;
		ConfFile conf;
		Window * wnd;
		FontRender * font_render;
		pthread_mutex_t mutex;
		unsigned int pos;
};







#endif //_CREEPING_H_
