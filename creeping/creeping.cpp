#include "creeping.h"
#include "debug_utils.h"
#include <sys/time.h>
//----------------------------------------------CThread -------------------

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
	pthread_join(pid, NULL);
	runing = false;
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


//----------------------------------------------CTimer -------------------



CTimer::
CTimer()
{
	interval = 1000000;
}

void CTimer::
start()
{
	gettimeofday(&start_time, NULL);
}
void CTimer::
set_interval(int microsec_interval)
{
	interval = microsec_interval;
}

int CTimer::
get_val()
{
	
	timeval current_val;
	gettimeofday(&current_val, NULL);
	int sec = current_val.tv_sec - start_time.tv_sec;
	int usec  = current_val.tv_usec - start_time.tv_usec;
	if(usec < 0){
		usec += 1000000;
		sec--;
	}
	int pos = (usec + sec * 1000000) / interval;
	return pos;
}

void CTimer::
set_val(int v)
{
	gettimeofday(&start_time, NULL);
	offset(v);
}

void CTimer::
offset(int v)
{
	int usec = v * interval;
	start_time.tv_sec += usec/1000000;
	start_time.tv_usec += usec%1000000;
	if(start_time.tv_usec < 0){
		start_time.tv_usec += 1000000;
		start_time.tv_sec--;
	}
	
}



//----------------------------------------------Creeping -------------------
Creeping::
Creeping() : CThread(&Creeping::redraw_window, this)
{
	DEBUG_PRINT_LINE;
	wnd = new_window();
	pthread_mutex_init(&mutex, NULL);
	font_render = NULL;
	is_once = false;
	DEBUG_PRINT_LINE;
}

Creeping::
~Creeping()
{
	DEBUG_PRINT_LINE;
	deinit();
	delete wnd;
	pthread_mutex_destroy(&mutex);
	DEBUG_PRINT_LINE;
}



void Creeping::
init()
{
	DEBUG_PRINT_LINE;

	deinit();
	
	font_render = new FontRender(conf.get_conf());
	
	std::vector<std::string>  text;
	std::string logo;
	
	MessageReader mr;
	mr.make(conf.get_conf()->lineText, conf.get_conf()->bRss, &text, &logo);

	std::vector<std::string>::iterator i;
	
	for (i = text.begin(); i != text.end(); i++)
	{
		CSurface * surf = new_surface(wnd);
		surf->set_tex_color(conf.get_conf()->window.clForeground);
		surf->set_text( &(*i), font_render);
	}
	
	if(!logo.empty())
	{
		CSurface * surf = new_surface(wnd);
		surf->set_image(&logo);
		surf->as_logo();		
	}

	DEBUG_PRINT_LINE;
}
	
void Creeping::
deinit()
{
	DEBUG_PRINT_LINE;

	if (wnd)
		wnd->delete_surfaces();

	
	if(font_render!= NULL){
		delete font_render;
		font_render = NULL;
	}
	
	DEBUG_PRINT_LINE;
}	





int Creeping::
open_conf(const char *filename)
{
	DEBUG_PRINT_LINE;
	return conf.open(filename);
}

void Creeping::
play_once()
{
	DEBUG_PRINT_LINE;
	is_once = true;
}



void * Creeping::
redraw_window(void * _this)
{
	DEBUG_PRINT_LINE;
	Creeping * self = (Creeping *) _this;
	int window_x, window_y, window_width, window_height;
	int redraw_timeout = std::max(1000000/self->conf.get_conf()->ScrollingSpeed,8333 );
	pthread_mutex_lock( &self->mutex);
	CTimer  timer;
	timer.set_interval(1000000/self->conf.get_conf()->ScrollingSpeed);
	self->wnd->open(&self->conf);
	self->wnd->get_current_rect(&window_x, &window_y, &window_width, &window_height);
	self->init();
	self->wnd->draw();
	timer.start();	
	pthread_mutex_unlock( &self->mutex);
	
	for(;;)
	{
		CTimer  work_time;
		work_time.set_interval(1);
		work_time.start();
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		
		pthread_mutex_lock( &self->mutex);

		self->wnd->clear();
		
		CSurface  * logo = self->wnd->get_logo();
		std::vector<CSurface*> * surfaces =  self->wnd->get_surfaces();
		std::vector<CSurface*>::iterator i = surfaces->begin();
		int current_pos = timer.get_val();
		int length = window_width - current_pos;
		if(!surfaces->empty() &&  (current_pos - (*i)->get_width()) > window_width)
		{
			
			timer.offset( (*i)->get_width());
			length += (*i)->get_width();
			delete (*i);
			surfaces = self->wnd->get_surfaces();
		}
		for (i = surfaces->begin(); i != surfaces->end(); i++)
		{
			(*i)->draw(length);
			length += (*i)->get_width() ;
			if(length > window_width)
				break;
		}
		
		if(logo != NULL)
			logo->draw(0);

		self->wnd->draw();

		pthread_mutex_unlock( &self->mutex);
		
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_testcancel();
		if(surfaces->empty() && self->is_once)
			pthread_exit(NULL);
		
		if(surfaces->empty() && self->is_once == false)
		{
			self->init();
			timer.set_val(0);
		}else{
			int time_sleep = redraw_timeout - work_time.get_val();
			if ( time_sleep < 0)
				continue;
			if(time_sleep > redraw_timeout)
				usleep(redraw_timeout);
			usleep(time_sleep);
		}
	}
	DEBUG_PRINT_LINE;
}
