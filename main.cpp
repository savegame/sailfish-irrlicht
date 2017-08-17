/** Example 001 HelloWorld

This tutorial shows how to set up the IDE for using the Irrlicht Engine and how
to write a simple HelloWorld program with it. The program will show how to use
the basics of the VideoDriver, the GUIEnvironment, and the SceneManager.
Microsoft Visual Studio is used as an IDE, but you will also be able to
understand everything if you are using a different one or even another
operating system than Windows.

You have to include the header file <irrlicht.h> in order to use the engine. The
header file can be found in the Irrlicht Engine SDK directory \c include. To let
the compiler find this header file, the directory where it is located has to be
added in your project as include path. This is different for every IDE and
compiler you use. Let's explain shortly how to do this in Visual Studio 2010:

- In Visual Studio 2010 select the Menu Project -> Properties. Select the
  "C/C++" - "General" option, and select the "Additional Include Directories".
  Add the \c include directory of the Irrlicht engine folder to the list of
  directories. Now the compiler will find the irrlicht.h header file. We also
  need the irrlicht.lib to be found, so select "Linker" - "General" and
  add the \c lib/Win64-visualStudio or \c lib/Win32-visualStudio directory
  to "Additional Library Directories". Which of the 2 Irrlicht versions you
  chose depends on the target platform for your application (win32 or x64).
  In your project properties you can see what your active solution platform
  is, you can use the same one for Irrlicht.

To be able to use the Irrlicht.DLL file, we need to link with the Irrlicht.lib.
In most IDE's you have to add irrlicht.lib (or irrlicht.a or irrlicht.so on
Linux) to your Linker input files.

For VisualStudio we can be lazy and use the pragma comment lib.
We also want to get rid of the console window, which pops up when starting a
program with main() (instead of WinMain). This is done by the second pragma.
We could also use the WinMain method, though losing platform independence then.
*/
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include <irrlicht.h>
#include "exampleHelper.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/*
This is the main method. We can now use main() on every platform.
*/
int main()
{

	IrrlichtDevice *device =
	    createDevice( video::EDT_OGLES2, dimension2d<u32>(640, 480), 16,
			false, false, false, 0);

	if (!device)
		return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");


	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();




	guienv->addStaticText(L"Hello World! This is Irrlicht with the burnings software renderer!",
		rect<s32>(10,10,260,22), true);

	const io::path mediaPath = getExampleMediaPath();

	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");
	if (!mesh)
	{
		device->drop();
		return 1;
	}
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );


	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture(mediaPath + "sydney.bmp") );
	}
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));


	while(device->run())
	{
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255,100,101,140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();
	}

	device->drop();

	return 0;
}

