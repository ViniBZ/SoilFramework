#include "soilslider.h"

SoilSlider::SoilSlider() : SoilWidget(SLIDER)
{
    
    update_draw = false;
    out_limit_x = false;
    
    ABSOLUTE_VIEW_MIN_SIZE.w = 2 * HANDLE_W;
    ABSOLUTE_VIEW_MIN_SIZE.h = HANDLE_H + 8 + RULER_H;
    ABSOLUTE_VIEW_MAX_SIZE.w = 1500;
    ABSOLUTE_VIEW_MAX_SIZE.h = ABSOLUTE_VIEW_MIN_SIZE.h;

    set_min_size(view_min_size.w, view_min_size.h);
    set_max_size(view_max_size.w, view_max_size.h);

    set_auto_size(false,false);

    range.min = 0;
    range.max = 10;
    seg_n = (range.max - range.min);
    range_size = seg_n + 1;

    value = range.max / 2;

    handle_rect.y = 0;
    handle_rect.w = HANDLE_W;
    handle_rect.h = HANDLE_H;
    //handle_rect.x is setted in set set_handle_rect_from_value()

    handle_pressed = false;
    ruler_visible = false;

    //auto_set_ruler should initiate tick_n and big_tick_int
    auto_set_ruler();

    draw_slider_handle();
    set_size(150, HANDLE_H + 18);

    update_draw = true;
    show_ruler(true);
}

