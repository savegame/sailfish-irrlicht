VERSION = 1.9.0

TEMPLATE = lib
TARGET = irrlicht
DESTDIR = $$OUT_PWD/../../lib

CONFIG += warn_off qt
#staticlib
QT += core quick
CONFIG += wayland-compositor link_pkgconfig
#QT += compositor widgets core quick

INCLUDEPATH += /usr/include/qt5/QtWaylandClient/5.4.0
INCLUDEPATH += third_party/wayland

debug: DEFINES += Q_ENABLE_OPENGL_FUNCTIONS_DEBUG

!ios: !android: !win32: PKGCONFIG += zlib
ios {
    CONFIG += staticlib
    LIBS +=  -Wl,-framework OpenGLES2  -Wl,-framework AppKit
#    glshaders.path=Contens/iOS
#    glshaders.files = $$PWD/media/Shaders
#    QMAKE_BUNDLE_DATA += glshaders
    DEFINES += DISABLE_GL_MAX_ELEMENTS_INDICES
}

!ios: DEFINES += _IRRDIR=\\\"$$PWD/\\\"
ios: DEFINES += _IRRDIR=\\\"\\\"
!x11: DEFINES += NO_IRR_COMPILE_WITH_X11_

DEFINES += _IRR_COMPILE_WITH_QGLFUNCTIONS_
win32: DEFINES += _IRR_WINDOWS_API_ _IRR_WINDOWS_
DEFINES += NO__IRR_COMPILE_WITH_IOS_DEVICE_
DEFINES -= _IRR_COMPILE_WITH_SAILFISH_DEVICE_
DEFINES -= _IRR_COMPILE_WITH_WINDOWS_DEVICE_
DEFINES += NO_IRR_COMPILE_WITH_SAILFISH_DEVICE_
DEFINES += NO_IRR_COMPILE_WITH_OGLES2_
DEFINES += NO_IRR_COMPILE_WITH_OGLES1_
DEFINES += NO_IRR_COMPILE_WITH_WEBGL1_
DEFINES += NO_IRR_COMPILE_WITH_X11_DEVICE_
DEFINES += NO_IRR_COMPILE_WITH_SDL_DEVICE_
DEFINES += NO_IRR_COMPILE_WITH_OPENGL_
DEFINES += NO__IRR_COMPILE_WITH_OSX_DEVICE_
DEFINES += NO_IRR_COMPILE_WITH_ANDROID_DEVICE_
DEFINES -= _IRR_OPENGL_USE_EXTPOINTER_
DEFINES += NO_IRR_OPENGL_USE_EXTPOINTER_
DEFINES += NO_IRR_COMPILE_WITH_GLX_MANAGER_

osx: {
    DEFINES += _IRR_OSX_PLATFORM_
}

ios | sailfish: {
    DEFINES += GLES2_PLATFORM=1
}

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/source/Irrlicht
INCLUDEPATH += $$PWD/source/qt
INCLUDEPATH += $$PWD/source/Irrlicht/libjpeg

#removespec.target = rpm/irrlicht.spec
#removespec.command = rm -f rpm/irrlicht.spec
#QMAKE_EXTRA_TARGETS += removespec
#PRE_TARGETDEPS += removespec.target


#media.files = media
#media.path = /usr/share/$$TARGET/
#INSTALLS += media

debug: DEFINES += _DEBUG
debug: DEFINES += _OUT_PWD_PATH=\\\"$$OUT_PWD\\\"
android : {
    DEFINES += _MEDIA_PATH=\\\"media/\\\"
}
else {
    DEFINES += _MEDIA_PATH=\\\"$$PWD/media/\\\"
}

include(source/qt/qt.pri )
include(source/Irrlicht/jpeglib/jpeglib.pri)
include(source/Irrlicht/libpng/libpng.pri)
include(irrlicht.pri)

include(source/Irrlicht/bzip2/bzip2.pri)
win32: include(source/Irrlicht/zlib/zlib.pri)
DEFINES += IRRLICHT_EXPORTS

#SOURCES += main.cpp
#SOURCES +=  examples/02.Quake3Map/main.cpp \
#SOURCES =  examples/05.UserInterface/main.cpp
#SOURCES += examples/16.Quake3MapShader/main.cpp

#HEADERS += examples/Demo/CDemo.h

#SOURCES += examples/Demo/CDemo.cpp \
#           examples/Demo/main.cpp

#DEFINES += USE_SDL_MIXER
#DEFINES += _IRR_SAILFISH_USE_RENDER_TARGET

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

