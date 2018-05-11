eglplatform_offscreen_ladir = $(libdir)/libhybris
eglplatform_offscreen_la_LTLIBRARIES = eglplatform_offscreen.la

eglplatform_offscreen_la_SOURCES = \
	eglplatform_offscreen.cpp \
	OffscreenNativeWindow.cpp

eglplatform_offscreen_la_CXXFLAGS = $(GLIB_CFLAGS) $(LIBHYBRIS_CFLAGS) \
	-I$(top_srcdir)/libwebos-gui

if WANT_DEBUG
eglplatform_offscreen_la_CXXFLAGS += -ggdb -O0
endif

eglplatform_offscreen_la_LDFLAGS = $(GLIB_LIBS) $(LIBHYBRIS_LIBS) ../libwebos-gui/libwebos-gui.la \
	-avoid-version -module -shared -export-dynamic

