// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_LINUX_H_INCLUDED__
#define __C_IRR_DEVICE_LINUX_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_X11_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include "ICursorControl.h"
#include "os.h"

#ifdef _IRR_COMPILE_WITH_X11_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#ifdef _IRR_LINUX_X11_VIDMODE_
#include <X11/extensions/xf86vmode.h>
#endif
#ifdef _IRR_LINUX_X11_RANDR_
#include <X11/extensions/Xrandr.h>
#endif
#include <X11/keysym.h>

#elif defined(SAILFISH)
#define KeySym uint32_t
#else
#define KeySym s32
#endif

#ifdef SAILFISH
#	include <wayland-client.h>
#	include <wayland-client-protocol.h>
#	include <wayland-egl.h>
#	include <EGL/egl.h>
#   include <QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-surface-extension-client-protocol.h>
#endif

namespace irr
{

	class CIrrDeviceLinux : public CIrrDeviceStub, public video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceLinux(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceLinux();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run() _IRR_OVERRIDE_;

		//! Cause the device to temporarily pause execution and let other processes to run
		// This should bring down processor usage without major performance loss for Irrlicht
		virtual void yield() _IRR_OVERRIDE_;

		//! Pause execution and let other processes to run for a specified amount of time.
		virtual void sleep(u32 timeMs, bool pauseTimer) _IRR_OVERRIDE_;

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text) _IRR_OVERRIDE_;

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const _IRR_OVERRIDE_;

		//! returns if window has focus.
		virtual bool isWindowFocused() const _IRR_OVERRIDE_;

		//! returns if window is minimized.
		virtual bool isWindowMinimized() const _IRR_OVERRIDE_;

		//! returns color format of the window.
		virtual video::ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void* windowId=0, core::rect<s32>* src=0 ) _IRR_OVERRIDE_;

		//! notifies the device that it should close itself
		virtual void closeDevice() _IRR_OVERRIDE_;

		//! \return Returns a pointer to a list with all video modes
		//! supported by the gfx adapter.
		virtual video::IVideoModeList* getVideoModeList() _IRR_OVERRIDE_;

		//! Sets if the window should be resizable in windowed mode.
		virtual void setResizable(bool resize=false) _IRR_OVERRIDE_;

		//! Resize the render window.
		virtual void setWindowSize(const irr::core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! Minimizes the window.
		virtual void minimizeWindow() _IRR_OVERRIDE_;

		//! Maximizes the window.
		virtual void maximizeWindow() _IRR_OVERRIDE_;

		//! Restores the window size.
		virtual void restoreWindow() _IRR_OVERRIDE_;

		//! Get the position of this window on screen
		virtual core::position2di getWindowPosition() _IRR_OVERRIDE_;

		//! Activate any joysticks, and generate events for them.
		virtual bool activateJoysticks(core::array<SJoystickInfo> & joystickInfo) _IRR_OVERRIDE_;

		//! Set the current Gamma Value for the Display
		virtual bool setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast ) _IRR_OVERRIDE_;

		//! Get the current Gamma Value for the Display
		virtual bool getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast ) _IRR_OVERRIDE_;

		//! gets text from the clipboard
		//! \return Returns 0 if no string is in there.
		virtual const c8* getTextFromClipboard() const;

		//! copies text to the clipboard
		//! This sets the clipboard selection and _not_ the primary selection which you have on X on the middle mouse button.
		virtual void copyToClipboard(const c8* text) const;

		//! Remove all messages pending in the system message loop
		virtual void clearSystemMessages() _IRR_OVERRIDE_;

		//! Get the device type
		virtual E_DEVICE_TYPE getType() const _IRR_OVERRIDE_
		{
				return EIDT_X11;
		}

#ifdef _IRR_COMPILE_WITH_X11_
		// convert an Irrlicht texture to a X11 cursor
		Cursor TextureToCursor(irr::video::ITexture * tex, const core::rect<s32>& sourceRect, const core::position2d<s32> &hotspot);
		Cursor TextureToMonochromeCursor(irr::video::ITexture * tex, const core::rect<s32>& sourceRect, const core::position2d<s32> &hotspot);
