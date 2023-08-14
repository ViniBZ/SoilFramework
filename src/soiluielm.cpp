#include "soiluielm.h"
int SoilUIElm::next_id=1;

// ------------------------------------------------ PUBLIC METHODS

SoilUIElm::SoilUIElm(UI_TYPE t)
{
    ui_type = t;
    init();
}

SoilUIElm::~SoilUIElm()
{

}
//--------------------------------------------------- GET ID
int SoilUIElm::get_id()
{
    return id;
}
//--------------------------------------------------- GET VIEW SIZE
SIZE SoilUIElm::get_view_size()
{
    return view_size;
}
//--------------------------------------------------- GET CONTENT SIZE
SIZE SoilUIElm::get_content_size()
{
    return content_size;
}
//--------------------------------------------------- GET CONTENT VIEW RECT
SOIL_RECT SoilUIElm::get_content_view_rect()
{
    return content_view_rect;
}
//--------------------------------------------------- SET SIZE
// this is the one the user will call in his code
SIZE SoilUIElm::set_size(int w,int h)
{
    //MARKER: I'm writing this here on sizes but it applies to every property.
    //make sure they have the right access type since this is made for
    //development.
    //For example here we have view_size. It should never be changed directly
    //but only by set_size()


    //set_size(w,h) is called from its parent SoilLayout
    //in the SoilLayout::load_auto_sizes() method for updating all variables
    //that depend on view_size, including scrollbar ones

    bool res = internal_set_size(w,h);
    set_size_virt(w,h);

    return view_size;
}
//--------------------------------------------------- SET MIN SIZE
SIZE SoilUIElm::set_min_size(int w,int h)
{
    bool changed = false;

    if(w < 0){w = view_min_size.w;}
    if(h < 0){h = view_min_size.h;}

    if(w < ABSOLUTE_VIEW_MIN_SIZE.w){w = ABSOLUTE_VIEW_MIN_SIZE.w;}
    if(w > ABSOLUTE_VIEW_MAX_SIZE.w){w = ABSOLUTE_VIEW_MAX_SIZE.w;}
    if(w != view_min_size.w){changed = true;}
    view_min_size.w = w;

    if(h < ABSOLUTE_VIEW_MIN_SIZE.h){h = ABSOLUTE_VIEW_MIN_SIZE.h;}
    if(h > ABSOLUTE_VIEW_MAX_SIZE.h){h = ABSOLUTE_VIEW_MAX_SIZE.h;}
    if(h != view_min_size.h){changed = true;}
    view_min_size.h = h;

    ////////////////////////////////////////////// SETTING FLAG
    if(changed && engine_running && engine_control_is_set)
    {
        //MARKER: MAYBE PUT THIS SET_SIZE TO RUN EVEN BEFORE ENGINE IS RUNNING
        set_size(-1,-1);
        if(AUTO_SIZE.w || AUTO_SIZE.h)
        {
            set_elm_ui_flag(3);
        }
    }
    return view_min_size;

}
//--------------------------------------------------- SET MAX SIZE
SIZE SoilUIElm::set_max_size(int w,int h)
{
    bool changed = false;

    if(w < 0){w = view_max_size.w;}
    if(h < 0){h = view_max_size.h;}

    if(w < ABSOLUTE_VIEW_MIN_SIZE.w){w = ABSOLUTE_VIEW_MIN_SIZE.w;}
    if(w > ABSOLUTE_VIEW_MAX_SIZE.w){w = ABSOLUTE_VIEW_MAX_SIZE.w;}
    if(w != view_max_size.w){changed = true;}
    view_max_size.w = w;

    if(h < ABSOLUTE_VIEW_MIN_SIZE.h){h = ABSOLUTE_VIEW_MIN_SIZE.h;}
    if(h > ABSOLUTE_VIEW_MAX_SIZE.h){h = ABSOLUTE_VIEW_MAX_SIZE.h;}
    if(h != view_max_size.h){changed = true;}
    view_max_size.h = h;

    ////////////////////////////////////////////// SETTING FLAG
    if(changed && engine_running && engine_control_is_set)
    {
        set_size(-1,-1);
        //MARKER: changing the limit sizes also affect the layout if the size changed, not only being AUTO
        if(AUTO_SIZE.w || AUTO_SIZE.h)
        {
            set_elm_ui_flag(3);
        }
    }
    return view_max_size;
}
//--------------------------------------------------- SET AUTO SIZE
void SoilUIElm::set_auto_size(bool auto_w, bool auto_h)
{
    //This is called to keep track of the sizes being
    //stablished in SoilLayout::load_auto_sizes(int,int);

    if(engine_running)
    {
       //return;
    }
    bool changed = false;
    if(AUTO_SIZE.w != auto_w || AUTO_SIZE.h != auto_h){changed = true;}

    AUTO_SIZE.w = auto_w;
    AUTO_SIZE.h = auto_h;
    
    if(changed && engine_running && engine_control_is_set)
    {
        set_elm_ui_flag(3);
    }
}
//------------------------------------------------------------- SET VISIBLE
void SoilUIElm::set_visible(bool v)
{
    visible = v;
    set_elm_ui_flag(3);
}
//------------------------------------------------------------- ACTIVATE HOVER ELM
void SoilUIElm::activate_hover_elm(bool open)
{
    if(!engine_running || !engine_control_is_set || hover_elm == NULL){return;}
    hover_open = open;
    if(open)
    {
        SoilUIElm* anchor_elm_pt = this;
        engine_control->set_anchor_hover_elm(anchor_elm_pt, id, hover_elm, hover_elm->get_id());

    }else{
        engine_control->set_anchor_hover_elm(NULL, 0, NULL, 0);    
    }

    set_elm_ui_flag(3);
}
//------------------------------------------------------------- IS FOCUSABLE
bool SoilUIElm::is_focusable()
{
    return focusable;
}
//------------------------------------------------------------- IS VISIBLE
bool SoilUIElm::is_visible()
{
    return visible;
}
//------------------------------------------------------------- IN HOVER
bool SoilUIElm::in_hover()
{
    //if this is within the current hover layout(elm)

    if(!engine_control_is_set || engine_control == NULL)
    {
        return false;
    }else{
        if(engine_control->hover_elm == NULL){return false;}
    }
    SoilUIElm* parent_pt = this;
    while(parent_pt != NULL)
    {
        if(parent_pt->id == engine_control->hover_elm_id){return true;}
        parent_pt = parent_pt->parent;
    }
    return false;

}

