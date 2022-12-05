#include "soiluielm.h"

// -------------------------------------------------------------- CALC SCROLLBAR BOUNDS
// the arrow_px properties shoud be initiated when calling this method
// -------------------------------------------------------------- CALC SCROLLBAR BOUNDS
//returns true if scroll_w/h changed
bool SoilUIElm::calc_scrollbar_properties()
{
    
    //MARKER: maybe make a scrollbar content lock system where if
    //the content shrinks and content_view_rect.x goes down(left) then the
    //content gets bigger the content_view_rect.x can restore the previous value
    //so it will keep the same part of the content in screen while it changes size

    //MARKER: fine check the rects values for pixel limits

    //view_rect here is the content_view_rect before being altered

    //this function calculates the coordinate area for the five parts of
    //a scrollbar: one arrow, the portion between the arrow and the bar,
    //the bar, the portion between the bar and the second arrow and
    //the second arrow
    //all relative to the view

    //content_view_bound = rect_to_bound(content_view_rect);

    content_view_rect.w = view_rect.w - (2*content_pad);
    content_view_rect.h = view_rect.h - (2*content_pad);

    bool ret = false;

    bool prev_scroll_w = scroll_w;
    bool prev_scroll_h = scroll_h;

    bigger_content_w = false;
    bigger_content_h = false;

    scroll_w = false;
    scroll_h = false;

    if(content_view_rect.w < (3 * arrow_w) || content_view_rect.h < (3 * arrow_w))
    {
        if(content_size.w > content_view_rect.w){bigger_content_w = true;}
        if(content_size.h > content_view_rect.h){bigger_content_h = true;}
        content_view_bound = rect_to_bound(content_view_rect);
        return false;
    }

    //checking if content is bigger than view
    if(content_size.w > content_view_rect.w)
    {
        scroll_w = true;
        content_view_rect.h -= arrow_w;

        if(content_size.h > content_view_rect.h)
        {
            scroll_h = true;
            content_view_rect.w -= arrow_w;
        }

    }else{
        if(content_size.h > content_view_rect.h)
        {
            scroll_h = true;
            content_view_rect.w -= arrow_w;
            if(content_size.w > content_view_rect.w)
            {
                scroll_w = true;
                content_view_rect.h -= arrow_w;
            }
        }
    }

    if(scroll_w){bigger_content_w = true;}
    if(scroll_h){bigger_content_h = true;}

    //CORRECT VIEW INFORMATION BEFORE GENERATING SCROLLBAR
    correct_content_view_coord();

    //MARKER: correct this, these are from inside the if below:

    int bar_gap;
    int bar_len;


    //HORIZONTAL ( SCROLL_W )
    if(scroll_w)
    {
        bar_gap = content_view_rect.w - (2 * arrow_w);

        //MARKER:careful with division by 0
        scroll_w_view_ratio = (double)content_view_rect.w / content_size.w;
        double bar_len_double = (double)scroll_w_view_ratio * bar_gap;

        bar_len = (int) bar_len_double + 1;
        if(bar_len < SCR_MIN_BAR_LEN){bar_len = SCR_MIN_BAR_LEN;}

        scroll_w_gap_ratio = (double)(bar_gap - bar_len) / (double)(content_size.w - content_view_rect.w);
        scroll_w_gap_inv_ratio = 1 / scroll_w_gap_ratio;
        double bar_pos_double = (double)scroll_w_gap_ratio * content_view_rect.x;
        int bar_pos = (int) bar_pos_double;

        //this +1 is for compensating some decimal rounding, that ends making
        //the right gap be no smaller than one pixel


        //bar position
        //int bar_pos = content_view_rect.x * bar_gap;
        //bar_pos /= content_size.w;

        //bar length
        //int bar_len = content_view_rect.w * bar_gap;
        //bar_len /= content_size.w;

        //PIXMAP COORDINATES

        //MARKER:IN_PAD:

        //scroll_w_px_rect holds the rect to print to texture_pixmap
        scroll_w_bounds[0].start_x = content_pad;
        scroll_w_bounds[0].start_y = content_view_rect.h + content_pad;
        scroll_w_bounds[0].end_x = arrow_w + content_pad;
        scroll_w_bounds[0].end_y = (content_view_rect.h + content_pad) + arrow_w;

        scroll_w_bounds[1] = scroll_w_bounds[0];
        scroll_w_bounds[1].start_x = scroll_w_bounds[0].end_x;
        scroll_w_bounds[1].end_x = scroll_w_bounds[1].start_x + bar_pos;

        scroll_w_bounds[2] = scroll_w_bounds[0];
        scroll_w_bounds[2].start_x = scroll_w_bounds[1].end_x;
        scroll_w_bounds[2].end_x = scroll_w_bounds[2].start_x + bar_len;

        scroll_w_bounds[3] = scroll_w_bounds[0];
        scroll_w_bounds[3].start_x = scroll_w_bounds[2].end_x;
        scroll_w_bounds[3].end_x = (content_view_rect.w + content_pad) - arrow_w;

        scroll_w_bounds[4] = scroll_w_bounds[0];
        scroll_w_bounds[4].start_x = scroll_w_bounds[3].end_x;
        scroll_w_bounds[4].end_x = (content_view_rect.w + content_pad);
    }

    //VERTICAL ( SCROLL_H )
    if(scroll_h)
    {
        bar_gap = content_view_rect.h - (2 * arrow_w);

        scroll_h_view_ratio = (double)content_view_rect.h / content_size.h;
        double bar_len_double = (double)scroll_h_view_ratio * bar_gap;

        bar_len = (int) bar_len_double + 1;
        if(bar_len < SCR_MIN_BAR_LEN){bar_len = SCR_MIN_BAR_LEN;}

        scroll_h_gap_ratio = (double)(bar_gap - bar_len) / (double)(content_size.h - content_view_rect.h);
        scroll_h_gap_inv_ratio = 1 / scroll_h_gap_ratio;
        double bar_pos_double = (double)scroll_h_gap_ratio * content_view_rect.y;
        int bar_pos = (int) bar_pos_double;

        //PIXMAP COORDINATES

        //MARKER:IN_PAD:

        //These are relative to the elm view
        scroll_h_bounds[0].start_x = content_view_rect.w + content_pad;
        scroll_h_bounds[0].start_y = content_pad;
        scroll_h_bounds[0].end_x = (content_view_rect.w + content_pad) + arrow_w;
        scroll_h_bounds[0].end_y = content_pad + arrow_w;

        scroll_h_bounds[1] = scroll_h_bounds[0];
        scroll_h_bounds[1].start_y = scroll_h_bounds[0].end_y;
        scroll_h_bounds[1].end_y = scroll_h_bounds[1].start_y + bar_pos;

        scroll_h_bounds[2] = scroll_h_bounds[0];
        scroll_h_bounds[2].start_y = scroll_h_bounds[1].end_y;
        scroll_h_bounds[2].end_y = scroll_h_bounds[2].start_y + bar_len;

        scroll_h_bounds[3] = scroll_h_bounds[0];
        scroll_h_bounds[3].start_y = scroll_h_bounds[2].end_y;
        scroll_h_bounds[3].end_y = (content_view_rect.h + content_pad) - arrow_w;

        scroll_h_bounds[4] = scroll_h_bounds[0];
        scroll_h_bounds[4].start_y = scroll_h_bounds[3].end_y;
        scroll_h_bounds[4].end_y = (content_view_rect.h + content_pad);
    }


    /////////////// UPDATE SCROLLBAR RECTS FROM THE NEWLY CALCULATED BOUNDS

    if(scroll_w != prev_scroll_w || scroll_h != prev_scroll_h)
    {
        ret = true;
    }

    scrollbar_bounds_to_rects();
    content_view_bound = rect_to_bound(content_view_rect);

    //printf_log();

    return ret;
}
// -------------------------------------------------------------- CONVERT SCROLLBAR BOUNDS TO RECTS
void SoilUIElm::scrollbar_bounds_to_rects()
{

    int i=0;
    while(i < 5)
    {
        scroll_w_rects[i] = bound_to_rect(scroll_w_bounds[i]);
        scroll_h_rects[i] = bound_to_rect(scroll_h_bounds[i]);
        i++;
    }

}

