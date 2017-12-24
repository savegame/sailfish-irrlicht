// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceSailfish.h"

#ifdef SAILFISH

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <time.h>
#include <locale.h>
#include "IEventReceiver.h"
#include "ISceneManager.h"
#include "IGUIEnvironment.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include "COSOperator.h"
#include "CColorConverter.h"
#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IGUISpriteBank.h"

#include <linux/input.h>
#include <Keycodes.h>
#include <wayland-util.h>
#include <QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-surface-extension-client-protocol.h>
#include <QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-hardware-integration-client-protocol.h>
#include <QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-touch-extension-client-protocol.h>
#include <QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-windowmanager-client-protocol.h>
#include <QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-touch-extension-client-protocol.h>
#include <wayland-client-protocol.h>

#if defined(_IRR_COMPILE_WITH_OGLES1_) || defined(_IRR_COMPILE_WITH_OGLES2_)
# include "CEGLManager.h"
#endif

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
# include <fcntl.h>
# include <unistd.h>

// linux/joystick.h includes linux/input.h, which #defines values for various KEY_FOO keys.
// These override the irr::KEY_FOO equivalents, which stops key handling from working.
// As a workaround, defining _INPUT_H stops linux/input.h from being included; it
// doesn't actually seem to be necessary except to pull in sys/ioctl.h.
# define _INPUT_H
#  include <sys/ioctl.h> // Would normally be included in linux/input.h
#  include <linux/joystick.h>
# undef _INPUT_H
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_

namespace irr
{
	namespace video
	{
#ifdef _IRR_COMPILE_WITH_OPENGL_
		IVideoDriver* createOpenGLDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
#endif

#ifdef _IRR_COMPILE_WITH_OGLES1_
        IVideoDriver* createOGLES1Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
#endif

#ifdef _IRR_COMPILE_WITH_OGLES2_
        IVideoDriver* createOGLES2Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
#endif

#ifdef _IRR_COMPILE_WITH_WEBGL1_
		IVideoDriver* createWebGL1Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
#endif
	}
} // end namespace irr


static struct window {
//	EGLContext egl_context;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct wl_egl_window *egl_window;
	irr::CIrrDeviceSailfish *irrDevice;
//	EGLSurface egl_surface;
} wlWindow;

struct wl_compositor *irr::CIrrDeviceSailfish::wlCompositor = NULL;
struct wl_shell      *irr::CIrrDeviceSailfish::wlShell      = NULL;
struct wl_seat       *irr::CIrrDeviceSailfish::wlSeat       = NULL;
struct wl_keyboard   *irr::CIrrDeviceSailfish::wlKeyboard   = NULL;
struct wl_touch      *irr::CIrrDeviceSailfish::wlTouch      = NULL;
struct wl_pointer    *irr::CIrrDeviceSailfish::wlPointer    = NULL;
struct wl_output     *irr::CIrrDeviceSailfish::wlOutput     = NULL;
struct qt_surface_extension *irr::CIrrDeviceSailfish::qtSurfaceExtension = NULL;

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                        uint32_t capabilities);
static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                       uint32_t format, int fd, uint32_t size);

//static void
//keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
//                      uint32_t serial, struct wl_surface *surface,
//                      struct wl_array *keys);

//static void
//keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
//                      uint32_t serial, struct wl_surface *surface);

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                    uint32_t serial, uint32_t time, uint32_t key,
                    uint32_t state);

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, uint32_t mods_depressed,
                          uint32_t mods_latched, uint32_t mods_locked,
                          uint32_t group);

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface,
                     wl_fixed_t sx, wl_fixed_t sy);

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface);

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
                      uint32_t time, wl_fixed_t sx, wl_fixed_t sy);

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                      uint32_t serial, uint32_t time, uint32_t button,
                      uint32_t state);

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value);
static void
touch_handle_down(void *data, struct wl_touch *wl_touch, uint32_t serial,
                  uint32_t time, struct wl_surface *surface,
                  int32_t id, wl_fixed_t x, wl_fixed_t y);
static void
touch_handle_up(void *data, struct wl_touch *wl_touch,
                uint32_t serial, uint32_t time, int32_t id);
static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
                    uint32_t time, int32_t id,
                    wl_fixed_t x, wl_fixed_t y);
static void
touch_handle_frame(void *data, struct wl_touch *wl_touch);
static void
touch_handle_cancel(void *data, struct wl_touch *wl_touch);

static void
output_handle_done(void *data, struct wl_output* wl_output);
static void
output_handle_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y,
                       int32_t physical_width, int32_t physical_height, int32_t subpixel,
                       const char *make, const char *model, int32_t transform);
static void
output_handle_mode(void *data, struct wl_output *wl_output,
                   uint32_t flags, int32_t width, int32_t height, int32_t refresh);
static void
output_handle_scale(void *data, struct wl_output *wl_output, int32_t factor);

static void
display_handle_error(void *data, struct wl_display *wl_display, void *object_id,
                  uint32_t code, const char *message);

static void
display_handle_delete_id(void *data, struct wl_display *wl_display, uint32_t id);

static void
surface_handle_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output);

static void
surface_handle_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output);

///**
// * onscreen_visibility - (none)
// * @visible: (none)
// */
//static void
//qt_extended_surface_handle_onscreen_visibility(void *data,
//                struct qt_extended_surface *qt_extended_surface,
//                int32_t visible);
///**
// * set_generic_property - (none)
// * @name: (none)
// * @value: (none)
// */
//static void
//qt_extended_surface_handle_set_generic_property(void *data,
//                 struct qt_extended_surface *qt_extended_surface,
//                 const char *name,
//                 struct wl_array *value);
///**
// * close - (none)
// */
//static void
//qt_extended_surface_handle_close(void *data,
//          struct qt_extended_surface *qt_extended_surface);

//struct qt_extended_surface_listener extended_surface_listener = {
//	qt_extended_surface_handle_onscreen_visibility,
//	qt_extended_surface_handle_set_generic_property,
//	qt_extended_surface_handle_close,
//};


struct wl_surface_listener surface_listener = {
	surface_handle_enter,
	surface_handle_leave
};

struct wl_display_listener display_listener = {
	display_handle_error,
	display_handle_delete_id,
};

static const struct wl_seat_listener seat_listener = {
	seat_handle_capabilities,
};

static struct wl_output_listener output_listener = {
	output_handle_geometry,
	output_handle_mode,
	output_handle_done,
	output_handle_scale,
};

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	irr::CIrrDeviceSailfish::keyboard_handle_enter,
	irr::CIrrDeviceSailfish::keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};

static const struct wl_touch_listener touch_listener = {
	touch_handle_down,
	touch_handle_up,
	touch_handle_motion,
	touch_handle_frame,
	touch_handle_cancel,
};

//static void
//hw_handle_client_backend(void *data,
//               struct qt_hardware_integration *qt_hardware_integration,
//               const char *name)
//{
//	irr::core::stringc m;
//	m = "client_backen: ";
//	m += name;
//	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
//}

//static void
//hw_handle_server_backend(void *data,
//               struct qt_hardware_integration *qt_hardware_integration,
//               const char *name)
//{
//	irr::core::stringc m;
//	m = "server_backen: ";
//	m += name;
//	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
//}

//static struct qt_hardware_integration_listener hw_listener = {
//	hw_handle_client_backend,
//	hw_handle_server_backend,
//};


/**
 * touch - (none)
 * @time: (none)
 * @id: (none)
 * @state: (none)
 * @x: (none)
 * @y: (none)
 * @normalized_x: (none)
 * @normalized_y: (none)
 * @width: (none)
 * @height: (none)
 * @pressure: (none)
 * @velocity_x: (none)
 * @velocity_y: (none)
 * @flags: (none)
 * @rawdata: (none)
 */
