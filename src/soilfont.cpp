#include "soilfont.h"

SoilFont::SoilFont()
{
    init();
}
SoilFont::~SoilFont()
{
    release_font();

    int s=0;
    while(s < size_n)
    {
        pixmap[s].release();
        s++;
    }

    delete[] pixmap;
    delete[] char_size;
    delete[] font_coord;
}
// ---------------------------------------- INITIALIZE VALUES
void SoilFont::init()
{
    font_coord = new SOIL_RECT[10];
    loaded = false;
    size_n=FONT_SIZE_N;
    font_len = FONT_LEN;

    pixmap = new SoilPixmap[size_n];
    char_size = new SIZE[size_n];

    alloc_font();
}

// ---------------------------------------- ALLOC FONT
void SoilFont::alloc_font()
{

    SIZE next_px_size;

    signed int s=size_n-1;
    while(s>=0)
    {
        if(s == size_n-1)
        {
            //the constants shown below were defined in global_definitions.h
            //they should have the right proportions
            //MAIN_FONT_PX_W = FONT_CHAR_W * FONT_LEN
            //MAIN_FONT_PX_H = FONT_CHAR_H * 2

            //MARKER:FIX this. create a way to set the resource sub dir
            //from SoilEngine
            pixmap[s].set_to_mono_mode();
			//pixmap[s].alloc_from_file("./font_images/soil_font.bmp");
			pixmap[s].alloc_from_file("/usr/share/soilframework_resources/font_images/soil_font.bmp");			

            int pxw = pixmap[s].size.w;
            int pxh = pixmap[s].size.h;
            int mw = MAIN_FONT_PX_W;
            int mh = MAIN_FONT_PX_H;

            if(pixmap[s].loaded && pxw == mw && pxh == mh)
            {
                char_size[s].w = FONT_CHAR_W;
                char_size[s].h = FONT_CHAR_H;
            }else{

                pixmap[s].alloc(mw,mh);
                if(!pixmap[s].loaded)
                {
                    loaded = false;
                    return;
                }
                draw_font_stub();
            }
            char_ratio = (double)char_size[s].h / char_size[s].w;

        }else{
            char_size[s] = get_char_size_from_font_size(s);

            pixmap[s].alloc(font_len * char_size[s].w, 2 * char_size[s].h);
            pixmap[s].spx_resize_paste_pixmap(pixmap[size_n-1],pixmap[size_n-1].get_full_rect(),pixmap[s].get_full_rect());
        }

        s--;
    }
    loaded = true;
}
// ---------------------------------------- DRAW FONT STUB
void SoilFont::draw_font_stub()
{
    if(!pixmap[size_n-1].loaded){return;}

    int s = size_n - 1 ;

    pixmap[s].spx_draw_solid_rect(pixmap[s].get_rect(0,0,pixmap[s].size.w,pixmap[s].size.h/2),0xFF000000);
    pixmap[s].spx_draw_solid_rect(pixmap[s].get_rect(0,pixmap[s].size.h/2,pixmap[s].size.w,pixmap[s].size.h/2),0xFFFFFFFF);

    int margin = 5;
    SOIL_RECT sqr_rect;
    sqr_rect.x = margin;
    sqr_rect.y = margin;
    sqr_rect.w = char_size[s].w - (2*margin);
    sqr_rect.h = char_size[s].h - (2*margin);

    int y = 0;
    int x = 0;
    unsigned char paint_color = 255;
    while(y < 2)
    {

        while(x < font_len)
        {
            pixmap[s].spx_draw_outline_rect(sqr_rect,4,paint_color);
            sqr_rect.x += char_size[s].w;
            x++;
        }

        paint_color = 0;

        sqr_rect.x = margin;
        x=0;

        sqr_rect.y += char_size[s].h;
        y++;
    }
}
// ---------------------------------------- GET CHAR INDEX
//based on UTF8 encoding
int SoilFont::get_char_index(SOIL_CHAR ch)
{
    int index = 190;
    if(ch.N == 1)
    {
        if(ch.B[0] < 127)
        {
            if(ch.B[0] > 31)
            {
                index = ch.B[0] - 32;
                return index;
            }
            return 0;
        }else{
            return index;
        }
    }
    if(ch.N == 2)
    {
        //0xc2
        if(ch.B[0] == 0xc2 )
        {
            if(ch.B[1] < 207 && ch.B[1] > 160){index = ch.B[1] - 66;}
            return index;
        }
        //0xc3
        if(ch.B[0] == 0xc3 )
        {
            if(ch.B[1] < 208 && ch.B[1] > 127){index = ch.B[1] - 2;}
            return index;
        }
    }
    return index;
}
// ---------------------------------------- PRINT FONT TO PIXMAP
void SoilFont::print_font_to_pixmap(SOIL_CHAR ch, int font_size, bool inverted, SOIL_RECT char_rect, SoilPixmap* dest_pixmap, SOIL_RECT dest_rect)
{
    validate_abs_index(FONT_SIZE_N, &font_size);
    int offset_x = get_char_index(ch);

    adjust_rect_to_size(char_size[font_size], &char_rect);
    char_rect.x += offset_x * char_size[font_size].w;
    if(inverted)
    {
        char_rect.y += char_size[font_size].h;
    }

    dest_pixmap->spx_paste_pixmap(pixmap[font_size], char_rect, dest_rect);
}
// ---------------------------------------- PRINT FONT TO PIXMAP
void SoilFont::print_font_to_pixmap(SOIL_CHAR ch, int font_size, bool inverted, SoilPixmap* dest_pixmap, SOIL_RECT dest_rect)
{
    validate_abs_index(FONT_SIZE_N, &font_size);
    int offset_x = get_char_index(ch);

    SOIL_RECT char_rect;
    char_rect.x = offset_x * char_size[font_size].w;
    char_rect.y = 0;
    char_rect.w = char_size[font_size].w;
    char_rect.h = char_size[font_size].h;

    if(inverted)
    {
        char_rect.y += char_size[font_size].h;
    }

    dest_pixmap->spx_paste_pixmap(pixmap[font_size], char_rect, dest_rect);
}
// ---------------------------------------- RELEASE FONT
void SoilFont::release_font()
{
    if(loaded)
    {
        pixmap->release();
    }
}

