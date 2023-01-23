#include "soilpixmap.h"

SoilPixmap::SoilPixmap()
{
    color_px_pt = NULL;
    mono_px_pt = NULL;
    color_mode = true;
    mono_mode = false;
    loaded = false;
    size.w = 0;
    size.h = 0;
    alloc_c = 0;

    dynamic_bound = false;
}
SoilPixmap::~SoilPixmap()
{
    release();
}

// ---------------------------------------- ALLOC PIXMAP 1
bool SoilPixmap::alloc(int w, int h)
{
    if(w <= 0 || h <= 0){return false;}
    if(dynamic_bound)
    {
        bool a = px_mem_bound.calculate(w,h);
        if(!a)
        {
            size.w = w;
            size.h = h;
            return true;
        }
        internal_release();

        int alloc_len = px_mem_bound.ALLOC_SIZE.w * px_mem_bound.ALLOC_SIZE.h;
        if(color_mode)
        {
            color_px_pt = new Uint32[alloc_len];
            if(color_px_pt != NULL){loaded = true;}
        }
        if(mono_mode){
            mono_px_pt = new unsigned char[alloc_len];
            if(mono_px_pt != NULL){loaded = true;}
        }   
        
    }else{
        if(w == size.w && h == size.h){return true;}
        internal_release();
        int alloc_len = w*h;
        if(color_mode)
        {
            color_px_pt = new Uint32[alloc_len];
            if(color_px_pt != NULL){loaded = true;}
        }
        if(mono_mode){
            mono_px_pt = new unsigned char[alloc_len];
            if(mono_px_pt != NULL){loaded = true;}
        }

    }

    if(loaded)
    {
        size.w = w;
        size.h = h;
    }else{
        release();
    }

    //soilobject callback

    return loaded;
}
// ---------------------------------------- ALLOC PIXMAP
bool SoilPixmap::alloc(SIZE s)
{
    return alloc(s.w,s.h);
}
// ---------------------------------------- ALLOC PIXMAP FROM FILE 2
bool SoilPixmap::alloc_from_file(const char* file_name)
{
    bool succeed = true;
    SDL_Surface * surface = IMG_Load(file_name);
    if(!surface){return false;}
    int w=surface->w;
    int h=surface->h;
    bool a = alloc(w,h);

    if(!a)
    {
        succeed = false;
    }

    if(succeed)
    {
        if(mono_mode)
        {
            a = sdl_surface_to_pixmap(surface, mono_px_pt, size);
            if(mono_px_pt == NULL){succeed = false;}
        }else{
            a = sdl_surface_to_pixmap(surface, color_px_pt, size);
            if(color_px_pt == NULL){succeed = false;}
        }
    }
    if(succeed)
    {
        loaded = true;
    }else{
        release();
    }
    SDL_FreeSurface(surface);

    return loaded;
}
// ---------------------------------------- ALLOC FROM PIXMAP
bool SoilPixmap::alloc_from_pixmap(const SoilPixmap& orig)
{
    bool res = alloc(orig.size.w, orig.size.h);
    if(!res){return false;}
    if(orig.color_mode)
    {
        set_to_color_mode();
    }else{
        set_to_mono_mode();
    }
    spx_paste_pixmap(orig, orig.get_full_rect(), get_full_rect());
    return true;
}
// ---------------------------------------- RELEASE MEMORY
//doesn't reset px_mem_bound
void SoilPixmap::internal_release()
{
    if(loaded)
    {
        if(color_mode)
        {
            delete[] color_px_pt;
            color_px_pt=NULL;
        }
        if(mono_mode){
            delete[] mono_px_pt;
            mono_px_pt=NULL;
        }

        //soilobject callback
    }
    loaded=false;
    size.w = 0;
    size.h = 0;
}
// ---------------------------------------- RELEASE PIXMAP
void SoilPixmap::release()
{
    internal_release();
    px_mem_bound.reset();
}
// ---------------------------------------- SET DYNAMIC BOUND
void SoilPixmap::set_dynamic_bound(bool dynamic, int margin_w, int margin_h)
{
    bool prev = dynamic_bound;
    dynamic_bound = dynamic;

    if(prev != dynamic_bound)
    {
        if(dynamic_bound)
        {
            px_mem_bound.set_margin(margin_w, margin_h);
            px_mem_bound.adapt_from_size(size.w, size.h);
        }
    }

}
// ---------------------------------------- SET MONO MODE
//return true if all went well, if for some absurdly unlikely event
//the other mode's pointer fail to be alloc'ed it returns false
bool SoilPixmap::set_to_mono_mode()
{
    if(loaded && color_mode)
    {
        mono_px_pt = new unsigned char[size.w*size.h];
        if(mono_px_pt == NULL){return false;}

        paste_pixmap_to_pixmap(color_px_pt,size,get_full_rect(),mono_px_pt,size,get_full_rect());
        delete[] color_px_pt;
    }
    mono_mode = true;
    color_mode = false;

    return true;

}
// ---------------------------------------- SET COLOR MODE
bool SoilPixmap::set_to_color_mode()
{
    if(loaded && mono_mode)
    {
        color_px_pt = new Uint32[size.w*size.h];
        if(color_px_pt == NULL){return false;}

        paste_pixmap_to_pixmap(mono_px_pt,size,get_full_rect(),color_px_pt,size,get_full_rect());
        delete[] mono_px_pt;
    }
    mono_mode = false;
    color_mode = true;

    return true;
}
// ---------------------------------------- PAINT PIXEL
bool SoilPixmap::spx_paint_pixel(int x, int y, PIXDATA color)
{
    if(!loaded){return false;}
    if(x < 0 || x >= size.w){return false;}
    if(y < 0 || y >= size.h){return false;}

    if(color_mode)
    {
        color_px_pt[(y * size.w) + x] = color.pix;
    }
    if(mono_mode)
    {
        mono_px_pt[(y * size.w) + x] = return_uchar_from_pix(color);
    }
    return true;
}
// ---------------------------------------- PAINT PIXEL
bool SoilPixmap::spx_paint_pixel(int x, int y, Uint32 color)
{
    if(!loaded){return false;}
    if(x < 0 || x >= size.w){return false;}
    if(y < 0 || y >= size.h){return false;}

    if(color_mode)
    {
        color_px_pt[(y * size.w) + x] = color;
    }
    if(mono_mode)
    {
        mono_px_pt[(y * size.w) + x] = return_uchar_from_pix(color);
    }
    return true;
}
// ---------------------------------------- PAINT PIXEL
bool SoilPixmap::spx_paint_pixel(int x, int y, unsigned char color)
{
    if(!loaded){return false;}
    if(x < 0 || x >= size.w){return false;}
    if(y < 0 || y >= size.h){return false;}

    if(color_mode)
    {
        color_px_pt[(y * size.w) + x] = return_uint32_from_pix(color);
    }
    if(mono_mode)
    {
        mono_px_pt[(y * size.w) + x] = color;
    }
    return true;
}
// ---------------------------------------- UCHAR PIXEL AT
unsigned char SoilPixmap::spx_uchar_pixel_at(int x, int y)
{
    unsigned char ret;
    if(!loaded){return ret;}
    if(x < 0 || x >= size.w){return ret;}
    if(y < 0 || y >= size.h){return ret;}

    if(color_mode)
    {
        ret = return_uchar_from_pix(color_px_pt[(y * size.w) + x]);
        return ret;
    }
    if(mono_mode)
    {
        return mono_px_pt[(y * size.w) + x];
    }
    return ret;
}