static void
qt_touch_handle_touch(void *data,
          struct qt_touch_extension *qt_touch_extension,
          uint32_t time,
          uint32_t id,
          uint32_t state,
          int32_t x,
          int32_t y,
          int32_t normalized_x,
          int32_t normalized_y,
          int32_t width,
          int32_t height,
          uint32_t pressure,
          int32_t velocity_x,
          int32_t velocity_y,
          uint32_t flags,
          struct wl_array *rawdata)
{
	irr::core::stringc m;
	m = "client_backend: ";
//	m += name;
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);

//	if(data)
//	{
//		irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
//		irr::SEvent irrevent;
//		irrevent.EventType = irr::EET_TOUCH_INPUT_EVENT;
//		irrevent.TouchInput.ID = id;
//		if(state == 1)
//			irrevent.TouchInput.Event = irr::ETIE_PRESSED_DOWN;
//		else
//			irrevent.TouchInput.Event = irr::ETIE_LEFT_UP;

//		irrevent.TouchInput.X = wl_fixed_to_int(x);
//		irrevent.TouchInput.Y = wl_fixed_to_int(y);
////		QT_TOUCH_EXTENSION_FLAGS_ENUM
//		device->setLastTouchPos(id, irrevent.TouchInput.X, irrevent.TouchInput.Y);
//		device->postEventFromUser(irrevent);
//	}
}

/**
 * configure - (none)
 * @flags: (none)
 */
static void
qt_touch_handle_configure(void *data,
          struct qt_touch_extension *qt_touch_extension,
          uint32_t flags)
{
	irr::core::stringc m;
	m = "client_backen: ";
//	m += name;
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
}

static struct qt_touch_extension_listener qt_touch_listener = {
	qt_touch_handle_touch,
	qt_touch_handle_configure
};

static void
surface_handle_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
	irr::os::Printer::log("surface_handle_enter", irr::ELL_DEBUG);
}

static void
surface_handle_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
	irr::os::Printer::log("surface_handle_leave", irr::ELL_DEBUG);
}

static void
display_handle_error(void *data,
                  struct wl_display *wl_display,
                  void *object_id,
                  uint32_t code,
                  const char *message)
{
	irr::core::stringc m = "display_handle_error() object_id: ";
	m+= (uint32_t)object_id;
	m+= "; code: ";
	m+= code;
	m+= "; message: \"";
	m+= message;
	m+= "\";";
	irr::os::Printer::log(m.c_str(), irr::ELL_ERROR);
}

static void
display_handle_delete_id(void *data,
                      struct wl_display *wl_display,
                      uint32_t id)
{
	irr::core::stringc m = "display_handle_delete_id() id:";
	m+= id;
	m+= ";";
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
}

// https://jan.newmarch.name/Wayland/Input/
static void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
           const char *interface, uint32_t version)
{
	if (strcmp(interface, "wl_compositor") == 0) {
		irr::CIrrDeviceSailfish::wlCompositor = (struct wl_compositor*)wl_registry_bind(registry,
		              id,
		              &wl_compositor_interface,
		              1);
		irr::os::Printer::log("[Good] wl_registry_bind() of \"wl_compositor_interface\" done");
	}
	else if (strcmp(interface, "wl_shell") == 0) {
		irr::CIrrDeviceSailfish::wlShell = (struct wl_shell*)wl_registry_bind(registry, id,
		                         &wl_shell_interface, 1);
		irr::os::Printer::log("[Good] wl_registry_bind() of \"wl_shell_interface\" done");
	}
	else if (strcmp(interface, "wl_seat") == 0) {
		irr::CIrrDeviceSailfish::wlSeat = (wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 1);
		wl_seat_add_listener(irr::CIrrDeviceSailfish::wlSeat, &seat_listener, data);
		irr::os::Printer::log("[Good] wl_registry_bind() of \"wl_seat\" done");
	}
	else if(strcmp(interface, "wl_output") == 0) {
		irr::CIrrDeviceSailfish::wlOutput = (wl_output*)wl_registry_bind(registry, id, &wl_output_interface, 1);
		irr::os::Printer::log("[Good] wl_registry_bind() of \"wl_output\" done");
		wl_output_add_listener(irr::CIrrDeviceSailfish::wlOutput, &output_listener, data);
		irr::os::Printer::log("[Good] wl_output_add_listener() done");
	}
	else if (strcmp(interface, "qt_surface_extension") == 0)
	{
		irr::CIrrDeviceSailfish::qtSurfaceExtension
		        = (struct qt_surface_extension*)wl_registry_bind(registry, id,
		                &qt_surface_extension_interface, 1);
	}
//	else if (strcmp(interface, "qt_touch_extension") == 0)
//	{
//		struct qt_touch_extension *qt_touch =
//		        (struct qt_touch_extension *)wl_registry_bind(registry, id,
//				                                              &qt_touch_extension_interface, 1);
//		qt_touch_extension_add_listener(qt_touch, &qt_touch_listener,data);
//	}
//	else if(strcmp(interface, "qt_hardware_integration") == 0)
//	{
//		//
//		struct qt_hardware_integration *hw_integration = (struct qt_hardware_integration*)wl_registry_bind(registry,id,&qt_hardware_integration_interface, 1);
//		qt_hardware_integration_add_listener(hw_integration, &hw_listener, data);
//	}
	else
	{
		irr::core::stringc m = "Global registry handler interface \"";
		m+=interface;
		m+="\";";
		irr::os::Printer::log(m.c_str());
	}
}

static void
global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
	irr::core::stringc message = "Got a registry losing event for ";
	message += id;
	irr::os::Printer::log(message.c_str());
}

static void
output_handle_done(void *data, struct wl_output* wl_output)
{
#ifdef _DEBUG
	{
		irr::os::Printer::log("wlOutput done;", irr::ELL_DEBUG);
	}
#endif
}

static void
output_handle_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y,
                       int32_t physical_width, int32_t physical_height, int32_t subpixel,
                       const char *make, const char *model, int32_t transform)
{

	{/// TODO Here we need set Up vector for Camera, if it created...
		irr::core::stringc m = "wlOutput::geometry (x:";
		m+= x;
		m+="; y:";
		m+= y;
		m+= "; w:";
		m+= physical_width;
		m+= "; h:";
		m+= physical_height;
		m+= "; sp:";
		m+= subpixel;
		m+= "; transform:";
		switch(transform)
		{
		case  WL_OUTPUT_TRANSFORM_NORMAL:
			m += "normal;";
			break;
		case  WL_OUTPUT_TRANSFORM_90:
			m += "90;";
			break;
		case  WL_OUTPUT_TRANSFORM_180:
			m += "180;";
			break;
		case  WL_OUTPUT_TRANSFORM_270:
			m += "270;";
			break;
		case  WL_OUTPUT_TRANSFORM_FLIPPED:
			m += "flipped;";
			break;
		case WL_OUTPUT_TRANSFORM_FLIPPED_90:
			m += "flipped 90;";
			break;
		case  WL_OUTPUT_TRANSFORM_FLIPPED_180:
			m += "flipped 180;";
			break;
		case  WL_OUTPUT_TRANSFORM_FLIPPED_270:
			m += "flipped 270;";
			break;
		}
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}

	irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
	if(device)
	{
		device->setPhysicalSize((irr::s32)physical_width, (irr::s32)physical_height);
	}
}