//------------------------------------------------------------- GET NEXT ELM
SoilUIElm* SoilUIElm::get_next_elm(bool only_focusable)
{
    if(engine_control == NULL){return NULL;}

    SoilUIElm* parent_elm = parent;
    SoilUIElm* next_elm = next;
    bool got_next = false;

    int count_control = 0;

    while(!got_next && count_control < 1000)
    {
        if(next_elm == NULL)
        {
            if(parent_elm == NULL)
            {
                next_elm = engine_control->main_layout->child;
                parent_elm = engine_control->main_layout;
            }else{

                next_elm = parent_elm->next;
                parent_elm = parent_elm->parent;
            }
        }else{
            if(next_elm->id == id)
            {
                next_elm = NULL;
                break;
            }
            if(next_elm->ui_type == WIDGET)
            {
                if(next_elm->focusable)
                {
                    got_next = true;
                    break;
                }else{
                    next_elm = next_elm->next;
                }

            }else{
                parent_elm = next_elm;
                next_elm = next_elm->child;
            }
        }  
        count_control++;
    }
    
    return next_elm;

}

//------------------------------------------------------------- GET PREV ELM
SoilUIElm* SoilUIElm::get_prev_elm(bool only_focusable)
{
  if(engine_control == NULL){return NULL;}

    SoilUIElm* parent_elm = parent;
    SoilUIElm* prev_elm = prev;
    bool got_prev = false;

    SoilUIElm* aux_elm;

    int count_control = 0;

    while(!got_prev && count_control < 1000)
    {
        if(prev_elm == NULL)
        {
            if(parent_elm == NULL)
            {
                //MARKER:MAKE last_child var to not have to use this loop to reach the last elm
                aux_elm = engine_control->main_layout->child;
                while(aux_elm != NULL)
                {
                    if(aux_elm->next == NULL)
                    {
                        break;
                    }
                    aux_elm = aux_elm->next;
                }
                prev_elm = aux_elm;
                parent_elm = engine_control->main_layout;
            }else{

                prev_elm = parent_elm->prev;
                parent_elm = parent_elm->parent;
            }
        }else{
            if(prev_elm->id == id)
            {
                prev_elm = NULL;
                break;
            }
            if(prev_elm->ui_type == WIDGET)
            {
                if(prev_elm->focusable)
                {
                    got_prev = true;
                    break;
                }else{
                    prev_elm = prev_elm->prev;
                }

            }else{
                parent_elm = prev_elm;
                //MARKER:MAKE last_child var to not have to use this loop to reach the last elm
                aux_elm = prev_elm->child;
                while(aux_elm != NULL)
                {
                    if(aux_elm->next == NULL)
                    {
                        break;
                    }
                    aux_elm = aux_elm->next;
                }
                prev_elm = aux_elm;
            }
        }  
        count_control++;
    }
    
    return prev_elm;
}
//------------------------------------------------------------- PROTECTED METHODS