//---------------------------------------------- SET SIZE
SIZE SoilSlider::set_size_virt(int w,int h)
{
    update_draw = false;
    pixmap.alloc(view_size.w,view_size.h);
    pixmap.spx_fill(0xFF000000);

    line_w = view_size.w - HANDLE_W;
    line_w_half = line_w / 2;

    ruler_px.alloc(line_w, RULER_H);
    ruler_px.spx_fill(0xFF000000);

    ratio_seg_w = (double)seg_n / line_w;
    ratio_w_seg = (double)line_w /seg_n;

    set_ruler(tick_n,big_tick_int);

    set_handle_rect_from_value();
    update_draw = true;
    draw();

    return view_size;

}
//---------------------------------------------- SET VALUE
void SoilSlider::set_value(int v)
{
    
    if(v < range.min){v = range.min;}
    if(v > range.max){v = range.max;}
    if(v == value){return;}

    int old_v = value;

    value = v;
    set_handle_rect_from_value();
    draw();
    

    if(SIGNALS[VALUE_CHANGED].active && external_control_is_set)
    {
        SIGNALS[VALUE_CHANGED].arguments.gen_arg = value;
        //external_control->soil_signal_response((SoilUIElm*)this, SIGNALS[VALUE_CHANGED].arguments,SIGNALS[VALUE_CHANGED].response);
        engine_control->push_SS(&SIGNALS[VALUE_CHANGED]);
    }
}
//----------------------------------------------- GET VALUE
int SoilSlider::get_value()
{
    return value;
}
//----------------------------------------------- SET RANGE
void SoilSlider::set_range(int min, int max)
{
    //if(min < 0){min = 0;}
    //if(max < 1){max = 1;}
    if(max < min){max = min + 1;}

    if(min == range.min && max == range.max){return;}

    range.min = min;
    range.max = max;

    seg_n = (range.max - range.min);
    range_size = seg_n + 1;

    ratio_seg_w = (double)seg_n / line_w;
    ratio_w_seg = (double)line_w /seg_n;

    int new_v = value;
    if(value < range.min){new_v = range.min;}
    if(value > range.max){new_v = range.max;}
    
    set_value(value);

    //
    if(SIGNALS[RANGE_CHANGED].active && external_control_is_set)
    {
        SIGNALS[RANGE_CHANGED].arguments.range_changed_arg.new_range_min = range.min;
        SIGNALS[RANGE_CHANGED].arguments.range_changed_arg.new_range_max = range.max;
        //external_control->soil_signal_response((SoilUIElm*)this, SIGNALS[RANGE_CHANGED].arguments,SIGNALS[RANGE_CHANGED].response);
        engine_control->push_SS(&SIGNALS[RANGE_CHANGED]);
    }
}
//----------------------------------------------- GET VALUE
RANGE SoilSlider::get_range()
{
    return range;
}
//---------------------------------------------- SHOW RULER
void SoilSlider::show_ruler(bool s)
{
    ruler_visible = s;
    draw();
}
//---------------------------------------------- SET RULER
void SoilSlider::set_ruler(int n, int big_int)
{
    if(n < 0){n = 0;}
    //this line is for the exact amount of ticks that can fit
    //beneath line_w

    tick_n = n;

    if(n > (line_w_half) + (line_w % 2)){n = (line_w_half) + (line_w % 2);}
    int tmp_tick_n = n;
    //MARKER: make it so even if tick_n has to be adjusted
    //for being too many for line_w it still holds the actual
    //value passed as argument for when the size w increases it
    //can show the ruler with the right amount of ticks

    big_tick_int = big_int;

    //----------------------------

    if(!ruler_px.loaded){return;}
    ruler_px.spx_fill(0xFF000000);

    double tick_w = (double)line_w / (tmp_tick_n - 1);
    double next_tick_x;

    POINT start;
    start.x = 0;
    start.y = 2;

    next_tick_x = start.x;

    int end_y;

    int t = 0;
    while(t < tmp_tick_n)
    {
        if(t % big_tick_int == 0)
        {
            end_y = RULER_H;
        }else{
            end_y = RULER_H / 2;
        }
        ruler_px.spx_draw_v_line(start,end_y,0xFFFFFFFF);

        next_tick_x += tick_w;
        start.x = next_tick_x;
        t++;
    }
    draw();
}
//---------------------------------------------- AUTO SET RULER
void SoilSlider::auto_set_ruler()
{
    int val = range_size - 1;
    int d=val;
    int b=val;
    //finds the smaller int divider for range_size-1
    while(d>1)
    {
        if(val%d==0){b=d;}
        d--;
    }
    set_ruler(range_size,b);
}
//---------------------------------------------- GET VALUE FROM X
void SoilSlider::set_value_from_x(int x)
{
    
    //send the coord IN the widget, not the window value
    x -= HANDLE_W_HALF;    
    if(x <= 0 && value == range.min){return;}
    if(x >= line_w && value == range.max){return;}

    double val_d = (double)x * ratio_seg_w;
    val_d += range.min;
    double mod = double_to_next_int(val_d,true);
    if(mod < 0.5){val_d += 1;}
    int val = val_d;

    set_value(val);
}
//---------------------------------------------- SET HANDLE RECT FROM VALUE
void SoilSlider::set_handle_rect_from_value()
{


    int value_offset = value - range.min;
    double line_x_offset = (double)value_offset * ratio_w_seg;
    handle_rect.x = line_x_offset;

}


//---------------------------------------------- PROCESS KEYDOWN EVENT
void SoilSlider::keydown_event_virt(SDL_Event E)
{
    if(E.key.keysym.sym == SDLK_LEFT || E.key.keysym.sym == SDLK_RIGHT)
    {
        int inc = 1;
        if(E.key.keysym.sym == SDLK_LEFT){inc = (-1);}
        set_value(value + inc);
    }
    if(E.key.keysym.sym == SDLK_HOME)
    {
        set_value(range.min);
    }
    if(E.key.keysym.sym == SDLK_END)
    {
        set_value(range.max);
    }
}
//---------------------------------------------- MOUSE LEFT DOWN EVENT
void SoilSlider::mouse_left_down_event_virt(int X, int Y,int in_view_x, int in_view_y, SDL_Event E)
{
    if(point_in_rect(handle_rect,in_view_x,in_view_y))
    {
        handle_pressed = true;
        draw();
    }

}
//---------------------------------------------- MOUSE REMOTE MOTION EVENT
void SoilSlider::mouse_remote_motion_event_virt(int X, int Y,int in_view_x, int in_view_y, SDL_Event E)
{
    if(handle_pressed)
    {
        
        //MARKER: maybe just have in_view_x somehow

        set_value_from_x(in_view_x);
    }
}

