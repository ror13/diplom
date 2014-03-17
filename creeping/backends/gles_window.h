//#ifndef EGL_WINDOW_H
//#define EGL_WINDOW_H

#include "window_platform_wrapper.h"
#include <pango/pango.h>

#include "base_window.h"

class GlesWindow : public CWindow
{
	public:
		GlesWindow();
		~GlesWindow();
		virtual void open(ConfFile * conf_file);
		virtual void close();
		virtual void hide();
		virtual void draw();
		virtual void clear();
		virtual void get_current_rect(int * x, int * y, 
										int * width, int * height);
	private:
		void set_background_color(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
		void set_background_color(int RGB, unsigned char A);
		void set_background_color(int RGBA);
		float bg_color[4];
		WndRect window_rect;
		NativeWindow n_window;
		GLfloat tex[8];
		GLfloat box[8];
};

class GlesSurface : public CSurface
{
	public:
		GlesSurface(CWindow * wnd);
		~GlesSurface();
		virtual void set_image(std::string * str);
		virtual void set_text(std::string *  str, FontRender * font_render);
		virtual void clear();
		virtual void draw(int x);
		virtual int get_width();
		virtual void set_tex_color(int RGB);
		
	private:
		void set_pixels_array();
		void scaled(GLfloat x,  GLfloat y,  GLfloat z);
		
		GLint format;
		GLfloat tex_color[4];
		GLfloat scaled_x;
		GLfloat scaled_y;
		GLfloat scaled_z;
		void free_texture();
		void create_texture();
		int tex_width,tex_height;
		GLuint  * texture;
		char * pixel_buffer;
};

//#endif EGL_WINDOW_H
