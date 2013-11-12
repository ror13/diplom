/*------------------------------------------------------------------------
 * A demonstration of OpenGL in a  ARGB window 
 *    => support for composited window transparency
 *
 * (c) 2011 by Wolfgang 'datenwolf' Draxinger
 *     See me at comp.graphics.api.opengl and StackOverflow.com

 * License agreement: This source code is provided "as is". You
 * can use this source code however you want for your own personal
 * use. If you give this source code to anybody else then you must
 * leave this message in it.
 *
 * This program is based on the simplest possible 
 * Linux OpenGL program by FTB (see info below)

  The simplest possible Linux OpenGL program? Maybe...

  (c) 2002 by FTB. See me in comp.graphics.api.opengl

  --
  <\___/>
  / O O \
  \_____/  FTB.


* http://stackoverflow.com/questions/4052940/how-to-make-an-opengl-rendering-context-with-transparent-background
* g++ ./1.c -lGLU -lGL -lglut -lX11 -lXrender
------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>

#define USE_CHOOSE_FBCONFIG

static void fatalError(const char *why)
{
    fprintf(stderr, "%s", why);
    exit(0x666);
}

static int Xscreen;
static Atom del_atom;
static Colormap cmap;
static Display *Xdisplay;
static XVisualInfo *visual;
static XRenderPictFormat *pict_format;
static GLXFBConfig *fbconfigs, fbconfig;
static int numfbconfigs;
static GLXContext render_context;
static Window Xroot, window_handle;
static GLXWindow glX_window_handle;
static int width, height;

static int VisData[] = {
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

static int isExtensionSupported(const char *extList, const char *extension)
{

  const char *start;
  const char *where, *terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if ( where || *extension == '\0' )
    return 0;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for ( start = extList; ; ) {
    where = strstr( start, extension );

    if ( !where )
      break;

    terminator = where + strlen( extension );

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return 1;

    start = terminator;
  }
  return 0;
}



static void describe_fbconfig(GLXFBConfig fbconfig)
{
    int doublebuffer;
    int red_bits, green_bits, blue_bits, alpha_bits, depth_bits;

    glXGetFBConfigAttrib(Xdisplay, fbconfig, GLX_DOUBLEBUFFER, &doublebuffer);
    glXGetFBConfigAttrib(Xdisplay, fbconfig, GLX_RED_SIZE, &red_bits);
    glXGetFBConfigAttrib(Xdisplay, fbconfig, GLX_GREEN_SIZE, &green_bits);
    glXGetFBConfigAttrib(Xdisplay, fbconfig, GLX_BLUE_SIZE, &blue_bits);
    glXGetFBConfigAttrib(Xdisplay, fbconfig, GLX_ALPHA_SIZE, &alpha_bits);
    glXGetFBConfigAttrib(Xdisplay, fbconfig, GLX_DEPTH_SIZE, &depth_bits);

    fprintf(stderr, "FBConfig selected:\n"
        "Doublebuffer: %s\n"
        "Red Bits: %d, Green Bits: %d, Blue Bits: %d, Alpha Bits: %d, Depth Bits: %d\n",
        doublebuffer == True ? "Yes" : "No", 
        red_bits, green_bits, blue_bits, alpha_bits, depth_bits);
}

static void createTheWindow()
{
    XEvent event;
    XWMHints *startup_state;
    int x,y, attr_mask;
    XSizeHints hints;
    XTextProperty textprop;
    XSetWindowAttributes attr = {0,};
    static char *title = "FTB's little OpenGL example - ARGB extension by WXD";

    Xdisplay = XOpenDisplay(NULL);
    if (!Xdisplay) {
        fatalError("Couldn't connect to X server\n");
    }
    Xscreen = DefaultScreen(Xdisplay);
    Xroot = RootWindow(Xdisplay, Xscreen);

    fbconfigs = glXChooseFBConfig(Xdisplay, Xscreen, VisData, &numfbconfigs);
    fbconfig = 0;
    for(int i = 0; i<numfbconfigs; i++) {
        visual = (XVisualInfo*) glXGetVisualFromFBConfig(Xdisplay, fbconfigs[i]);
        if(!visual)
            continue;

        pict_format = XRenderFindVisualFormat(Xdisplay, visual->visual);
        if(!pict_format)
            continue;

        fbconfig = fbconfigs[i];
        if(pict_format->direct.alphaMask > 0) {
            break;
        }
    }

    if(!fbconfig) {
        fatalError("No matching FB config found");
    }

    describe_fbconfig(fbconfig);

    /* Create a colormap - only needed on some X clients, eg. IRIX */
    cmap = XCreateColormap(Xdisplay, Xroot, visual->visual, AllocNone);

    attr.colormap = cmap;
    attr.background_pixmap = None;
    attr.border_pixmap = None;
    attr.border_pixel = 0;
    attr.event_mask =
        StructureNotifyMask;

    attr_mask = 
        CWBackPixmap|
        CWColormap|
        CWBorderPixel|
        CWEventMask;

    width = DisplayWidth(Xdisplay, DefaultScreen(Xdisplay))/2;
    height = DisplayHeight(Xdisplay, DefaultScreen(Xdisplay))/2;
    x=width/2, y=height/2;

    window_handle = XCreateWindow(  Xdisplay,
                    Xroot,
                    x, y, width, height,
                    0,
                    visual->depth,
                    InputOutput,
                    visual->visual,
                    attr_mask, &attr);

    if( !window_handle ) {
        fatalError("Couldn't create the window\n");
    }

    glX_window_handle = window_handle;


   textprop.value = (unsigned char*)title;
    textprop.encoding = XA_STRING;
    textprop.format = 8;
    textprop.nitems = strlen(title);

    hints.x = x;
    hints.y = y;
    hints.max_width = hints.min_width = width;
    hints.max_height = hints.min_height = height;
    hints.flags = PPosition | PSize | PMinSize | PMaxSize;

    startup_state = XAllocWMHints();
    startup_state->initial_state = NormalState;
    startup_state->flags = StateHint;

    XSetWMProperties(Xdisplay, window_handle,&textprop, &textprop,
            NULL, 0,
            &hints,
            startup_state,
            NULL);



    XMapWindow(Xdisplay, window_handle);

    if ((del_atom = XInternAtom(Xdisplay, "WM_DELETE_WINDOW", 0)) != None) {
        XSetWMProtocols(Xdisplay, window_handle, &del_atom, 1);
    }
