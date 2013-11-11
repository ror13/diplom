//#ifndef EGL_WINDOW_H
//#define EGL_WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <pango/pango.h>

#include "base_window.h"

struct VC_RECT_T{
	int x;
	int y;
	int width;
	int height;
};

class SdlWindow : public Window
{
	public:
		SdlWindow();
		~SdlWindow();
		virtual void open(ConfFile * conf_file);
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
		VC_RECT_T window_rect;
		GLfloat tex[8];
		GLfloat box[8];
		SDL_Window* displayWindow;
		SDL_GLContext displayContext;
};

class SdlSurface : public Surface
{
	public:
		SdlSurface(Window * wnd);
		~SdlSurface();
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
