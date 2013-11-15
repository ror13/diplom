
#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include <vector>
#include <string>

#include "font_render.h"
class CWindow;
class CSurface;

CWindow  * new_window();
CSurface * new_surface(CWindow * wnd);

/*
class Window
{
	public:
		Window();
		virtual ~Window();
		virtual void open() {};
		virtual void update() {};
		virtual void clear() {};
		virtual void draw() {};
		void reg_child(Surface * child);
		void unreg_child(Surface * child);
	private:
		std::vector <Surface *> childs;
};

class Surface
{
	public:
		Surface(CWindow * wnd);
		virtual ~Surface();
		virtual void set_image(std::string *  str) {};
		virtual void set_text(std::string *  str) {};
		virtual void * get_data() {};
		virtual void update_pos() {};
		virtual void clear() {};
		virtual void draw() {};
		virtual int get_width() {};
	protected:
		CWindow * parrent;	
};
*/


class CWindow
{
	public:
		CWindow();
		virtual ~CWindow();
		virtual void open(ConfFile * conf_file) = 0;
		virtual void hide() = 0;
		virtual void clear() = 0;
		virtual void draw() = 0;
		virtual void get_current_rect(int * x, int * y, 
										int * width, int * height) = 0;
		void reg_child(CSurface * child);
		void unreg_child(CSurface * child);
		void reg_logo(CSurface * lg);
		void unreg_logo(CSurface * lg);
		CSurface  * get_logo();
		std::vector <CSurface *> * get_surfaces();
		void delete_surfaces();
	private:
		CSurface * logo;
		std::vector <CSurface *> childs;
};

class CSurface
{
	public:
		CSurface(CWindow * wnd);
		virtual ~CSurface();
		virtual void set_image(std::string * str) = 0;
		virtual void set_text(std::string *  str, FontRender * font_render) = 0;
		virtual void clear() = 0;
		virtual void draw(int x) = 0;
		virtual int get_width() = 0;
		virtual void set_tex_color(int RGB) = 0;
		void as_logo();
		
	protected:
		bool is_logo;
		CWindow * parrent;	
};

#endif //BASE_WINDOW_H