#define _NET_WM_STATE_REMOVE        0    // remove/unset property
#define _NET_WM_STATE_ADD           1    // add/set property
#define _NET_WM_STATE_TOGGLE        2    // toggle property

Atom wmStateAbove = XInternAtom( Xdisplay, "_NET_WM_STATE_ABOVE", 1 );
if( wmStateAbove != None ) {
    printf( "_NET_WM_STATE_ABOVE has atom of %ld\n", (long)wmStateAbove );
} else {
    printf( "ERROR: cannot find atom for _NET_WM_STATE_ABOVE !\n" );
}

Atom wmNetWmState = XInternAtom( Xdisplay, "_NET_WM_STATE", 1 );
if( wmNetWmState != None ) {
    printf( "_NET_WM_STATE has atom of %ld\n", (long)wmNetWmState );
} else {
    printf( "ERROR: cannot find atom for _NET_WM_STATE !\n" );
}
// set window always on top hint
if( wmStateAbove != None ) {
    XClientMessageEvent xclient;
    memset( &xclient, 0, sizeof (xclient) );
    //
    //window  = the respective client window
    //message_type = _NET_WM_STATE
    //format = 32
    //data.l[0] = the action, as listed below
    //data.l[1] = first property to alter
    //data.l[2] = second property to alter
    //data.l[3] = source indication (0-unk,1-normal app,2-pager)
    //other data.l[] elements = 0
    //
    xclient.type = ClientMessage;
    xclient.window = window_handle; // GDK_WINDOW_XID(window);
    xclient.message_type = wmNetWmState; //gdk_x11_get_xatom_by_name_for_display( display, "_NET_WM_STATE" );
    xclient.format = 32;
    xclient.data.l[0] = _NET_WM_STATE_ADD; // add ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
    xclient.data.l[1] = wmStateAbove; //gdk_x11_atom_to_xatom_for_display (display, state1);
    xclient.data.l[2] = 0; //gdk_x11_atom_to_xatom_for_display (display, state2);
    xclient.data.l[3] = 0;
    xclient.data.l[4] = 0;
    //gdk_wmspec_change_state( FALSE, window,
    //  gdk_atom_intern_static_string ("_NET_WM_STATE_BELOW"),
    //  GDK_NONE );
    XSendEvent( Xdisplay,
      //mywin - wrong, not app window, send to root window!
      Xroot, // !! DefaultRootWindow( display ) !!!
      False,
      SubstructureRedirectMask | SubstructureNotifyMask,
      (XEvent *)&xclient );
  }
{  
  struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
} hints = {
  2, 0, 0, 0, 0,
};

XChangeProperty (Xdisplay, window_handle,
                 XInternAtom (Xdisplay, "_MOTIF_WM_HINTS", False),
                 XInternAtom (Xdisplay, "_MOTIF_WM_HINTS", False),
                 32, PropModeReplace,
                 (const unsigned char *) &hints,
                 sizeof (hints) / sizeof (long));
}
}

static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    fputs("Error at context creation", stderr);
    return 0;
}

