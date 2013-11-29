## Makefile.am -- Process this file with automake to produce Makefile.in
#SUBDIRS = tests

if DEBUG
DBG = -DWITH_DEBUG_UTILS 
else
DBG =
endif




INCLUDES = @GDK_CFLAGS@ @GTHREAD_CFLAGS@ @GL_CFLAGS@   @PANGOFT2_CFLAGS@  @WINDOWSYSTEM_CFLAGS@ -DWITH_GLES_BCK  $(DBG)
bin_PROGRAMS = creeping
creeping_SOURCES = base_window.cpp \
		   debug_utils.h \
		   base_window.h\
		   base_window_backend.h  \
		   creeping.cpp  \
		   creeping.h  \
		   file_reader.cpp  \
		   file_reader.h \
		   backends/window_platform_wrapper.h \
		   backends/window_platform_wrapper.cpp \
		   backends/gles_window.cpp \
		   backends/gles_window.h   \
		   font_render.cpp  \
		   font_render.h \
		   main.cpp \
		   message_reader.h \
		   message_reader.cpp
creeping_LDADD = @GDK_LIBS@ @GTHREAD_LIBS@ @GL_LIBS@    @PANGOFT2_LIBS@  @WINDOWSYSTEM_LIBS@    -lpthread  -lm -ldl -lGL  -lpthread  \
	-L$(STAGING_DIR)/usr/lib/lib 

#bin_SCRIPTS = creepingfeed


