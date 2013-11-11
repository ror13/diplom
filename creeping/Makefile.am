## Makefile.am -- Process this file with automake to produce Makefile.in
#SUBDIRS = tests

INCLUDES = @GDK_CFLAGS@ @GTHREAD_CFLAGS@ @EGL_CFLAGS@   @PANGOFT2_CFLAGS@ -DWITH_SDL_BCK -DWITH_DEBUG_UTILS
bin_PROGRAMS = creeping
creeping_SOURCES = base_window.cpp debug_utils.h   base_window.h	base_window_backend.h  creeping.cpp  creeping.h  file_reader.cpp  file_reader.h backends/sdl_window.cpp backends/sdl_window.h   font_render.cpp  font_render.h main.cpp message_reader.h message_reader.cpp
creeping_LDADD = @GDK_LIBS@ @GTHREAD_LIBS@ @EGL_LIBS@    @PANGOFT2_LIBS@  -lGLESv2    -lpthread  -lm -ldl -lGL  -lpthread  \
	-L$(STAGING_DIR)/usr/lib/lib 

#bin_SCRIPTS = creepingfeed


