#ifndef SOILLAYOUT_H
#define SOILLAYOUT_H

#include "soiluielm.h"

class SoilLayout : public SoilUIElm
{
public:

    friend class SoilEngine;

    //MARKER:this is temporary, find a way to call for recursive reload ui methods from outside
    friend class SoilColorButton;
    friend class SoilDropMenu;

    //MARKER: make these protected
    ALIGNMENT_TYPE alignment;
    //
    SoilUIElm* first;
    //MARKER: check if last is really necessary
    SoilUIElm* last;
    int elm_n;

    //stores the total min/max for the elements within
    ////////////// PUBLIC METHODS
    SoilLayout(LAYOUT_TYPE t,ALIGNMENT a);
    SoilLayout();
    void init();
    bool add_elm(SoilUIElm* e, int pos);
    bool add_elm(SoilUIElm* e);
    bool add_elm_before(SoilUIElm* ins_e, SoilUIElm* ref_e);
    bool add_elm_after(SoilUIElm* ins_e, SoilUIElm* ref_e);
    bool remove_elm(SoilUIElm* e);
    SIZE set_size(int w, int h);

    LAYOUT_TYPE get_layout_type();
    ALIGNMENT get_alignment();

    void set_layout_type(LAYOUT_TYPE t);
    void set_alignment(ALIGNMENT a);
    void recursive_print_ext(SoilPixmap* px, SOIL_RECT dest_rect);
    void recursive_print_elm_area();

protected:
    LAYOUT_TYPE layout_type;
    bool is_main_layout;
    bool is_modal_layout;
    //set true inside SoilEngine::update_main_layout_from_window()
    //so it doesn't call SoilEngine::update_window_from_main_layout()
    bool updated_by_window;
    int IN_PAD;
    int OUT_PAD;

    //auto_size_n accounts for the number of elm:
    //AUTO_SIZE.w if this is HORIZONTAL
    //AUTO_SIZE.h if this is VERTICAL
    int auto_elm_n;
    //////////////// PRIVATE METHODS
    LAYOUT_TYPE validate_layout_type(LAYOUT_TYPE t);
    ALIGNMENT validate_alignment(ALIGNMENT a);

    bool check_no_looped_nesting(SoilUIElm* elm);

    void set_as_main_layout();
    void set_as_modal_layout();
    void recursive_validate_for_execution(int MAX_W, int MAX_H, int MIN_W, int MIN_H);
    void recursive_set_engine_properties(SoilPixmap* d, SoilPixmap* a, int pix_d, SoilEngineControl* eng_c, SoilFont* f, SoilControl* ext_c);
    void auto_elm_count();
    void set_size_limits_by_content();
    void update_content_size();
    void recursive_load_auto_sizes(int free_space_w, int free_space_h,bool do_all);
    void load_auto_sizes(int free_space_w, int free_space_h);
    void recursive_set_positions(bool do_all);
    void set_positions();
    void recursive_set_elm_window_area(bool do_all);
    void set_elm_window_area(SoilUIElm* elm);
    virtual void recursive_print(bool do_all);
    virtual void print();

    void recursive_process_mouse_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    //virtual POINT move_content_view(int mx, int my);
    void recursive_print_log(bool first_to_print);
};

#endif // SOILLAYOUT_H
