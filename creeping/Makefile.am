## Makefile.am -- Process this file with automake to produce Makefile.in
#SUBDIRS = tests

if DEBUG
DBG = -DWITH_DEBUG_UTILS 
else
DBG =
endif




INCLUDES = @LIBCURL_CFLAGS@ @GDK_CFLAGS@ @GTHREAD_CFLAGS@ @GL_CFLAGS@  @PANGOFT2_CFLAGS@  @WINDOWSYSTEM_CFLAGS@ -DWITH_GLES_BCK  $(DBG)
bin_PROGRAMS = creeping
creeping_SOURCES = main.cpp\
		   utils.h \
		   utils.cpp \
		   cthread.h \
		   cthread.cpp \
		   debug_utils.h \
		   base_window.cpp \
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
		   message_reader.h \
		   message_reader.cpp 
creeping_LDFLAGS =  -Wl,--allow-multiple-definition
creeping_LDADD =  @GDK_LIBS@  @GTHREAD_LIBS@ @GL_LIBS@   @PANGOFT2_LIBS@  @WINDOWSYSTEM_LIBS@ @LIBCURL_LIBS@ 


