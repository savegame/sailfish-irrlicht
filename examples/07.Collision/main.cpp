/** Example 007 Collision

We will describe 2 methods: Automatic collision detection for moving through
3d worlds with stair climbing and sliding, and manual scene node and triangle
picking using a ray.  In this case, we will use a ray coming out from the
camera, but you can use any ray.

To start, we take the program from tutorial 2, which loads and displays a
quake 3 level. We will use the level to walk in it and to pick triangles from.
In addition we'll place 3 animated models into it for triangle picking. The
following code starts up the engine and loads the level, as per tutorial 2.
*/
#include <irrlicht.h>
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <QGuiApplication>
#include <QTimer>
#include <QObject>

#include "irrapp.h"

int main(int argc, char** argv)
{

	QGuiApplication app(argc,argv);
	IrrApp irrApp(&app);
	return app.exec();
}

/*
**/