// -------------------------------------------------------------- CORRECT CONTENT VIEW COORD
void SoilUIElm::correct_content_view_coord()
{

    //MARKER:make it so it only gets to here if the layout is scrolled

    //this function corrects the content_view_rect.x/y position
    //for the situation where you scroll the content then increase
    //the size of the layout resulting in a empty space where the size
    //increased
    int excess = content_view_bound.end_x - content_size.w;
    if(excess > 0)
    {
        content_view_rect.x -= excess;
        if(content_view_rect.x < 0){content_view_rect.x = 0;}
    }

    excess = content_view_bound.end_y - content_size.h;
    if(excess > 0)
    {
        content_view_rect.y -= excess;
        if(content_view_rect.y < 0){content_view_rect.y = 0;}
    }
    content_view_bound = rect_to_bound(content_view_rect);


}


//--------------------------------------------------- PRINT SCROLLBARS
void SoilUIElm::print_scrollbars()
{
    if(!dest_pixmap_is_set){return;}

    if(scroll_w)
    {
        //scroll intersection
        SOIL_RECT SI[5];
        bool rect_on_screen[5];

        int s = 0;
        while(s < 5)
        {
            SI[s] = rect_intersection(scroll_w_rects[s],win_cut_rect);
            if(SI[s].w > 0 && SI[s].h > 0)
            {
                SI[s] = subtract_from_rect_pos(SI[s],win_cut_rect.x,win_cut_rect.y);
                SI[s] = add_to_rect_pos(SI[s],win_pos_rect.x,win_pos_rect.y);

                rect_on_screen[s] = true;

            }else{
                rect_on_screen[s] = false;
            }
            s++;
        }

        if(rect_on_screen[0])
        {
            dest_pixmap->spx_paste_pixmap(*arrows_pixmap,arrows_px_rect[1],SI[0]);
        }
        if(rect_on_screen[1])
        {
            dest_pixmap->spx_draw_solid_rect(SI[1],0xFF000000);
        }
        if(rect_on_screen[2])
        {
            //dest_pixmap->spx_draw_solid_rect(SI[2],0xFFFFFFFF);
            dest_pixmap->spx_draw_scroll_rect(SI[2],false,0);
        }
        if(rect_on_screen[3])
        {
            dest_pixmap->spx_draw_solid_rect(SI[3],0xFF000000);
        }
        if(rect_on_screen[4])
        {
            dest_pixmap->spx_paste_pixmap(*arrows_pixmap,arrows_px_rect[3],SI[4]);
        }
    }

    if(scroll_h)
    {

        //scroll intersection
        SOIL_RECT SI[5];
        bool rect_on_screen[5];
        int s = 0;
        while(s < 5)
        {
            SI[s] = rect_intersection(scroll_h_rects[s],win_cut_rect);
            if(SI[s].w > 0 && SI[s].h > 0)
            {
                SI[s] = subtract_from_rect_pos(SI[s],win_cut_rect.x,win_cut_rect.y);
                SI[s] = add_to_rect_pos(SI[s],win_pos_rect.x,win_pos_rect.y);
                rect_on_screen[s] = true;
            }else{
                rect_on_screen[s] = false;
            }
            s++;
        }
        if(rect_on_screen[0])
        {
            dest_pixmap->spx_paste_pixmap(*arrows_pixmap,arrows_px_rect[2],SI[0]);
        }
        if(rect_on_screen[1])
        {
            dest_pixmap->spx_draw_solid_rect(SI[1],0xFF000000);
        }
        if(rect_on_screen[2])
        {
            //dest_pixmap->spx_draw_solid_rect(SI[2],0xFFFFFFFF);
            dest_pixmap->spx_draw_scroll_rect(SI[2],false,0);
        }
        if(rect_on_screen[3])
        {
            dest_pixmap->spx_draw_solid_rect(SI[3],0xFF000000);
        }
        if(rect_on_screen[4])
        {
            dest_pixmap->spx_paste_pixmap(*arrows_pixmap,arrows_px_rect[0],SI[4]);
        }
    }

}
//--------------------------------------------- SCROLL MOUSE DOWN EVENT
void SoilUIElm::scroll_w_mouse_left_down_event(int in_view_x, int in_view_y, SDL_Event E)
{

    //if(!engine_control_is_set){return;} SHOULD I?
    //MARKER: maybe make it move not a constant in content_view_rect
    //but a value in the scrollbar measuring

    int i = 0;
    while(i < 5)
    {
        if(point_in_bound(scroll_w_bounds[i], in_view_x, in_view_y))
        {

            if(i == 2)
            {
                // bar
                scroll_w_bar_hold = true;
                scroll_hold_x = in_view_x;
                scroll_hold_y = in_view_y;


            }else{
                // arrow

                move_content_view(scroll_w_motion_value[i], 0);
                if(engine_control_is_set && !scroll_move_flag.in_control)
                {
                    scroll_move_flag.value = (i+1);//flag value
                    engine_control->push_EF(&scroll_move_flag);
                }
            }
            break;
        }
        i++;
    }

}
//--------------------------------------------- SCROLL MOUSE UP EVENT
void SoilUIElm::scroll_w_mouse_left_up_event(int in_view_x, int in_view_y, SDL_Event E)
{


}
//--------------------------------------------- SCROLL MOUSE MOTION EVENT
void SoilUIElm::scroll_w_mouse_motion_event(int in_view_x, int in_view_y, SDL_Event E)
{


}
//--------------------------------------------- SCROLL MOUSE DOWN EVENT
void SoilUIElm::scroll_h_mouse_left_down_event(int in_view_x, int in_view_y, SDL_Event E)
{

    //MARKER: maybe make it move not a constant in content_view_rect
    //but a value in the scrollbar measuring
    int i = 0;
    while(i < 5)
    {
        if(point_in_bound(scroll_h_bounds[i], in_view_x, in_view_y))
        {
            if(i == 2)
            {
                //bar
                scroll_h_bar_hold = true;
                scroll_hold_x = in_view_x;
                scroll_hold_y = in_view_y;
            }else{
                //arrow
                move_content_view(0, scroll_h_motion_value[i]);
                if(engine_control_is_set && !scroll_move_flag.in_control)
                {
                    scroll_move_flag.value = (i+6);//flag value
                    engine_control->push_EF(&scroll_move_flag);
                }
            }
            break;
        }
        i++;
    }


}
//--------------------------------------------- SCROLL MOUSE UP EVENT
void SoilUIElm::scroll_h_mouse_left_up_event(int in_view_x, int in_view_y, SDL_Event E)
{


}
//--------------------------------------------- SCROLL MOUSE MOTION EVENT
void SoilUIElm::scroll_h_mouse_motion_event(int in_view_x, int in_view_y, SDL_Event E)
{

}

