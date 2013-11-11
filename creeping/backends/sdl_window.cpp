#include <stdio.h>
#include <assert.h>
#include <pango/pangoft2.h>
#include <gdk/gdk.h>

#include "sdl_window.h"
#include "debug_utils.h"

//------------------------------ MainWindow ---------------------------------------------------
SdlWindow::
SdlWindow()
{
	DEBUG_PRINT_LINE;
	memset(&window_rect,0,sizeof(window_rect));
	//graphics_get_display_size(0 /* LCD */, (unsigned int *)&window_rect.width, (unsigned int *)&window_rect.height);
	memset(&bg_color,0,sizeof(bg_color));
	box[0] = -1;	box[1] = -1;
	box[2] = 1;		box[3] = -1;
	box[4] = -1;	box[5] = 1;
	box[6] = 1;		box[7] = 1;
	
			
	tex[0] = 0;		tex[1] = 0;
	tex[2] = 1;		tex[3] = 0;
	tex[4] = 0;		tex[5] = 1;
	tex[6] = 1;		tex[7] = 1;
	DEBUG_PRINT_LINE;
}

SdlWindow::
~SdlWindow()
{
	DEBUG_PRINT_LINE;
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	DEBUG_PRINT_LINE;
	
}

void SdlWindow::
set_background_color(int RGBA)
{
	DEBUG_PRINT_LINE;
	set_background_color(RGBA & 0xFF, RGBA >> 8 & 0xFF, RGBA >> 16 & 0xFF,  RGBA >> 24 & 0xFF);
	DEBUG_PRINT_LINE;
}

void SdlWindow::
set_background_color(int RGB, unsigned char A)
{
	DEBUG_PRINT_LINE;

	set_background_color(RGB & 0xFF, RGB >> 8 & 0xFF, RGB >> 16 & 0xFF,  A );
	DEBUG_PRINT_LINE;
}

void SdlWindow::
set_background_color(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
	DEBUG_PRINT_LINE;
	bg_color[0] = 1.0f/255*R;
	bg_color[1] = 1.0f/255*G;
	bg_color[2] = 1.0f/255*B;
	bg_color[3] = 1.0f/255*125;
	DEBUG_PRINT_LINE;
}

void SdlWindow::
clear()
{
	glClear( GL_COLOR_BUFFER_BIT );
	
}

void SdlWindow::
draw()
{
	SDL_GL_SwapWindow(displayWindow);
}

void SdlWindow::
hide()
{
//	eglSwapBuffers(display, surface);
}

void SdlWindow::
get_current_rect(int * x, int * y, 
										int * width, int * height)
{
	DEBUG_PRINT_LINE;
	*x = window_rect.x;
	*y = window_rect.y;
	*width = window_rect.width;
	*height = window_rect.height;
	DEBUG_PRINT_LINE;
}

void SdlWindow::
open(ConfFile * conf_file)
{
	DEBUG_PRINT_LINE;
	int view_id;
	int32_t success = 0;

	VC_RECT_T src_rect;
	int display_width = 1920, display_height = 1080;
	
	
    
    conf_file->wnd_pos_from_conf(display_width, display_height, (int*)&window_rect.x, (int*)&window_rect.y, 
						(int*)&view_id, (int*)&window_rect.width, (int*)&window_rect.height);
	
	set_background_color(conf_file->get_conf()->window.clBackground, 
						 conf_file->get_conf()->window.TransparencyFactor );
	

	
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    displayWindow = SDL_CreateWindow("SDL Application", window_rect.x,window_rect.y, window_rect.width, window_rect.height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    displayContext = SDL_GL_CreateContext(displayWindow);
	
	
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0,box);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glEnable(GL_TEXTURE_2D);
	glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
	glClear( GL_COLOR_BUFFER_BIT );
	

	DEBUG_PRINT_LINE;
}

//------------------------------ layout ---------------------------------------------------

SdlSurface::
SdlSurface(Window * wnd):Surface(wnd)
{
	DEBUG_PRINT_LINE;
	texture = NULL;
	pixel_buffer = NULL;
	format = GL_RGBA;
	
	tex_color[0] = 0.0f;
	tex_color[1] = 0.0f;
	tex_color[2] = 0.0f;
	tex_color[3] = 1.0f;
	
	scaled_x = 1.0f;
	scaled_y = 1.0f;
	scaled_z = 1.0f;
	DEBUG_PRINT_LINE;	
}

void SdlSurface::
scaled(GLfloat x,  GLfloat y,  GLfloat z)
{
	DEBUG_PRINT_LINE;
	scaled_x = x;
	scaled_y = y;
	scaled_z = z;
	DEBUG_PRINT_LINE;
}

void SdlSurface::
set_tex_color(int RGB)
{
	DEBUG_PRINT_LINE;
	tex_color[0] = 1.0f/255*(RGB  & 0xFF);
	tex_color[1] = 1.0f/255*(RGB >> 8 & 0xFF);
	tex_color[2] = 1.0f/255* (RGB >> 16 & 0xFF);
	tex_color[3] = 1.0f;
	DEBUG_PRINT_LINE;
}

SdlSurface::~SdlSurface()
{
	DEBUG_PRINT_LINE;
	free_texture();
	if(pixel_buffer != NULL)
		delete [] pixel_buffer;
	DEBUG_PRINT_LINE;
}