//MARKER:if set_size is called, can the user get the actual size the
//elm will have before SoilEngine::reload_ui() is processed
//--------------------------------------------------- INIT
void SoilUIElm::init()
{
    log_nest = ">";
    id = next_id;
    next_id++;

    //SET EVENT PROPERTIES TO FALSE
    engine_running = false;
    focus_elm = false;
    mouse_left_down_elm = false;
    mouse_over_elm = false;
    mouse_wheel_down_elm = false;

    on_window = false;
    visible = true;

    view_size.w = 100;
    view_size.h = 100;
    content_pad = 0;

    //MARKER: set the sizes according to the display DPI
    ABSOLUTE_VIEW_MIN_SIZE.w = 0;
    ABSOLUTE_VIEW_MIN_SIZE.h = 0;
    ABSOLUTE_VIEW_MAX_SIZE.w = 2000;
    ABSOLUTE_VIEW_MAX_SIZE.h = 1000;

    view_min_size.w = ABSOLUTE_VIEW_MIN_SIZE.w;
    view_min_size.h = ABSOLUTE_VIEW_MIN_SIZE.h;
    view_max_size.w = ABSOLUTE_VIEW_MAX_SIZE.w;
    view_max_size.h = ABSOLUTE_VIEW_MAX_SIZE.h;

    content_size.w = 0;
    content_size.h = 0;

    view_rect.x = 0;
    view_rect.y = 0;
    view_rect.w = view_size.w;
    view_rect.h = view_size.h;
    view_bound = rect_to_bound(view_rect);

    content_view_rect.x = view_rect.x;
    content_view_rect.y = view_rect.y;
    content_view_rect.w = view_rect.w - (2*content_pad);
    content_view_rect.h = view_rect.h - (2*content_pad);
    content_view_bound = rect_to_bound(content_view_rect);

    rel_pos_rect.x = 0;
    rel_pos_rect.y = 0;
    rel_pos_rect.w = view_size.w;
    rel_pos_rect.h = view_size.h;

    reset_rect(&win_cut_rect);
    reset_rect(&win_pos_rect);
    reset_rect(&abs_pos_rect);

    rel_pos_bound = rect_to_bound(rel_pos_rect);
    reset_bound(&win_cut_bound);
    reset_bound(&win_pos_bound);
    reset_bound(&abs_pos_bound);



    added = false;
    dest_pixmap_is_set = false;
    arrows_pixmap_is_set = false;
    engine_control_is_set = false;
    external_control_is_set = false;
    dest_pixmap = NULL;
    arrows_pixmap = NULL;
    engine_control = NULL;
    font = NULL;
    external_control = NULL;
    pixel_density = 0;

    focusable = true;
    respond_tab_move_focus = true;

    //hover_elm = false;
    hover_elm = NULL;
    hover_open = false;

    parent = NULL;
    child = NULL;
    prev = NULL;
    next = NULL;

    set_auto_size(false,false);

    //USER_RESIZABLE.w = false;
    //USER_RESIZABLE.h = false;

    /////////////////////////////// scroll properties
    bigger_content_w = false;
    bigger_content_h = false;
    scrollable = false;
    scroll_w = false;
    scroll_h = false;

    //content_view_rect_ratio_x = 0;//content_view_rect.x
    //content_view_rect_ratio_y = 0;

    reset_rect(&scroll_w_rects[0]);
    reset_rect(&scroll_w_rects[1]);
    reset_rect(&scroll_w_rects[2]);
    reset_rect(&scroll_w_rects[3]);
    reset_rect(&scroll_w_rects[4]);

    reset_bound(&scroll_w_bounds[0]);
    reset_bound(&scroll_w_bounds[1]);
    reset_bound(&scroll_w_bounds[2]);
    reset_bound(&scroll_w_bounds[3]);
    reset_bound(&scroll_w_bounds[4]);

    reset_rect(&scroll_h_rects[0]);
    reset_rect(&scroll_h_rects[1]);
    reset_rect(&scroll_h_rects[2]);
    reset_rect(&scroll_h_rects[3]);
    reset_rect(&scroll_h_rects[4]);

    reset_bound(&scroll_h_bounds[0]);
    reset_bound(&scroll_h_bounds[1]);
    reset_bound(&scroll_h_bounds[2]);
    reset_bound(&scroll_h_bounds[3]);
    reset_bound(&scroll_h_bounds[4]);

    scroll_w_bar_hold = false;
    scroll_h_bar_hold = false;

    scroll_w_motion_value[0] = (-SCR_MOV);
    scroll_w_motion_value[1] = (-SCR_BIG_MOV);
    scroll_w_motion_value[2] = 0;//this has no use except padding
    scroll_w_motion_value[3] = SCR_BIG_MOV;
    scroll_w_motion_value[4] = SCR_MOV;

    scroll_h_motion_value[0] = (-SCR_MOV);
    scroll_h_motion_value[1] = (-SCR_BIG_MOV);
    scroll_h_motion_value[2] = 0;//this has no use except padding
    scroll_h_motion_value[3] = SCR_BIG_MOV;
    scroll_h_motion_value[4] = SCR_MOV;
    //////////////////////////////////////////// SIGNALS
    init_signals();// I was not calling this because of the struct contstructor

    elm_ui_flag = 0;
    //////////////////////////////////////////// FLAGS
    init_flag(&scroll_move_flag);
    scroll_move_flag.interval = SCR_INTERVAL;//time interval


}

//--------------------------------------------------- INIT SIGNALS
void SoilUIElm::init_signals()
{
    int i = 0;
    while(i < SOIL_SIGNAL_N)
    {
        SIGNALS[i].signal = (SOIL_SIGNAL)i;
        SIGNALS[i].elm = (SoilUIElm*)this;
        SIGNALS[i].active = false;
        i++;
    }


}

//--------------------------------------------------- SET AUTO SIZE CALC
SIZE SoilUIElm::set_auto_calc_size(int w, int h)
{
    SIZE ret;
    ret.w = 0;
    ret.h = 0;

    if(w >= 0)
    {
        if(w < view_min_size.w){w = view_min_size.w;}
        if(w > view_max_size.w){w = view_max_size.w;}

        ret.w = w - auto_calc_size.w;
        auto_calc_size.w = w;
    }
    if(h >= 0)
    {
        if(h < view_min_size.h){h = view_min_size.h;}
        if(h > view_max_size.h){h = view_max_size.h;}

        ret.h = h - auto_calc_size.h;
        auto_calc_size.h = h;
    }
    return ret;
}
//--------------------------------------------------- INTERNAL SET SIZE
bool SoilUIElm::internal_set_size(int w, int h)
{
    if(w < 0){w = view_size.w;}
    if(h < 0){h = view_size.h;}

    bool changed = false;

    if(w < view_min_size.w){w = view_min_size.w;}
    if(w > view_max_size.w){w = view_max_size.w;}
    if(w != view_size.w)
    {
        changed = true;
        view_size.w = w;
    }

    if(h < view_min_size.h){h = view_min_size.h;}
    if(h > view_max_size.h){h = view_max_size.h;}
    if(h != view_size.h)
    {
        changed = true;
        view_size.h = h;
    }

    auto_calc_size.w = view_size.w;
    auto_calc_size.h = view_size.h;

    // FLAGS FOR THE RELOAD UI
    if(changed)
    {
        if(engine_running && engine_control_is_set)
        {
            set_elm_ui_flag(3);
            update_from_size();

            if(SIGNALS[SIZE_CHANGED].active && external_control_is_set)
            {

                //external_control->soil_signal_response((SoilUIElm*)this,SIGNALS[SIZE_CHANGED].arguments,SIGNALS[SIZE_CHANGED].response);
                SIGNALS[SIZE_CHANGED].arguments.size_changed_arg.size.w = view_size.w;
                SIGNALS[SIZE_CHANGED].arguments.size_changed_arg.size.h = view_size.h;
                engine_control->push_SS(&SIGNALS[SIZE_CHANGED]);

                //external_control->soil_signal_response((SoilUIElm*)this,size_changed.arguments,size_changed.response);
            }
        }
    }
    return true;

}
//--------------------------------------------------- SET ELM UI FLAG
void SoilUIElm::set_elm_ui_flag(int f)
{
    if(f > elm_ui_flag)
    {
        elm_ui_flag = f;
        if(f == 3 && parent != NULL)
        {
            //parent->set_elm_ui_flag(3);
            parent->elm_ui_flag = 3;
        }
        if(engine_running && engine_control_is_set)
        {
            engine_control->set_ui_flag(f);
        }
    }
}
//--------------------------------------------------- SET CONTENT SIZE
void SoilUIElm::set_content_size(int w, int h)
{
    if(w >=0)
    {
        content_size.w = w;
    }
    if(h >=0)
    {
        content_size.h = h;
    }
    //this and set_size call calc_scroll_properties() because these
    //are the two properties that involve setting scorllbars or not
    //view_size and content_size
    if(scrollable)
    {
        bool scroll_changed = calc_scrollbar_properties();

        //if(scroll_changed)
        //{
            //MARKER: see if it will not confict when
            //this function is called from SoilLayout
            //in the update_content_size
            //during the process of reload_ui
            set_elm_ui_flag(2);
        //}


    }
}