#ifdef _IRR_LINUX_XCURSOR_
		Cursor TextureToARGBCursor(irr::video::ITexture * tex, const core::rect<s32>& sourceRect, const core::position2d<s32> &hotspot);
#endif
#endif

	private:

		//! create the driver
		void createDriver();

		bool createWindow();

		void createKeyMap();

		void pollJoysticks();

		void initXAtoms();

		bool switchToFullscreen(bool reset=false);

#ifdef _IRR_COMPILE_WITH_X11_
		bool createInputContext();
		void destroyInputContext();
		EKEY_CODE getKeyCode(XEvent &event);
#endif

		//! Implementation of the linux cursor control
		class CCursorControl : public gui::ICursorControl
		{
		public:

			CCursorControl(CIrrDeviceLinux* dev, bool null);

			~CCursorControl();

			//! Changes the visible state of the mouse cursor.
			virtual void setVisible(bool visible) _IRR_OVERRIDE_
			{
				if (visible==IsVisible)
					return;
				IsVisible = visible;
#ifdef _IRR_COMPILE_WITH_X11_
				if (!Null)
				{
					if ( !IsVisible )
						XDefineCursor( Device->XDisplay, Device->XWindow, InvisCursor );
					else
						XUndefineCursor( Device->XDisplay, Device->XWindow );
				}
#endif
			}

			//! Returns if the cursor is currently visible.
			virtual bool isVisible() const _IRR_OVERRIDE_
			{
				return IsVisible;
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<f32> &pos) _IRR_OVERRIDE_
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(f32 x, f32 y) _IRR_OVERRIDE_
			{
				setPosition((s32)(x*Device->Width), (s32)(y*Device->Height));
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<s32> &pos) _IRR_OVERRIDE_
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(s32 x, s32 y) _IRR_OVERRIDE_
			{
#ifdef _IRR_COMPILE_WITH_X11_

				if (!Null)
				{
					if (UseReferenceRect)
					{
						XWarpPointer(Device->XDisplay,
							None,
							Device->XWindow, 0, 0,
							Device->Width,
							Device->Height,
							ReferenceRect.UpperLeftCorner.X + x,
							ReferenceRect.UpperLeftCorner.Y + y);

					}
					else
					{
						XWarpPointer(Device->XDisplay,
							None,
							Device->XWindow, 0, 0,
							Device->Width,
							Device->Height, x, y);
					}
					XFlush(Device->XDisplay);
				}
#endif
				CursorPos.X = x;
				CursorPos.Y = y;
			}

			//! Returns the current position of the mouse cursor.
			virtual const core::position2d<s32>& getPosition() _IRR_OVERRIDE_
			{
				updateCursorPos();
				return CursorPos;
			}

			//! Returns the current position of the mouse cursor.
			virtual core::position2d<f32> getRelativePosition() _IRR_OVERRIDE_
			{
				updateCursorPos();

				if (!UseReferenceRect)
				{
					return core::position2d<f32>(CursorPos.X / (f32)Device->Width,
						CursorPos.Y / (f32)Device->Height);
				}

				return core::position2d<f32>(CursorPos.X / (f32)ReferenceRect.getWidth(),
						CursorPos.Y / (f32)ReferenceRect.getHeight());
			}

			virtual void setReferenceRect(core::rect<s32>* rect=0) _IRR_OVERRIDE_
			{
				if (rect)
				{
					ReferenceRect = *rect;
					UseReferenceRect = true;

					// prevent division through zero and uneven sizes

					if (!ReferenceRect.getHeight() || ReferenceRect.getHeight()%2)
						ReferenceRect.LowerRightCorner.Y += 1;

					if (!ReferenceRect.getWidth() || ReferenceRect.getWidth()%2)
						ReferenceRect.LowerRightCorner.X += 1;
				}
				else
					UseReferenceRect = false;
			}

			//! Sets the active cursor icon
			virtual void setActiveIcon(gui::ECURSOR_ICON iconId) _IRR_OVERRIDE_;

			//! Gets the currently active icon
			virtual gui::ECURSOR_ICON getActiveIcon() const _IRR_OVERRIDE_
			{
				return ActiveIcon;
			}

			//! Add a custom sprite as cursor icon.
			virtual gui::ECURSOR_ICON addIcon(const gui::SCursorSprite& icon) _IRR_OVERRIDE_;

			//! replace the given cursor icon.
			virtual void changeIcon(gui::ECURSOR_ICON iconId, const gui::SCursorSprite& icon) _IRR_OVERRIDE_;

			//! Return a system-specific size which is supported for cursors. Larger icons will fail, smaller icons might work.
			virtual core::dimension2di getSupportedIconSize() const _IRR_OVERRIDE_;

#ifdef _IRR_COMPILE_WITH_X11_
			//! Set platform specific behavior flags.
			virtual void setPlatformBehavior(gui::ECURSOR_PLATFORM_BEHAVIOR behavior) _IRR_OVERRIDE_ {PlatformBehavior = behavior; }

			//! Return platform specific behavior.
			virtual gui::ECURSOR_PLATFORM_BEHAVIOR getPlatformBehavior() const _IRR_OVERRIDE_ { return PlatformBehavior; }

			void update();
			void clearCursors();
#endif
		private:

			void updateCursorPos()
			{
#ifdef _IRR_COMPILE_WITH_X11_
				if (Null)
					return;

				if ( PlatformBehavior&gui::ECPB_X11_CACHE_UPDATES && !os::Timer::isStopped() )
				{
					u32 now = os::Timer::getTime();
					if (now <= LastQuery)
						return;
					LastQuery = now;
				}

				Window tmp;
				int itmp1, itmp2;
				unsigned  int maskreturn;
				XQueryPointer(Device->XDisplay, Device->XWindow,
					&tmp, &tmp,
					&itmp1, &itmp2,
					&CursorPos.X, &CursorPos.Y, &maskreturn);

				if (CursorPos.X < 0)
					CursorPos.X = 0;
				if (CursorPos.X > (s32) Device->Width)
					CursorPos.X = Device->Width;
				if (CursorPos.Y < 0)
					CursorPos.Y = 0;
				if (CursorPos.Y > (s32) Device->Height)
					CursorPos.Y = Device->Height;
#endif
			}

			CIrrDeviceLinux* Device;
			core::position2d<s32> CursorPos;
			core::rect<s32> ReferenceRect;
#ifdef _IRR_COMPILE_WITH_X11_
			gui::ECURSOR_PLATFORM_BEHAVIOR PlatformBehavior;
			u32 LastQuery;
			Cursor InvisCursor;

			struct CursorFrameX11
			{
				CursorFrameX11() : IconHW(0) {}
				CursorFrameX11(Cursor icon) : IconHW(icon) {}

				Cursor IconHW;	// hardware cursor
			};

			struct CursorX11
			{
				CursorX11() {}
				explicit CursorX11(Cursor iconHw, u32 frameTime=0) : FrameTime(frameTime)
				{
					Frames.push_back( CursorFrameX11(iconHw) );
				}
				core::array<CursorFrameX11> Frames;
				u32 FrameTime;
			};

			core::array<CursorX11> Cursors;

			void initCursors();
#endif
			bool IsVisible;
			bool Null;
			bool UseReferenceRect;
			gui::ECURSOR_ICON ActiveIcon;
			u32 ActiveIconStartTime;
		};

		friend class CCursorControl;