// ---------------------------------------- UINT32 PIXEL AT
Uint32 SoilPixmap::spx_uint32_pixel_at(int x, int y)
{
    Uint32 ret;
    if(!loaded){return ret;}
    if(x < 0 || x >= size.w){return ret;}
    if(y < 0 || y >= size.h){return ret;}

    if(color_mode)
    {
        ret = color_px_pt[(y * size.w) + x];
        return ret;
    }
    if(mono_mode)
    {
        ret = return_uint32_from_pix(mono_px_pt[(y * size.w) + x]);
        return ret;
    }
    return ret;
}

// ---------------------------------------- PIXDATA PIXEL AT
PIXDATA SoilPixmap::spx_pixdata_pixel_at(int x, int y)
{
    PIXDATA ret;
    if(!loaded){return ret;}
    if(x < 0 || x >= size.w){return ret;}
    if(y < 0 || y >= size.h){return ret;}

    if(color_mode)
    {
        ret = return_pixdata_from_pix(color_px_pt[(y * size.w) + x]);
        return ret;
    }
    if(mono_mode)
    {
        ret = return_pixdata_from_pix(mono_px_pt[(y * size.w) + x]);
        return ret;
    }
    return ret;
}


// ---------------------------------------- FILL PIXMAP
void SoilPixmap::spx_fill(unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color = return_uint32_from_pix(color);
            fill_pixmap(color_px_pt,size,get_full_rect(),full_color);
        }
        if(mono_mode)
        {
            fill_pixmap(mono_px_pt,size,get_full_rect(),color);
        }
    }
}
// ---------------------------------------- FILL PIXMAP
void SoilPixmap::spx_fill(Uint32 color)
{


    if(loaded)
    {
        if(color_mode)
        {
            fill_pixmap(color_px_pt,size,get_full_rect(),color);
        }
        if(mono_mode)
        {
            unsigned char mono_color = return_uchar_from_pix(color);
            fill_pixmap(mono_px_pt,size,get_full_rect(),mono_color);
        }
    }
}
// ---------------------------------------- FILL PIXMAP
void SoilPixmap::spx_fill(PIXDATA color)
{
    if(loaded)
    {
        if(color_mode)
        {

            fill_pixmap(color_px_pt,size,get_full_rect(),color.pix);
        }
        if(mono_mode)
        {
            unsigned char mono_color = return_uchar_from_pix(color);
            fill_pixmap(mono_px_pt,size,get_full_rect(),mono_color);
        }
    }
}
// ---------------------------------------- MASK FILL PIXMAP
void SoilPixmap::spx_mask_fill(Uint32 color, Uint32 mask)
{
    
}