static void
output_handle_mode(void *data, struct wl_output *wl_output,
                   uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{

	irr::core::stringc m = "wlOutput::mode (";
	m+= " w:";
	m+= width;
	m+= "; h:";
	m+= height;
	m+= "; refresh:";
	m+= refresh;
	m+= "; flags:";
	m+= flags;
	m+= " - ";

	irr::CIrrDeviceSailfish *dev = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
	/// TODO Here we need set Up vector for activeCamera !!!
	if(dev->qtExtendedSurface)
	switch(flags)
	{
	case  WL_OUTPUT_TRANSFORM_NORMAL:
		m += "normal;";
		qt_extended_surface_set_content_orientation(dev->qtExtendedSurface, QT_EXTENDED_SURFACE_ORIENTATION_PORTRAITORIENTATION );
		        break;
	case  WL_OUTPUT_TRANSFORM_90:
		m += "90;";
		qt_extended_surface_set_content_orientation(dev->qtExtendedSurface, QT_EXTENDED_SURFACE_ORIENTATION_LANDSCAPEORIENTATION );
		break;
	case  WL_OUTPUT_TRANSFORM_180:
		qt_extended_surface_set_content_orientation(dev->qtExtendedSurface, QT_EXTENDED_SURFACE_ORIENTATION_PRIMARYORIENTATION );
		m += "180;";
		break;
	case  WL_OUTPUT_TRANSFORM_270:
		qt_extended_surface_set_content_orientation(dev->qtExtendedSurface, QT_EXTENDED_SURFACE_ORIENTATION_INVERTEDLANDSCAPEORIENTATION );
		m += "270;";
		break;
	case  WL_OUTPUT_TRANSFORM_FLIPPED:
		m += "flipped;";
		break;
	case WL_OUTPUT_TRANSFORM_FLIPPED_90:
		m += "flipped 90;";
		break;
	case  WL_OUTPUT_TRANSFORM_FLIPPED_180:
		m += "flipped 180;";
		break;
	case  WL_OUTPUT_TRANSFORM_FLIPPED_270:
		m += "flipped 270;";
		break;
	}
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);

	dev->setWindowSize( irr::core::dimension2du(width,height) );
	//irr::SEvent event;
	//event.EventType = irr::EET_GYROSCOPE_EVENT;
	//event.GyroscopeEvent
//        dev->setWindowSize( irr::core::dimension2du(480,320) );}
}

static void
output_handle_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "wlOutput::scale (";
		m+= wl_fixed_to_double(factor);
		m+= "; )";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
}

static void
shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
	wl_shell_surface_pong (shell_surface, serial);
}

static void
shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{
	struct window *window  = (struct window *)data;
	wl_egl_window_resize (window->egl_window, width, height, 0, 0);
}

static void
shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface)
{
	int i = 0;
}

struct wl_shell_surface_listener irr::CIrrDeviceSailfish::shell_surface_listener =
{
	shell_surface_ping,
	shell_surface_configure,
	shell_surface_popup_done
};

static void
wl_callback_done(void *data, wl_callback *wl_callback, uint32_t callback_data)
{
	int i = 0;
}

struct wl_callback_listener wlCallbackListener = {
	wl_callback_done,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                         uint32_t capabilities)
{
	if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && !irr::CIrrDeviceSailfish::wlPointer) {
		irr::os::Printer::log("Display has a pointer");
		irr::CIrrDeviceSailfish::wlPointer = wl_seat_get_pointer(seat);
		irr::os::Printer::log("Wayland add pointer listener");
		wl_pointer_add_listener(irr::CIrrDeviceSailfish::wlPointer, &pointer_listener, data);
	} else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && irr::CIrrDeviceSailfish::wlPointer) {
		wl_pointer_destroy(irr::CIrrDeviceSailfish::wlPointer);
		irr::CIrrDeviceSailfish::wlPointer = NULL;
	}

	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		irr::os::Printer::log("Display has a keyboard");
		irr::CIrrDeviceSailfish::wlKeyboard = wl_seat_get_keyboard(seat);
		irr::os::Printer::log("Wayland add keyboard listener");
		wl_keyboard_add_listener(irr::CIrrDeviceSailfish::wlKeyboard, &keyboard_listener, data);
	}
	else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
		irr::os::Printer::log("Display has no a keyboard",irr::ELL_WARNING);
		wl_keyboard_destroy(irr::CIrrDeviceSailfish::wlKeyboard);
		irr::CIrrDeviceSailfish::wlKeyboard = NULL;
	}

	if (capabilities & WL_SEAT_CAPABILITY_TOUCH && !irr::CIrrDeviceSailfish::wlTouch) {
		irr::os::Printer::log("Display has a touch screen");
		irr::CIrrDeviceSailfish::wlTouch = wl_seat_get_touch(seat);
		irr::os::Printer::log("Wayland add touch listener");
		wl_touch_add_listener(irr::CIrrDeviceSailfish::wlTouch, &touch_listener, data);
	}
	else if (capabilities & WL_SEAT_CAPABILITY_TOUCH && irr::CIrrDeviceSailfish::wlTouch) {
		wl_touch_destroy(irr::CIrrDeviceSailfish::wlTouch);
		irr::CIrrDeviceSailfish::wlTouch = NULL;
	}
}

void
irr::CIrrDeviceSailfish::qt_extended_surface_handle_onscreen_visibility(
        void *data,struct qt_extended_surface *qt_extended_surface, int32_t visible)
{
	irr::core::stringc m = "qt_extended_surface_handle_onscreen_visibility";
	irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish *>(data);
	switch(visible)
	{
	case 5: //fullscreen
		m = "Window is focused.";
		device->WindowHasFocus = true;
		device->WindowMinimized = false;
		break;
	case 3: //in pause
		m = "Window is minimized";
		device->WindowHasFocus = false;
		device->WindowMinimized = true;
		break;
	}
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG );
}

void
irr::CIrrDeviceSailfish::qt_extended_surface_handle_set_generic_property(
        void *data, struct qt_extended_surface *qt_extended_surface,
        const char *name, struct wl_array *value)
{
	irr::core::stringc m = "qt_extended_surface_handle_set_generic_property";
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG );
}

void
irr::CIrrDeviceSailfish::qt_extended_surface_handle_close(
        void *data, struct qt_extended_surface *qt_extended_surface)
{
	irr::core::stringc m = "Close IrrlichtDevice. Bye bye!";
	irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
	device->closeDevice();
}

//// Wayland keyboard
static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                       uint32_t format, int fd, uint32_t size)
{
}

void
irr::CIrrDeviceSailfish::keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                      uint32_t serial, struct wl_surface *surface,
                      struct wl_array *keys)
{
	irr::os::Printer::log("Keyboard gained focus", irr::ELL_DEBUG);
	irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
//	device->WindowHasFocus = true;
//	device->WindowMinimized = false;
}