#ifdef _IRR_COMPILE_WITH_X11_
		friend class COpenGLDriver;

		Display *XDisplay;
		XVisualInfo* VisualInfo;
		int Screennr;
		Window XWindow;
		XSetWindowAttributes WndAttributes;
		XSizeHints* StdHints;
		XImage* SoftwareImage;
		XIM XInputMethod;
		XIC XInputContext;
		bool HasNetWM;
		mutable core::stringc Clipboard;
		#ifdef _IRR_LINUX_X11_VIDMODE_
		XF86VidModeModeInfo OldVideoMode;
		#endif
		#ifdef _IRR_LINUX_X11_RANDR_
		SizeID OldRandrMode;
		Rotation OldRandrRotation;
		#endif
#elif defined(SAILFISH)
	public:
		const  struct wl_registry_listener wlListener;
		static struct wl_compositor *wlCompositor;
		static struct wl_shell *wlShell;
		static struct wl_shell_surface_listener shell_surface_listener;
		static struct wl_seat  *wlSeat; // Group of Wayland input devices (mice, keyboard and touch)
		static struct wl_keyboard *wlKeyboard;
		static struct wl_touch *wlTouch;
		static struct wl_pointer *wlPointer;
		static struct wl_output *wlOutput;
		static struct qt_surface_extension *qtSurfaceExtension;

		static void
		keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
		                      uint32_t serial, struct wl_surface *surface,
		                      struct wl_array *keys);

		static void
		keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
		                      uint32_t serial, struct wl_surface *surface);

		/**
		 * onscreen_visibility - (none)
		 * @visible: (none)
		 */
		static void
		qt_extended_surface_handle_onscreen_visibility(void *data,
		                struct qt_extended_surface *qt_extended_surface,
		                int32_t visible);
		/**
		 * set_generic_property - (none)
		 * @name: (none)
		 * @value: (none)
		 */
		static void
		qt_extended_surface_handle_set_generic_property(void *data,
		                 struct qt_extended_surface *qt_extended_surface,
		                 const char *name,
		                 struct wl_array *value);
		/**
		 * close - (none)
		 */
		static void
		qt_extended_surface_handle_close(void *data,
		          struct qt_extended_surface *qt_extended_surface);

		struct qt_extended_surface_listener extended_surface_listener = {
			qt_extended_surface_handle_onscreen_visibility,
			qt_extended_surface_handle_set_generic_property,
			qt_extended_surface_handle_close,
		};

		EKEY_CODE getKeyCode(uint32_t key);
	protected: //Wayland callbacks
		void seatHandleCapabilities(void *data, struct wl_seat *seat,
		                        uint32_t capabilities);
	private:
		struct wl_surface *wlSurface;
		struct qt_extended_surface *qtExtendedSurface;
		struct wl_egl_window *wlEGLWindow;
		struct wl_region *wlRegion;
		struct wl_shell_surface *wlShellSurface;
		struct wl_display * wlDisplay;
		struct wl_registry *wlRegistry;
		struct wl_callback *wlCallback;
