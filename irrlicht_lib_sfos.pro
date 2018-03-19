VERSION = 1.9.0

sailfish: TEMPLATE = app #lib
x11: TEMPLATE = app #lib
macx: TEMPLATE = app #lib
macx: CONFIG -= qt

!macx: CONFIG += link_pkgconfig
CONFIG += warn_off
sailfish: CONFIG += sailfishapp wayland-compositor
macx|x11: CONFIG-=qt
macx|x11: DEFINES += _DESKTOP
sailfish:QT += compositor widgets core quick
sailfish: INCLUDEPATH += /usr/include/qt5/QtWaylandClient/5.4.0
TARGET = irrlicht
INCLUDEPATH += third_party/wayland

sailfish: PKGCONFIG += wayland-client wayland-egl sdl2 SDL2_mixer
PKGCONFIG += zlib libpng egl
!sailfish: PKGCONFIG += bzip2
x11: PKGCONFIG += x11 xrandr gl glesv2 xxf86vm sdl
sailfish: LIBS += -lbz2
#x11: LIBS += -lIrrlicht -lIrrXML
sailfish: DEFINES += _IRR_COMPILE_WITH_SAILFISH_DEVICE_
sailfish: DEFINES += NO_IRR_COMPILE_WITH_X11_
!sailfish: DEFINES += _IRR_COMPILE_WITH_OPENGL_
!sailfish: DEFINES += NO_IRR_COMPILE_WITH_OGLES1_
x11: DEFINES += _IRR_LINUX_X11_RANDR_

#INCLUDEPATH+=/usr/include

macx {
    QMAKE_CXXFLAGS += $$system('/opt/local/bin/sdl-config --cflags')
    QMAKE_CXXFLAGS += -I/opt/local/include/
    LIBS += $$system('/opt/local/bin/sdl-config --libs')
    LIBS += -Wl,-framework,OpenGL -Wl,-framework,Cocoa -Wl,-framework,Foundation -Wl,-framework,AppKit
    LIBS += -Wl,-framework,SDL -Wl,framework,SDL_image
    LIBS += -Wl,-framework,UIKit
    DEFINES -= _IRR_COMPILE_WITH_OGLES2_
}

INCLUDEPATH += include
INCLUDEPATH += source/Irrlicht
INCLUDEPATH += source/Irrlicht/libjpeg

removespec.target = rpm/irrlicht.spec
removespec.command = rm -f rpm/irrlicht.spec
QMAKE_EXTRA_TARGETS += removespec
#POST_TARGETDEPS += removespec.target

media.files = media
media.path = /usr/share/$$TARGET/

x11 {
	link.target=$$OUT_PWD/media
	link.commands=[ ! -f $$OUT_PWD/media ] && ln -s $$PWD/media $$OUT_PWD/media
	QMAKE_EXTRA_TARGETS += link
	PRE_TARGETDEPS += $$link.target
}

INSTALLS += media

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
SOURCES += examples/13.RenderToTexture/main.cpp

DISTFILES += \
    rpm/irrlicht-sailfish.yaml \
    irrlicht.png \
    irrlicht.desktop \
    rpm/irrlicht.spec \
    media/Shaders/DFGLES2Screen.vsh \
    media/Shaders/DFGLES2Screen.fsh