void
irr::CIrrDeviceSailfish::keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                      uint32_t serial, struct wl_surface *surface)
{
	irr::os::Printer::log("Keyboard lost focus", irr::ELL_DEBUG);
	irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
//	device->WindowHasFocus = false;
//	device->WindowMinimized = true;
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                    uint32_t serial, uint32_t time, uint32_t key,
                    uint32_t state)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "Key is ";
		m+= key;
		m+=" state is ";
		m+= state;
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
	{
		irr::SEvent irrevent;
		irrevent.EventType = irr::EET_KEY_INPUT_EVENT;
//		irr::core::stringw str;
		irrevent.KeyInput.Char = device->Key2WChar(key);
		/// TODO get Modifiers status
		///
		irrevent.KeyInput.Shift = false;
		irrevent.KeyInput.Control = false;
		switch(state)
		{
		case 1: //down
			irrevent.KeyInput.Key = device->getKeyCode(key);
			irrevent.KeyInput.SystemKeyCode = key;
			irrevent.KeyInput.PressedDown = true;
			break;
		case 0: //up
			irrevent.KeyInput.Key = device->getKeyCode(key);
			irrevent.KeyInput.SystemKeyCode = key;
			irrevent.KeyInput.PressedDown = false;
			break;
		}

		device->postEventFromUser(irrevent);
	}
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, uint32_t mods_depressed,
                          uint32_t mods_latched, uint32_t mods_locked,
                          uint32_t group)
{

#ifdef _DEBUG
	{
		irr::core::stringc m = "Modifiers depressed ";
		m+= mods_depressed;
		m+=", latched ";
		m+= mods_latched;
		m+=", locked ";
		m+=mods_locked;
		m+=", group ";
		m+=group;
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	/// TODO: read modifiers and add it to Irrlicht
}
/// Wayland pointer
static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface,
                     wl_fixed_t sx, wl_fixed_t sy)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[S:";
		m+= serial;
		m+= "] Pointer entered surface ";
		m+= (unsigned long)surface;
		m+=" at ";
		m+= sx;
		m+=" ";
		m+= sy;
		m+=";";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	/// TODO: create SEvent with mouse
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[S:";
		m+= serial;
		m+= "] Pointer left surface ";
		m+= (unsigned long)surface;
		m+=";";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	/// TODO: create SEvent with mouse
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
                      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[T:";
		m+= time;
		m+= "] Pointer moved at ";
		m+= sx;
		m+= ", ";
		m+= sy;
		m+=";";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	/// TODO: create SEvent with mouse
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                      uint32_t serial, uint32_t time, uint32_t button,
                      uint32_t state)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[S:";
		m+= serial;
		m+=  "][T:";
		m+= time;
		m+= "] Pointer button ";
		m+= button;
		m+=(state == 1)?" is down;":" is up;";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	/// TODO: create SEvent with mouse
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[T:";
		m+= time;
		m+= "] Pointer handle axis ";
		m+= axis;
		m+=" wl_fixed_t [";
		m+=value;
		m+= ";";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	/// TODO: create SEvent with mouse
}

/// Wayland touchscreen
static void
touch_handle_down(void *data, struct wl_touch *wl_touch, uint32_t serial,
                  uint32_t time, struct wl_surface *surface,
                  int32_t id, wl_fixed_t x, wl_fixed_t y)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[T:";
		m+= time;
		m+= "] [S:";
		m+= serial;
		m+= " Touch down on ";
		m+= (unsigned long)surface;
		m+= " with id(";
		m+= id;
		m+= ") at ";
		m+= wl_fixed_to_double(x);
		m+= ", ";
		m+= wl_fixed_to_double(y);
		m+= ";";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	if(data)
	{
		irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
		irr::SEvent irrevent;
		irrevent.EventType = irr::EET_TOUCH_INPUT_EVENT;
		irrevent.TouchInput.ID = id;
		irrevent.TouchInput.Event = irr::ETIE_PRESSED_DOWN;
		irrevent.TouchInput.X = wl_fixed_to_int(x);
		irrevent.TouchInput.Y = wl_fixed_to_int(y);
		device->setLastTouchPos(id, irrevent.TouchInput.X, irrevent.TouchInput.Y);
		device->postEventFromUser(irrevent);
	}
}

static void
touch_handle_up(void *data, struct wl_touch *wl_touch,
                uint32_t serial, uint32_t time, int32_t id)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[T:";
		m+= time;
		m+= "] [S:";
		m+= serial;
		m+= " Touch up with id(";
		m+= id;
		m+= ");";
		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	if(data)
	{
		irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
		irr::SEvent irrevent;
		irrevent.EventType = irr::EET_TOUCH_INPUT_EVENT;
		irrevent.TouchInput.ID = id;
		irrevent.TouchInput.Event = irr::ETIE_LEFT_UP;
		irrevent.TouchInput.X = device->getLastTouchPos(id).X;
		irrevent.TouchInput.Y = device->getLastTouchPos(id).Y;
		device->postEventFromUser(irrevent);
	}
}

static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
                    uint32_t time, int32_t id,
                    wl_fixed_t x, wl_fixed_t y)
{
#ifdef _DEBUG
	{
		irr::core::stringc m = "[T:";
		m+= time;
		m+= "] Touch motion with id(";
		m+= id;
		m+= ") at ";
		m+= wl_fixed_to_double(x);
		m+= ", ";
		m+= wl_fixed_to_double(y);
		m+= ";";
//		irr::os::Printer::log(m.c_str(), irr::ELL_DEBUG);
	}
#endif
	if(data)
	{
		irr::CIrrDeviceSailfish *device = reinterpret_cast<irr::CIrrDeviceSailfish*>(data);
		irr::SEvent irrevent;
		irrevent.EventType = irr::EET_TOUCH_INPUT_EVENT;
		irrevent.TouchInput.ID = id;
		irrevent.TouchInput.Event = irr::ETIE_MOVED;
		irrevent.TouchInput.X = wl_fixed_to_int(x);
		irrevent.TouchInput.Y = wl_fixed_to_int(y);
		device->setLastTouchPos(id, irrevent.TouchInput.X, irrevent.TouchInput.Y);
		device->postEventFromUser(irrevent);
	}
}

static void
touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
#ifdef _DEBUG
//	irr::os::Printer::log("Wayland touch handle frame", irr::ELL_DEBUG);
#endif
}

static void
touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
#ifdef _DEBUG
	irr::os::Printer::log("Wayland touch handle cancel", irr::ELL_DEBUG);
#endif
}

irr::EKEY_CODE irr::CIrrDeviceSailfish::getKeyCode(uint32_t key)
{
	EKEY_CODE keyCode = KEY_NONE;

	SKeyMap mp;
	mp.wlKey = key;
	const s32 idx = KeyMap.binary_search(mp);
	if (idx != -1)
	{
		keyCode = (EKEY_CODE)KeyMap[idx].IrrKey;
	}
	if (keyCode == 0)
	{
		// Any value is better than none, that allows at least using the keys.
		// Worst case is that some keys will be identical, still better than _all_
		// unknown keys being identical.
		if ( !mp.wlKey )
		{
			keyCode = (EKEY_CODE)key;
			os::Printer::log("No such IrrKey, using event keycode", core::stringc(key).c_str(), ELL_INFORMATION);
		}
		else if (idx == -1)
		{
			keyCode = (EKEY_CODE)mp.wlKey;
			os::Printer::log("EKEY_CODE not found, using orig. Wayland keycode", core::stringc(mp.wlKey).c_str(), ELL_INFORMATION);
		}
		else
		{
			keyCode = (EKEY_CODE)mp.wlKey;
			os::Printer::log("EKEY_CODE is 0, using orig. Wayland keycode", core::stringc(mp.wlKey).c_str(), ELL_INFORMATION);
		}
	}
	return keyCode;
}

void irr::CIrrDeviceSailfish::setLastTouchPos(int touchID, irr::s32 X, irr::s32 Y) {
//	if( touchID < 10 && touchID >= 0 )
	{
		m_touchPos[touchID] = core::vector2di(X,Y);
//		m_touchPos.

	}
}

irr::core::vector2di irr::CIrrDeviceSailfish::getLastTouchPos(irr::s32 touchID)
{
//	if( touchID < 10 && touchID >= 0 )
	{
		return m_touchPos[touchID];
	}
}

void irr::CIrrDeviceSailfish::seatHandleCapabilities(void *data, wl_seat *seat, uint32_t capabilities)
{
	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		irr::os::Printer::log("Display has a pointer");
	}

	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		irr::os::Printer::log("Display has a keyboard");
		wlKeyboard = wl_seat_get_keyboard(seat);
		irr::os::Printer::log("Wayland add keyboard listener");
		wl_keyboard_add_listener(wlKeyboard, &keyboard_listener, data);
	}
	else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
		irr::os::Printer::log("Display has no a keyboard",irr::ELL_WARNING);
		wl_keyboard_destroy(wlKeyboard);
		wlKeyboard = NULL;
	}

	if (capabilities & WL_SEAT_CAPABILITY_TOUCH) {
		irr::os::Printer::log("Display has a touch screen");
	}
}

