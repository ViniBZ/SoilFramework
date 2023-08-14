#include "soiluielm.h"

SIZE SoilUIElm::set_size_virt(int w, int h){SIZE size; return size;}
void SoilUIElm::text_input_event_virt(SDL_Event E){}
void SoilUIElm::keydown_event_virt(SDL_Event E){}
void SoilUIElm::keyup_event_virt(SDL_Event E){}
void SoilUIElm::mouse_left_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_left_up_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_left_click_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_wheel_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_wheel_up_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_wheel_roll_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_remote_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_enter_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::mouse_leave_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E){}
void SoilUIElm::set_as_focus_elm_virt(bool f)
{
    set_elm_ui_flag(1);
}
void SoilUIElm::set_as_mouse_left_down_elm_virt(bool f){}
void SoilUIElm::print(){}
POINT SoilUIElm::move_content_view_virt(int mx, int my){POINT res;return res;}
void SoilUIElm::recursive_print(bool do_all){}