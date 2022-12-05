#ifndef SOILPIXMAP_H
#define SOILPIXMAP_H

//#include "global_definitions.h"

#include "pixmap_functions.h"
#include "soilobject.h"
#include "soilpxmembound.h"

//this class is for 4 byte(32 bits) pixel format mostly
//but the image data can be in 1 byte(8 bits) with mono_mode


class SoilPixmap : public SoilObject
{
public:

    //MARKER: make a dynamic allocation system like the membounds for SoilPixmap 
    //so it doesnt have to be allocating all the time in SoilTextWidget for example

    bool loaded;
    bool mono_mode;
    bool color_mode;
    SIZE size;
    Uint32 * color_px_pt;
    unsigned char* mono_px_pt;

    int alloc_c;

    bool dynamic_bound;
    SoilPxMemBound px_mem_bound;

    SoilPixmap();
    ~SoilPixmap();
    bool alloc(int w,int h);
    bool alloc(SIZE s);
    bool alloc_from_file(const char* file_name);
    bool alloc_from_pixmap(const SoilPixmap& orig);//sets the color_mode to orig color_mode
    void internal_release();
    void release();

    void set_dynamic_bound(bool dynamic, int margin_w, int margin_h);

    bool set_to_mono_mode();
    bool set_to_color_mode();

    //soil pixmap functions
    bool spx_paint_pixel(int x, int y, PIXDATA color);
    bool spx_paint_pixel(int x, int y, Uint32 color);
    bool spx_paint_pixel(int x, int y, unsigned char color);

    unsigned char spx_uchar_pixel_at(int x, int y);
    Uint32 spx_uint32_pixel_at(int x, int y);
    PIXDATA spx_pixdata_pixel_at(int x, int y);

    void spx_fill(unsigned char color);
    void spx_fill(Uint32 color);
    void spx_fill(PIXDATA color);

    void spx_mask_fill(Uint32 color, Uint32 mask);

    void spx_paste_pixmap(const SoilPixmap& orig, SOIL_RECT orig_rect,SOIL_RECT paste_rect);
    void spx_resize_paste_pixmap(const SoilPixmap& orig, SOIL_RECT orig_rect,SOIL_RECT paste_rect);

    void spx_invert_colors(SOIL_RECT rect);
    void spx_invert_colors();
    void spx_alpha_blend(const SoilPixmap& orig, SOIL_RECT orig_rect,SOIL_RECT paste_rect);//on top of THIS px

    void spx_draw_h_line(POINT start, int end_x, unsigned char color);
    void spx_draw_h_line(POINT start, int end_x, Uint32 color);
    void spx_draw_v_line(POINT start, int end_y, unsigned char color);
    void spx_draw_v_line(POINT start, int end_y, Uint32 color);
    void spx_draw_cursor_line(POINT start, int end_y, unsigned char color);
    void spx_draw_cursor_line(POINT start, int end_y, Uint32 color);
    void spx_draw_outline_rect(SOIL_RECT rect,int bold, unsigned char color);
    void spx_draw_outline_rect(SOIL_RECT rect,int bold, Uint32 color);
    void spx_draw_solid_rect(SOIL_RECT rect, unsigned char color);
    void spx_draw_solid_rect(SOIL_RECT rect, Uint32 color);
    void spx_draw_patch_rect(SOIL_RECT rect, unsigned char color);
    void spx_draw_patch_rect(SOIL_RECT rect, Uint32 color);
    void spx_draw_scroll_rect(SOIL_RECT rect, bool dark, int border_disp);

    void spx_hsv_color_wheel(int hue, double sat, double val);
    void spx_color_wheel();
    void spx_color_square(Uint32 base_color);

    SOIL_RECT get_rect(int x, int y, int w, int h)const;
    SOIL_RECT get_full_rect()const;
    bool validate_rect_in_size(SOIL_RECT* rect);//validate in relation to this pixmap's size

    bool spx_save_img_file(std::string file_name,int format, int cmpr);
};

#endif // SOILPIXMAP_H