static void createTheRenderContext()
{
    int dummy;
    if (!glXQueryExtension(Xdisplay, &dummy, &dummy)) {
        fatalError("OpenGL not supported by X server\n");
    }


    {
        render_context = glXCreateNewContext(Xdisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
        if (!render_context) {
            fatalError("Failed to create a GL context\n");
        }
    }

    if (!glXMakeContextCurrent(Xdisplay, glX_window_handle, glX_window_handle, render_context)) {
        fatalError("glXMakeCurrent failed for window\n");
    }
}

/*  6----7
   /|   /|
  3----2 |
  | 5--|-4
  |/   |/
  0----1

*/

GLfloat cube_vertices[][8] =  {
    /*  X     Y     Z   Nx   Ny   Nz    S    T */
    {-1.0, -1.0,  1.0, 0.0, 0.0, 1.0, 0.0, 0.0}, // 0
    { 1.0, -1.0,  1.0, 0.0, 0.0, 1.0, 1.0, 0.0}, // 1
    { 1.0,  1.0,  1.0, 0.0, 0.0, 1.0, 1.0, 1.0}, // 2
    {-1.0,  1.0,  1.0, 0.0, 0.0, 1.0, 0.0, 1.0}, // 3

    { 1.0, -1.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0}, // 4
    {-1.0, -1.0, -1.0, 0.0, 0.0, -1.0, 1.0, 0.0}, // 5
    {-1.0,  1.0, -1.0, 0.0, 0.0, -1.0, 1.0, 1.0}, // 6
    { 1.0,  1.0, -1.0, 0.0, 0.0, -1.0, 0.0, 1.0}, // 7

    {-1.0, -1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0}, // 5
    {-1.0, -1.0,  1.0, -1.0, 0.0, 0.0, 1.0, 0.0}, // 0
    {-1.0,  1.0,  1.0, -1.0, 0.0, 0.0, 1.0, 1.0}, // 3
    {-1.0,  1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 1.0}, // 6

    { 1.0, -1.0,  1.0,  1.0, 0.0, 0.0, 0.0, 0.0}, // 1
    { 1.0, -1.0, -1.0,  1.0, 0.0, 0.0, 1.0, 0.0}, // 4
    { 1.0,  1.0, -1.0,  1.0, 0.0, 0.0, 1.0, 1.0}, // 7
    { 1.0,  1.0,  1.0,  1.0, 0.0, 0.0, 0.0, 1.0}, // 2

    {-1.0, -1.0, -1.0,  0.0, -1.0, 0.0, 0.0, 0.0}, // 5
    { 1.0, -1.0, -1.0,  0.0, -1.0, 0.0, 1.0, 0.0}, // 4
    { 1.0, -1.0,  1.0,  0.0, -1.0, 0.0, 1.0, 1.0}, // 1
    {-1.0, -1.0,  1.0,  0.0, -1.0, 0.0, 0.0, 1.0}, // 0

    {-1.0, 1.0,  1.0,  0.0,  1.0, 0.0, 0.0, 0.0}, // 3
    { 1.0, 1.0,  1.0,  0.0,  1.0, 0.0, 1.0, 0.0}, // 2
    { 1.0, 1.0, -1.0,  0.0,  1.0, 0.0, 1.0, 1.0}, // 7
    {-1.0, 1.0, -1.0,  0.0,  1.0, 0.0, 0.0, 1.0}, // 6
};

static void draw_cube(void)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * 8, &cube_vertices[0][0]);
    glNormalPointer(GL_FLOAT, sizeof(GLfloat) * 8, &cube_vertices[0][3]);
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat) * 8, &cube_vertices[0][6]);

    glDrawArrays(GL_QUADS, 0, 24);
}

float const light0_dir[]={0,1,0,0};
float const light0_color[]={78./255., 80./255., 184./255.,1};

float const light1_dir[]={-1,1,1,0};
float const light1_color[]={255./255., 220./255., 97./255.,1};

float const light2_dir[]={0,-1,0,0};
float const light2_color[]={31./255., 75./255., 16./255.,1};

static void redrawTheWindow()
{
    float const aspect = (float)width / (float)height;

    static float a=0;
    static float b=0;
    static float c=0;

    glDrawBuffer(GL_BACK);

    glViewport(0, 0, width, height);

    // Clear with alpha = 0.0, i.e. full transparency
        glClearColor(0.0, 0.0, 0.0, 0.5);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-aspect, aspect, -1, 1, 2.5, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLightfv(GL_LIGHT0, GL_POSITION, light0_dir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_color);

    glLightfv(GL_LIGHT1, GL_POSITION, light1_dir);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_color);

    glLightfv(GL_LIGHT2, GL_POSITION, light2_dir);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_color);

    glTranslatef(0., 0., -5.);

    glRotatef(a, 1, 0, 0);
    glRotatef(b, 0, 1, 0);
    glRotatef(c, 0, 0, 1);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glColor4f(1., 1., 1., 0.5);

    glCullFace(GL_FRONT);
    draw_cube();
    glCullFace(GL_BACK);
    draw_cube();

    a = fmod(a+0.1, 360.);
    b = fmod(b+0.5, 360.);
    c = fmod(c+0.25, 360.);

    glXSwapBuffers(Xdisplay, glX_window_handle);
}

int main(int argc, char *argv[])
{
    createTheWindow();
    createTheRenderContext();

    while (1) {
        redrawTheWindow();
    }

    return 0;
}