//--------------------------------------------------- SET DEST PIXMAP
void SoilUIElm::set_dest_pixmap(SoilPixmap* d)
{

    if(engine_running)
    {
        //return;
    }
    if(d == NULL)
    {
        return;
    }
    dest_pixmap = d;
    dest_pixmap_is_set = true;
}
//--------------------------------------------------- SET ARROWS PIXMAP
void SoilUIElm::set_arrows_pixmap(SoilPixmap* a)
{

    if(engine_running)
    {
        //return;
    }
    //MARKER: might want to pass the rects as parameters...or not
    if(a == NULL)
    {
        return;
    }
    arrows_pixmap = a;
    arrows_pixmap_is_set = true;

    //these are 8 rects for the four directions on the top line
    //and the four directions with inverted colors on the second line
    arrows_px_rect[0].x = 0;
    arrows_px_rect[0].y = 0;
    arrows_px_rect[0].w = arrows_pixmap->size.w / 4;
    arrows_px_rect[0].h = arrows_pixmap->size.h / 2;

    ///////// DEFINING ARROW_W
    arrow_w = arrows_px_rect[0].w;

    int i = 1;
    int mod;
    while(i < 8)
    {
        arrows_px_rect[i] = arrows_px_rect[0];
        mod = i % 4;
        if(mod != 0){arrows_px_rect[i].x = arrows_px_rect[0].w * mod;}
        if(i > 3){arrows_px_rect[i].y = arrows_px_rect[0].h;}
        i++;
    }

}
//--------------------------------------------------- SET ARROWS PIXMAP
void SoilUIElm::set_pixel_density(int pix_d)
{

    if(engine_running)
    {
        //return;
    }
    pixel_density = pix_d;

}
//--------------------------------------------------- SET ENGINE CONTROL
void SoilUIElm::set_engine_control(SoilEngineControl* eng_c)
{

    if(engine_running)
    {
        //return;
    }
    if(eng_c == NULL)
    {
        return;
    }

    engine_control = eng_c;
    engine_control_is_set = true;


}
//--------------------------------------------------- SET FONT
void SoilUIElm::set_font(SoilFont* f)
{
    font = f;
}

