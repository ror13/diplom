#include "base_window.h"
#include "debug_utils.h"



#ifdef WITH_GLES_BCK

#include "backends/gles_window.h"

CWindow  * new_window()
{
	DEBUG_PRINT_LINE;
	return (CWindow  *) new GlesWindow;
}
CSurface * new_surface(CWindow * wnd)
{
	DEBUG_PRINT_LINE;
	return (CSurface *) new GlesSurface(wnd);
}
#endif


#ifdef WITH_SDL_BCK

#include "backends/sdl_window.h"

CWindow  * new_window()
{
	DEBUG_PRINT_LINE;
	return (CWindow  *) new SdlWindow;
}
CSurface * new_surface(CWindow * wnd)
{
	DEBUG_PRINT_LINE;
	return (CSurface *) new SdlSurface(wnd);
}

//#elseif WITH_GDK_BCK

//IT NOW NOW SUPORTED
//STOP


#endif


CWindow::
CWindow()
{
	logo = NULL;
}
CWindow::
~CWindow()
{
	DEBUG_PRINT_LINE;
	delete_surfaces();
	//unreg_logo();
	DEBUG_PRINT_LINE;
}

void CWindow::
reg_child(CSurface * child)
{
	DEBUG_PRINT_LINE;
	childs.push_back(child);
	DEBUG_PRINT_LINE;
}

void CWindow::
unreg_child(CSurface * child)
{
	DEBUG_PRINT_LINE;
	std::vector<CSurface*>::iterator i;
	for (i = childs.begin(); i != childs.end(); i++)
		if(*i == child)
		{
			childs.erase(i);
			break;
		}
	DEBUG_PRINT_LINE;
}

void CWindow::reg_logo(CSurface * lg)
{
	DEBUG_PRINT_LINE;
	unreg_logo(lg);
	logo = lg;
	DEBUG_PRINT_LINE;
}

void CWindow::unreg_logo(CSurface * lg)
{
	DEBUG_PRINT_LINE;
	if (logo != NULL && logo != lg)
		delete logo;
	logo = NULL;
	DEBUG_PRINT_LINE;
}

CSurface  * CWindow::get_logo()
{
	DEBUG_PRINT_LINE;
	return logo;
}

void CWindow::delete_surfaces()
{
	DEBUG_PRINT_LINE;
	while (!childs.empty())
	{
		delete *childs.begin();
	}
	unreg_logo(NULL);
	DEBUG_PRINT_LINE;
}
std::vector <CSurface *> * CWindow::get_surfaces()
{
	return &childs;
}


CSurface::
CSurface(CWindow * wnd)
{
	DEBUG_PRINT_LINE;
	is_logo = false;
	parrent = wnd;
	parrent->reg_child(this);
	DEBUG_PRINT_LINE;
}

void CSurface::as_logo()
{
	DEBUG_PRINT_LINE;
	is_logo = true;
	parrent->unreg_child(this);
	parrent->reg_logo(this);
	DEBUG_PRINT_LINE;
}
		

CSurface::
~CSurface()
{
	DEBUG_PRINT_LINE;
	if(is_logo)
		parrent->unreg_logo(this);
	else
		parrent->unreg_child(this);
	DEBUG_PRINT_LINE;
}
