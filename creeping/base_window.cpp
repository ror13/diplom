#include "base_window.h"
#include "debug_utils.h"



#ifdef WITH_EGL_BCK

#include "backends/egl_window.h"

Window  * new_window()
{
	DEBUG_PRINT_LINE;
	return (Window  *) new EglWindow;
}
Surface * new_surface(Window * wnd)
{
	DEBUG_PRINT_LINE;
	return (Surface *) new EglSurface(wnd);
}
#endif


#ifdef WITH_SDL_BCK

#include "backends/sdl_window.h"

Window  * new_window()
{
	DEBUG_PRINT_LINE;
	return (Window  *) new SdlWindow;
}
Surface * new_surface(Window * wnd)
{
	DEBUG_PRINT_LINE;
	return (Surface *) new SdlSurface(wnd);
}

//#elseif WITH_GDK_BCK

//IT NOW NOW SUPORTED
//STOP


#endif


Window::
Window()
{
	logo = NULL;
}
Window::
~Window()
{
	DEBUG_PRINT_LINE;
	delete_surfaces();
	//unreg_logo();
	DEBUG_PRINT_LINE;
}

void Window::
reg_child(Surface * child)
{
	DEBUG_PRINT_LINE;
	childs.push_back(child);
	DEBUG_PRINT_LINE;
}

void Window::
unreg_child(Surface * child)
{
	DEBUG_PRINT_LINE;
	std::vector<Surface*>::iterator i;
	for (i = childs.begin(); i != childs.end(); i++)
		if(*i == child)
		{
			childs.erase(i);
			break;
		}
	DEBUG_PRINT_LINE;
}

void Window::reg_logo(Surface * lg)
{
	DEBUG_PRINT_LINE;
	unreg_logo(lg);
	logo = lg;
	DEBUG_PRINT_LINE;
}

void Window::unreg_logo(Surface * lg)
{
	DEBUG_PRINT_LINE;
	if (logo != NULL && logo != lg)
		delete logo;
	logo = NULL;
	DEBUG_PRINT_LINE;
}

Surface  * Window::get_logo()
{
	DEBUG_PRINT_LINE;
	return logo;
}

void Window::delete_surfaces()
{
	DEBUG_PRINT_LINE;
	while (!childs.empty())
	{
		delete *childs.begin();
	}
	unreg_logo(NULL);
	DEBUG_PRINT_LINE;
}
std::vector <Surface *> * Window::get_surfaces()
{
	return &childs;
}


Surface::
Surface(Window * wnd)
{
	DEBUG_PRINT_LINE;
	is_logo = false;
	parrent = wnd;
	parrent->reg_child(this);
	DEBUG_PRINT_LINE;
}

void Surface::as_logo()
{
	DEBUG_PRINT_LINE;
	is_logo = true;
	parrent->unreg_child(this);
	parrent->reg_logo(this);
	DEBUG_PRINT_LINE;
}
		

Surface::
~Surface()
{
	DEBUG_PRINT_LINE;
	if(is_logo)
		parrent->unreg_logo(this);
	else
		parrent->unreg_child(this);
	DEBUG_PRINT_LINE;
}