//--------------------------------------------------- SET EXTERNAL CONTROL
void SoilUIElm::set_external_control(SoilControl* ext_c)
{

    if(engine_running)
    {
        //return;
    }
    if(ext_c == NULL)
    {
        return;
    }

    external_control = ext_c;
    external_control_is_set = true;

}
//--------------------------------------------------- SET ENGINE PROPERTIES
void SoilUIElm::set_engine_properties(SoilPixmap* d, SoilPixmap *a, int pix_d, SoilEngineControl *eng_c, SoilFont* f, SoilControl* ext_c)
{
    set_dest_pixmap(d);
    set_arrows_pixmap(a);
    set_pixel_density(pix_d);
    set_engine_control(eng_c);
    set_font(f);
    set_external_control(ext_c);
}
//--------------------------------------------------- RESET BASE ELM AREA
void SoilUIElm::reset_base_elm_area(SOIL_RECT new_rel_pos_rect, SOIL_RECT new_win_pos_rect, SOIL_RECT new_win_cut_rect)
{
    rel_pos_rect = new_rel_pos_rect;
    rel_pos_bound = rect_to_bound(rel_pos_rect);
    win_pos_rect = new_win_pos_rect;
    win_pos_bound = rect_to_bound(win_pos_rect);
    win_cut_rect = new_win_cut_rect;
    win_cut_bound = rect_to_bound(win_cut_rect);
}
//--------------------------------------------------- RESET BASE ELM AREA
void SoilUIElm::reset_base_elm_area()
{
    reset_base_elm_area(SOIL_RECT{0,0,view_size.w,view_size.h}, SOIL_RECT{0,0,view_size.w,view_size.h}, SOIL_RECT{0,0,view_size.w,view_size.h});
}
//--------------------------------------------------- SET HOVER ELM AREA
void SoilUIElm::set_hover_elm_area(SIZE win_size, SIZE anchor_view_size, SOIL_RECT anchor_win_pos_rect, SOIL_RECT anchor_win_cut_rect)
{
    SOIL_RECT win_rect;
    win_rect.x = 0;
    win_rect.y = 0;
    win_rect.w = win_size.w;
    win_rect.h = win_size.h;

    //anchor_corners is relative to the window rect
    //0:top left
    //1: top right
    //2: bottom right
    //3: bottom left
    POINT anchor_corners[4];
    anchor_corners[0].x = anchor_win_pos_rect.x - anchor_win_cut_rect.x;
    anchor_corners[0].y = anchor_win_pos_rect.y - anchor_win_cut_rect.y;

    anchor_corners[1].x = anchor_corners[0].x + anchor_view_size.w;
    anchor_corners[1].y = anchor_corners[0].y;

    anchor_corners[2].x = anchor_corners[1].x;
    anchor_corners[2].y = anchor_corners[1].y + anchor_view_size.h;

    anchor_corners[3].x = anchor_corners[0].x;
    anchor_corners[3].y = anchor_corners[0].y + anchor_view_size.h;

    SOIL_RECT hover_rect;

    hover_rect.x = anchor_corners[3].x;
    hover_rect.y = anchor_corners[3].y;
    hover_rect.w = view_size.w;
    hover_rect.h = view_size.h;

    SOIL_RECT hover_win_pos_rect = rect_intersection(win_rect, hover_rect);
    

}
//--------------------------------------------------- VALIDATE FOR EXECUTION
// here engine_running is set to true
void SoilUIElm::validate_for_execution()
{

    //MARKER: make the exception system so if something goes wrong it can
    // signal way back to SoilEngine and abort the program, or not?
    //I don't know if that is really necessary given how solid this logic
    //should be, for example with SoilEngineControl being set in all
    //the elements that would use it
    if(engine_running)
    {
        //return;
    }
    if(hover_elm != NULL)
    {
        //MARKER: this would be better to be here.... but the hover_elm which is a SoilLayout will need to 
        //call recursive_validate_for_execution()...
        //hover_elm->validate_for_execution();
    }
    //correcting limit sizes
    if(view_max_size.w < view_min_size.w)
    {
        view_max_size.w = view_min_size.w;
    }
    if(view_max_size.h < view_min_size.h)
    {
        view_max_size.h = view_min_size.h;
    }

    set_size(view_size.w,view_size.h);
    update_from_size();

    engine_running = true;

}

//--------------------------------------------------- UPDATE FROM SIZE
void SoilUIElm::update_from_size()
{


    //This method is meant to update all properties that depend on the
    //SoilUIElm 's view_size
    view_rect.w = view_size.w;
    view_rect.h = view_size.h;

    view_bound = rect_to_bound(view_rect);

    //MARKER:IN_PAD:
    content_view_rect.w = view_size.w - (2*content_pad);
    content_view_rect.h = view_size.h - (2*content_pad);
    content_view_bound = rect_to_bound(content_view_rect);
    set_content_size(-1,-1);

}