void SdlSurface::
set_image(std::string * str)
{
	DEBUG_PRINT_LINE;
	int p_x, p_y, p_w, p_h;
	parrent->get_current_rect(&p_x, &p_y, &p_w, &p_h);
	GdkPixbuf * load_pixbuf = gdk_pixbuf_new_from_file(str->c_str(), NULL);
	unlink(str->c_str());

	if (!load_pixbuf)
		return;

	int width = gdk_pixbuf_get_width(load_pixbuf);
	int height = gdk_pixbuf_get_height(load_pixbuf);
	tex_height = p_h;
	tex_width = width * tex_height / height;
	GdkPixbuf *rez_pixbuf = gdk_pixbuf_scale_simple(load_pixbuf,
								tex_width, tex_height, GDK_INTERP_BILINEAR);

	if (!rez_pixbuf) 
	{
		g_object_unref(load_pixbuf);
		return;
	}

	gint channels = gdk_pixbuf_get_n_channels(rez_pixbuf);

	if(channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;
	else 
		format = 1;

	if(pixel_buffer != NULL)
		delete []  pixel_buffer;

	pixel_buffer  =  new char[tex_width * tex_height * channels];
	memcpy(pixel_buffer,gdk_pixbuf_get_pixels(rez_pixbuf),tex_width * tex_height * channels);
	g_object_unref(rez_pixbuf);
	scaled(1.0f,-1.0f,1.0f);
	
	DEBUG_PRINT_LINE;
}

void SdlSurface::
set_text(std::string *  str, FontRender * font_render)
{
	DEBUG_PRINT_LINE;
	PangoLayout *layout = (PangoLayout *) font_render->rendering(str->c_str(), &tex_width, &tex_height);
	
	FT_Bitmap bitmap;

	bitmap.rows = tex_height;
	bitmap.width = tex_width;
	bitmap.pitch = bitmap.width;
	bitmap.buffer = (unsigned char *)malloc(bitmap.width *  bitmap.rows );
	bitmap.num_grays = 256;
	bitmap.pixel_mode = ft_pixel_mode_grays;
	memset (bitmap.buffer,0x0,bitmap.width *  bitmap.rows);
	
	pango_ft2_render_layout (&bitmap, layout, 0 /* x position*/, 0 /* y position*/);
	
	if(pixel_buffer != NULL)
		delete []  pixel_buffer;
	pixel_buffer = new char[tex_width *  tex_height * 4]; 
	guint32 * p_b = (guint32 *) pixel_buffer;
	memset (pixel_buffer, 0x00, tex_width * tex_height * 4);
	
	
	GLuint rgb;
	rgb =  ((guint32) (tex_color[0]  * 255.0))        |
           (((guint32) (tex_color[1] * 255.0)) << 8)  |
           (((guint32) (tex_color[2] * 255.0)) << 16);

	guint8 *row, *row_end;
	row = bitmap.buffer + bitmap.rows * bitmap.width; /* past-the-end */
    row_end = bitmap.buffer;      /* beginning */
	int i;
	if (tex_color[3] == 1.0)
		while (row != row_end){
			row -= bitmap.width;
			for (i = 0; i < bitmap.width; i++)
				*p_b++ = rgb | (((guint32) row[i]) << 24);
		}
	else
		while (row != row_end){
			row -= bitmap.width;
			for (i = 0; i < bitmap.width; i++)
				*p_b++ = rgb | (((guint32) (tex_color[3] * row[i])) << 24);
		}	
	
	
	free(bitmap.buffer); // free bitmap buffer !
	format = GL_RGBA;
	DEBUG_PRINT_LINE;	
}

int SdlSurface::
get_width()
{
	DEBUG_PRINT_LINE;
	return tex_width;	
}

void SdlSurface::
set_pixels_array()
{
	DEBUG_PRINT_LINE;
	
	create_texture();

	glBindTexture( GL_TEXTURE_2D, *texture );
	glPixelStorei (GL_UNPACK_ALIGNMENT, 4);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0,
									format, GL_UNSIGNED_BYTE,  pixel_buffer );
	glFlush();
	delete [] pixel_buffer;
	pixel_buffer = NULL;
	DEBUG_PRINT_LINE;
}



void SdlSurface::
clear()
{
	DEBUG_PRINT_LINE;
	DEBUG_PRINT_LINE;
}

void SdlSurface::
draw(int x)
{
	DEBUG_PRINT_LINE;
	if(pixel_buffer != NULL)
		set_pixels_array();
	if(texture == NULL)
		return;
	int p_x, p_y, p_w, p_h;
	parrent->get_current_rect(&p_x, &p_y, &p_w, &p_h);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(0, 0, 0);


		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glBindTexture(GL_TEXTURE_2D,*texture );
		
		glViewport(x, 0,tex_width,tex_height);

		glScalef(scaled_x, scaled_y, scaled_z);
		
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		
		
		glBindTexture(GL_TEXTURE_2D,0 );
		
		glDisable(GL_BLEND);
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	DEBUG_PRINT_LINE;
}

void SdlSurface::
free_texture()
{
	DEBUG_PRINT_LINE;
	glBindTexture(GL_TEXTURE_2D,0 );
	glDeleteTextures(1,texture);
	free(texture);
	texture = NULL;
	DEBUG_PRINT_LINE;
}

void SdlSurface::
create_texture()
{
	DEBUG_PRINT_LINE;
	if(texture != NULL)
		free_texture();
	texture = (GLuint*) calloc(1,sizeof(GLuint));
	glGenTextures( 1, texture );
	DEBUG_PRINT_LINE;
}