wchar_t irr::CIrrDeviceSailfish::Key2WChar(uint32_t key) const
{
	if( key < 0 || key > 254 )
		return *(L".");
	return Key2Wchar[key];
}

namespace irr
{

//! constructor
CIrrDeviceSailfish::CIrrDeviceSailfish(const SIrrlichtCreationParameters& param)
    : CIrrDeviceStub(param),
      Width(param.WindowSize.Width), Height(param.WindowSize.Height),
      WindowHasFocus(true), WindowMinimized(false),
      ExternalWindow(false), AutorepeatSupport(0),
      wlListener ({
                 global_registry_handler,
                 global_registry_remover
      }),
      PhysicalWidth(0), PhysicalHeight(0)
{
#ifdef _DEBUG
	setDebugName("CIrrDeviceSailfish");
#endif

	// print version, distribution etc.
	// thx to LynxLuna for pointing me to the uname function
	core::stringc linuxversion;
	struct utsname LinuxInfo;
	uname(&LinuxInfo);

	linuxversion += LinuxInfo.sysname;
	linuxversion += " ";
	linuxversion += LinuxInfo.release;
	linuxversion += " ";
	linuxversion += LinuxInfo.version;
	linuxversion += " ";
	linuxversion += LinuxInfo.machine;

	Operator = new COSOperator(linuxversion, this);
	os::Printer::log(linuxversion.c_str(), ELL_INFORMATION);

//	m_touchPos.reallocate(10);
//	m_touchPos.
	// create keymap
	createKeyMap();

	// create window
	if (CreationParams.DriverType != video::EDT_NULL)
	{
		// create the window, only if we do not use the null device
		if (!createWindow())
			return;
	}

	// create cursor control
	CursorControl = new CCursorControl(this, CreationParams.DriverType == video::EDT_NULL);

	// create driver
	createDriver();

	if (!VideoDriver)
		return;

	createGUIAndScene();
}


//! destructor
CIrrDeviceSailfish::~CIrrDeviceSailfish()
{
//	qt_extended_surface_destroy(qtExtendedSurface );
//	wl_shell_surface_destroy(wlShellSurface);
//	wl_surface_destroy(wlSurface);
	if(wlDisplay)
	{
		if ( GUIEnvironment )
		{
			GUIEnvironment->drop();
			GUIEnvironment = NULL;
		}

		if ( SceneManager )
		{
			SceneManager->drop();
			SceneManager = NULL;
		}

		if ( VideoDriver )
		{
			VideoDriver->drop();
			VideoDriver = NULL;
		}

		if (ContextManager)
		{
			ContextManager->destroyContext();
			ContextManager->destroySurface();
		}

		wl_display_disconnect((wl_display*)wlDisplay);
	}

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	for (u32 joystick = 0; joystick < ActiveJoysticks.size(); ++joystick)
	{
		if (ActiveJoysticks[joystick].fd >= 0)
		{
			close(ActiveJoysticks[joystick].fd);
		}
	}
#endif
}

bool CIrrDeviceSailfish::switchToFullscreen(bool reset)
{
	if (!CreationParams.Fullscreen)
		return true;
	if (reset)
	{
		return true;
	}
	getVideoModeList();
	return CreationParams.Fullscreen;
}

bool CIrrDeviceSailfish::createWindow()
{
/* Jolla 1 */
//	int width = 960,
//	    height = 540;
/* Jolla C */
//	int width = CreationParams.WindowSize.Width,
//	    height = CreationParams.WindowSize.Height;

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;
	EGLConfig config;
	EGLint fbAttribs[] =
	{
	    EGL_SURFACE_TYPE,     EGL_WINDOW_BIT,
	    EGL_RENDERABLE_TYPE,  EGL_OPENGL_ES2_BIT,
	    EGL_RED_SIZE,         8,
	    EGL_GREEN_SIZE,       8,
	    EGL_BLUE_SIZE,        8,
	    EGL_DEPTH_SIZE,       8,
//	    EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RGB,
	    EGL_NONE
	};
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

	// first
//	wl_display_list();
	wlDisplay = wl_display_connect(NULL);
	if (wlDisplay == NULL) {
		os::Printer::log("Can't connect to wayland display", ELOG_LEVEL::ELL_ERROR);
		return false;
	}
	else
		os::Printer::log("[Good] wl_display_connect() done");
	wl_display_add_listener(wlDisplay, &display_listener, this);

	wlRegistry = wl_display_get_registry(wlDisplay);
	wl_registry_add_listener(wlRegistry, &wlListener, this);

	// This call the attached listener global_registry_handler
	wl_display_dispatch(wlDisplay);
	wl_display_roundtrip(wlDisplay);

	// If at this point, global_registry_handler didn't set the
	// compositor, nor the shell, bailout !
	if (CIrrDeviceSailfish::wlCompositor == NULL || CIrrDeviceSailfish::wlShell == NULL) {
		os::Printer::log("No wlCompositor and wlShell", ELOG_LEVEL::ELL_ERROR);
		return false;
	}
	else {
		os::Printer::log("[Good] Okay, we got a compositor and a shell... That's something !");
	}
	nativeDisplay = wlDisplay;
        Width = CreationParams.WindowSize.Width*0.5;//wlCompositor->width;
        Height = CreationParams.WindowSize.Height*0.5;//wlCompositor->height;

	// second
	wlSurface = wl_compositor_create_surface(CIrrDeviceSailfish::wlCompositor);
	if (wlSurface == NULL) {
		os::Printer::log("Can't create compositor surface on Wayland", ELOG_LEVEL::ELL_ERROR);
		return false;
	} else {
		os::Printer::log("[Good] Created compositor surface on Wayland");
	}
	wl_surface_add_listener(wlSurface, &surface_listener, this);

	qtExtendedSurface = qt_surface_extension_get_extended_surface(qtSurfaceExtension, wlSurface);
	if(qtExtendedSurface == NULL) {
		os::Printer::log("Can't get qt_extended_surface pointer");
		return false;
	}
	qt_extended_surface_add_listener(qtExtendedSurface, &extended_surface_listener, this );
//	qt_extended_surface_set_content_orientation(qtExtendedSurface, QT_EXTENDED_SURFACE_ORIENTATION_LANDSCAPEORIENTATION );

	wlShellSurface = wl_shell_get_shell_surface(CIrrDeviceSailfish::wlShell, wlSurface);
	if (wlShellSurface == NULL) {
		os::Printer::log("Can't create shell surface", ELOG_LEVEL::ELL_ERROR);
		return false;
	} else {
		os::Printer::log("[Good] Created shell surface");
	}

	wlWindow.shell_surface = wlShellSurface;
	wlWindow.surface = wlSurface;
	wl_shell_surface_add_listener(wlShellSurface, &shell_surface_listener, &wlWindow);
//	wlWindow.egl_context =

//	wl_shell_surface_set_toplevel(wlShellSurface);
        wl_shell_surface_set_fullscreen(wlShellSurface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_SCALE, 5, wlOutput);

//        wl_surface_set_buffer_transform(wlSurface, WL_OUTPUT_TRANSFORM_180 );

	// creating window
	wlRegion = wl_compositor_create_region(CIrrDeviceSailfish::wlCompositor);
	wl_region_add(wlRegion, 0, 0, Width, Height);
	wl_surface_set_opaque_region(wlSurface, wlRegion);
        wl_region_destroy(wlRegion);
	wlEGLWindow = wl_egl_window_create(wlSurface, Width, Height);
//        wl_surface_set_buffer_scale(wlSurface, 1 );
	if (wlEGLWindow == EGL_NO_SURFACE) {
		os::Printer::log("No window !?", ELOG_LEVEL::ELL_ERROR);
		return false;
	}
	else
		os::Printer::log("[Good] Wayland Window created !");

	nativeWindow = (NativeWindowType)wlEGLWindow;
	// create EGL Context
	Display = eglGetDisplay( nativeDisplay );
	if ( Display == EGL_NO_DISPLAY )
	{
		os::Printer::log("No EGL Display...", ELOG_LEVEL::ELL_ERROR);
		return false;
	}
	else
		os::Printer::log("[Good] Got EGLDisplay form nativeDisplay",ELOG_LEVEL::ELL_INFORMATION);

	if ( !eglInitialize(Display, &majorVersion, &minorVersion) )
	{
		os::Printer::log("No Initialisation...", ELOG_LEVEL::ELL_ERROR);
		return false;
	}
	else
	{
		core::stringc message = "[Good] eglInitialize Version: ";
		message += majorVersion;
		message += ".";
		message += minorVersion;
		os::Printer::log(message.c_str());
	}

	// Get configs
	if ( (eglGetConfigs(Display, NULL, 0, &numConfigs) != EGL_TRUE) || (numConfigs == 0))
	{
		os::Printer::log("No configuration...", ELOG_LEVEL::ELL_ERROR);
		return false;
	}

	// Choose config
	if ( (eglChooseConfig(Display, fbAttribs, &config, 1, &numConfigs) != EGL_TRUE) || (numConfigs != 1))
	{
		os::Printer::log("No configuration with Attributes...", ELOG_LEVEL::ELL_ERROR);
		return false;
	}

	// Create a surface
	Surface = eglCreateWindowSurface(Display, config, nativeWindow, NULL);
	if ( Surface == EGL_NO_SURFACE )
	{
		os::Printer::log("No surface...\n");
		return false;
	}

	// Create a GL context
	Context = eglCreateContext(Display, config, EGL_NO_CONTEXT, contextAttribs );
	if ( Context == EGL_NO_CONTEXT )
	{
		os::Printer::log("No context...", ELOG_LEVEL::ELL_ERROR);
		return false;
	}

	// Make the context current
	if ( !eglMakeCurrent(Display, Surface, Surface, Context) )
	{
		os::Printer::log("Could not make the current window current !", ELOG_LEVEL::ELL_ERROR);
		return false;
	}

	wlWindow.egl_window = wlEGLWindow;
	wlWindow.irrDevice = this;

	wlCallback = wl_display_sync(wlDisplay);
	wl_callback_add_listener( wlCallback, &wlCallbackListener, this);
///////////////////////////////////////////////////////// TEST /////////////////////
//	GLint val = 0;
//	core::stringc str;
//	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&val);
//	str = "GL_MAX_TEXTURE_SIZE:";
//	str += val;
//	os::Printer::log(str.c_str());
//	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&val);
//	str = "GL_MAX_TEXTURE_IMAGE_UNITS:";
//	str += val;
//	os::Printer::log(str.c_str());
//	str = "GL_VERSION:";
//	str += (char*)glGetString(GL_VERSION);
//	os::Printer::log(str.c_str());
///////////////////////////////////////////////////////////////////////////////////
	return true;
}


//! create the driver
void CIrrDeviceSailfish::createDriver()
{
	switch(CreationParams.DriverType)
	{
	case video::EDT_OGLES2:
#ifdef _IRR_COMPILE_WITH_OGLES2_
	    {
		    video::SExposedVideoData data;
			data.OGLESWayland.nativeDisplay = nativeDisplay;
			data.OGLESWayland.Window = nativeWindow;
			data.OGLESWayland.Display = Display;
			data.OGLESWayland.Surface = Surface;
			data.OGLESWayland.Context = Context;

			ContextManager = new video::CEGLManager();
			ContextManager->initialize(CreationParams, data);

			VideoDriver = video::createOGLES2Driver(CreationParams, FileSystem, ContextManager);
	    }
#else
		os::Printer::log("No OpenGL-ES2 support compiled in.", ELL_ERROR);
#endif
		break;
	default:
		os::Printer::log("No Wayland support compiled in. Only Null driver available.", ELL_ERROR);
		break;
	}
}

//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceSailfish::run()
{
	os::Timer::tick();

	if(!Close && wlDisplay)
	{
		wl_display_dispatch(wlDisplay);
	}
	else
		irr::os::Printer::log( "Close", irr::ELL_DEBUG);

	if (!Close)
		pollJoysticks();

	return !Close;
}


//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceSailfish::yield()
{
	struct timespec ts = {0,1};
	nanosleep(&ts, NULL);
}


//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceSailfish::sleep(u32 timeMs, bool pauseTimer=false)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);

