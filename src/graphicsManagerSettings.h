#ifndef GRAPHICSMANAGERSETTINGS_H
#define GRAPHICSMANAGERSETTINGS_H

#define GFX_DEBUG

#ifndef GFX_DEFAULT_GL_VERSION_MAJOR
	#define GFX_DEFAULT_GL_VERSION_MAJOR 3
#endif
#ifndef GFX_DEFAULT_GL_VERSION_MINOR
	#define GFX_DEFAULT_GL_VERSION_MINOR 3
#endif

#ifndef GFX_DEFAULT_WINDOW_ASPECT_RATIO_X
	#define GFX_DEFAULT_WINDOW_ASPECT_RATIO_X 16.f
#endif
#ifndef GFX_DEFAULT_WINDOW_ASPECT_RATIO_Y
	#define GFX_DEFAULT_WINDOW_ASPECT_RATIO_Y 9.f
#endif
#ifndef GFX_DEFAULT_WINDOW_WIDTH
	#define GFX_DEFAULT_WINDOW_WIDTH 800
#endif
#ifndef GFX_DEFAULT_WINDOW_HEIGHT
	#define GFX_DEFAULT_WINDOW_HEIGHT 450
#endif
#ifndef GFX_DEFAULT_WINDOW_FLAGS
	#define GFX_DEFAULT_WINDOW_FLAGS (SDL_WINDOW_OPENGL | \
	                                  SDL_WINDOW_ALLOW_HIGHDPI | \
	                                  SDL_WINDOW_RESIZABLE | \
	                                  SDL_WINDOW_INPUT_FOCUS | \
	                                  SDL_WINDOW_MOUSE_FOCUS)
#endif

#ifndef GFX_DEFAULT_FREQUENCY
	#define GFX_DEFAULT_FREQUENCY 22050
#endif
#ifndef GFX_DEFAULT_CHANNELS
	#define GFX_DEFAULT_CHANNELS 2
#endif
#ifndef GFX_DEFAULT_CHUNKSIZE
	#define GFX_DEFAULT_CHUNKSIZE 2048
#endif

#define GFX_WINDOW_MODE_WINDOWED 0
#define GFX_WINDOW_MODE_FULLSCREEN SDL_WINDOW_FULLSCREEN

// Note: it's a good idea to use either windowbox or
// stretch mode. Fill has a tendency to affect the fov,
// which may not be intended and gives users with higher
// resolutions significant advantages in some cases.
#define GFX_WINDOW_VIEWPORT_MODE_WINDOWBOX 0
#define GFX_WINDOW_VIEWPORT_MODE_STRETCH   1
#define GFX_WINDOW_VIEWPORT_MODE_FILL      2

#ifndef GFX_DEFAULT_VIEWPORT_MODE
	#define GFX_DEFAULT_VIEWPORT_MODE GFX_WINDOW_VIEWPORT_MODE_WINDOWBOX
#endif

#endif
