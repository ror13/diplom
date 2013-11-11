//#ifndef EGL_WINDOW_H
//#define EGL_WINDOW_H

#include <bcm_host.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <pango/pango.h>

#include "base_window.h"

class EglWindow : public Window
{
	public:
		EglWindow();
		~EglWindow();
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
		
		EGLDisplay display;
		EGLSurface surface;
		EGLContext context;
		float bg_color[4];
		VC_RECT_T window_rect;
		GLfloat tex[8];
		GLfloat box[8];
		EGL_DISPMANX_WINDOW_T nativewindow;
		DISPMANX_ELEMENT_HANDLE_T dispman_element;
		DISPMANX_DISPLAY_HANDLE_T dispman_display;
		DISPMANX_UPDATE_HANDLE_T dispman_update;

};

class EglSurface : public Surface
{
	public:
		EglSurface(Window * wnd);
		~EglSurface();
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
