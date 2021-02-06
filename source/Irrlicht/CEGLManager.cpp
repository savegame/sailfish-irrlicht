// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CEGLManager.h"

#ifdef _IRR_COMPILE_WITH_EGL_MANAGER_

#include "irrString.h"
#include "os.h"

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#include <android/native_activity.h>
#endif

namespace irr
{
namespace video
{

CEGLManager::CEGLManager() : IContextManager(), EglWindow(0), EglDisplay(EGL_NO_DISPLAY),
    EglSurface(EGL_NO_SURFACE), EglContext(EGL_NO_CONTEXT), EglConfig(0), MajorVersion(0), MinorVersion(0)
{
	#ifdef _DEBUG
	setDebugName("CEGLManager");
	#endif
}

CEGLManager::~CEGLManager()
{
    destroyContext();
    destroySurface();
    terminate();
}

bool CEGLManager::initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data)
{
	// store new data
	Params=params;
	Data=data;

	if (EglWindow != 0 && EglDisplay != EGL_NO_DISPLAY)
        return true;

	// Window is depend on platform.
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	EglWindow = (NativeWindowType)Data.OpenGLWin32.HWnd;
	Data.OpenGLWin32.HDc = GetDC((HWND)EglWindow);
	EglDisplay = eglGetDisplay((NativeDisplayType)Data.OpenGLWin32.HDc);
#elif defined(_IRR_EMSCRIPTEN_PLATFORM_)
	EglWindow = 0;
	EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#elif defined(_IRR_COMPILE_WITH_SAILFISH_DEVICE_)
	{
		EglDisplay = (EGLDisplay)Data.OGLESWayland.Display;
		EglWindow = (NativeWindowType)Data.OGLESWayland.Window;
		EglSurface = (EGLSurface)Data.OGLESWayland.Surface;
		EglContext = (EGLContext)Data.OGLESWayland.Context;
		nativeDisplay = (NativeDisplayType)Data.OGLESWayland.nativeDisplay;
	}
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	EglWindow = (NativeWindowType)Data.OpenGLLinux.X11Window;
	EglDisplay = eglGetDisplay((NativeDisplayType)Data.OpenGLLinux.X11Display);
#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
	EglWindow = (NativeWindowType)Data.OGLES_SDL.nativeWindow;
	// EglDisplay = eglGetDisplay((NativeDisplayType)Data.OGLES_SDL.nativeDisplay);
	EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow =	(ANativeWindow*)Data.OGLESAndroid.Window;
	EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#elif defined(_IRR_COMPILE_WITH_FB_DEVICE_)
	EglWindow = (NativeWindowType)Data.OpenGLFB.Window;
	EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif

	// We must check if EGL display is valid.
	if (EglDisplay == EGL_NO_DISPLAY)
    {
		os::Printer::log("Could not get EGL display.");
		terminate();
        return false;
    }

	// Initialize EGL here.
	if (!eglInitialize(EglDisplay, &MajorVersion, &MinorVersion))
    {
		os::Printer::log("Could not initialize EGL display.");

        EglDisplay = EGL_NO_DISPLAY;
		terminate();
        return false;
    }
	else
		os::Printer::log("EGL version", core::stringc(MajorVersion+(MinorVersion*0.1f)).c_str());

    return true;
}

void CEGLManager::terminate()
{
    if (EglWindow == 0 && EglDisplay == EGL_NO_DISPLAY)
        return;

	if (EglDisplay != EGL_NO_DISPLAY)
	{
		// We should unbind current EGL context before terminate EGL.
		eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		eglTerminate(EglDisplay);
		EglDisplay = EGL_NO_DISPLAY;
	}

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	if (Data.OpenGLWin32.HDc)
    {
		ReleaseDC((HWND)EglWindow, (HDC)Data.OpenGLWin32.HDc);
        Data.OpenGLWin32.HDc = 0;
    }
#endif

    MajorVersion = 0;
    MinorVersion = 0;
}

bool CEGLManager::generateSurface()
{
    if (EglDisplay == EGL_NO_DISPLAY)
        return false;

    if (EglSurface != EGL_NO_SURFACE)
        return true;

	// We should assign new WindowID on platforms, where WindowID may change at runtime,
	// at this time only Android support this feature.
	// this needs an update method instead!

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow = (ANativeWindow*)Data.OGLESAndroid.Window;
#endif

	EGLint EglOpenGLBIT = 0;

	// We need proper OpenGL BIT.
	switch (Params.DriverType)
	{
	case EDT_OGLES1:
		EglOpenGLBIT = EGL_OPENGL_ES_BIT;
		break;
	case EDT_OGLES2:
	case EDT_WEBGL1:
		EglOpenGLBIT = EGL_OPENGL_ES2_BIT;
		break;
	default:
		break;
	}

	EGLint Attribs[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_BUFFER_SIZE, 32,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		// EGL_SAMPLE_BUFFERS, Params.AntiAlias ? 1:0,
		// EGL_SAMPLES, Params.AntiAlias,
#ifdef EGL_VERSION_1_3
		// EGL_RENDERABLE_TYPE, EglOpenGLBIT,
#endif
		EGL_NONE
	};

	EglConfig = 0;
	EGLint NumConfigs = 0;
	u32 Steps = 5;

	if (!eglChooseConfig(EglDisplay, Attribs, &EglConfig, 0, &NumConfigs) || NumConfigs<=0) {
        printf("Cannot find an EGL config.\n");
		return false;
    }

    // EGLConfig *configs = (EGLConfig *)malloc(sizeof(EGLConfig) * NumConfigs);
    // if (!eglChooseConfig(EglDisplay, Attribs, configs, NumConfigs, &NumConfigs) || NumConfigs<=0) {
    //     printf("Cannot find an EGL config.\n");
    //     return false;
    // }

	// Choose the best EGL config.
	// while (!eglChooseConfig(EglDisplay, Attribs, &EglConfig, 1, &NumConfigs) || !NumConfigs)
	// {
	// 	switch (Steps)
	// 	{
	// 	case 5: // samples
	// 		if (Attribs[19] > 2)
	// 			--Attribs[19];
	// 		else
	// 		{
	// 			Attribs[17] = 0;
	// 			Attribs[19] = 0;
	// 			--Steps;
	// 		}
	// 		break;
	// 	case 4: // alpha
	// 		if (Attribs[7])
	// 		{
	// 			Attribs[7] = 0;

	// 			if (Params.AntiAlias)
	// 			{
	// 				Attribs[17] = 1;
	// 				Attribs[19] = Params.AntiAlias;
	// 				Steps = 5;
	// 			}
	// 		}
	// 		else
	// 			--Steps;
	// 		break;
	// 	case 3: // stencil
	// 		if (Attribs[15])
	// 		{
	// 			Attribs[15] = 0;

	// 			if (Params.AntiAlias)
	// 			{
	// 				Attribs[17] = 1;
	// 				Attribs[19] = Params.AntiAlias;
	// 				Steps = 5;
	// 			}
	// 		}
	// 		else
	// 			--Steps;
	// 		break;
	// 	case 2: // depth size
	// 		if (Attribs[13] > 16)
	// 		{
	// 			Attribs[13] -= 8;
	// 		}
	// 		else
	// 			--Steps;
	// 		break;
	// 	case 1: // buffer size
	// 		if (Attribs[9] > 16)
	// 		{
	// 			Attribs[9] -= 8;
	// 		}
	// 		else
	// 			--Steps;
	// 		break;
	// 	default:
	// 		os::Printer::log("Could not get config for EGL display.");
	// 		return false;
	// 	}
	// }

	if (Params.AntiAlias && !Attribs[17])
		os::Printer::log("No multisampling.");

	if (Params.WithAlphaChannel && !Attribs[7])
		os::Printer::log("No alpha.");

	if (Params.Stencilbuffer && !Attribs[15])
		os::Printer::log("No stencil buffer.");

	if (Params.ZBufferBits > Attribs[13])
		os::Printer::log("No full depth buffer.");

	if (Params.Bits > Attribs[9])
		os::Printer::log("No full color buffer.");

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
    EGLint Format = 0;
    eglGetConfigAttrib(EglDisplay, EglConfig, EGL_NATIVE_VISUAL_ID, &Format);

    ANativeWindow_setBuffersGeometry(EglWindow, 0, 0, Format);
#endif

	// Now we are able to create EGL surface.
	EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, EglWindow, 0);

