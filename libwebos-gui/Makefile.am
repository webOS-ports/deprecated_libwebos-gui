NULL =

lib_LTLIBRARIES = libwebos-gui.la

libwebos_gui_la_SOURCES = \
	util/fdpass.c \
	OffscreenNativeWindowBuffer.cpp \
	WebosSurfaceManagerClient.cpp \
	WebosSurfaceManager.cpp \
	WebosSurfaceManagerRemoteClient.cpp \
	BufferAllocator.cpp
libwebos_gui_la_CPPFLAGS = $(GLIB_CFLAGS) $(LIBHYBRIS_CFLAGS)
libwebos_gui_la_LIBADD = $(GLIB_LIBS) $(LIBHYBRIS_LIBS)

libwebos_guidir = $(includedir)/webos-gui
libwebos_gui_DATA = \
	OffscreenNativeWindowBuffer.h \
	WebosSurfaceManagerClient.h \
	WebosSurfaceManager.h \
	WebosSurfaceManagerRemoteClient.h

libwebos_gui_la_LDFLAGS = -version-info "1":"0":"0"

