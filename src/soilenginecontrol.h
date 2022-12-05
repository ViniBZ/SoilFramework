#ifndef SOILENGINECONTROL_H
#define SOILENGINECONTROL_H
#include "soilobject.h"

//the idea is to use this to insert pointers to the ELM_FLAG within the elements
//SoilEngine would go through this dynamic list getting the flags and porcesisng them
//this class doesn't have any reference to what SoilUIElm is so engine will get
//to the elm by the id and an id indexed element array
class SoilEngineControl : public SoilObject
{
public:
    //////////////// TIMED FLAGS (ELM_FLAG)
    SIZE window_size;

    ELM_FLAG* first_EF;
    
    ELM_FLAG* last_EF;
    ELM_FLAG* previous_EF;
    ELM_FLAG* current_EF;
    int EF_n;

    //////////////// SOIL SIGNALS

    SOIL_SIGNAL_STRUCT* first_SS;
    
    SOIL_SIGNAL_STRUCT* last_SS;
    SOIL_SIGNAL_STRUCT* previous_SS;
    SOIL_SIGNAL_STRUCT* current_SS;
    int SS_n;
    


    //////////////// GENERAL EVENT CONTROL FLAGS
    //0: nothing
    //1,2,3: reload_ui. this is the opt for SoilEngine::reload_ui(int opt)
    int ui_flag;
    
    SoilUIElm* main_layout;

    SoilUIElm* focus_elm;
    SoilUIElm* ahead_focus_elm;
    SoilUIElm* mouse_left_down_elm;
    SoilUIElm* mouse_wheel_down_elm;
    SoilUIElm* mouse_over_elm;

    SoilUIElm* anchor_elm;
    SoilUIElm* hover_elm;

    int focus_elm_id;
    int ahead_focus_elm_id;
    int mouse_left_down_elm_id;
    int mouse_wheel_down_elm_id;
    int mouse_over_elm_id;

    int anchor_elm_id;
    int hover_elm_id;

    //the scrollable layout lowest in level to the mouse_over_elm
    int next_scrollable_elm;
    SoilEngineControl();
    void set_main_layout(SoilUIElm* m);
    //////////////// TIMED FLAGS METHODS
    void push_EF(ELM_FLAG* f);
    ELM_FLAG* next_EF();
    void pop_EF();//pop last seen flag
    void reset_EF();

    //////////////// SOIL SIGNALS METHODS
    void push_SS(SOIL_SIGNAL_STRUCT* s);
    SOIL_SIGNAL_STRUCT* next_SS();
    void pop_SS();//pop last seen flag
    void reset_SS();


    //////////////// UI FLAG
    //ui_flag here refers to this classe's ui_flag

    void set_ui_flag(int uf);
    int pop_ui_flag();

    //////////////// GENERAL EVENT CONTROL FLAGS METHODS
    void set_ahead_focus_elm(SoilUIElm* elm, int elm_id);
    void set_focus_elm(SoilUIElm* elm, int elm_id);
    void set_next_focus_elm();
    void set_mouse_left_down_elm(SoilUIElm* elm, int elm_id);
    void set_mouse_wheel_down_elm(SoilUIElm* elm, int elm_id);
    void set_mouse_over_elm(SoilUIElm* elm, int elm_id);

    void set_anchor_hover_elm(SoilUIElm* a_elm, int a_elm_id, SoilUIElm* h_elm, int h_elm_id);

    void set_window_size(int win_size_w, int win_size_h);
    void set_window_size(SIZE win_size);
    SIZE get_window_size();

};

#endif // SOILENGINECONTROL_H
