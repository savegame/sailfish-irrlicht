#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <qt5/QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-windowmanager-client-protocol.h>
#include <qt5/QtWaylandClient/5.4.0/QtWaylandClient/private/wayland-hardware-integration-client-protocol.h>
#include <qt5/QtWaylandClient/5.4.0/QtWaylandClient/private/qwayland-hardware-integration.h>

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_surface *surface;
struct wl_egl_window *egl_window;
struct wl_region *region;
struct wl_callback *callback;
struct wl_shell *shell;
struct wl_shell_surface *shell_surface;
struct qt_windowmanager *windowmanager;
struct qt_hardware_integration *hardware;

// input devices
struct wl_seat *seat;
struct wl_pointer *pointer;

static void qt_handle_hints(void *data,
                            struct qt_windowmanager *qt_windowmanager,
                            int32_t show_is_fullscreen);
static void qt_handle_quit(void *data,
                           struct qt_windowmanager *qt_windowmanager);

static void hardware_client_backend(void *data,
                   struct qt_hardware_integration *qt_hardware_integration,
                   const char *name);

static void hardware_server_backend(void *data,
                   struct qt_hardware_integration *qt_hardware_integration,
                   const char *name);

struct qt_hardware_integration_listener hardware_listener = {
	hardware_client_backend,
	hardware_server_backend
};

EGLDisplay egl_display;
EGLConfig egl_conf;
EGLSurface egl_surface;
EGLContext egl_context;

static void
qt_handle_hints(void *data, struct qt_windowmanager *object, int32_t show_is_fullscreen)
{
	fprintf(stderr, "qt_handle_hints: show_is_fullscreen=%d\n",show_is_fullscreen);
}

static void
qt_handle_quit(void *data, struct qt_windowmanager *object)
{
	fprintf(stderr, "qt_handle_quit:\n");
}


static void hardware_client_backend(void *data,
                   struct qt_hardware_integration *qt_hardware_integration,
                   const char *name)
{
	fprintf(stderr, "hardware_client_backend for \"%s\"\n", name );
}

static void hardware_server_backend(void *data,
                   struct qt_hardware_integration *qt_hardware_integration,
                   const char *name)
{
	fprintf(stderr, "hardware_server_backend for \"%s\"\n", name );
}

static struct qt_windowmanager_listener windowmanager_listener =
{
	qt_handle_hints,
	qt_handle_quit
};

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface,
                     wl_fixed_t sx, wl_fixed_t sy)
{
	fprintf(stderr, "Pointer entered surface %p at %d %d\n", surface, sx, sy);
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface)
{
	fprintf(stderr, "Pointer left surface %p\n", surface);
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
                      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
	fprintf(stderr,"Pointer moved at %d %d\n", sx, sy);
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                      uint32_t serial, uint32_t time, uint32_t button,
                      uint32_t state)
{
	fprintf(stderr,"Pointer button\n");
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value)
{
	fprintf(stderr,"Pointer handle axis\n");
}

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                         uint32_t caps)
{
	if ((caps & WL_SEAT_CAPABILITY_POINTER) && !pointer) {
		pointer = (wl_pointer*)wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, NULL);
	} else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && pointer) {
		wl_pointer_destroy(pointer);
		pointer = NULL;
	}
}

static const struct wl_seat_listener seat_listener = {
	seat_handle_capabilities,
};

