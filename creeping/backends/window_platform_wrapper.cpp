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

    

    n_window->cmap = XCreateColormap(n_window->Xdisplay, n_window->Xroot, n_window->visual->visual, AllocNone);

    attr.colormap = n_window->cmap;
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
	if(!n_window->Xdisplay)
		return;
	DEBUG_PRINT_LINE;
	glXMakeCurrent( n_window->Xdisplay, 0, 0 );
	glXDestroyContext( n_window->Xdisplay, n_window->render_context );
	DEBUG_PRINT_LINE;
	XDestroyWindow( n_window->Xdisplay, n_window->window_handle );
	XFreeColormap( n_window->Xdisplay, n_window->cmap );
	XCloseDisplay( n_window->Xdisplay );
	DEBUG_PRINT_LINE;

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

#ifdef USE_WND_PLATFORM_WINDOWS_GL
#include <assert.h>
#include <tchar.h>

#ifdef  assert
#define verify(expr) if(!expr) assert(0)
#else verify(expr) expr
#endif

        




//-------------------------------dwnapi--------------
#include <winable.h>
#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
#define DWM_BB_BLURREGION             0x00000002
#define PFD_SUPPORT_COMPOSITION       0x00008000
 
typedef struct _DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;
 
typedef struct _MARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} MARGINS, *PMARGINS;
 
extern "C"
{
    typedef HRESULT (WINAPI *t_DwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
    typedef HRESULT (WINAPI *t_DwmExtendFrameIntoClientArea)(HWND hwnd, const MARGINS *pMarInset);
}
 
void DwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset) {
    HMODULE shell;
 
    shell = LoadLibrary("dwmapi.dll");
    if (shell) {
        t_DwmExtendFrameIntoClientArea set_window_frame_into_client_area = reinterpret_cast<t_DwmExtendFrameIntoClientArea>(GetProcAddress (shell, "DwmExtendFrameIntoClientArea"));
        set_window_frame_into_client_area(hwnd, pMarInset);
 
        FreeLibrary (shell);
    }
 
}
 
void DwmEnableBlurBehindWindow(HWND hwnd, const DWM_BLURBEHIND* pBlurBehind) {
    HMODULE shell;
 
    shell = LoadLibrary("dwmapi.dll");
    if (shell) {
        t_DwmEnableBlurBehindWindow set_window_blur = reinterpret_cast<t_DwmEnableBlurBehindWindow>(GetProcAddress (shell, "DwmEnableBlurBehindWindow"));
        set_window_blur(hwnd, pBlurBehind);
 
        FreeLibrary (shell);
    }
}
//---------------------------------------------
static LRESULT CALLBACK WindowFunc(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam) {
/*switch (msg)	
	{
		case WM_WINDOWPOSCHANGING:		
		{
				SetWindowPos(hWnd,
    HWND_TOPMOST,
    0,
    0,
    1920,
    1080,
    SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
				return 0;
		}
}*/
return DefWindowProc(hWnd,msg,wParam,lParam);

}
HINSTANCE hThisInst;
void init_window_system(NativeWindow* n_window)
{
	 hThisInst= (HINSTANCE)GetModuleHandle(NULL);
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowFunc;
    wc.cbClsExtra  = 0;
    wc.cbWndExtra  = 0;
    wc.hInstance = hThisInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
    wc.lpszClassName = "CL";

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("RegisterClassEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
        return ;
    }
}
void deinit_window_system(NativeWindow* n_window)
{
	
}
void get_screen_size(int * width, int * height)
{
	 RECT desktop;
   const HWND hDesktop = GetDesktopWindow();
   GetWindowRect(hDesktop, &desktop);
   *width = desktop.right;
   *height = desktop.bottom;
}

void create_native_window(NativeWindow* n_window, WndRect * wndrect, int view_id)
{


     n_window->hWnd = CreateWindowEx(WS_EX_TOPMOST, "CL", "CL",
                    WS_VISIBLE | WS_POPUP | WS_OVERLAPPED, wndrect->x, wndrect->y, wndrect->w, wndrect->h,
                    NULL, NULL, hThisInst, NULL);

    if(!n_window->hWnd) {
        MessageBox(NULL, _T("CreateWindowEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
        return ;
    }

    DWM_BLURBEHIND bb = {0};
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(n_window->hWnd, &bb);

    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,                                // Version Number
      PFD_DRAW_TO_WINDOW      |         // Format Must Support Window
      PFD_SUPPORT_OPENGL      |         // Format Must Support OpenGL
      PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
      PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
      PFD_TYPE_RGBA,                    // Request An RGBA Format
      32,                               // Select Our Color Depth
      0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
      8,                                // An Alpha Buffer
      0,                                // Shift Bit Ignored
      0,                                // No Accumulation Buffer
      0, 0, 0, 0,                       // Accumulation Bits Ignored
      24,                               // 16Bit Z-Buffer (Depth Buffer)
      8,                                // Some Stencil Buffer
      0,                                // No Auxiliary Buffer
      PFD_MAIN_PLANE,                   // Main Drawing Layer
      0,                                // Reserved
      0, 0, 0                           // Layer Masks Ignored
   };     

   HDC hdc = GetDC(n_window->hWnd);
   int PixelFormat = ChoosePixelFormat(hdc, &pfd);
   if (PixelFormat == 0) {
      assert(0);
      return  ;
   }

   BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
   if (bResult==FALSE) {
      assert(0);
      return  ;
   }

   n_window->m_hrc = wglCreateContext(hdc);
   if (!n_window->m_hrc){
      assert(0);
      return ;
   }
   
   wglMakeCurrent(hdc, n_window->m_hrc);

   ReleaseDC(n_window->hWnd, hdc);
   //BlockInput(true); disable interrupt from input devices


}
void destroy_native_window(NativeWindow* n_window)
{
	if (n_window->hWnd != 0)
	{
		HDC hdc = GetDC(n_window->hWnd);
		if (hdc != 0)
		{
			wglMakeCurrent (hdc, 0);
			if (n_window->m_hrc != 0)
			{
				wglDeleteContext (n_window->m_hrc);
				n_window->m_hrc = 0;
			}
			ReleaseDC (n_window->hWnd, hdc);
		}
		DestroyWindow (n_window->hWnd);
		n_window->hWnd = 0;
	}
}
void swap_opengl_buffers(NativeWindow* n_window)
{
            HDC hdc = GetDC(n_window->hWnd);
            wglMakeCurrent(hdc, n_window->m_hrc);

 

            SwapBuffers(hdc);
            ReleaseDC(n_window->hWnd, hdc);
            
            MSG msg;
            if (PeekMessage (&msg, n_window->hWnd, 0, 0, PM_REMOVE) != 0)
            {
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}

            
            


}
#endif