// ---------------------------------------- PASTE PIXMAP
void SoilPixmap::spx_paste_pixmap(const SoilPixmap& orig, SOIL_RECT orig_rect,SOIL_RECT paste_rect)
{
    //The validation for the pixmap limits is done within
    //paste_pixmap_to_pixmap(Uint32*,SIZE,SOIL_RECT,Uint32*,SIZE,SOIL_RECT)
    if(loaded)
    {
        if(!orig.loaded){return;}
        /////////////// THIS->COLOR MODE
        if(color_mode)
        {
            if(orig.color_mode)
            {
                paste_pixmap_to_pixmap(orig.color_px_pt,orig.size,orig_rect,color_px_pt,size,paste_rect);
                return;
            }
            if(orig.mono_mode)
            {
                paste_pixmap_to_pixmap(orig.mono_px_pt,orig.size,orig_rect,color_px_pt,size,paste_rect);
                return;
            }
        }
        /////////////// THIS->MONO MODE
        if(mono_mode)
        {
            if(orig.color_mode)
            {
                paste_pixmap_to_pixmap(orig.color_px_pt,orig.size,orig_rect,mono_px_pt,size,paste_rect);
                return;
            }
            if(orig.mono_mode)
            {
                paste_pixmap_to_pixmap(orig.mono_px_pt,orig.size,orig_rect,mono_px_pt,size,paste_rect);
                return;
            }
        }
    }//if loaded
}

// ---------------------------------------- RESIZE PASTE PIXMAP
void SoilPixmap::spx_resize_paste_pixmap(const SoilPixmap& orig, SOIL_RECT orig_rect,SOIL_RECT paste_rect)
{
    if(loaded)
    {
        /////////////// THIS->COLOR MODE
        if(color_mode)
        {
            if(orig.color_mode)
            {
                
                resize_pixmap(orig.color_px_pt,orig.size,orig_rect,color_px_pt,size,paste_rect);
                return;
            }
            if(orig.mono_mode)
            {
                
                resize_pixmap(orig.mono_px_pt,orig.size,orig_rect,color_px_pt,size,paste_rect);
                return;
            }
        }
        /////////////// THIS->MONO MODE
        if(mono_mode)
        {
            if(orig.color_mode)
            {
                
                resize_pixmap(orig.color_px_pt,orig.size,orig_rect,mono_px_pt,size,paste_rect);
                return;
            }
            if(orig.mono_mode)
            {
                
                resize_pixmap(orig.mono_px_pt,orig.size,orig_rect,mono_px_pt,size,paste_rect);
                return;
            }
        }
    }
}