	if (EGL_NO_SURFACE == EglSurface) {
		EGLint error = eglGetError();
		core::stringc error_msg = core::stringc(error);
		# define case_error(x) case x: error_msg = (#x); break;
		switch(error) {
		// case_error(EGL_NO_SURFACE)
		case_error(EGL_BAD_DISPLAY)
		case_error(EGL_NOT_INITIALIZED)
		case_error(EGL_BAD_CONFIG)
		case_error(EGL_BAD_NATIVE_WINDOW)
		case_error(EGL_BAD_ATTRIBUTE)
		case_error(EGL_BAD_ALLOC)
		case_error(EGL_BAD_MATCH)
			break;
		}
		os::Printer::log( (core::stringc("EGLError: ") + error_msg).c_str(), ELL_ERROR );
		EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, 0, 0);
	}

	if (EGL_NO_SURFACE == EglSurface) {
		os::Printer::log("Could not create EGL surface.");
		return false;
	}

#ifdef EGL_VERSION_1_2
	// if (MinorVersion > 1)
		// eglBindAPI(EGL_OPENGL_ES_API);
#endif

    if (Params.Vsync)
		eglSwapInterval(EglDisplay, 1);

	if (testEGLError())
	{
		os::Printer::log("Could not make EGL context current.");
		return false;
	}

    return true;
}

