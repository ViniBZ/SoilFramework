

#ifndef sdl_functions_h
#define sdl_functions_h

#include <iostream>

#include "global_definitions.h"


typedef struct SDL_GUI_POINTER_PACK
{
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    SDL_Texture* sdl_texture;
    int pixel_density;

    SIZE sdl_texture_size;

    bool sdl_gui_loaded;

}SDL_GUI_PT;

//-------------------------------------------------------------- INIT WINDOW
SDL_Window* sdl_init_window(int w, int h);
//-------------------------------------------------------------- INIT RENDERER
SDL_Renderer* sdl_init_renderer(SDL_Window* window);
//-------------------------------------------------------------- INIT TEXTURE
SDL_Texture* sdl_init_texture(int display_index, SDL_GUI_PT*pt);
//-------------------------------------------------------------- UPDATE WINDOW
void sdl_update_window(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture,
                       SOIL_RECT* window_rect,SIZE *main_size, Uint32 *main_pixmap);
//-------------------------------------------------------------- CLOSE SDL
void sdl_close(SDL_GUI_PT* pointers);
//-------------------------------------------------------------- INIT SDL
SDL_GUI_PT sdl_init(int w,int h);
//-------------------------------------------------------------- INIT SDL
void sdl_reset_gui_pt(SDL_GUI_PT* sdl_gui_pt);
//-------------------------------------------------------------- GET SDL RECT
SDL_Rect make_sdl_rect(int x, int y, int w, int h);

#endif // sdl_functions_h 