//		struct qt_extended_surface *qtExt

		/// Native System informations
		NativeDisplayType nativeDisplay;
		NativeWindowType nativeWindow;
		/// EGL components
		EGLDisplay  Display;/// EGL display
		EGLContext  Context;/// EGL context
		EGLSurface  Surface;/// EGL surface
		s32  PhysicalWidth, PhysicalHeight;
		u32  dpi;
	public:
		void setPhysicalSize(s32  width, s32 height)
		{
			PhysicalWidth = width;
			PhysicalHeight = height;
		}

	private:
#endif
		u32  Width, Height;
		bool WindowHasFocus;
		bool WindowMinimized;
		bool UseXVidMode;
		bool UseXRandR;
		bool ExternalWindow;
		int  AutorepeatSupport;

		struct SKeyMap
		{
			SKeyMap() {}
#ifndef SAILFISH
			SKeyMap(s32 x11, s32 win32)
				: X11Key(x11), Win32Key(win32)
			{
			}

			KeySym X11Key;
			s32 Win32Key;

			bool operator<(const SKeyMap& o) const
			{
				return X11Key<o.X11Key;
			}
#else
			SKeyMap(KeySym key, s32 irrKey)
			    : wlKey(key), IrrKey(irrKey)
			{
			}

			KeySym wlKey;
			s32 IrrKey;

			bool operator<(const SKeyMap& o) const
			{
				return wlKey<o.wlKey;
			}
#endif
		};

		core::array<SKeyMap> KeyMap;

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
		struct JoystickInfo
		{
			int	fd;
			int	axes;
			int	buttons;

			SEvent persistentData;

			JoystickInfo() : fd(-1), axes(0), buttons(0) { }
		};
		core::array<JoystickInfo> ActiveJoysticks;
#endif
	};


} // end namespace irr

#endif // _IRR_COMPILE_WITH_X11_DEVICE_
#endif // __C_IRR_DEVICE_LINUX_H_INCLUDED__

