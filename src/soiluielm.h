#ifndef SOILUIELM_H
#define SOILUIELM_H

//#include "global_definitions.h"
#include "soilobject.h"
//#include "soilrect.h"
#include "soilenginecontrol.h"
#include "soilcontrol.h"
#include "soilfont.h"
#include "soilstring.h"


//scrollbar motion values
#define SCR_MOV 4
#define SCR_BIG_MOV 20
#define SCR_INTERVAL 10
#define SCR_MIN_BAR_LEN 20


class SoilUIElm : public SoilObject
{
public:
    friend class SoilLayout;
    friend class SoilEngine;
    //MARKER: delete "friend class UserControl;"
    friend class UserControl;
    friend class SoilColorButton;



    std::string log_nest;//test var

    //////////////////////////// LAYOUT POLICIES

    //MARKER:make these protected, and set setter and getter methods
    SIZE_POLICY AUTO_SIZE;
    //////////////////////////// SOIL SIGNAL
    ////////////////////////methods
    SoilUIElm(UI_TYPE t);
    virtual ~SoilUIElm();
    int get_id();
    SIZE get_view_size();
    SIZE get_content_size();
    virtual SIZE set_size(int w,int h);
    virtual SIZE set_min_size(int w,int h);
    virtual SIZE set_max_size(int w,int h);
    virtual void set_auto_size(bool auto_w, bool auto_h);
    virtual void set_visible(bool v);
    virtual void activate_hover_elm(bool open);
    bool is_focusable();
    bool is_visible();
    bool in_hover();//if this is within the current hover layout(elm)
    virtual SoilUIElm* get_next_elm(bool only_focusable);
    virtual SoilUIElm* get_prev_elm(bool only_focusable);

protected:

    //////////////////////// SIGNALS

    
    //These are used to signal external_control
    //and trigger events in your program
    SOIL_SIGNAL_STRUCT SIGNALS[SOIL_SIGNAL_N];

    //////////////////////// LAYOUT PROPERTIES
    UI_TYPE ui_type;
    SIZE ABSOLUTE_VIEW_MIN_SIZE;
    SIZE ABSOLUTE_VIEW_MAX_SIZE;
    SIZE view_min_size;
    SIZE view_max_size;
    bool on_window;//if part of its area is on window
    SIZE view_size;//total size this elm occupies in layout
    SIZE content_size;
    SOIL_RECT view_rect;
    BOUND_RECT view_bound;
    SOIL_RECT content_view_rect;//rect relative to content
    //content_view_rect in SoilLayout is the view without scroll bars
    BOUND_RECT content_view_bound;
    SOIL_RECT rel_pos_rect;//position relative to the parent layout content
    SOIL_RECT win_cut_rect;//rect displayed on the window relative to its own view
    SOIL_RECT win_pos_rect;//win_cut_rect relative to the window
    SOIL_RECT abs_pos_rect;//total area realtive to the window
    //BOUND_RECT versions of the variables above
    BOUND_RECT rel_pos_bound;
    BOUND_RECT win_cut_bound;
    BOUND_RECT win_pos_bound;
    BOUND_RECT abs_pos_bound;
    //for SoilLayout::load_auto_size()
    SIZE auto_calc_size;
    //////////////////////////// PARENT PROPERTIES
    SoilUIElm* parent;
    SoilUIElm* child;
    SoilUIElm* next;
    SoilUIElm* prev;
    //////////////////////////// SOIL TIMED FLAGS
    OBJ_FLAG scroll_move_flag;
    //////////////////////////// ENGINE / EXTERNAL PROPERTIES
    bool added;
    bool dest_pixmap_is_set;
    bool arrows_pixmap_is_set;
    bool engine_control_is_set;
    bool external_control_is_set;
    ////////////////////////ENGINE PROPERTIES

    int id;//starts at 1
    static int next_id;
    bool engine_running;
    int pixel_density;
    SoilPixmap* dest_pixmap;
    SoilPixmap* arrows_pixmap;
    SOIL_RECT arrows_px_rect[8];
    SoilEngineControl* engine_control;
    SoilFont* font;
    SoilControl* external_control;

    bool focusable;
    bool respond_tab_move_focus;


