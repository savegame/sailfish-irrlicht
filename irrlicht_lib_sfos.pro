VERSION = 1.9.0

TEMPLATE = lib
TARGET = irrlicht
DESTDIR = ../../lib

CONFIG += warn_off staticlib
CONFIG += wayland-compositor link_pkgconfig
#QT += compositor widgets core quick

INCLUDEPATH += /usr/include/qt5/QtWaylandClient/5.4.0
INCLUDEPATH += third_party/wayland

PKGCONFIG += wayland-client wayland-egl sdl2 SDL2_mixer
PKGCONFIG += zlib libpng egl
LIBS += -lbz2 -Wl,-rpath,/usr/lib
DEFINES += _IRR_COMPILE_WITH_SAILFISH_DEVICE_
DEFINES += NO_IRR_COMPILE_WITH_X11_

x11 {
	DEFINES -= _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	DEFINES += _IRR_COMPILE_WITH_X11_
	DEFINES += _IRR_COMPILE_WITH_OGLES2_
	DEFINES += NO_IRR_COMPILE_WITH_OGLES1_
	DEFINES += _IRR_COMPILE_WITH_EGL_MANAGER_
}

INCLUDEPATH += include
INCLUDEPATH += source/Irrlicht
INCLUDEPATH += source/Irrlicht/libjpeg

#removespec.target = rpm/irrlicht.spec
#removespec.command = rm -f rpm/irrlicht.spec
#QMAKE_EXTRA_TARGETS += removespec
#PRE_TARGETDEPS += removespec.target


#media.files = media
#media.path = /usr/share/$$TARGET/
#INSTALLS += media

debug: DEFINES += _DEBUG
debug: DEFINES += _OUT_PWD_PATH=\\\"$$OUT_PWD\\\"

#include(source/qt/qt.pri )
include(source/Irrlicht/jpeglib/jpeglib.pri)
include(irrlicht.pri)

#SOURCES += main.cpp
#SOURCES +=  examples/02.Quake3Map/main.cpp \
#SOURCES =  examples/05.UserInterface/main.cpp
#SOURCES += examples/16.Quake3MapShader/main.cpp

#HEADERS += examples/Demo/CDemo.h

#SOURCES += examples/Demo/CDemo.cpp \
#           examples/Demo/main.cpp

DEFINES += USE_SDL_MIXER

#    waylandtest_main.cpp
#    examples/07.Collision/main.cpp \
#    examples/07.Collision/irrapp.cpp
#SOURCES += examples/13.RenderToTexture/main.cpp

DISTFILES += \
#    rpm/irrlicht-sailfish.yaml \
#    irrlicht.png \
#    irrlicht.desktop \
#    rpm/irrlicht.spec \
    media/Shaders/DFGLES2Screen.vsh \
    media/Shaders/DFGLES2Screen.fsh

