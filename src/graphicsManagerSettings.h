#ifndef GRAPHICSMANAGERSETTINGS_H
#define GRAPHICSMANAGERSETTINGS_H

#define GFX_DEFAULT_GL_VERSION_MAJOR 3
#define GFX_DEFAULT_GL_VERSION_MINOR 3

#define GFX_DEFAULT_WINDOW_ASPECT_RATIO_X 16.f
#define GFX_DEFAULT_WINDOW_ASPECT_RATIO_Y 9.f
#define GFX_DEFAULT_WINDOW_WIDTH 800
#define GFX_DEFAULT_WINDOW_HEIGHT 450
#define GFX_DEFAULT_WINDOW_FLAGS (SDL_WINDOW_OPENGL | \
                                  SDL_WINDOW_ALLOW_HIGHDPI | \
                                  SDL_WINDOW_RESIZABLE | \
                                  SDL_WINDOW_INPUT_FOCUS | \
                                  SDL_WINDOW_MOUSE_FOCUS)

#define GFX_DEFAULT_FREQUENCY 22050
#define GFX_DEFAULT_CHANNELS 2
#define GFX_DEFAULT_CHUNKSIZE 2048

#define SPRITE_MAX_BATCH_SIZE (6*64)  /** Move this! **/
#define GFX_TEXTURE_SAMPLER_NUM 1

#define GFX_DEFAULT_BIAS_MIP 0.f
#define GFX_DEFAULT_BIAS_LOD 0

#define GFX_WINDOW_MODE_WINDOWED   0
#define GFX_WINDOW_MODE_FULLSCREEN SDL_WINDOW_FULLSCREEN

#define GFX_WINDOW_FILL_WINDOWBOX 0
#define GFX_WINDOW_FILL_STRETCH   1

#endif