    ////////////////////////HOVER ELEMENT / WIDGET PROPERTIES
    //bool hover_elm;
    SoilUIElm* hover_elm;
    bool hover_open;

/*
    SIZE hover_win_size;
    POINT hover_win_rel_start;

    SOIL_RECT hover_win_pos_rect;
    SOIL_RECT hover_win_cut_rect;

    BOUND_RECT hover_win_pos_bound;
    BOUND_RECT hover_win_cut_bound;
    */

    ////////////////////////EVENT PROPERTIES
    //the status for its ui update:
    //0: do nothing
    //1: just reprint
    //2: recalculate its area on window and reprint
    //3: full reload, recalculate size, position and the number 2 above
    int elm_ui_flag;
    bool visible;

    bool focus_elm;
    bool mouse_left_down_elm;
    bool mouse_wheel_down_elm;
    bool mouse_over_elm;

    //value where mouse_left_down_elm was pressed
    int mouse_left_down_elm_x;
    int mouse_left_down_elm_y;

    //Holds the current mouse coords when the cursor is over the elm
    int mouse_over_elm_x;
    int mouse_over_elm_y;

    //coord for the pressed element relative to the elm view
    int mouse_wheel_down_elm_x;
    int mouse_wheel_down_elm_y;

    //these are for keeping how much the parent layouts' view moved in remote_mouse_motion with mouse wheel down
    //it will restore the position of the wheel in this elm only upon ui_reloaded()
    //because if the parent view moves, the cursor doesn't move relative to this view, but until reload_ui()
    //is called mouse_wheel_down_elm_x/y has to move with the cursor
    int mouse_wheel_down_mx;
    int mouse_wheel_down_my;

    /////////////////////////////// SCROLLBAR / CONTENT PROPERTIES
    //bigger_content is for when the content_size.h is bigger than the view_size.h
    int content_pad;

    bool bigger_content_w;
    bool bigger_content_h;
    //scroll_w/h if the scrollbar is active
    //if scroll_w/h is true more_content_w/h is also true
    bool scrollable;
    bool scroll_w;
    bool scroll_h;


    int arrow_w;

    /////////////////////////// SCROLLBAR POSITION
    // the ratio between content_view_rect.x/y and content_size.w/h
    // this is used to keep the proportional scollbar position
    // when the content_size changes
    //double content_view_rect_ratio_x;
    //double content_view_rect_ratio_y;

    /////////////////////////// RATIO VARIABLES
    //the ratio between the content_view and the content_size
    double scroll_w_view_ratio;
    double scroll_h_view_ratio;
    //and the ratio between the scrollbar gap and the content_size
    double scroll_w_gap_ratio;
    double scroll_h_gap_ratio;

    // 1 / scroll_w/h_gap_ratio
    double scroll_w_gap_inv_ratio;
    double scroll_h_gap_inv_ratio;

    //////////////////////// SCROLLBAR HOLD BAR VARIABLES
    // when you mouse down and hold on the scrollbar bar
    bool scroll_w_bar_hold;
    bool scroll_h_bar_hold;

    //coord for the point where mouse down on the scrollbar
    //relative to the scrollbar, relative to view
    int scroll_hold_x;
    int scroll_hold_y;

    //////////////////////// SCROLLBAR SECTIONS VARIABLES
    int scroll_w_motion_value[5];
    int scroll_h_motion_value[5];

    //elm view coords for two arrows, scroll_bar
    //and two gaps between arrows and scrollbar
    //there are five BOUND_RECT in each
    BOUND_RECT scroll_w_bounds[5];
    BOUND_RECT scroll_h_bounds[5];
    //there are five SOIL_RECT in each
    SOIL_RECT scroll_w_rects[5];
    SOIL_RECT scroll_h_rects[5];

    //the same as above but for window coordinates
    //SOIL_RECT scroll_w_win_rect[5];
    //SOIL_RECT scroll_h_win_rect[5];
    //BOUND_RECT scroll_w_win_bound[5];
    //BOUND_RECT scroll_h_win_bound[5];
    ////////////////////////////////////// METHODS
    void init();
    void init_signals();

    //virtual void internal_set_size(int w, int h,bool update_properties);
    //void preset_size(int w, int h);
    //set_auto_calc_size() returns the diference between the new
    //auto_calc_size and the previous one
    SIZE set_auto_calc_size(int w, int h);

    bool internal_set_size(int w, int h);

    void set_elm_ui_flag(int f);

    void set_content_size(int w, int h);