// ---------------------------------------- INVERT COLORS
void SoilPixmap::spx_invert_colors(SOIL_RECT rect)
{
    if(loaded)
    {
        if(color_mode)
        {
            invert_colors(color_px_pt,size,rect);
        }
        if(mono_mode)
        {
            invert_colors(mono_px_pt,size,rect);
        }
    }
}

// ---------------------------------------- INVERT COLORS
void SoilPixmap::spx_invert_colors()
{
    if(loaded)
    {
        if(color_mode)
        {
            invert_colors(color_px_pt,size);
        }
        if(mono_mode)
        {
            invert_colors(mono_px_pt,size);
        }
    }
}
// ---------------------------------------- ALPHA BLEND
//blend orig on top of this pixmap
void SoilPixmap::spx_alpha_blend(const SoilPixmap& orig, SOIL_RECT orig_rect,SOIL_RECT paste_rect)
{
    if(loaded)
    {
        if(color_mode)
        {
            alpha_blend_pixmap(color_px_pt,size,paste_rect,orig.color_px_pt,orig.size,orig_rect,color_px_pt,size,paste_rect);
        }
    }
}
// ---------------------------------------- SPX DRAW ANTI ALIASING LINE
void SoilPixmap::spx_draw_aa_line(POINT start, POINT end, int bold, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_aa_line(color_px_pt,size,start,end,bold,full_color);
        }
        if(mono_mode)
        {
            draw_aa_line(mono_px_pt,size,start,end,bold,color);
        }
    }
}
// ---------------------------------------- SPX DRAW ANTI ALIASING LINE
void SoilPixmap::spx_draw_aa_line(POINT start, POINT end, int bold, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_aa_line(color_px_pt,size,start,end,bold,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_aa_line(mono_px_pt,size,start,end,bold,mono_color);
        }
    }
}
// ---------------------------------------- SPX DRAW H LINE
void SoilPixmap::spx_draw_h_line(POINT start, int end_x, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_h_line(color_px_pt,size,start,end_x,full_color);
        }
        if(mono_mode)
        {
            draw_h_line(mono_px_pt,size,start,end_x,color);
        }
    }
}
// ---------------------------------------- SPX DRAW H LINE
void SoilPixmap::spx_draw_h_line(POINT start, int end_x, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_h_line(color_px_pt,size,start,end_x,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_h_line(mono_px_pt,size,start,end_x,mono_color);
        }
    }
}

// ---------------------------------------- SPX DRAW V LINE
void SoilPixmap::spx_draw_v_line(POINT start, int end_y, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_v_line(color_px_pt,size,start,end_y,full_color);
        }
        if(mono_mode)
        {
            draw_v_line(mono_px_pt,size,start,end_y,color);
        }
    }
}
// ---------------------------------------- SPX DRAW V LINE
void SoilPixmap::spx_draw_v_line(POINT start, int end_y, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_v_line(color_px_pt,size,start,end_y,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_v_line(mono_px_pt,size,start,end_y,mono_color);
        }
    }
}

// ---------------------------------------- DRAW VERTICAL LINE
void SoilPixmap::spx_draw_cursor_line(POINT start, int end_y, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_cursor_line(color_px_pt,size,start,end_y,full_color);
        }
        if(mono_mode)
        {
            draw_cursor_line(mono_px_pt,size,start,end_y,color);
        }
    }
}

// ---------------------------------------- DRAW VERTICAL LINE
void SoilPixmap::spx_draw_cursor_line(POINT start, int end_y, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_cursor_line(color_px_pt,size,start,end_y,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_cursor_line(mono_px_pt,size,start,end_y,mono_color);
        }
    }
}

