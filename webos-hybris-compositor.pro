TARGET = WebosHybrisCompositor
TEMPLATE = lib

CONFIG += warn_on
CONFIG += building-libs
CONFIG += depend_includepath

CONFIG += link_pkgconfig
PKGCONFIG = glib-2.0 gthread-2.0 gio-unix-2.0

DISTRO_TYPE = $$(DISTRO_TYPE)
contains(DISTRO_TYPE, release) {
    CONFIG -= debug
}

HEADERS += \
    src/HybrisCompositor.h \
    src/HybrisCompositorRemoteClient.h \
    src/HybrisCompositorClient.h

SOURCES += \
    src/HybrisCompositor.cpp \
    src/HybrisCompositorRemoteClient.cpp \
    src/HybrisCompositorClient.cpp

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
    src/HybrisCompositorClient.h

INSTALLS += headers

target.path = $$STAGING_LIBDIR
INSTALLS += target