    void set_dest_pixmap(SoilPixmap* d);
    void set_arrows_pixmap(SoilPixmap* a);
    void set_pixel_density(int pix_d);
    void set_engine_control(SoilEngineControl* eng_c);
    virtual void set_font(SoilFont* f);
    void set_external_control(SoilControl *ext_c);
    virtual void set_engine_properties(SoilPixmap* d, SoilPixmap *a, int pix_d, SoilEngineControl *eng_c, SoilFont* f, SoilControl* ext_c);
    virtual void reset_base_elm_area(SOIL_RECT new_rel_pos_rect, SOIL_RECT new_win_pos_rect, SOIL_RECT new_win_cut_rect);
    virtual void reset_base_elm_area();
    virtual void set_hover_elm_area(SIZE win_size, SIZE anchor_view_size, SOIL_RECT anchor_win_pos_rect, SOIL_RECT anchor_win_cut_rect);

    virtual void validate_for_execution();
    void update_from_size();


    void convert_win_to_view_coord(int wx, int wy, int* vx, int* vy);

    void ui_reloaded();

    void print_log();
    void print_elm_area();
    /////////////////////////////////////// EVENTS METHODS
    //process_...() calls uielm_...()
    //ui_elm...() does the basic ui things for SoilUIElm
    //uielm_...() calls the virtual function that can be inherited
    void process_text_input_event(SDL_Event E);
    void text_input_event(SDL_Event E);
    
    void process_keydown_event(SDL_Event E);
    void keydown_event(SDL_Event E);
    
    void process_keyup_event(SDL_Event E);
    void keyup_event(SDL_Event E);

    void process_mouse_event(int X, int Y,int in_view_x, int in_view_y, SDL_Event E);
    void mouse_left_down_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_left_up_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_left_click_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_wheel_down_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_wheel_up_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_wheel_roll_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_motion_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_remote_motion_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_enter_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    void mouse_leave_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);

    void set_as_focus_elm(bool f);
    void set_as_mouse_left_down_elm(bool f);

    /////////////////////////////////////// VIRTUAL INHERITABLE METHODS
    virtual SIZE set_size_virt(int w, int h);

    virtual void text_input_event_virt(SDL_Event E);
    virtual void keydown_event_virt(SDL_Event E);
    virtual void keyup_event_virt(SDL_Event E);

    virtual void mouse_left_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_left_up_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_left_click_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_wheel_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_wheel_up_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_wheel_roll_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_remote_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_enter_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_leave_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);

    virtual void set_as_focus_elm_virt(bool f);
    virtual void set_as_mouse_left_down_elm_virt(bool f);

    virtual void recursive_print(bool do_all);
    virtual void print();

    //////////////////////////////////

    virtual void process_flag(int value);
    void init_flag(OBJ_FLAG* flag);
    void load_mouse_coord_to_signal_arg(SOIL_SIGNAL_STRUCT * sig,Uint8 button,int X,int Y,int in_view_x,int in_view_y);

    /////////////////////////////////////// SCROLLBAR METHODS
    //void init_scrollbar_properties();
    bool calc_scrollbar_properties();//returns true if scroll_w/h changed
    void scrollbar_bounds_to_rects();
    void correct_content_view_coord();
    void print_scrollbars();

    void scroll_w_mouse_left_down_event(int in_view_x, int in_view_y, SDL_Event E);
    void scroll_w_mouse_left_up_event(int in_view_x, int in_view_y, SDL_Event E);
    void scroll_w_mouse_motion_event(int in_view_x, int in_view_y, SDL_Event E);

    void scroll_h_mouse_left_down_event(int in_view_x, int in_view_y, SDL_Event E);
    void scroll_h_mouse_left_up_event(int in_view_x, int in_view_y, SDL_Event E);
    void scroll_h_mouse_motion_event(int in_view_x, int in_view_y, SDL_Event E);

    void scroll_h_wheel(int dir);
    void check_scroll_move_mouse_motion();
    void adjust_content_view_to_fit_rect(SOIL_RECT rect, bool x_to_start, bool y_to_start);
    //void process_scroll_w_event(int X, int Y, SDL_Event E);
    //void process_scroll_h_event(int X, int Y, SDL_Event E);
    void move_scrollbar_w(int m);
    void move_scrollbar_h(int m);
    //return actual amount moved in POINT for x and y
    virtual POINT move_content_view(int mx, int my);
};

#endif // SOILUIELM_H
