#include "soilprogressbar.h"

SoilProgressBar::SoilProgressBar() : SoilWidget(PROGRESSBAR)
{
    value = 0;
    decimal_resolution = 0;

    value_str_pt = value_text.get_soil_string_pt();
    *value_str_pt = value;

    set_size(150, 25);
}
//---------------------------------------------- VALIDATE FOR EXECUTION
void SoilProgressBar::validate_for_execution()
{
    SoilUIElm::validate_for_execution();
    draw();
}
//---------------------------------------------- SET SIZE
SIZE SoilProgressBar::set_size(int w,int h)
{
    //MARKER: set better these sizes and its limits
    if(h != 50){h = 25;}
    int prev_w = view_size.w;
    SIZE new_size = SoilUIElm::set_size(w,h);

    if(prev_w != view_size.w)
    {
        pixmap.alloc(view_size.w, view_size.h);
        draw();
    }
    return new_size;
}
//---------------------------------------------- SET VALUE
void SoilProgressBar::set_value(int v)
{
    if(v < 0){v = 0;}
    if(v > 100){v = 100;}
    value = v;
    *value_str_pt = value;
    value_str_pt->append('%');
    draw();

    
}

//---------------------------------------------- SET VALUE
void SoilProgressBar::draw()
{
    if(!pixmap.loaded){return;}

    pixmap.spx_fill(0xFF000000);

    SOIL_RECT drawing_rect;
    drawing_rect.x = 1;
    drawing_rect.y = 1;
    drawing_rect.w = view_size.w - 2;
    drawing_rect.h = 23;

    pixmap.spx_draw_outline_rect(drawing_rect, 1, 0xFFFFFFFF);

    float float_view_w = (float)view_size.w;
    float_view_w -= 6;
    float float_value = (float)value;
    float float_value_w = (float_view_w / 100) * float_value;
    int value_w = (int)float_value_w;

    drawing_rect.x += 2;
    drawing_rect.y += 2;
    drawing_rect.w = value_w;
    drawing_rect.h -= 4;

    SIZE text_px_size = value_text.get_text_px_size(3);

    POINT text_offset;
    text_offset.x = (view_size.w / 2) - (text_px_size.w / 2);
    text_offset.y = 5;

    value_text.draw_full_px(&pixmap,text_offset, font, 3);
    pixmap.spx_invert_colors(drawing_rect);

    elm_ui_flag = 3;
    if(engine_control_is_set)
    {
        engine_control->set_ui_flag(3);
    }
}