//---------------------------------------------- SET AS MOUSE LEFT DOWN ELM
void SoilSlider::set_as_mouse_left_down_elm_virt(bool f)
{
    if(!f)
    {
        handle_pressed = false;
        draw();
    }
}

//---------------------------------------------- VALIDATE FOR EXECUTION
void SoilSlider::validate_for_execution()
{
    SoilUIElm::validate_for_execution();
    draw();

}

//---------------------------------------------- DRAW SLIDER HANDLE
void SoilSlider::draw_slider_handle()
{

    //MARKER: This is where a customized handle image would come
    if(!handle_px[0].loaded)
    {
        handle_px[0].alloc(HANDLE_W,HANDLE_H);
        handle_px[0].spx_fill(0xFF000000);
    }

    if(!handle_px[1].loaded)
    {
        handle_px[1].alloc(HANDLE_W,HANDLE_H);
        handle_px[1].spx_fill(0xFF000000);
    }

    POINT start;
    start.x = HANDLE_W / 2;
    start.y = HANDLE_W;

    int end_y = HANDLE_H;

    handle_px[0].spx_draw_v_line(start,end_y,0xFFFFFFFF);
    handle_px[1].spx_draw_v_line(start,end_y,0xFFFFFFFF);

    handle_rect.x = 0;
    handle_rect.y = 0;
    handle_rect.w = HANDLE_W;
    handle_rect.h = HANDLE_W;
    
    handle_px[0].spx_draw_outline_rect(handle_rect,1,0xFFFFFFFF);

    SOIL_RECT pressed_handle_rect;
    pressed_handle_rect.x = 1;
    pressed_handle_rect.y = 1;
    pressed_handle_rect.w = HANDLE_W-2;
    pressed_handle_rect.h = HANDLE_W-2;

    handle_px[1].spx_draw_solid_rect(pressed_handle_rect,0xFFFFFFFF);

    handle_rect.h = HANDLE_H;
}

//---------------------------------------------- DRAW
void SoilSlider::draw()
{
    if(!update_draw){return;}
    if(!pixmap.loaded){return;}
    pixmap.spx_fill(0xFF000000);

    POINT h_line_start;
    h_line_start.y = HANDLE_H + 1;
    h_line_start.x = HANDLE_W / 2;

    //int end_x = view_size.w - (h_line_start.x);
    int end_x = h_line_start.x + line_w;

    SOIL_RECT outline_rect;
    outline_rect.x = 0;
    outline_rect.y = 0;
    outline_rect.w = view_size.w;
    outline_rect.h = view_size.h;

    pixmap.spx_draw_h_line(h_line_start,end_x,0xFFFFFFFF);
    h_line_start.y++;
    pixmap.spx_draw_h_line(h_line_start,end_x,0xFFFFFFFF);
    h_line_start.y++;
    pixmap.spx_draw_h_line(h_line_start,end_x,0xFFFFFFFF);

    //------------------------------ handle
    if(handle_pressed)
    {
        pixmap.spx_paste_pixmap(handle_px[1],handle_px[1].get_full_rect(),handle_rect);
    }else{
        pixmap.spx_paste_pixmap(handle_px[0],handle_px[0].get_full_rect(),handle_rect);
    }


    //MARKER: apparently spx_draw_solid_rect()
    //and spx_draw_h_line have different ways to handle coord limits
    //better revise all limits
    //------------------------------ ruler
    if(ruler_visible)
    {
        SOIL_RECT ruler_rect;
        ruler_rect.x = HANDLE_W / 2;
        ruler_rect.y = HANDLE_H + 4;
        ruler_rect.w = line_w;
        ruler_rect.h = RULER_H;


        //pixmap.spx_draw_solid_rect(ruler_rect,0xFFFF00FF);
        pixmap.spx_paste_pixmap(ruler_px,ruler_px.get_full_rect(),ruler_rect);

    }
    if(engine_control_is_set)
    {
        elm_ui_flag = 3;
        engine_control->set_ui_flag(3);
    }

}
