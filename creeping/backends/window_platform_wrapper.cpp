#include "window_platform_wrapper.h"
#include "debug_utils.h"
#include <stdio.h>
#include <string.h>

#ifdef USE_WND_PLATFORM_LINUX_RPI
void 
init_window_system(NativeWindow* n_window)
{
	if(!is_init_wnd)
	{
		bcm_host_init();
		is_init_wnd = true;
	}
}

void 
deinit_window_system(NativeWindow* n_window)
{
	if(is_init_wnd)
	{
		bcm_host_deinit();
		is_init_wnd = false;
	}
}

void 
get_screen_size(int * width, int * height)
{
	graphics_get_display_size(0 /* LCD */, (unsigned int *)width, (unsigned int *)height);
}

void 
create_native_window(NativeWindow* n_window, WndRect * wndrect, int view_id)
{
	EGLBoolean result;
	EGLint num_config;
	VC_RECT_T src_rect;
	EGLConfig config;
	
	const EGLint attribute_list[] =   
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
	
	n_window->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(display!=EGL_NO_DISPLAY);

	result = eglInitialize(n_window->display, NULL, NULL);
	assert(EGL_FALSE != result);

	result = eglChooseConfig(n_window->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	n_window->context = eglCreateContext(n_window->display, config, EGL_NO_CONTEXT, NULL);
	assert(context!=EGL_NO_CONTEXT);


      
	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width  = wndrect->w  << 16;
	src_rect.height = wndrect->h  << 16;        

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );

    
	n_window->dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      view_id/*layer*/, (VC_RECT_T *) wndrect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, DISPMANX_NO_ROTATE/*transform*/);
      
	n_window->nativewindow.element = dispman_element;
	n_window->nativewindow.width = wndrect->w;
	n_window->nativewindow.height = wndrect->h;
	vc_dispmanx_update_submit_sync( dispman_update );
    
    
	n_window->surface = eglCreateWindowSurface( n_window->display, config, &n_window->nativewindow, NULL );
	assert(surface != EGL_NO_SURFACE);

	result = eglMakeCurrent(n_window->display, n_window->surface, n_window->surface, n_window->context);
	assert(EGL_FALSE != result);
}

void 
destroy_native_window(NativeWindow* n_window)
{
	
}

void 
swap_opengl_buffers(NativeWindow* n_window)
{
	eglSwapBuffers(n_window->display, n_window->surface);
}

#endif

#ifdef USE_WND_PLATFORM_LINUX_X11

#define _NET_WM_STATE_REMOVE        0    // remove/unset property
#define _NET_WM_STATE_ADD           1    // add/set property
#define _NET_WM_STATE_TOGGLE        2    // toggle property

void 
init_window_system(NativeWindow* n_window)
{
	
}

void 
deinit_window_system(NativeWindow* n_window)
{
	
}

void 
get_screen_size(int * width, int * height)
{
	
}