void CEGLManager::destroySurface()
{
    if (EglSurface == EGL_NO_SURFACE)
        return;

	// We should unbind current EGL context before destroy EGL surface.
	eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(EglDisplay, EglSurface);
    EglSurface = EGL_NO_SURFACE;
}

bool CEGLManager::generateContext()
{
    if (EglDisplay == EGL_NO_DISPLAY || EglSurface == EGL_NO_SURFACE)
        return false;

    if (EglContext != EGL_NO_CONTEXT)
        return true;

	EGLint OpenGLESVersion = 0;

	switch (Params.DriverType)
	{
	case EDT_OGLES1:
		OpenGLESVersion = 1;
		break;
	case EDT_OGLES2:
	case EDT_WEBGL1:
		OpenGLESVersion = 2;
		break;
	default:
		break;
	}

    EGLint ContextAttrib[] =
	{
#ifdef EGL_VERSION_1_3
		EGL_CONTEXT_CLIENT_VERSION, OpenGLESVersion,
#endif
		EGL_NONE, 0
	};
#if !defined(_IRR_COMPILE_WITH_SAILFISH_DEVICE_)

//#else
	EglContext = eglCreateContext(EglDisplay, EglConfig, EGL_NO_CONTEXT, ContextAttrib);
#endif
	if (testEGLError())
	{
		os::Printer::log("Could not create EGL context.", ELL_ERROR);
		return false;
	}

	os::Printer::log("EGL context created with OpenGLESVersion: ", core::stringc((int)OpenGLESVersion), ELL_DEBUG);

	eglMakeCurrent(EglDisplay,EglSurface,EglSurface,EglContext);

	if (testEGLError())
	{
		os::Printer::log("Could not make EGL context current.");
		return false;
	}

    return true;
}

void CEGLManager::destroyContext()
{
    if (EglContext == EGL_NO_CONTEXT)
        return;

	// We must unbind current EGL context before destroy it.
	eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(EglDisplay, EglContext);

    EglContext = EGL_NO_CONTEXT;
}

bool CEGLManager::activateContext(const SExposedVideoData& videoData)
{
//	// Make the context current
//	if ( !eglMakeCurrent(Display, Surface, Surface, Context) )
//	{
//		os::Printer::log("Could not make the current window current !\n");
//		return false;
//	}SOGLESSDL
	eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);

	if (testEGLError())
	{
		os::Printer::log("Could not make EGL context current.");
		return false;
	}

	return true;
}

const SExposedVideoData& CEGLManager::getContext() const
{
	return Data;
}

bool CEGLManager::swapBuffers()
{
    return (eglSwapBuffers(EglDisplay, EglSurface)==EGL_TRUE);
}

bool CEGLManager::testEGLError()
{
#if defined(EGL_VERSION_1_0) && defined(_DEBUG)
	EGLint status = eglGetError();

	switch (status)
	{
		case EGL_SUCCESS:
            return false;
		case EGL_NOT_INITIALIZED :
			os::Printer::log("EGL_ERROR: Not Initialized", ELL_ERROR);
            break;
		case EGL_BAD_ACCESS:
			os::Printer::log("EGL_ERROR: Bad Access", ELL_ERROR);
            break;
		case EGL_BAD_ALLOC:
			os::Printer::log("EGL_ERROR: Bad Alloc", ELL_ERROR);
            break;
		case EGL_BAD_ATTRIBUTE:
			os::Printer::log("EGL_ERROR: Bad Attribute", ELL_ERROR);
            break;
		case EGL_BAD_CONTEXT:
			os::Printer::log("EGL_ERROR: Bad Context", ELL_ERROR);
            break;
		case EGL_BAD_CONFIG:
			os::Printer::log("EGL_ERROR: Bad Config", ELL_ERROR);
            break;
		case EGL_BAD_CURRENT_SURFACE:
			os::Printer::log("EGL_ERROR: Bad Current Surface", ELL_ERROR);
            break;
		case EGL_BAD_DISPLAY:
			os::Printer::log("EGL_ERROR: Bad Display", ELL_ERROR);
            break;
		case EGL_BAD_SURFACE:
			os::Printer::log("EGL_ERROR: Bad Surface", ELL_ERROR);
            break;
		case EGL_BAD_MATCH:
			os::Printer::log("EGL_ERROR: Bad Match", ELL_ERROR);
            break;
		case EGL_BAD_PARAMETER:
			os::Printer::log("EGL_ERROR: Bad Parameter", ELL_ERROR);
            break;
		case EGL_BAD_NATIVE_PIXMAP:
			os::Printer::log("EGL_ERROR: Bad Native Pixmap", ELL_ERROR);
            break;
		case EGL_BAD_NATIVE_WINDOW:
			os::Printer::log("EGL_ERROR: Bad Native Window", ELL_ERROR);
            break;
		case EGL_CONTEXT_LOST:
			os::Printer::log("EGL_ERROR: Context Lost", ELL_ERROR);
            break;
        default:
            break;
	};

	return true;
#else
	return false;
#endif
}

}
}

#endif