//--------------------------------------------- SCROLL H WHEEL
void SoilUIElm::scroll_h_wheel(int dir)
{

    POINT motion;

    dir *= (-1) * SCR_BIG_MOV;

    SoilUIElm* elm = (SoilUIElm*) this;
    while(elm != NULL)
    {
        if(elm->scrollable && elm->bigger_content_h)
        {
            motion = elm->move_content_view(0,dir);
            if(motion.y != 0){break;}
        }
        elm = elm->parent;
    }
}
//--------------------------------------------- CHECK SCROLL MOVE MOUSE MOTION
void SoilUIElm::check_scroll_move_mouse_motion()
{

    //this method interrupt scrollbar movement if the mouse cursor
    //is not on the right part of the scroll area
    if(!mouse_over_elm)
    {
        scroll_move_flag.value = 0;
        return;
    }

    if(scroll_move_flag.value < 6)
    {
        if(!point_in_bound(scroll_w_bounds[scroll_move_flag.value - 1],mouse_over_elm_x,mouse_over_elm_y))
        {
            scroll_move_flag.value = 0;
        }
    }else{
        if(scroll_move_flag.value < 11)
        {
            if(!point_in_bound(scroll_h_bounds[scroll_move_flag.value - 6],mouse_over_elm_x,mouse_over_elm_y))
            {
                scroll_move_flag.value = 0;
            }
        }
    }

}
//--------------------------------------------- ADUST CONTENT VIEW TO FIT RECT
void SoilUIElm::adjust_content_view_to_fit_rect(SOIL_RECT rect, bool x_to_start, bool y_to_start)
{

    adjust_rect_to_px_size(content_size, &rect);
    int mov_x = 0;
    int mov_y = 0;

    int content_view_ex = content_view_rect.x + content_view_rect.w;
    int content_view_ey = content_view_rect.y + content_view_rect.h;

    int rect_ex = rect.x + rect.w;
    int rect_ey = rect.y + rect.h;

    if(rect.w > content_view_rect.w)
    {
        if(x_to_start)
        {
            if(rect.x < content_view_rect.x){mov_x = (-1)* (content_view_rect.x - rect.x);}
        }
    }

}
//--------------------------------------------- MOVE SCROLLBAR W
void SoilUIElm::move_scrollbar_w(int m)
{


    //m is the value in pixels the scrollbar has been moved
    //it still has to calculate the motion of the content_view
    if(m > 0 && m > scroll_w_rects[3].w)
    {
        m = scroll_w_rects[3].w;

    }
    if(m < 0 && (m*(-1)) > scroll_w_rects[1].w)
    {
        m = scroll_w_rects[1].w * (-1);
    }
    scroll_hold_x += m;

    double view_motion_double = (double)m * scroll_w_gap_inv_ratio;

    int view_motion = (int)view_motion_double;

    move_content_view(view_motion, 0);


}
//--------------------------------------------- MOVE SCROLLBAR H
void SoilUIElm::move_scrollbar_h(int m)
{

    //m is the value in pixels the scrollbar has been moved
    if(m > 0 && m > scroll_h_rects[3].h)
    {
        m = scroll_h_rects[3].h;

    }
    if(m < 0 && (m*(-1)) > scroll_h_rects[1].h)
    {
        m = scroll_h_rects[1].h * (-1);
    }
    scroll_hold_y += m;

    double view_motion_double = (double)m * scroll_h_gap_inv_ratio;

    int view_motion = (int)view_motion_double;
    move_content_view(0,view_motion);

}