void 
create_native_window(NativeWindow* n_window, WndRect * wndrect, int view_id)
{
	XRenderPictFormat *pict_format;
	XEvent event;
	Colormap cmap;
	GLXFBConfig *fbconfigs, fbconfig;
	Atom del_atom;
	int numfbconfigs;
	int dummy;	
    XWMHints *startup_state;
    int attr_mask;
    XSizeHints hints;
    XTextProperty textprop;
    XSetWindowAttributes attr = {0,};
    int VisData[] = 
	{
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DOUBLEBUFFER, True,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 16,
		None
	};

    n_window->Xdisplay = XOpenDisplay(NULL);
    assert(n_window->Xdisplay);
    
    n_window->Xscreen = DefaultScreen(n_window->Xdisplay);
    n_window->Xroot = RootWindow(n_window->Xdisplay, n_window->Xscreen);

    fbconfigs = glXChooseFBConfig(n_window->Xdisplay, n_window->Xscreen, VisData, &numfbconfigs);
    fbconfig = 0;
    for(int i = 0; i<numfbconfigs; i++) {
        n_window->visual = (XVisualInfo*) glXGetVisualFromFBConfig(n_window->Xdisplay, fbconfigs[i]);
        if(!n_window->visual)
            continue;

        pict_format = XRenderFindVisualFormat(n_window->Xdisplay, n_window->visual->visual);
        if(!pict_format)
            continue;

        fbconfig = fbconfigs[i];
        if(pict_format->direct.alphaMask > 0) {
            break;
        }
    }

    assert(fbconfig);
    
    int doublebuffer;
    int red_bits, green_bits, blue_bits, alpha_bits, depth_bits;

    glXGetFBConfigAttrib(n_window->Xdisplay, fbconfig, GLX_DOUBLEBUFFER, &doublebuffer);
    glXGetFBConfigAttrib(n_window->Xdisplay, fbconfig, GLX_RED_SIZE, &red_bits);
    glXGetFBConfigAttrib(n_window->Xdisplay, fbconfig, GLX_GREEN_SIZE, &green_bits);
    glXGetFBConfigAttrib(n_window->Xdisplay, fbconfig, GLX_BLUE_SIZE, &blue_bits);
    glXGetFBConfigAttrib(n_window->Xdisplay, fbconfig, GLX_ALPHA_SIZE, &alpha_bits);
    glXGetFBConfigAttrib(n_window->Xdisplay, fbconfig, GLX_DEPTH_SIZE, &depth_bits);

    

    cmap = XCreateColormap(n_window->Xdisplay, n_window->Xroot, n_window->visual->visual, AllocNone);

    attr.colormap = cmap;
    attr.background_pixmap = None;
    attr.border_pixmap = None;
	attr.border_pixel = 0;
	attr.event_mask = StructureNotifyMask;

	attr_mask = 
        CWBackPixmap|
        CWColormap|
        CWBorderPixel|
        CWEventMask;

	n_window->window_handle = XCreateWindow(  n_window->Xdisplay,
                    n_window->Xroot,
                    wndrect->x, 
                    wndrect->y, 
                    wndrect->w, 
                    wndrect->h,
                    0,
                    n_window->visual->depth,
                    InputOutput,
                    n_window->visual->visual,
                    attr_mask, &attr);

    assert(n_window->window_handle );

    hints.x = wndrect->x;
    hints.y = wndrect->y;
    hints.max_width = hints.min_width = wndrect->w;
    hints.max_height = hints.min_height = wndrect->h;
    hints.flags = PPosition | PSize | PMinSize | PMaxSize;

    startup_state = XAllocWMHints();
    startup_state->initial_state = NormalState;
    startup_state->flags = StateHint;

    XSetWMProperties(n_window->Xdisplay, 
			n_window->window_handle,
			NULL, 
			NULL,
            NULL, 
            0,
            &hints,
            startup_state,
            NULL);
	XFree(startup_state);
 
    XMapWindow(n_window->Xdisplay, n_window->window_handle);

    if ((del_atom = XInternAtom(n_window->Xdisplay, "WM_DELETE_WINDOW", 0)) != None)
        XSetWMProtocols(n_window->Xdisplay, n_window->window_handle, &del_atom, 1);

	Atom wmStateAbove = XInternAtom( n_window->Xdisplay, "_NET_WM_STATE_ABOVE", 1 );
	Atom wmNetWmState = XInternAtom( n_window->Xdisplay, "_NET_WM_STATE", 1 );

	if( wmStateAbove != None ) 
	{
		XClientMessageEvent xclient;
		memset(&xclient, 0, sizeof (xclient));
		xclient.type = ClientMessage;
		xclient.window = n_window->window_handle;
		xclient.message_type = wmNetWmState; 
		xclient.format = 32;
		xclient.data.l[0] = _NET_WM_STATE_ADD;
		xclient.data.l[1] = wmStateAbove; 
		xclient.data.l[2] = 0; 
		xclient.data.l[3] = 0;
		xclient.data.l[4] = 0;
		XSendEvent( n_window->Xdisplay,
		  n_window->Xroot,
		  False,
		  SubstructureRedirectMask | SubstructureNotifyMask,
		  (XEvent *)&xclient );
	}

	struct {
		unsigned long flags;
		unsigned long functions;
		unsigned long decorations;
		long input_mode;
		unsigned long status;
	} hts = { 2, 0, 0, 0, 0,};

	XChangeProperty(n_window->Xdisplay, n_window->window_handle,
					XInternAtom (n_window->Xdisplay, "_MOTIF_WM_HINTS", False),
					XInternAtom (n_window->Xdisplay, "_MOTIF_WM_HINTS", False),
					32, PropModeReplace,
					(const unsigned char *) &hts,
					sizeof (hints) / sizeof (long));

	
	    
    assert(glXQueryExtension(n_window->Xdisplay, &dummy, &dummy));

	n_window->render_context = glXCreateNewContext(n_window->Xdisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
	assert(n_window->render_context);

    assert(glXMakeContextCurrent(n_window->Xdisplay,
		(GLXWindow) n_window->window_handle, 
		(GLXWindow) n_window->window_handle, 
		n_window->render_context));
	
}

void 
destroy_native_window(NativeWindow* n_window)
{
	
}

void 
swap_opengl_buffers(NativeWindow* n_window)
{
	glXSwapBuffers(n_window->Xdisplay, (GLXWindow) n_window->window_handle);
}
#endif

#ifdef USE_WND_PLATFORM_LINUX_QT

void 
init_window_system(NativeWindow* n_window)
{
	DEBUG_PRINT_LINE;

    DEBUG_PRINT_LINE;
}
void 
deinit_window_system(NativeWindow* n_window)
{
	
}
void 
get_screen_size(int * width, int * height)
{
	DEBUG_PRINT_LINE;
	//QDesktopWidget desk;

    *width = 1920;//desk.width() ;
    *height = 1080;;//desk.height() ;
    DEBUG_PRINT_LINE;
}
void 
create_native_window(NativeWindow* n_window, WndRect * wndrect, int view_id)
{
	DEBUG_PRINT_LINE;
	int argc = 0;
    char** argv = NULL;
    n_window->app = new QApplication(argc,argv);
	DEBUG_PRINT_LINE;QGLFormat glFormat(QGL::Rgba | QGL::DoubleBuffer | QGL::DepthBuffer | QGL::AlphaChannel);
    DEBUG_PRINT_LINE;n_window->window = new QGLWidget (glFormat, NULL, NULL, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    DEBUG_PRINT_LINE;n_window->window->resize(wndrect->w,wndrect->h);
    DEBUG_PRINT_LINE;n_window->window->show();

    DEBUG_PRINT_LINE;n_window->window->makeCurrent();
    DEBUG_PRINT_LINE;
}
void 
destroy_native_window(NativeWindow* n_window)
{
	
}
void 
swap_opengl_buffers(NativeWindow* n_window)
{
	n_window->window->swapBuffers ();
	qApp->processEvents();
}
#endif