void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                        const char *interface, uint32_t version)
{
	fprintf(stderr,"Got a registry event for %s id %d\n", interface, id);
	if (strcmp(interface, "wl_compositor") == 0) {
		compositor = (wl_compositor*)wl_registry_bind(registry,
		                              id,
		                              &wl_compositor_interface,
		                              1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		shell = (wl_shell*)wl_registry_bind(registry, id,
		                         &wl_shell_interface, 1);

	} else if (strcmp(interface, "wl_seat") == 0) {
		seat = (wl_seat*)wl_registry_bind(registry, id,
		                        &wl_seat_interface, 1);
		wl_seat_add_listener(seat, &seat_listener, NULL);
	}
	else if ( strcmp(interface, "qt_windowmanger") == 0)
	{
		windowmanager = (qt_windowmanager*)wl_registry_bind(registry, id, &qt_windowmanager_interface, 1);
		qt_windowmanager_add_listener(windowmanager, &windowmanager_listener, NULL);
		fprintf(stderr, "Add %s_listener;\n",interface);
		qt_windowmanager_open_url(windowmanager, 1, "https://sea.sashikknox.ga");
	}
	else if ( strcmp(interface, "qt_hardware_integration") == 0)
	{
		hardware = (qt_hardware_integration*)wl_registry_bind(registry, id, &qt_hardware_integration_interface, 1);
		qt_hardware_integration_add_listener(hardware, &hardware_listener, NULL);
		fprintf(stderr, "Add %s_listener;\n",interface);
	}
//	else // unused regustry handlers
//	{
//		fprintf(stderr,"Got registry handle: %s\n", interface);
//	}
}

static void
global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
	fprintf(stderr,"Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
	global_registry_handler,
	global_registry_remover
};


static void
redraw(void *data, struct wl_callback *callback, uint32_t time) {
	fprintf(stderr,"Redrawing\n");
}

static const struct wl_callback_listener frame_listener = {
	redraw
};

static void
configure_callback(void *data, struct wl_callback *callback, uint32_t  time)
{
	if (callback == NULL)
		redraw(data, NULL, time);
}

static struct wl_callback_listener configure_callback_listener = {
	configure_callback,
};

static void
create_window() {
	egl_window = wl_egl_window_create(surface,
	                                  480, 360);
	if (egl_window == EGL_NO_SURFACE) {
		fprintf(stderr, "Can't create egl window\n");
		exit(1);
	} else {
		fprintf(stderr, "Created egl window\n");
	}

	egl_surface =
	        eglCreateWindowSurface(egl_display,
	                               egl_conf,
	                               egl_window, NULL);

	if (eglMakeCurrent(egl_display, egl_surface,
	                   egl_surface, egl_context)) {
		fprintf(stderr, "Made current\n");
	} else {
		fprintf(stderr, "Made current failed\n");
	}

	glClearColor(1.0, 1.0, 0.0, 0.1);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();


	if (eglSwapBuffers(egl_display, egl_surface)) {
		fprintf(stderr, "Swapped buffers\n");
	} else {
		fprintf(stderr, "Swapped buffers failed\n");
	}
	wl_display_dispatch(display);
	wl_display_roundtrip(display);
}


static void
handle_ping(void *data, struct wl_shell_surface *shell_surface,
            uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
	fprintf(stderr, "Pinged and ponged\n");
}

static void
handle_configure(void *data, struct wl_shell_surface *shell_surface,
                 uint32_t edges, int32_t width, int32_t height)
{
}

static void
handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};


static void
init_egl() {
	EGLint major, minor, count, n, size;
	EGLConfig* configs;
	EGLBoolean ret;
	int i;
	EGLint config_attribs[] = {
	    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_DEPTH_SIZE, 24,
	    EGL_STENCIL_SIZE, 8,
	    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	    EGL_NONE
	};

	static const EGLint context_attribs[] = {
	    EGL_CONTEXT_CLIENT_VERSION, 2,
	    EGL_NONE
	};


	egl_display = eglGetDisplay((EGLNativeDisplayType) display);
	if (egl_display == EGL_NO_DISPLAY) {
		fprintf(stderr, "Can't create egl display\n");
		exit(1);
	} else {
		fprintf(stderr, "Created egl display\n");
	}

	if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
		fprintf(stderr, "Can't initialise egl display\n");
		exit(1);
	}
	printf("EGL major: %d, minor %d\n", major, minor);

	if (! eglBindAPI(EGL_OPENGL_ES_API)) {
		fprintf(stderr, "Can't bind API\n");
		exit(1);
	} else {
		fprintf(stderr, "Bound API\n");
	}

	eglGetConfigs(egl_display, NULL, 0, &count);
	printf("EGL has %d configs\n", count);

	//*configs = malloc(sizeof(EGLConfig)*count);
