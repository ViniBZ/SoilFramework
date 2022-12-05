#ifndef SOILSLIDER_H
#define SOILSLIDER_H

#include "soilwidget.h"

#define HANDLE_W 10
#define HANDLE_W_HALF HANDLE_W/2
#define HANDLE_H 20

#define RULER_H 8
//RULER_H is the height of the big tick, the small shall be half the big

class SoilSlider : public SoilWidget
{
public:

    SoilSlider();
    virtual SIZE set_size_virt(int w,int h);//
    void set_value(int v);//
    int get_value();
    void set_range(int min, int max);//
    RANGE get_range();

    void show_ruler(bool s);
    void set_ruler(int n, int big_int);
    void auto_set_ruler();

private:
    //the slider will have a number of possible values
    //raneg_size is that number
    //range_min and range_max included

    SoilPixmap handle_px[2];//0 is normal, 1 is pressed
    SOIL_RECT handle_rect;
    bool out_limit_x;

    int value;

    int seg_n;//number of segments, or in how many parts the slider bar is divided
    int range_size;//number of possible values (seg_n + 1)
    RANGE range;

    double ratio_w_seg;
    double ratio_seg_w;

    int line_w;
    int line_w_half;//int division

    bool handle_pressed;

    //MARKER: check to see if I'm following some standard for properties names
    SoilPixmap ruler_px;
    bool ruler_visible;
    int tick_n;
    int big_tick_int;

    bool update_draw;

    //----------------------------- METHODS


    void set_value_from_x(int x);
    void set_handle_rect_from_value();

    virtual void keydown_event_virt(SDL_Event E);
    virtual void mouse_left_down_event_virt(int X, int Y,int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_remote_motion_event_virt(int X, int Y,int in_view_x, int in_view_y, SDL_Event E);

    virtual void set_as_mouse_left_down_elm_virt(bool f);

    virtual void validate_for_execution();
    void draw_slider_handle();
    void draw();

};

#endif // SOILSLIDER_H
