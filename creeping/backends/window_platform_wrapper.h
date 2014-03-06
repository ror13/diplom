

typedef struct
{
	int x,y,w,h;
} WndRect;

#ifdef USE_WND_PLATFORM_LINUX_RPI
#include <bcm_host.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef struct
{
	bool is_init_wnd;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	EGL_DISPMANX_WINDOW_T nativewindow;
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
} NativeWindow;
#endif

#ifdef USE_WND_PLATFORM_LINUX_X11
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>
#include <assert.h>

typedef struct
{
	int Xscreen;
	Display *Xdisplay;
	XVisualInfo *visual;
	GLXContext render_context;
	Window Xroot, window_handle;
} NativeWindow;
#endif

#ifdef USE_WND_PLATFORM_LINUX_QT
#include <QMainWindow>
#include <QtWidgets/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDesktopWidget>

typedef struct
{
	QApplication * app;
	QGLWidget * window;
} NativeWindow;
#endif

#ifdef USE_WND_PLATFORM_WINDOWS_GL

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include <dwmapi.h>


#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

typedef struct
{
	HWND hWnd;           
	HGLRC m_hrc;
} NativeWindow;
#endif


void init_window_system(NativeWindow* n_window);
void deinit_window_system(NativeWindow* n_window);
void get_screen_size(int * width, int * height);
void create_native_window(NativeWindow* n_window, WndRect * wndrect, int view_id);
void destroy_native_window(NativeWindow* n_window);
void swap_opengl_buffers(NativeWindow* n_window);