//	configs.resize(count);
//	EGLConfig configs[count];
	configs = new EGLConfig[count];

	ret = eglChooseConfig(egl_display, config_attribs,
	                      configs, count, &n);

	for (i = 0; i < n; i++) {
		fprintf(stderr,"----------------------------\nconfig[%d] is :\n", i);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_BUFFER_SIZE, &size);
		fprintf(stderr,"EGL_BUFFER_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_ALPHA_SIZE, &size);
		fprintf(stderr,"EGL_ALPHA_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_BLUE_SIZE, &size);
		fprintf(stderr,"EGL_BLUE_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_GREEN_SIZE, &size);
		fprintf(stderr,"EGL_GREEN_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_RED_SIZE, &size);
		fprintf(stderr,"EGL_RED_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_DEPTH_SIZE, &size);
		fprintf(stderr,"EGL_DEPTH_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_STENCIL_SIZE, &size);
		fprintf(stderr,"EGL_STENCIL_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_CONFIG_CAVEAT, &size);
		fprintf(stderr,"EGL_CONFIG_CAVEAT = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_CONFIG_ID, &size);
		fprintf(stderr,"EGL_CONFIG_ID = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_LEVEL, &size);
		fprintf(stderr,"EGL_LEVEL = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_MAX_PBUFFER_HEIGHT, &size);
		fprintf(stderr,"EGL_MAX_PBUFFER_HEIGHT = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_MAX_PBUFFER_PIXELS, &size);
		fprintf(stderr,"EGL_MAX_PBUFFER_PIXELS = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_MAX_PBUFFER_WIDTH, &size);
		fprintf(stderr,"EGL_MAX_PBUFFER_WIDTH = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_NATIVE_RENDERABLE, &size);
		fprintf(stderr,"EGL_NATIVE_RENDERABLE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_NATIVE_VISUAL_ID, &size);
		fprintf(stderr,"EGL_NATIVE_VISUAL_ID = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_NATIVE_VISUAL_TYPE, &size);
		fprintf(stderr,"EGL_NATIVE_VISUAL_TYPE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_SAMPLES, &size);
		fprintf(stderr,"EGL_SAMPLES = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_SAMPLE_BUFFERS, &size);
		fprintf(stderr,"EGL_SAMPLE_BUFFERS = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_SURFACE_TYPE, &size);
		fprintf(stderr,"EGL_SURFACE_TYPE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_TRANSPARENT_TYPE, &size);
		fprintf(stderr,"EGL_TRANSPARENT_TYPE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_TRANSPARENT_BLUE_VALUE, &size);
		fprintf(stderr,"EGL_TRANSPARENT_BLUE_VALUE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_TRANSPARENT_GREEN_VALUE, &size);
		fprintf(stderr,"EGL_TRANSPARENT_GREEN_VALUE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_TRANSPARENT_RED_VALUE, &size);
		fprintf(stderr,"EGL_TRANSPARENT_RED_VALUE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_NONE, &size);
		fprintf(stderr,"EGL_NONE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_BIND_TO_TEXTURE_RGB, &size);
		fprintf(stderr,"EGL_BIND_TO_TEXTURE_RGB = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_BIND_TO_TEXTURE_RGBA, &size);
		fprintf(stderr,"EGL_BIND_TO_TEXTURE_RGBA = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_MIN_SWAP_INTERVAL, &size);
		fprintf(stderr,"EGL_MIN_SWAP_INTERVAL = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_MAX_SWAP_INTERVAL, &size);
		fprintf(stderr,"EGL_MAX_SWAP_INTERVAL = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_LUMINANCE_SIZE, &size);
		fprintf(stderr,"EGL_LUMINANCE_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_ALPHA_MASK_SIZE, &size);
		fprintf(stderr,"EGL_ALPHA_MASK_SIZE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_COLOR_BUFFER_TYPE, &size);
		fprintf(stderr,"EGL_COLOR_BUFFER_TYPE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_RENDERABLE_TYPE, &size);
		fprintf(stderr,"EGL_RENDERABLE_TYPE = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_MATCH_NATIVE_PIXMAP, &size);
		fprintf(stderr,"EGL_MATCH_NATIVE_PIXMAP = %d\n", size);
		eglGetConfigAttrib(egl_display,
		                   configs[i], EGL_CONFORMANT, &size);
		fprintf(stderr,"EGL_CONFORMANT = %d\n", size);
		fprintf(stderr,"----------------------------\n");
//		break;
	}
	egl_conf = configs[8];
	egl_context =
	        eglCreateContext(egl_display,
	                         egl_conf,
	                         EGL_NO_CONTEXT, context_attribs);

}

int main(int argc, char **argv)
{
	display = wl_display_connect(NULL);
	if (display == NULL) {
		fprintf(stderr, "Can't connect to display\n");
		exit(1);
	}
	fprintf(stderr,"connected to display\n");

	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	if (compositor == NULL) {
		fprintf(stderr, "Can't find compositor\n");
		exit(1);
	} else {
		fprintf(stderr, "Found compositor\n");
	}

	surface = wl_compositor_create_surface(compositor);
	if (surface == NULL) {
		fprintf(stderr, "Can't create surface\n");
		exit(1);
	} else {
		fprintf(stderr, "Created surface %p\n", surface);
	}

	shell_surface = wl_shell_get_shell_surface(shell, surface);
	wl_shell_surface_set_toplevel(shell_surface);


	wl_shell_surface_add_listener(shell_surface,
	                              &shell_surface_listener, NULL);

	init_egl();
	create_window();

	callback = wl_display_sync(display);
	wl_callback_add_listener(callback, &configure_callback_listener,
	                         NULL);

	while (wl_display_dispatch(display) != -1) {
		;
	}

	wl_display_disconnect(display);
	printf("disconnected from display\n");

	exit(0);
}
