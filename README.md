# Irrlicht 1.9 OpenGLES port for Sailfish-Wayland
This is port of Irrlicht Engine, forked from SVN repository [https://svn.code.sf.net/p/irrlicht/code/branches/ogl-es].

This is dirty code, but its works, you can compile all irrlicht examples ( just need set EDT_OGLES2 driver type when creating device).
Now it compile with QtCreator as Application for sailfish ( or as Application for Linux Desktop, or as Application for OSX by XCode (tested on 8.0 - 9.0)).


* irrlicht.pro - its project file for Sailfish IDE (QtCreator), if you want compile that for Sailfish, you need add 
```
CONFIG+=sailfish
```
to **qmake** options in Project Setting. Or, if you build it from terminal:
```bash
mkdir /home/developer/builddir
cd /home/developer/builddir
qmake -r CONFIG+=release CONFIG-=debug CONFIG+=sailfish /path/to/project/irrlicht.pro
make -j4
make INSTALLROOT=/some/path/to/install/ install
```
but i dont now how it build a RPM package from terminal by qmake and make, in QtCreator its use \*.yaml file to create \*.spec file and build RPM. That mean, i recomended use QtCreator.

if you want build some examples, you need add **main.cpp** file of example to irrlicht.pro file 
```qmake
SOURCES += $$PWD/examples/16.Quake3MapShader/main.cpp
```
and application will build as that example. 

If you want build Irrlciht as shared library *libIrrlicht.so*, you need change **TEMPLATE** from **app** to **lib**, and change **TARGET** to Irrlicht, and remove ***main.cpp*** from **SOURCES**

### For programmers
Most changes for Wayland proting situated in source/Irrlicht/CIrrDeviceSailfish.cpp (\*.h), and CEGLManager.cpp (\*.h) under macros 
```cpp
#ifdef _IRR_COMPILE_WITH_OGLES2_
.....
#endif
```

# Sorry for the dirty code ;-)
