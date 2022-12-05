#include "sdl_functions.h"

//-------------------------------------------------------------- INIT WINDOW
SDL_Window* sdl_init_window(int w, int h)
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return NULL;
    }

    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        printf( "Warning: Linear texture filtering not enabled!" );
    }

    //Create window
    SDL_Window* window = SDL_CreateWindow( "FLUID 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_HIDDEN);
    if( window == NULL )
    {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return NULL;
    }


    return window;
}
//-------------------------------------------------------------- INIT RENDERER
SDL_Renderer* sdl_init_renderer(SDL_Window* window)
{
    //Create vsynced renderer for window
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( renderer == NULL )
    {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return NULL;
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        //printf("\nresult:%d\n",res);
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        SDL_DestroyRenderer(renderer);
        return NULL;
    }
   
    return renderer;

}
//-------------------------------------------------------------- INIT TEXTURE
SDL_Texture* sdl_init_texture(int display_index, SDL_GUI_PT*pt)
{
    SDL_DisplayMode display_mode;


    if(SDL_GetCurrentDisplayMode(display_index,&display_mode)<0)
    {
        printf( "\nUnable to load texture! SDL_image Error: %s\n", IMG_GetError() );
        return NULL;
    }
    pt->sdl_texture_size.w=display_mode.w;
    pt->sdl_texture_size.h=display_mode.h;

    //The final texture
    SDL_Texture* texture = NULL;

    int rmask = 0xff000000;
    int gmask = 0x00ff0000;
    int bmask = 0x0000ff00;
    int amask = 0x000000ff;

    //Load image at specified path
    SDL_Surface* surface = SDL_CreateRGBSurface(0,display_mode.w,display_mode.h,32,rmask,gmask,bmask,amask);
    if( surface == NULL )
    {
        printf( "\nUnable to load texture! SDL_image Error: %s\n", IMG_GetError() );

        return NULL;
    }


    //Color key image
    SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0, 0xFF, 0xFF ) );

    //Create texture from surface pixels
    texture = SDL_CreateTextureFromSurface( pt->sdl_renderer, surface );
    //Get rid of old surface
    SDL_FreeSurface( surface );
    if( texture == NULL )
    {
        printf( "\nUnable to create texture from surface! SDL Error: %s\n", SDL_GetError() );
        return NULL;
    }
    //Return success
    return texture;
}
//-------------------------------------------------------------- UPDATE WINDOW

void sdl_update_window(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture,
                       SOIL_RECT* window_rect,SIZE *main_size, Uint32 *main_pixmap)
{
    //window rect is the area for the texture to update which is equal to the
    //window size and corresponds to the same rect in the pixmap

    //The texture should never be smaller than the window
    SDL_Rect window_sdl_rect;
    window_sdl_rect.x = window_rect->x;
    window_sdl_rect.y = window_rect->y;
    window_sdl_rect.w = window_rect->w;
    window_sdl_rect.h = window_rect->h;

    SDL_RenderClear( renderer );
    SDL_UpdateTexture(texture,&window_sdl_rect,main_pixmap,main_size->w*(sizeof(Uint32)));

    SDL_RenderCopy(renderer,texture,&window_sdl_rect,&window_sdl_rect);
    SDL_RenderPresent(renderer);

}
//-------------------------------------------------------------- CLOSE SDL
void sdl_close(SDL_GUI_PT* pointers)
{
    if(pointers->sdl_texture!=NULL)
    {
        SDL_DestroyTexture(pointers->sdl_texture);
    }
    if(pointers->sdl_renderer!=NULL)
    {
        SDL_DestroyRenderer(pointers->sdl_renderer);
    }
    if(pointers->sdl_window!=NULL)
    {
        SDL_DestroyWindow(pointers->sdl_window);
        IMG_Quit();
        SDL_Quit();
    }
    pointers->sdl_gui_loaded=false;

}


//-------------------------------------------------------------- INIT SDL
SDL_GUI_PT sdl_init(int w,int h)
{
    SDL_GUI_PT res;
    res.sdl_window=NULL;
    res.sdl_renderer=NULL;
    res.sdl_texture=NULL;
    //WINDOW
    res.sdl_window=sdl_init_window(w,h);
    if(res.sdl_window==NULL)
    {
        sdl_close(&res);
        return res;
    }

    //RENDERER
    res.sdl_renderer=sdl_init_renderer(res.sdl_window);
    if(res.sdl_renderer==NULL)
    {
        sdl_close(&res);
        return res;
    }
    //TEXTURE
    int display_index=SDL_GetWindowDisplayIndex(res.sdl_window);
    float tmp_dpi;
    SDL_GetDisplayDPI(display_index,NULL,&tmp_dpi,NULL);
    res.pixel_density = (int)tmp_dpi;

    if(display_index<0)
    {
        sdl_close(&res);
        return res;
    }
    res.sdl_texture=sdl_init_texture(display_index,&res);
    if(res.sdl_texture==NULL)
    {
        sdl_close(&res);
        return res;
    }
    res.sdl_gui_loaded=true;
    return res;


}
//-------------------------------------------------------------- INIT SDL
void sdl_reset_gui_pt(SDL_GUI_PT* sdl_gui_pt)
{
    sdl_gui_pt->sdl_window=NULL;
    sdl_gui_pt->sdl_renderer=NULL;
    sdl_gui_pt->sdl_texture=NULL;
    sdl_gui_pt->pixel_density = 0;
    sdl_gui_pt->sdl_gui_loaded=false;
}
//-------------------------------------------------------------- GET SDL RECT
SDL_Rect make_sdl_rect(int x, int y, int w, int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return rect;
}
