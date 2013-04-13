TARGET = WebosHybrisCompositor
TEMPLATE = lib

CONFIG += warn_on
CONFIG += building-libs
CONFIG += depend_includepath
CONFIG += create_pc
CONFIG += create_prl no_install_prl

CONFIG += link_pkgconfig
PKGCONFIG = glib-2.0 gthread-2.0 gio-unix-2.0

DISTRO_TYPE = $$(DISTRO_TYPE)
contains(DISTRO_TYPE, release) {
    CONFIG -= debug
}

HEADERS += \
    src/HybrisCompositor.h \
    src/HybrisCompositorRemoteClient.h \
    src/HybrisCompositorClient.h \
    src/OffscreenNativeWindow.h

SOURCES += \
    src/util/fdpass.c \
    src/HybrisCompositor.cpp \
    src/HybrisCompositorRemoteClient.cpp \
    src/HybrisCompositorClient.cpp \
    src/OffscreenNativeWindow.cpp \
    src/OffscreenNativeWindowBuffer.cpp

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $(TARGET)

STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(STAGING_DIR)/include

STAGING_LIBDIR = $$(STAGING_LIBDIR)
isEmpty(STAGING_LIBDIR):STAGING_LIBDIR = $$(STAGING_DIR)/lib

headers.path = $${STAGING_INCDIR}/WebosHybrisCompositor
headers.files += \
    src/HybrisCompositor.h \
    src/HybrisCompositorRemoteClient.h \
    src/HybrisCompositorClient.h \
    src/OffscreenNativeWindow.h

INSTALLS += headers

target.path = $$STAGING_LIBDIR
INSTALLS += target

QMAKE_PKGCONFIG_NAME = libWebosHybrisCompositor
QMAKE_PKGCONFIG_DESCRIPTION = The webOS compositor library based on libhybris
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