// ---------------------------------------- MAKE FONT DISPLAY FILE
void SoilFont::make_font_display_file()
{
    if(!loaded){return;}


    SoilPixmap px;
    int total_w = 0;
    int total_h = 0;
    signed int s = size_n - 1;
    while(s >= 0)
    {
        if(pixmap[s].size.w > total_w){total_w = pixmap[s].size.w;}
        total_h += pixmap[s].size.h;
        s--;
    }

    px.alloc(total_w,total_h);
    if(!px.loaded){return;}
    px.spx_fill(0xFFFF0000);

    int total_s =0;

    SOIL_RECT paste_rect;
    int next_y = 0;
    s = size_n - 1;
    while(s >= 0)
    {
        paste_rect = pixmap[s].get_full_rect();
        paste_rect.y = next_y;

        total_s += pixmap[s].size.w * pixmap[s].size.h;

        px.spx_paste_pixmap(pixmap[s],pixmap[s].get_full_rect(),paste_rect);

        next_y += pixmap[s].size.h;
        s--;
    }
    px.spx_save_img_file("font_images/display_font.bmp",2,0);
    px.release();

}

// -------------------------------------------------------------- GET CHAR SIZE BY FONT SIZE
SIZE SoilFont::get_char_size_from_font_size(int font_size)
{
    if(font_size < 0){font_size = 0;}
    if(font_size >= FONT_SIZE_N){font_size = FONT_SIZE_N - 1;}

    //FONT_CHAR_W/H is the size of the biggest font size
    //which is the base for calculating every other
    double w = (double)FONT_CHAR_W;
    double h = (double)FONT_CHAR_H;
    SIZE res;
    int s = FONT_SIZE_N - 1;
    while(s > font_size)
    {
        w *= 0.9;
        h *= 0.9;

        s--;
    }
    res.w = round_double(w);
    res.h = round_double(h);
    return res;
}
