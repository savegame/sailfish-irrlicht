VERSION = 1.9.0
sailfish: TEMPLATE = app #lib
x11: TEMPLATE = lib

CONFIG += link_pkgconfig warn_off
sailfish: CONFIG += sailfishapp
x11: CONFIG-=qt

TARGET = irrlicht
#DESTDIR = bin

sailfish: PKGCONFIG += wayland-client wayland-egl dbus-1
PKGCONFIG += zlib libpng egl
!sailfish: PKGCONFIG += bzip2
x11: PKGCONFIG += x11 xrandr gl glesv2 xxf86vm sdl
sailfish: LIBS += -lbz2

sailfish: DEFINES += SAILFISH
!sailfish: DEFINES += _IRR_COMPILE_WITH_OPENGL_
!sailfish: DEFINES += NO_IRR_COMPILE_WITH_OGLES1_
x11: DEFINES += _IRR_LINUX_X11_RANDR_

INCLUDEPATH += include
INCLUDEPATH += source/Irrlicht
INCLUDEPATH += source/Irrlicht/libjpeg

removespec.target = rpm/irrlicht.spec
removespec.command = rm -f rpm/irrlicht.spec
QMAKE_EXTRA_TARGETS += removespec

media.files = media
media.path = /usr/share/$$TARGET/

INSTALLS += media

debug: DEFINES += _DEBUG

include(source/Irrlicht/jpeglib/jpeglib.pri)
include(irrlicht.pri)

#SOURCES += main.cpp
#SOURCES +=  examples/02.Quake3Map/main.cpp \
SOURCES +=  \
#	examples/16.Quake3MapShader/main.cpp \
    #examples/07.Collision/main.cpp \
    #examples/07.Collision/irrapp.cpp
    examples/13.RenderToTexture/main.cpp

DISTFILES += \
    rpm/irrlicht-sailfish.yaml \
    irrlicht.png \
    irrlicht.desktop \
    rpm/irrlicht.spec