// ---------------------------------------- DRAW OUTLINE RECT
void SoilPixmap::spx_draw_outline_rect(SOIL_RECT rect, int bold, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_outline_rect(color_px_pt,size,rect,bold,full_color);
        }
        if(mono_mode)
        {
            draw_outline_rect(mono_px_pt,size,rect,bold,color);
        }
    }
}
// ---------------------------------------- DRAW OUTLINE RECT
void SoilPixmap::spx_draw_outline_rect(SOIL_RECT rect, int bold, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_outline_rect(color_px_pt,size,rect,bold,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_outline_rect(mono_px_pt,size,rect,bold,mono_color);
        }
    }
}
// ---------------------------------------- DRAW SOLID RECT
void SoilPixmap::spx_draw_solid_rect(SOIL_RECT rect, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_solid_rect(color_px_pt,size,rect,color);
        }
        if(mono_mode)
        {
            draw_solid_rect(color_px_pt,size,rect,color);
        }
    }
}
// ---------------------------------------- DRAW SOLID RECT
void SoilPixmap::spx_draw_solid_rect(SOIL_RECT rect, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_solid_rect(color_px_pt,size,rect,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_solid_rect(mono_px_pt,size,rect,mono_color);
        }
    }
}
// ---------------------------------------- DRAW PATCH RECT
void SoilPixmap::spx_draw_patch_rect(SOIL_RECT rect, unsigned char color)
{
    if(loaded)
    {
        if(color_mode)
        {
            Uint32 full_color;
            full_color = return_uint32_from_pix(color);
            draw_patch_rect(color_px_pt,size,rect,color);
        }
        if(mono_mode)
        {
            draw_patch_rect(color_px_pt,size,rect,color);
        }
    }
}
// ---------------------------------------- DRAW PATCH RECT
void SoilPixmap::spx_draw_patch_rect(SOIL_RECT rect, Uint32 color)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_patch_rect(color_px_pt,size,rect,color);
        }
        if(mono_mode)
        {
            unsigned char mono_color;
            mono_color = return_uchar_from_pix(color);
            draw_patch_rect(mono_px_pt,size,rect,mono_color);
        }
    }
}
// ---------------------------------------- DRAW SCROLL RECT
void SoilPixmap::spx_draw_scroll_rect(SOIL_RECT rect, bool dark, int border_disp)
{
    if(loaded)
    {
        if(color_mode)
        {
            draw_scroll_rect(color_px_pt,size,rect,dark, border_disp);
        }
        if(mono_mode)
        {
            draw_scroll_rect(mono_px_pt,size,rect,dark, border_disp);
        }
    }
}
// ---------------------------------------- DRAW COLOR WHEEL (HUE, SATURATION, VALUE)
void SoilPixmap::spx_hsv_color_wheel(int hue, double sat, double val)
{

}
// ---------------------------------------- DRAW COLOR WHEEL
void SoilPixmap::spx_color_wheel()
{
    if(!loaded || size.w < 30 || size.h < 30){return;}
    int big_rad = size.w / 2;
    int sml_rad = big_rad - 20;
    POINT C;
    C.x = big_rad;
    C.y = big_rad;
    int HYP;
    int precision = 20 + (size.w / 8);
    if(precision < 25){precision = 25;}
    if(precision > 100){precision = 25;}

    int x = 0;
    int y = 0;
    while(y < size.h)
    {
        while(x < size.w)
        {
            int cat_x;
            int cat_y;
            if(x < big_rad)
            {
                cat_x = big_rad - x;
            }else{
                cat_x = x - big_rad;
            }
            if(y < big_rad)
            {
                cat_y = big_rad - y;
            }else{
                cat_y = y - big_rad;
            }


            HYP = get_hypotenuse(cat_x, cat_y, precision);
            if(HYP < big_rad && HYP > sml_rad)
            {
                spx_paint_pixel(x,y,0xFF774488);
            }else{
                spx_paint_pixel(x,y,0xFF000000);
            }
            x++;
        }
        x=0;
        y++;
    }
}
// ---------------------------------------- DRAW COLOR SQUARE
void SoilPixmap::spx_color_square(Uint32 base_color)
{
    if(!loaded){return;}

    int x = 0;
    int y = 0;

    //BASE COLOR PIXDATA
    PIXDATA base_pixdata;

    base_pixdata.pix = base_color;

    int highest_comp;
    double comp_ratio;

    double color_inc_r;
    double color_inc_g;
    double color_inc_b;

    double color_inc_comp_r;
    double color_inc_comp_g;
    double color_inc_comp_b;

    double r;
    double g;
    double b;

    //determine highest color
    int highest_color = base_pixdata.color.r;
    int which_is_highest = 1;
    if(base_pixdata.color.g > highest_color)
    {
        highest_color = base_pixdata.color.g;
        which_is_highest = 2;
    }
    if(base_pixdata.color.b > highest_color)
    {
        highest_color = base_pixdata.color.b;
        which_is_highest = 3;
    }

    if(highest_color == 0)
    {
        //if highest color == 0 , grayscale
        color_inc_r = 1;
        color_inc_g = 1;
        color_inc_b = 1;

        color_inc_comp_r = 0;
        color_inc_comp_g = 0;
        color_inc_comp_b = 0;
    }else{
        //highest_comp = 255 - highest_color
        double highest_comp = (double)255 - highest_color;
        //comp_ratio = highest_comp / highest_color
        double comp_ratio = (double)highest_comp / highest_color;
        //color += comp_ratio * color
        r = (double)base_pixdata.color.r + (comp_ratio * base_pixdata.color.r);
        g = (double)base_pixdata.color.g + (comp_ratio * base_pixdata.color.g);
        b = (double)base_pixdata.color.b + (comp_ratio * base_pixdata.color.b);

        //color_inc = color / width;
        color_inc_r = r / size.w;
        color_inc_g = g / size.w;
        color_inc_b = b / size.w;

        double highest_color_inc;
        if(which_is_highest == 1){highest_color_inc = color_inc_r;}
        if(which_is_highest == 2){highest_color_inc = color_inc_g;}
        if(which_is_highest == 3){highest_color_inc = color_inc_b;}
        //color_inc_c = (highest_color_inc - color_inc) / height
        color_inc_comp_r = (highest_color_inc - color_inc_r) / size.h;
        color_inc_comp_g = (highest_color_inc - color_inc_g) / size.h;
        color_inc_comp_b = (highest_color_inc - color_inc_b) / size.h;

    }
    PIXDATA pixdata;
    pixdata.color.a = 255;
    while(y < size.h)
    {
        r = 0;
        g = 0;
        b = 0;
        while(x < size.w)
        {   
            pixdata.color.r = r;
            pixdata.color.g = g;
            pixdata.color.b = b;
            spx_paint_pixel(x,y,pixdata.pix);
            r += color_inc_r;
            g += color_inc_g;
            b += color_inc_b;
            x++;
        }
        color_inc_r += color_inc_comp_r;
        color_inc_g += color_inc_comp_g;
        color_inc_b += color_inc_comp_b;

        x = 0;
        y++;
    }
    
}
// ---------------------------------------- GET RECT
SOIL_RECT SoilPixmap::get_rect(int x, int y, int w, int h)const
{
    SOIL_RECT rect;
    rect.x=x;
    rect.y=y;
    rect.w=w;
    rect.h=h;

    adjust_rect_to_px_size(size,&rect);

    return rect;
}