//--------------------------------------------------- CONVERT WIN TO VIEW COORD
void SoilUIElm::convert_win_to_view_coord(int wx, int wy, int* vx, int* vy)
{
    wx -= win_pos_bound.start_x;
    wx += win_cut_bound.start_x;

    wy -= win_pos_bound.start_y;
    wy += win_cut_bound.start_y;

    *vx = wx;
    *vy = wy;
}
//----------------------------------------------- UI RELOADED
void SoilUIElm::ui_reloaded()
{
    if(mouse_wheel_down_elm)
    {
        mouse_wheel_down_elm_x += mouse_wheel_down_mx;
        mouse_wheel_down_elm_y += mouse_wheel_down_my;
        mouse_wheel_down_mx = 0;
        mouse_wheel_down_my = 0;
    }
}
//----------------------------------------------- PRINT LOG
void SoilUIElm::print_log()
{

    printf("\n");
    printf("\n%s>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",log_nest.c_str());
    printf("\n%sSOILUIELM:: PRINTLOG() ID:%d ",log_nest.c_str(),id);
    printf("\n%s>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",log_nest.c_str());
    if(ui_type == LAYOUT)
    {
        printf("\n%sLAYOUT ID:%d ",log_nest.c_str(),id);
    }
    if(ui_type == WIDGET)
    {
        printf("\n%sWIDGET ID:%d ",log_nest.c_str(),id);
    }


    printf("\n%sview_size.w:%d ",log_nest.c_str(),view_size.w);
    printf("\n%sview_size.h:%d ",log_nest.c_str(),view_size.h);
    printf("\n%scontent_size.w:%d ",log_nest.c_str(),content_size.w);
    printf("\n%scontent_size.h:%d ",log_nest.c_str(),content_size.h);
    printf("\n%scontent_view_rect.x:%d ",log_nest.c_str(),content_view_rect.x);
    printf("\n%scontent_view_rect.y:%d ",log_nest.c_str(),content_view_rect.y);
    printf("\n%scontent_view_rect.w:%d ",log_nest.c_str(),content_view_rect.w);
    printf("\n%scontent_view_rect.h:%d ",log_nest.c_str(),content_view_rect.h);
    printf("\n%scontent_view_bound.start_x:%d ",log_nest.c_str(),content_view_bound.start_x);
    printf("\n%scontent_view_bound.start_y:%d ",log_nest.c_str(),content_view_bound.start_y);
    printf("\n%scontent_view_bound.end_x:%d ",log_nest.c_str(),content_view_bound.end_x);
    printf("\n%scontent_view_bound.end_y:%d ",log_nest.c_str(),content_view_bound.end_y);
    printf("\n");
    printf("\n%sview_min_size.w:%d ",log_nest.c_str(),view_min_size.w);
    printf("\n%sview_min_size.h:%d ",log_nest.c_str(),view_min_size.h);
    printf("\n%sview_max_size.w:%d ",log_nest.c_str(),view_max_size.w);
    printf("\n%sview_max_size.h:%d ",log_nest.c_str(),view_max_size.h);
    printf("\n");
    printf("\n%srel_pos_rect.x:%d ",log_nest.c_str(),rel_pos_rect.x);
    printf("\n%srel_pos_rect.y:%d ",log_nest.c_str(),rel_pos_rect.y);
    printf("\n%srel_pos_rect.w:%d ",log_nest.c_str(),rel_pos_rect.w);
    printf("\n%srel_pos_rect.h:%d ",log_nest.c_str(),rel_pos_rect.h);
    printf("\n%srel_pos_bound.start_x:%d ",log_nest.c_str(),rel_pos_bound.start_x);
    printf("\n%srel_pos_bound.start_y:%d ",log_nest.c_str(),rel_pos_bound.start_y);
    printf("\n%srel_pos_bound.end_x:%d ",log_nest.c_str(),rel_pos_bound.end_x);
    printf("\n%srel_pos_bound.end_y:%d ",log_nest.c_str(),rel_pos_bound.end_y);
    printf("\n");
    printf("\n%swin_cut_rect.x:%d ",log_nest.c_str(),win_cut_rect.x);
    printf("\n%swin_cut_rect.y:%d ",log_nest.c_str(),win_cut_rect.y);
    printf("\n%swin_cut_rect.w:%d ",log_nest.c_str(),win_cut_rect.w);
    printf("\n%swin_cut_rect.h:%d ",log_nest.c_str(),win_cut_rect.h);
    printf("\n%swin_cut_bound.start_x:%d ",log_nest.c_str(),win_cut_bound.start_x);
    printf("\n%swin_cut_bound.start_y:%d ",log_nest.c_str(),win_cut_bound.start_y);
    printf("\n%swin_cut_bound.end_x:%d ",log_nest.c_str(),win_cut_bound.end_x);
    printf("\n%swin_cut_bound.end_y:%d ",log_nest.c_str(),win_cut_bound.end_y);
    printf("\n");
    printf("\n%swin_pos_rect.x:%d ",log_nest.c_str(),win_pos_rect.x);
    printf("\n%swin_pos_rect.y:%d ",log_nest.c_str(),win_pos_rect.y);
    printf("\n%swin_pos_rect.w:%d ",log_nest.c_str(),win_pos_rect.w);
    printf("\n%swin_pos_rect.h:%d ",log_nest.c_str(),win_pos_rect.h);
    printf("\n%swin_pos_bound.start_x:%d ",log_nest.c_str(),win_pos_bound.start_x);
    printf("\n%swin_pos_bound.start_y:%d ",log_nest.c_str(),win_pos_bound.start_y);
    printf("\n%swin_pos_bound.end_x:%d ",log_nest.c_str(),win_pos_bound.end_x);
    printf("\n%swin_pos_bound.end_y:%d ",log_nest.c_str(),win_pos_bound.end_y);

    printf("\n");
    printf("\nSCROLLBAR W RECT / BOUND 0");
    printf("\n%sscroll_w_rects[0].x:%d ",log_nest.c_str(),scroll_w_rects[0].x);
    printf("\n%sscroll_w_rects[0].y:%d ",log_nest.c_str(),scroll_w_rects[0].y);
    printf("\n%sscroll_w_rects[0].w:%d ",log_nest.c_str(),scroll_w_rects[0].w);
    printf("\n%sscroll_w_rects[0].h:%d ",log_nest.c_str(),scroll_w_rects[0].h);
    printf("\n%sscroll_w_bounds[0].start_x:%d ",log_nest.c_str(),scroll_w_bounds[0].start_x);
    printf("\n%sscroll_w_bounds[0].start_y:%d ",log_nest.c_str(),scroll_w_bounds[0].start_y);
    printf("\n%sscroll_w_bounds[0].end_x:%d ",log_nest.c_str(),scroll_w_bounds[0].end_x);
    printf("\n%sscroll_w_bounds[0].end_y:%d ",log_nest.c_str(),scroll_w_bounds[0].end_y);

    printf("\n");
    printf("\nSCROLLBAR W RECT / BOUND 1");
    printf("\n%sscroll_w_rects[1].x:%d ",log_nest.c_str(),scroll_w_rects[1].x);
    printf("\n%sscroll_w_rects[1].y:%d ",log_nest.c_str(),scroll_w_rects[1].y);
    printf("\n%sscroll_w_rects[1].w:%d ",log_nest.c_str(),scroll_w_rects[1].w);
    printf("\n%sscroll_w_rects[1].h:%d ",log_nest.c_str(),scroll_w_rects[1].h);
    printf("\n%sscroll_w_bounds[1].start_x:%d ",log_nest.c_str(),scroll_w_bounds[1].start_x);
    printf("\n%sscroll_w_bounds[1].start_y:%d ",log_nest.c_str(),scroll_w_bounds[1].start_y);
    printf("\n%sscroll_w_bounds[1].end_x:%d ",log_nest.c_str(),scroll_w_bounds[1].end_x);
    printf("\n%sscroll_w_bounds[1].end_y:%d ",log_nest.c_str(),scroll_w_bounds[1].end_y);


    printf("\n");
    printf("\nSCROLLBAR W RECT / BOUND 2");
    printf("\n%sscroll_w_rects[2].x:%d ",log_nest.c_str(),scroll_w_rects[2].x);
    printf("\n%sscroll_w_rects[2].y:%d ",log_nest.c_str(),scroll_w_rects[2].y);
    printf("\n%sscroll_w_rects[2].w:%d ",log_nest.c_str(),scroll_w_rects[2].w);
    printf("\n%sscroll_w_rects[2].h:%d ",log_nest.c_str(),scroll_w_rects[2].h);
    printf("\n%sscroll_w_bounds[2].start_x:%d ",log_nest.c_str(),scroll_w_bounds[2].start_x);
    printf("\n%sscroll_w_bounds[2].start_y:%d ",log_nest.c_str(),scroll_w_bounds[2].start_y);
    printf("\n%sscroll_w_bounds[2].end_x:%d ",log_nest.c_str(),scroll_w_bounds[2].end_x);
    printf("\n%sscroll_w_bounds[2].end_y:%d ",log_nest.c_str(),scroll_w_bounds[2].end_y);


    printf("\n");
    printf("\nSCROLLBAR W RECT / BOUND 3");
    printf("\n%sscroll_w_rects[3].x:%d ",log_nest.c_str(),scroll_w_rects[3].x);
    printf("\n%sscroll_w_rects[3].y:%d ",log_nest.c_str(),scroll_w_rects[3].y);
    printf("\n%sscroll_w_rects[3].w:%d ",log_nest.c_str(),scroll_w_rects[3].w);
    printf("\n%sscroll_w_rects[3].h:%d ",log_nest.c_str(),scroll_w_rects[3].h);
    printf("\n%sscroll_w_bounds[3].start_x:%d ",log_nest.c_str(),scroll_w_bounds[3].start_x);
    printf("\n%sscroll_w_bounds[3].start_y:%d ",log_nest.c_str(),scroll_w_bounds[3].start_y);
    printf("\n%sscroll_w_bounds[3].end_x:%d ",log_nest.c_str(),scroll_w_bounds[3].end_x);
    printf("\n%sscroll_w_bounds[3].end_y:%d ",log_nest.c_str(),scroll_w_bounds[3].end_y);


    printf("\n");
    printf("\nSCROLLBAR W RECT / BOUND 4");
    printf("\n%sscroll_w_rects[4].x:%d ",log_nest.c_str(),scroll_w_rects[4].x);
    printf("\n%sscroll_w_rects[4].y:%d ",log_nest.c_str(),scroll_w_rects[4].y);
    printf("\n%sscroll_w_rects[4].w:%d ",log_nest.c_str(),scroll_w_rects[4].w);
    printf("\n%sscroll_w_rects[4].h:%d ",log_nest.c_str(),scroll_w_rects[4].h);
    printf("\n%sscroll_w_bounds[4].start_x:%d ",log_nest.c_str(),scroll_w_bounds[4].start_x);
    printf("\n%sscroll_w_bounds[4].start_y:%d ",log_nest.c_str(),scroll_w_bounds[4].start_y);
    printf("\n%sscroll_w_bounds[4].end_x:%d ",log_nest.c_str(),scroll_w_bounds[4].end_x);
    printf("\n%sscroll_w_bounds[4].end_y:%d ",log_nest.c_str(),scroll_w_bounds[4].end_y);


    printf("\n");
    printf("\nSCROLLBAR H RECT / BOUND 0");
    printf("\n%sscroll_h_rects[0].x:%d ",log_nest.c_str(),scroll_h_rects[0].x);
    printf("\n%sscroll_h_rects[0].y:%d ",log_nest.c_str(),scroll_h_rects[0].y);
    printf("\n%sscroll_h_rects[0].w:%d ",log_nest.c_str(),scroll_h_rects[0].w);
    printf("\n%sscroll_h_rects[0].h:%d ",log_nest.c_str(),scroll_h_rects[0].h);
    printf("\n%sscroll_h_bounds[0].start_x:%d ",log_nest.c_str(),scroll_h_bounds[0].start_x);
    printf("\n%sscroll_h_bounds[0].start_y:%d ",log_nest.c_str(),scroll_h_bounds[0].start_y);
    printf("\n%sscroll_h_bounds[0].end_x:%d ",log_nest.c_str(),scroll_h_bounds[0].end_x);
    printf("\n%sscroll_h_bounds[0].end_y:%d ",log_nest.c_str(),scroll_h_bounds[0].end_y);

    printf("\n");
    printf("\nSCROLLBAR H RECT / BOUND 1");
    printf("\n%sscroll_h_rects[1].x:%d ",log_nest.c_str(),scroll_h_rects[1].x);
    printf("\n%sscroll_h_rects[1].y:%d ",log_nest.c_str(),scroll_h_rects[1].y);
    printf("\n%sscroll_h_rects[1].w:%d ",log_nest.c_str(),scroll_h_rects[1].w);
    printf("\n%sscroll_h_rects[1].h:%d ",log_nest.c_str(),scroll_h_rects[1].h);
    printf("\n%sscroll_h_bounds[1].start_x:%d ",log_nest.c_str(),scroll_h_bounds[1].start_x);
    printf("\n%sscroll_h_bounds[1].start_y:%d ",log_nest.c_str(),scroll_h_bounds[1].start_y);
    printf("\n%sscroll_h_bounds[1].end_x:%d ",log_nest.c_str(),scroll_h_bounds[1].end_x);
    printf("\n%sscroll_h_bounds[1].end_y:%d ",log_nest.c_str(),scroll_h_bounds[1].end_y);

    printf("\n");
    printf("\nSCROLLBAR H RECT / BOUND 2");
    printf("\n%sscroll_h_rects[2].x:%d ",log_nest.c_str(),scroll_h_rects[2].x);
    printf("\n%sscroll_h_rects[2].y:%d ",log_nest.c_str(),scroll_h_rects[2].y);
    printf("\n%sscroll_h_rects[2].w:%d ",log_nest.c_str(),scroll_h_rects[2].w);
    printf("\n%sscroll_h_rects[2].h:%d ",log_nest.c_str(),scroll_h_rects[2].h);
    printf("\n%sscroll_h_bounds[2].start_x:%d ",log_nest.c_str(),scroll_h_bounds[2].start_x);
    printf("\n%sscroll_h_bounds[2].start_y:%d ",log_nest.c_str(),scroll_h_bounds[2].start_y);
    printf("\n%sscroll_h_bounds[2].end_x:%d ",log_nest.c_str(),scroll_h_bounds[2].end_x);
    printf("\n%sscroll_h_bounds[2].end_y:%d ",log_nest.c_str(),scroll_h_bounds[2].end_y);

    printf("\n");
    printf("\nSCROLLBAR H RECT / BOUND 3");
    printf("\n%sscroll_h_rects[3].x:%d ",log_nest.c_str(),scroll_h_rects[3].x);
    printf("\n%sscroll_h_rects[3].y:%d ",log_nest.c_str(),scroll_h_rects[3].y);
    printf("\n%sscroll_h_rects[3].w:%d ",log_nest.c_str(),scroll_h_rects[3].w);
    printf("\n%sscroll_h_rects[3].h:%d ",log_nest.c_str(),scroll_h_rects[3].h);
    printf("\n%sscroll_h_bounds[3].start_x:%d ",log_nest.c_str(),scroll_h_bounds[3].start_x);
    printf("\n%sscroll_h_bounds[3].start_y:%d ",log_nest.c_str(),scroll_h_bounds[3].start_y);
    printf("\n%sscroll_h_bounds[3].end_x:%d ",log_nest.c_str(),scroll_h_bounds[3].end_x);
    printf("\n%sscroll_h_bounds[3].end_y:%d ",log_nest.c_str(),scroll_h_bounds[3].end_y);

    printf("\n");
    printf("\nSCROLLBAR H RECT / BOUND 4");
    printf("\n%sscroll_h_rects[4].x:%d ",log_nest.c_str(),scroll_h_rects[4].x);
    printf("\n%sscroll_h_rects[4].y:%d ",log_nest.c_str(),scroll_h_rects[4].y);
    printf("\n%sscroll_h_rects[4].w:%d ",log_nest.c_str(),scroll_h_rects[4].w);
    printf("\n%sscroll_h_rects[4].h:%d ",log_nest.c_str(),scroll_h_rects[4].h);
    printf("\n%sscroll_h_bounds[4].start_x:%d ",log_nest.c_str(),scroll_h_bounds[4].start_x);
    printf("\n%sscroll_h_bounds[4].start_y:%d ",log_nest.c_str(),scroll_h_bounds[4].start_y);
    printf("\n%sscroll_h_bounds[4].end_x:%d ",log_nest.c_str(),scroll_h_bounds[4].end_x);
    printf("\n%sscroll_h_bounds[4].end_y:%d ",log_nest.c_str(),scroll_h_bounds[4].end_y);


    std::string bool_str;
    if(AUTO_SIZE.w){bool_str = "TRUE";}else{bool_str = "FALSE";}
    printf("\n%sAUTO_SIZE.w : %s",log_nest.c_str(),bool_str.c_str());
    if(AUTO_SIZE.h){bool_str = "TRUE";}else{bool_str = "FALSE";}
    printf("\n%sAUTO_SIZE.h : %s",log_nest.c_str(),bool_str.c_str());
    if(arrows_pixmap_is_set){bool_str = "TRUE";}else{bool_str = "FALSE";}
    printf("\n%sarrows_pixmap_is_set : %s",log_nest.c_str(),bool_str.c_str());
    if(scroll_w){bool_str = "TRUE";}else{bool_str = "FALSE";}
    printf("\n%sscroll w : %s",log_nest.c_str(),bool_str.c_str());
    if(scroll_h){bool_str = "TRUE";}else{bool_str = "FALSE";}
    printf("\n%sscroll h : %s",log_nest.c_str(),bool_str.c_str());
    if(on_window){bool_str = "TRUE";}else{bool_str = "FALSE";}
    printf("\n%son_window : %s",log_nest.c_str(),bool_str.c_str());

    printf("\n");
    printf("\n%s------------------------------------------",log_nest.c_str());
    printf("\n%sSOILUIELM::(END) PRINTLOG() ID:%d ",log_nest.c_str(),id);
    printf("\n%s------------------------------------------",log_nest.c_str());
}
void SoilUIElm::print_elm_area()
{
    printf("\n\nELM ID:%d  -  ",id);
    if(on_window)
    {
        printf("on window");
    }else{
        printf("NOT on window");
    }
    
    printf("\nREL . x: %d. y: %d. w: %d. h: %d.",rel_pos_rect.x,rel_pos_rect.y,rel_pos_rect.w,rel_pos_rect.h);
    printf("\nWPOS. x: %d. y: %d. w: %d. h: %d.",win_pos_rect.x,win_pos_rect.y,win_pos_rect.w,win_pos_rect.h);
    printf("\nWCUT. x: %d. y: %d. w: %d. h: %d.",win_cut_rect.x,win_cut_rect.y,win_cut_rect.w,win_cut_rect.h);
    printf("\nABS . x: %d. y: %d. w: %d. h: %d.",abs_pos_rect.x,abs_pos_rect.y,abs_pos_rect.w,abs_pos_rect.h);
    
}