	if (pauseTimer && !wasStopped)
		Timer->start();
}


//! sets the caption of the window
void CIrrDeviceSailfish::setWindowCaption(const wchar_t* text)
{
//	irr::core::stringw title(text);
	/// TODO wchar_t to char
	irr::core::stringc str = text;
	wl_shell_surface_set_title(wlShellSurface, str.c_str() );
}


//! presents a surface in the client area
bool CIrrDeviceSailfish::present(video::IImage* image, void* windowId, core::rect<s32>* srcRect)
{
	/// TODO some specific wayland ( maybe Qt ) code here
	return true;
}


//! notifies the device that it should close itself
void CIrrDeviceSailfish::closeDevice()
{
	Close = true;
}


//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceSailfish::isWindowActive() const
{
	return (WindowHasFocus && !WindowMinimized);
}


//! returns if window has focus.
bool CIrrDeviceSailfish::isWindowFocused() const
{
	return WindowHasFocus;
}


//! returns if window is minimized.
bool CIrrDeviceSailfish::isWindowMinimized() const
{
	return WindowMinimized;
}


//! returns color format of the window.
video::ECOLOR_FORMAT CIrrDeviceSailfish::getColorFormat() const
{
	/// TODO test here if it 16 bit or 32 bit color in eglConfigs
	return video::ECF_R5G6B5;
}


//! Sets if the window should be resizable in windowed mode.
void CIrrDeviceSailfish::setResizable(bool resize)
{
	/// is it need in sailfish?
}

//! Resize the render window.
void CIrrDeviceSailfish::setWindowSize(const irr::core::dimension2d<u32>& size)
{
	CreationParams.WindowSize = size;
	if(PhysicalHeight != 0 && PhysicalWidth != 0)
	{
#define mm2inch 25,4000508
		f32 dpiW = (f32)CreationParams.WindowSize.Width/(f32)PhysicalWidth;
		f32 dpiH = (f32)CreationParams.WindowSize.Height/(f32)PhysicalHeight;
		dpi = mm2inch*((dpiW > dpiH)?dpiW:dpiH);
	}
}

//! Return pointer to a list with all video modes supported by the gfx adapter.
video::IVideoModeList* CIrrDeviceSailfish::getVideoModeList()
{
	/// TODO maybe need more code here
	VideoModeList->addMode( core::dimension2du(Width, Height), 32 );
	return VideoModeList;
}


//! Minimize window
void CIrrDeviceSailfish::minimizeWindow()
{
	/// no need in Sailfish
}


//! Maximize window
void CIrrDeviceSailfish::maximizeWindow()
{
	/// is it need in sailfish?
	if(wlOutput)
		wl_shell_surface_set_maximized(wlShellSurface, wlOutput);
}


//! Restore original window size
void CIrrDeviceSailfish::restoreWindow()
{
	/// TODO is it relly need in Sailfish
}

