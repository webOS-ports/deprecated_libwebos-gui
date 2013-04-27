NULL =

lib_LTLIBRARIES = libwebos-gui.la

libwebos_gui_la_SOURCES = \
	src/util/fdpass.c \
	src/OffscreenNativeWindowBuffer.cpp \
	src/OffscreenNativeWindow.cpp \
	src/WebosSurfaceManagerClient.cpp \
	src/WebosSurfaceManager.cpp \
	src/WebosSurfaceManagerRemoteClient.cpp
libwebos_gui_la_CPPFLAGS = $(GLIB_CFLAGS)
libwebos_gui_la_LIBADD = $(GLIB_LIBS) -lhybris-eglplatformcommon

libwebos_guidir = $(includedir)/webos-gui
libwebos_gui_DATA = \
	src/OffscreenNativeWindow.h \
	src/WebosSurfaceManagerClient.h \
	src/WebosSurfaceManager.h \
	src/WebosSurfaceManagerRemoteClient.h

libwebos_gui_la_LDFLAGS = -version-info "1":"0":"0"

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = webos-gui.pc

EXTRA_DIST = \
	MAINTAINERS \
	webos-gui.pc \
	$(NULL)

MAINTAINERCLEANFILES = \
	aclocal.m4 compile config.guess config.sub \
	configure depcomp install-sh ltmain.sh     \
	Makefile.in missing config.h.in            \
	mkinstalldirs *~ \
	$(NULL)
