VERSION = 1.9.0
sailfish: TEMPLATE = app #lib
x11: TEMPLATE = app #lib
macx: TEMPLATE = app #lib

!macx: CONFIG += link_pkgconfig
CONFIG += warn_off
sailfish: CONFIG += sailfishapp wayland-compositor
macx|x11: CONFIG-=qt
sailfish:QT += compositor widgets core
#sailfish: CONFIG += wayland-compositor
sailfish: INCLUDEPATH += /usr/include/qt5/QtWaylandClient/5.4.0
TARGET = irrlicht
#DESTDIR = bin

sailfish: PKGCONFIG += wayland-client wayland-egl sdl2 SDL2_image SDL2_mixer SDL2_ttf
PKGCONFIG += zlib libpng egl
!sailfish: PKGCONFIG += bzip2
x11: PKGCONFIG += x11 xrandr gl glesv2 xxf86vm sdl
sailfish: LIBS += -lbz2

sailfish: DEFINES += SAILFISH
!sailfish: DEFINES += _IRR_COMPILE_WITH_OPENGL_
!sailfish: DEFINES += NO_IRR_COMPILE_WITH_OGLES1_
x11: DEFINES += _IRR_LINUX_X11_RANDR_

macx {
    # need homebrew sdl1 installed (wrong include path)
    QMAKE_CXXFLAGS += $$system('/opt/local/bin/sdl-config --cflags')
    QMAKE_CXXFLAGS += -I/opt/local/include/
    LIBS += $$system('/opt/local/bin/sdl-config --libs')
    LIBS += -Wl,-framework,OpenGL -Wl,-framework,Cocoa -Wl,-framework,Foundation -Wl,-framework,AppKit
#    DEFINES += NO_IRR_COMPILE_WITH_OSX_DEVICE_
    DEFINES += _IRR_COMPILE_WITH_OGLES2_
#    DEFINES +=  _IRR_COMPILE_WITH_SDL_DEVICE_
#    DEFINES += _IRR_COMPILE_WITH_OPENGL_
}

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
        examples/16.Quake3MapShader/main.cpp \
#    waylandtest_main.cpp
    #examples/07.Collision/main.cpp \
    #examples/07.Collision/irrapp.cpp
#    examples/13.RenderToTexture/main.cpp

DISTFILES += \
    rpm/irrlicht-sailfish.yaml \
    irrlicht.png \
    irrlicht.desktop