core::position2di CIrrDeviceSailfish::getWindowPosition()
{
	int wx = 0, wy = 0;
	/// no need in Sailfish
	return core::position2di(wx, wy);
}

void CIrrDeviceSailfish::createKeyMap()
{
	Key2Wchar[KEY_0] =  *L"0";
	Key2Wchar[KEY_1] = *L"1";
	Key2Wchar[KEY_2] = *L"2";
	Key2Wchar[KEY_3] = *L"3";
	Key2Wchar[KEY_4] = *L"4";
	Key2Wchar[KEY_5] = *L"5";
	Key2Wchar[KEY_6] = *L"6";
	Key2Wchar[KEY_7] = *L"7";
	Key2Wchar[KEY_8] = *L"8";
	Key2Wchar[KEY_9] = *L"9";

	Key2Wchar[KEY_A] = *L"a";
	Key2Wchar[KEY_B] = *L"b";
	Key2Wchar[KEY_C] = *L"c";
	Key2Wchar[KEY_D] = *L"d";
	Key2Wchar[KEY_E] = *L"e";
	Key2Wchar[KEY_F] = *L"f";
	Key2Wchar[KEY_G] = *L"g";
	Key2Wchar[KEY_H] = *L"h";
	Key2Wchar[KEY_I] = *L"i";
	Key2Wchar[KEY_J] = *L"j";
	Key2Wchar[KEY_K] = *L"k";
	Key2Wchar[KEY_L] = *L"l";
	Key2Wchar[KEY_M] = *L"m";
	Key2Wchar[KEY_N] = *L"n";
	Key2Wchar[KEY_O] = *L"o";
	Key2Wchar[KEY_P] = *L"p";
	Key2Wchar[KEY_Q] = *L"q";
	Key2Wchar[KEY_R] = *L"r";
	Key2Wchar[KEY_S] = *L"s";
	Key2Wchar[KEY_T] = *L"t";
	Key2Wchar[KEY_U] = *L"u";
	Key2Wchar[KEY_V] = *L"v";
	Key2Wchar[KEY_W] = *L"w";
	Key2Wchar[KEY_X] = *L"x";
	Key2Wchar[KEY_Y] = *L"y";
	Key2Wchar[KEY_Z] = *L"z";

	Key2Wchar[KEY_SPACE]     = *L" ";
	Key2Wchar[KEY_DOT]       = *L".";
	Key2Wchar[KEY_SLASH]     = *L"/";
	Key2Wchar[KEY_BACKSLASH] = *L"\\";
	Key2Wchar[KEY_QUESTION]  = *L"?";

	KeyMap.clear();
	KeyMap.push_back(SKeyMap(KEY_0, KEY_KEY_0));
	KeyMap.push_back(SKeyMap(KEY_1, KEY_KEY_1));
	KeyMap.push_back(SKeyMap(KEY_2, KEY_KEY_2));
	KeyMap.push_back(SKeyMap(KEY_3, KEY_KEY_3));
	KeyMap.push_back(SKeyMap(KEY_4, KEY_KEY_4));
	KeyMap.push_back(SKeyMap(KEY_5, KEY_KEY_5));
	KeyMap.push_back(SKeyMap(KEY_6, KEY_KEY_6));
	KeyMap.push_back(SKeyMap(KEY_7, KEY_KEY_7));
	KeyMap.push_back(SKeyMap(KEY_8, KEY_KEY_8));
	KeyMap.push_back(SKeyMap(KEY_9, KEY_KEY_9));
//	KeyMap.push_back(SKeyMap(KEY_, KEY_OEM_4));
	KeyMap.push_back(SKeyMap(KEY_BACKSLASH, KEY_OEM_5));
//	KeyMap.push_back(SKeyMap(XK_bracketright, KEY_OEM_6));
//	KeyMap.push_back(SKeyMap(XK_asciicircum, KEY_OEM_5));
//	KeyMap.push_back(SKeyMap(XK_dead_circumflex, KEY_OEM_5));
//	KeyMap.push_back(SKeyMap(XK_degree, 0)); //?
//	KeyMap.push_back(SKeyMap(XK_underscore, KEY_MINUS)); //?
//	KeyMap.push_back(SKeyMap(XK_grave, KEY_OEM_3));
//	KeyMap.push_back(SKeyMap(XK_dead_grave, KEY_OEM_3));
//	KeyMap.push_back(SKeyMap(XK_acute, KEY_OEM_6));
//	KeyMap.push_back(SKeyMap(XK_dead_acute, KEY_OEM_6));
	KeyMap.push_back(SKeyMap(KEY_A, KEY_KEY_A));
	KeyMap.push_back(SKeyMap(KEY_B, KEY_KEY_B));
	KeyMap.push_back(SKeyMap(KEY_C, KEY_KEY_C));
	KeyMap.push_back(SKeyMap(KEY_D, KEY_KEY_D));
	KeyMap.push_back(SKeyMap(KEY_E, KEY_KEY_E));
	KeyMap.push_back(SKeyMap(KEY_F, KEY_KEY_F));
	KeyMap.push_back(SKeyMap(KEY_G, KEY_KEY_G));
	KeyMap.push_back(SKeyMap(KEY_H, KEY_KEY_H));
	KeyMap.push_back(SKeyMap(KEY_I, KEY_KEY_I));
	KeyMap.push_back(SKeyMap(KEY_J, KEY_KEY_J));
	KeyMap.push_back(SKeyMap(KEY_K, KEY_KEY_K));
	KeyMap.push_back(SKeyMap(KEY_L, KEY_KEY_L));
	KeyMap.push_back(SKeyMap(KEY_M, KEY_KEY_M));
	KeyMap.push_back(SKeyMap(KEY_N, KEY_KEY_N));
	KeyMap.push_back(SKeyMap(KEY_O, KEY_KEY_O));
	KeyMap.push_back(SKeyMap(KEY_P, KEY_KEY_P));
	KeyMap.push_back(SKeyMap(KEY_Q, KEY_KEY_Q));
	KeyMap.push_back(SKeyMap(KEY_R, KEY_KEY_R));
	KeyMap.push_back(SKeyMap(KEY_S, KEY_KEY_S));
	KeyMap.push_back(SKeyMap(KEY_T, KEY_KEY_T));
	KeyMap.push_back(SKeyMap(KEY_U, KEY_KEY_U));
	KeyMap.push_back(SKeyMap(KEY_V, KEY_KEY_V));
	KeyMap.push_back(SKeyMap(KEY_W, KEY_KEY_W));
	KeyMap.push_back(SKeyMap(KEY_X, KEY_KEY_X));
	KeyMap.push_back(SKeyMap(KEY_Y, KEY_KEY_Y));
	KeyMap.push_back(SKeyMap(KEY_Z, KEY_KEY_Z));
	KeyMap.push_back(SKeyMap(KEY_F1, EKC_KEY_F1));
	KeyMap.push_back(SKeyMap(KEY_F2, EKC_KEY_F2));
	KeyMap.push_back(SKeyMap(KEY_F3, EKC_KEY_F3));
	KeyMap.push_back(SKeyMap(KEY_F4, EKC_KEY_F4));
	KeyMap.push_back(SKeyMap(KEY_F5, EKC_KEY_F5));
	KeyMap.push_back(SKeyMap(KEY_F6, EKC_KEY_F6));
	KeyMap.push_back(SKeyMap(KEY_F7, EKC_KEY_F7));
	KeyMap.push_back(SKeyMap(KEY_F8, EKC_KEY_F8));
	KeyMap.push_back(SKeyMap(KEY_F9, EKC_KEY_F9));
	KeyMap.push_back(SKeyMap(KEY_F10, EKC_KEY_F10));
	KeyMap.push_back(SKeyMap(KEY_F11, EKC_KEY_F11));
	KeyMap.push_back(SKeyMap(KEY_F12, EKC_KEY_F12));
	KeyMap.push_back(SKeyMap(KEY_ESC, KEY_ESCAPE));
	KeyMap.push_back(SKeyMap(KEY_INSERT, EKC_KEY_INSERT));
	KeyMap.push_back(SKeyMap(KEY_DELETE, EKC_KEY_DELETE));
	KeyMap.push_back(SKeyMap(KEY_HOME, EKC_KEY_HOME));
	KeyMap.push_back(SKeyMap(KEY_LEFT, EKC_KEY_LEFT));
	KeyMap.push_back(SKeyMap(KEY_UP, EKC_KEY_UP));
	KeyMap.push_back(SKeyMap(KEY_RIGHT, EKC_KEY_RIGHT));
	KeyMap.push_back(SKeyMap(KEY_DOWN, EKC_KEY_DOWN));
//	KeyMap.push_back(SKeyMap(KEY_PRIOR, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(KEY_PAGEUP, KEY_PRIOR));
	KeyMap.push_back(SKeyMap(KEY_NEXT, EKC_KEY_NEXT));
	KeyMap.push_back(SKeyMap(KEY_PAGEDOWN, EKC_KEY_NEXT));
	KeyMap.push_back(SKeyMap(KEY_END, EKC_KEY_END));
	KeyMap.push_back(SKeyMap(KEY_HOME, EKC_KEY_HOME));
	KeyMap.push_back(SKeyMap(KEY_NUMLOCK, EKC_KEY_NUMLOCK));
	KeyMap.push_back(SKeyMap(KEY_SPACE, EKC_KEY_SPACE));
	KeyMap.push_back(SKeyMap(KEY_BACKSPACE, EKC_KEY_BACK));
	KeyMap.sort();
}

