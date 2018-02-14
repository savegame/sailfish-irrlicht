// This is a Demo of the Irrlicht Engine (c) 2005-2009 by N.Gebhardt.
// This file is not documented.

#include <irrlicht.h>
#ifdef _IRR_WINDOWS_
#include <windows.h>
#endif

#include <stdio.h>

#include "CMainMenu.h"
#include "CDemo.h"

using namespace irr;

#ifdef _MSC_VER

#pragma comment(lib, "Irrlicht.lib")
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char* argv[])
#endif
{
#ifndef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	CMainMenu menu;

	if (menu.run())
	{
		CDemo demo(menu.getFullscreen(),
					menu.getMusic(),
					menu.getShadows(),
					menu.getAdditive(),
					menu.getVSync(),
					menu.getAntiAliasing(),
					menu.getDriverType());
		demo.run();
	}
#else

	CDemo demo( true,
	            true,
	            true,
	            false,
	            false,
	            false,
	            irr::video::EDT_OGLES2);
	demo.run();
#endif
	return 0;
}