// ---------------------------------------- GET FULL RECT
SOIL_RECT SoilPixmap::get_full_rect()const
{
    SOIL_RECT rect;
    rect.x=0;
    rect.y=0;
    rect.w=size.w;
    rect.h=size.h;

    return rect;
}
// ---------------------------------------- VALIDATE PIXMAP
//in relation to this pixmap's size
//return false if there is no intersection of the rect with size
//it doesn't accepts negative w/h
bool SoilPixmap::validate_rect_in_size(SOIL_RECT* rect)
{
    SOIL_RECT res_rect = *rect;

    if(res_rect.w <= 0 || res_rect.h <= 0){return false;}

    int res_rect_ex = res_rect.x + res_rect.w;
    int res_rect_ey = res_rect.y + res_rect.h;
    if(res_rect.x < 0)
    {
        res_rect.x = 0;
    }else{
        if(res_rect.x >= size.w){return false;}
    }

    if(res_rect.y < 0)
    {
        res_rect.y = 0;
    }else{
        if(res_rect.y >= size.h){return false;}
    }

    if(res_rect_ex > size.w){res_rect.w -= (res_rect_ex - size.w);}
    if(res_rect_ey > size.h){res_rect.h -= (res_rect_ey - size.h);}

    return true;
}
// ---------------------------------------- SAVE IMG FILE
bool SoilPixmap::spx_save_img_file(std::string file_name,int format, int cmpr)
{
    if(!loaded){return false;}

    bool res = false;
    if(color_mode)
    {
        res = save_img_file(color_px_pt,size,file_name,format, cmpr);
    }
    if(mono_mode)
    {
        res = save_img_file(mono_px_pt,size,file_name,format, cmpr);
    }
    return res;

}
