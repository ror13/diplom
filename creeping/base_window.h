
#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include <vector>
#include <string>

#include "font_render.h"
class Window;
class Surface;

Window  * new_window();
Surface * new_surface(Window * wnd);

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
		Surface(Window * wnd);
		virtual ~Surface();
		virtual void set_image(std::string *  str) {};
		virtual void set_text(std::string *  str) {};
		virtual void * get_data() {};
		virtual void update_pos() {};
		virtual void clear() {};
		virtual void draw() {};
		virtual int get_width() {};
	protected:
		Window * parrent;	
};
*/


class Window
{
	public:
		Window();
		virtual ~Window();
		virtual void open(ConfFile * conf_file) = 0;
		virtual void hide() = 0;
		virtual void clear() = 0;
		virtual void draw() = 0;
		virtual void get_current_rect(int * x, int * y, 
										int * width, int * height) = 0;
		void reg_child(Surface * child);
		void unreg_child(Surface * child);
		void reg_logo(Surface * lg);
		void unreg_logo(Surface * lg);
		Surface  * get_logo();
		std::vector <Surface *> * get_surfaces();
		void delete_surfaces();
	private:
		Surface * logo;
		std::vector <Surface *> childs;
};

class Surface
{
	public:
		Surface(Window * wnd);
		virtual ~Surface();
		virtual void set_image(std::string * str) = 0;
		virtual void set_text(std::string *  str, FontRender * font_render) = 0;
		virtual void clear() = 0;
		virtual void draw(int x) = 0;
		virtual int get_width() = 0;
		virtual void set_tex_color(int RGB) = 0;
		void as_logo();
		
	protected:
		bool is_logo;
		Window * parrent;	
};

#endif //BASE_WINDOW_H