bool CIrrDeviceSailfish::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
{
#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)

	joystickInfo.clear();

	u32 joystick;
	for (joystick = 0; joystick < 32; ++joystick)
	{
		// The joystick device could be here...
		core::stringc devName = "/dev/js";
		devName += joystick;

		SJoystickInfo returnInfo;
		JoystickInfo info;

		info.fd = open(devName.c_str(), O_RDONLY);
		if (-1 == info.fd)
		{
			// ...but Ubuntu and possibly other distros
			// create the devices in /dev/input
			devName = "/dev/input/js";
			devName += joystick;
			info.fd = open(devName.c_str(), O_RDONLY);
			if (-1 == info.fd)
			{
				// and BSD here
				devName = "/dev/joy";
				devName += joystick;
				info.fd = open(devName.c_str(), O_RDONLY);
			}
		}

		if (-1 == info.fd)
			continue;

#ifdef __FreeBSD__
		info.axes=2;
		info.buttons=2;
#else
		ioctl( info.fd, JSIOCGAXES, &(info.axes) );
		ioctl( info.fd, JSIOCGBUTTONS, &(info.buttons) );
		fcntl( info.fd, F_SETFL, O_NONBLOCK );
#endif

		(void)memset(&info.persistentData, 0, sizeof(info.persistentData));
		info.persistentData.EventType = irr::EET_JOYSTICK_INPUT_EVENT;
		info.persistentData.JoystickEvent.Joystick = ActiveJoysticks.size();

		// There's no obvious way to determine which (if any) axes represent a POV
		// hat, so we'll just set it to "not used" and forget about it.
		info.persistentData.JoystickEvent.POV = 65535;

		ActiveJoysticks.push_back(info);

		returnInfo.Joystick = joystick;
		returnInfo.PovHat = SJoystickInfo::POV_HAT_UNKNOWN;
		returnInfo.Axes = info.axes;
		returnInfo.Buttons = info.buttons;

#ifndef __FreeBSD__
		char name[80];
		ioctl( info.fd, JSIOCGNAME(80), name);
		returnInfo.Name = name;
#endif

		joystickInfo.push_back(returnInfo);
	}

	for (joystick = 0; joystick < joystickInfo.size(); ++joystick)
	{
		char logString[256];
		(void)sprintf(logString, "Found joystick %u, %u axes, %u buttons '%s'",
			joystick, joystickInfo[joystick].Axes,
			joystickInfo[joystick].Buttons, joystickInfo[joystick].Name.c_str());
		os::Printer::log(logString, ELL_INFORMATION);
	}

	return true;
#else
	return false;
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
}


void CIrrDeviceSailfish::pollJoysticks()
{
#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	if (0 == ActiveJoysticks.size())
		return;

	for (u32 j= 0; j< ActiveJoysticks.size(); ++j)
	{
		JoystickInfo & info =  ActiveJoysticks[j];

		struct js_event event;
		while (sizeof(event) == read(info.fd, &event, sizeof(event)))
		{
			switch(event.type & ~JS_EVENT_INIT)
			{
			case JS_EVENT_BUTTON:
				if (event.value)
						info.persistentData.JoystickEvent.ButtonStates |= (1 << event.number);
				else
						info.persistentData.JoystickEvent.ButtonStates &= ~(1 << event.number);
				break;

			case JS_EVENT_AXIS:
				if (event.number < SEvent::SJoystickEvent::NUMBER_OF_AXES)
					info.persistentData.JoystickEvent.Axis[event.number] = event.value;
				break;

			default:
				break;
			}
		}

		// Send an irrlicht joystick event once per ::run() even if no new data were received.
		(void)postEventFromUser(info.persistentData);
	}
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
}


//! Set the current Gamma Value for the Display
bool CIrrDeviceSailfish::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	/// TODO set gamma ramp settings if it posible
	return false;
}


//! Get the current Gamma Value for the Display
bool CIrrDeviceSailfish::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	brightness = 0.f;
	contrast = 0.f;
	/// TODO get gamma ramp settings if it posible
	return false;
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const c8* CIrrDeviceSailfish::getTextFromClipboard() const
{
	/// TODO Get data from clipboard
	return 0;
}

//! copies text to the clipboard
void CIrrDeviceSailfish::copyToClipboard(const c8* text) const
{
	/// TODO put data clipboard Wayland
}

//! Remove all messages pending in the system message loop
void CIrrDeviceSailfish::clearSystemMessages()
{
	/// TODO ?
}

CIrrDeviceSailfish::CCursorControl::CCursorControl(CIrrDeviceSailfish* dev, bool null)
	: Device(dev)
	, IsVisible(true), Null(null), UseReferenceRect(false)
	, ActiveIcon(gui::ECI_NORMAL), ActiveIconStartTime(0)
{
	/// TODO
}

CIrrDeviceSailfish::CCursorControl::~CCursorControl()
{
	// Do not clearCursors here as the display is already closed
	// TODO (cutealien): droping cursorcontrol earlier might work, not sure about reason why that's done in stub currently.
}

//! Sets the active cursor icon
void CIrrDeviceSailfish::CCursorControl::setActiveIcon(gui::ECURSOR_ICON iconId)
{
	/// TODO
}


//! Add a custom sprite as cursor icon.
gui::ECURSOR_ICON CIrrDeviceSailfish::CCursorControl::addIcon(const gui::SCursorSprite& icon)
{
	/// TODO
	return gui::ECI_NORMAL;
}

//! replace the given cursor icon.
void CIrrDeviceSailfish::CCursorControl::changeIcon(gui::ECURSOR_ICON iconId, const gui::SCursorSprite& icon)
{
	/// TODO
}

irr::core::dimension2di CIrrDeviceSailfish::CCursorControl::getSupportedIconSize() const
{
	// this returns the closest match that is smaller or same size, so we just pass a value which should be large enough for cursors
	unsigned int width=64, height=64;
	return core::dimension2di(width, height);
}

} // end namespace

#endif // SAILFISH