//--------------------------------------------- MOVE CONTENT VIEW
POINT SoilUIElm::move_content_view(int mx, int my)
{

    //these are the four directions the content view position may move
    //in relation to the content and the control so it doesn't move
    //beyond the content size
    if(mx > 0)
    {
        int excess = (content_view_bound.end_x + mx) - content_size.w;
        if(excess > 0){mx -= excess;}

        content_view_rect.x += mx;
    }
    if(mx < 0)
    {
        int excess = content_view_rect.x + mx;
        if(excess < 0){mx -= excess;}
        content_view_rect.x += mx;
    }
    if(my > 0)
    {
        int orig_my = my;
        int excess = (content_view_bound.end_y + my) - content_size.h;
        if(excess > 0){my -= excess;}

        content_view_rect.y += my;

    }
    if(my < 0)
    {
        int excess = content_view_rect.y + my;
        if(excess < 0){my -= excess;}

        content_view_rect.y += my;
    }
    //remake scrollbar
    calc_scrollbar_properties();

    if(engine_running)
    {
        if(external_control_is_set && SIGNALS[CONTENT_VIEW_MOTION].active)
        {
            SIGNALS[CONTENT_VIEW_MOTION].arguments.content_view_arg.content_view_pos.x = content_view_rect.x;
            SIGNALS[CONTENT_VIEW_MOTION].arguments.content_view_arg.content_view_pos.y = content_view_rect.y;

            //external_control->soil_signal_response((SoilUIElm*)this,SIGNALS[CONTENT_VIEW_MOTION].arguments,SIGNALS[CONTENT_VIEW_MOTION].response);
            engine_control->push_SS(&SIGNALS[CONTENT_VIEW_MOTION]);
        }
        set_elm_ui_flag(2);
        engine_control->set_ui_flag(2);
    }


    ///////////////////// CALLBACK FOR MOVE CONTENT EVENT
    POINT ret;
    ret.x = mx;
    ret.y = my;

    return ret;

}
