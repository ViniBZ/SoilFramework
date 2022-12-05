#include "soilenginecontrol.h"

SoilEngineControl::SoilEngineControl()
{

    first_EF = NULL;
    last_EF = NULL;
    current_EF = NULL;
    previous_EF = NULL;
    EF_n = 0;


    first_SS = NULL;
    last_SS = NULL;
    current_SS = NULL;
    previous_SS = NULL;
    SS_n = 0;

    ui_flag = 0;

    main_layout = NULL;

    focus_elm = NULL;
    ahead_focus_elm = NULL;
    mouse_left_down_elm = NULL;
    mouse_wheel_down_elm = NULL;
    mouse_over_elm = NULL;

    hover_elm = NULL;

    focus_elm_id = 0;
    ahead_focus_elm_id = 0;
    mouse_left_down_elm_id = 0;
    mouse_wheel_down_elm_id = 0;
    mouse_over_elm_id = 0;

    hover_elm_id = 0;
}
//------------------------------------------------ SET MAIN LAYOUT
void SoilEngineControl::set_main_layout(SoilUIElm* m)
{
    main_layout = m;
}
//------------------------------------------------ PUSH
//just append by changing the next variable in the flags
//no new instance is created or memory allocated
void SoilEngineControl::push_EF(ELM_FLAG* f)
{
    
    if(f == NULL){return;}
    if(f->in_control){return;}
    if(EF_n == 0)
    {
        first_EF = f;
        last_EF = f;
        current_EF = f;

        first_EF->prev = NULL;
        first_EF->next = NULL;


    }else{

        f->next = NULL;
        f->prev = last_EF;
        last_EF->next = f;
        last_EF = f;

        if(current_EF == NULL){current_EF = last_EF;}
    }
    f->in_control = true;
    EF_n++;
}
//------------------------------------------------ NEXT FLAG
ELM_FLAG* SoilEngineControl::next_EF()
{

    if(current_EF == NULL)
    {
        return NULL;
    }
    previous_EF = current_EF;
    current_EF = current_EF->next;

    return previous_EF;
}
//------------------------------------------------ POP
void SoilEngineControl::pop_EF()
{

    if(previous_EF == NULL)
    {

        return;
    }
    previous_EF->in_control = false;
    if(previous_EF->prev == NULL)
    {
        first_EF = previous_EF->next;
    }else{
        previous_EF->prev->next = previous_EF->next;
    }
    if(previous_EF->next == NULL)
    {
        last_EF = previous_EF->prev;
    }else{
        previous_EF->next->prev = previous_EF->prev;
    }
    previous_EF = previous_EF->prev;
    EF_n--;

}
//------------------------------------------------ RESET
void SoilEngineControl::reset_EF()
{

    previous_EF = NULL;
    current_EF = first_EF;

}
//------------------------------------------------ PUSH
//just append by changing the next variable in the flags
//no new instance is created or memory allocated
void SoilEngineControl::push_SS(SOIL_SIGNAL_STRUCT* s)
{

    if(s == NULL){return;}
    if(s->in_control){return;}
    if(SS_n == 0)
    {
        first_SS = s;
        last_SS = s;
        current_SS = s;

        first_SS->prev = NULL;
        first_SS->next = NULL;


    }else{

        s->next = NULL;
        s->prev = last_SS;
        last_SS->next = s;
        last_SS = s;

        if(current_SS == NULL){current_SS = last_SS;}
    }
    s->in_control = true;
    SS_n++;
}
//------------------------------------------------ NEXT FLAG
SOIL_SIGNAL_STRUCT* SoilEngineControl::next_SS()
{

    if(current_SS == NULL)
    {
        return NULL;
    }
    previous_SS = current_SS;
    current_SS = current_SS->next;

    return previous_SS;
}
//------------------------------------------------ POP
void SoilEngineControl::pop_SS()
{

    if(previous_SS == NULL)
    {

        return;
    }
    previous_SS->in_control = false;
    if(previous_SS->prev == NULL)
    {
        first_SS = previous_SS->next;
    }else{
        previous_SS->prev->next = previous_SS->next;
    }
    if(previous_SS->next == NULL)
    {
        last_SS = previous_SS->prev;
    }else{
        previous_SS->next->prev = previous_SS->prev;
    }
    previous_SS = previous_SS->prev;
    SS_n--;

}
//------------------------------------------------ RESET
void SoilEngineControl::reset_SS()
{

    previous_SS = NULL;
    current_SS = first_SS;

}
//------------------------------------------------ SET UI FLAG
void SoilEngineControl::set_ui_flag(int uf)
{
    //this is the parameter used in SoilEngine::reload_ui(int)
    //a bigger ui_flag would be a bigger ui_reload work
    if(uf > ui_flag)
    {
        ui_flag = uf;
    }

}
//------------------------------------------------ POP UI FLAG
int SoilEngineControl::pop_ui_flag()
{

    int ret = ui_flag;
    ui_flag = 9;
    return ret;
}
//------------------------------------------------ SET NEXT FOCUS ELM
void SoilEngineControl::set_ahead_focus_elm(SoilUIElm* elm, int elm_id)
{
    //this is set before SoilUIElm::set_as_focus_elm(false)
    //is set in the previously focused elm, so it can see what elm will
    //be focused next, this is used by SoilDropMenu for example
    ahead_focus_elm = elm;
    ahead_focus_elm_id = elm_id;
}
//------------------------------------------------ SET FOCUS ELM
void SoilEngineControl::set_focus_elm(SoilUIElm* elm, int elm_id)
{

    focus_elm = elm;
    focus_elm_id = elm_id;

}
//------------------------------------------------ SET NEXT FOCUS ELM
void SoilEngineControl::set_next_focus_elm()
{
    SoilUIElm* elm;
    if(focus_elm == NULL)
    {

    }

    bool found_next = false;
    while(!found_next)
    {
        //elm = elm->next; 

    }
}
//------------------------------------------------ SET MOUSE DOWN ELM
void SoilEngineControl::set_mouse_left_down_elm(SoilUIElm* elm, int elm_id)
{

    mouse_left_down_elm = elm;
    mouse_left_down_elm_id = elm_id;

}
//------------------------------------------------ SET MOUSE DOWN ELM
void SoilEngineControl::set_mouse_wheel_down_elm(SoilUIElm* elm, int elm_id)
{

    mouse_wheel_down_elm = elm;
    mouse_wheel_down_elm_id = elm_id;

}
//------------------------------------------------ SET MOUSE OVER ELM
void SoilEngineControl::set_mouse_over_elm(SoilUIElm* elm, int elm_id)
{

    mouse_over_elm = elm;
    mouse_over_elm_id = elm_id;

}
//------------------------------------------------ SET HOVER WIDGET
void SoilEngineControl::set_anchor_hover_elm(SoilUIElm* a_elm, int a_elm_id, SoilUIElm* h_elm, int h_elm_id)
{
    anchor_elm = a_elm;
    anchor_elm_id = a_elm_id;

    hover_elm = h_elm;
    hover_elm_id = h_elm_id;
}
//------------------------------------------------ SET WINDOW SIZE
void SoilEngineControl::set_window_size(int win_size_w, int win_size_h)
{
    window_size.w = win_size_w;
    window_size.h = win_size_h;
}
//------------------------------------------------ SET WINDOW SIZE
void SoilEngineControl::set_window_size(SIZE win_size)
{
    window_size = win_size;
}
//------------------------------------------------ GET WINDOW SIZE
SIZE SoilEngineControl::get_window_size()
{
    return window_size;
}