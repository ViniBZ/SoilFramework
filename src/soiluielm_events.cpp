#include "soiluielm.h"
//----------------------------------------------- PROCESS TEXT INPUT EVENT
void SoilUIElm::process_text_input_event(SDL_Event E)
{

    if(!engine_control_is_set){return;}

    text_input_event(E);
}
//----------------------------------------------- TEXT INPUT EVENT
void SoilUIElm::text_input_event(SDL_Event E)
{
    text_input_event_virt(E);

    if(SIGNALS[TEXT_INPUT].active && engine_control_is_set)
    {
        engine_control->push_SS(&SIGNALS[TEXT_INPUT]);
    }
}

//----------------------------------------------- PROCESS KEYDOWN EVENT
void SoilUIElm::process_keydown_event(SDL_Event E)
{
    if(!engine_control_is_set){return;}

    keydown_event(E);
}
//----------------------------------------------- KEYDOWN EVENT
void SoilUIElm::keydown_event(SDL_Event E)
{
    keydown_event_virt(E);

    if(SIGNALS[KEY_DOWN].active && engine_control_is_set)
    {
        SIGNALS[KEY_DOWN].arguments.key_arg.keysym = E.key.keysym;
        engine_control->push_SS(&SIGNALS[KEY_DOWN]);
    }

    //MANAGING TAB / SHIFT + TAB FOR FOCUS
    if(respond_tab_move_focus)
    {
        int keys_pressed_n;
        const Uint8 * keys_pressed = SDL_GetKeyboardState(&keys_pressed_n);
        bool shift_pressed = false;
        if(keys_pressed[SDL_SCANCODE_LSHIFT] || keys_pressed[SDL_SCANCODE_LSHIFT])
        {
            shift_pressed = true;
        }
        //printf("\n\nSoilUIElm::(events)KEY PRESSEDF ON ELM");
        if(keys_pressed[SDL_SCANCODE_TAB])
        {
            if(engine_control != NULL)
            {
                SoilUIElm* next_elm = NULL;
                if(keys_pressed[SDL_SCANCODE_LSHIFT] || keys_pressed[SDL_SCANCODE_LSHIFT])
                {
                    next_elm = get_prev_elm(true);
                }else{
                    next_elm = get_next_elm(true);
                }
                if(next_elm != NULL)
                {
                    next_elm->set_as_focus_elm(true);
                    //printf("\n\nSoilUIElm:: DEFINYING NEW FOCUS ELM");
                }
            }
        }
    }

}
//----------------------------------------------- PROCESS KEYUP EVENT
void SoilUIElm::process_keyup_event(SDL_Event E)
{
    keyup_event(E);
    if(SIGNALS[KEY_UP].active && engine_control_is_set)
    {
        engine_control->push_SS(&SIGNALS[KEY_UP]);
    }

}
//----------------------------------------------- KEYUP EVENT
void SoilUIElm::keyup_event(SDL_Event E)
{
    keyup_event_virt(E);
}
//----------------------------------------------- PROCESS MOUSE EVENT
void SoilUIElm::process_mouse_event(int X, int Y,int in_view_x, int in_view_y, SDL_Event E)
{
    //
    if(!engine_control_is_set){return;}
    //////////////////////////////////////////// MOUSE DOWN
    if(E.type == SDL_MOUSEBUTTONDOWN)
    {
        if(E.button.button == SDL_BUTTON_LEFT)
        {
            
            mouse_left_down_event(X,Y,in_view_x,in_view_y,E);
        }
        if(E.button.button == SDL_BUTTON_MIDDLE)
        {
            mouse_wheel_down_event(X,Y,in_view_x,in_view_y,E);
        }

        //SEND SIGNAL MOUSE DOWN


    }//////////////////////////////////////////// MOUSE DOWN END


    //////////////////////////////////////////// MOUSE UP
    if(E.type == SDL_MOUSEBUTTONUP)
    {
        //mouse_left_click_event()
        //is called within mouse_left_up_event()
        bool clicked = false;
        if(E.button.button == SDL_BUTTON_LEFT)
        {
            //this has to be set before mouse_left_up_event
            //which will unset mouse_left_up_elm
            clicked = mouse_left_down_elm;
            mouse_left_up_event(X,Y,in_view_x,in_view_y,E);
        }
        if(E.button.button == SDL_BUTTON_MIDDLE)
        {
            mouse_wheel_up_event(X,Y,in_view_x,in_view_y,E);
        }
         //SEND SIGNAL MOUSE UP
        if(clicked)
        {
            mouse_left_click_event(X,Y,in_view_x,in_view_y,E);
        }

    }//////////////////////////////////////////// MOUSE UP END

    //////////////////////////////////////////// MOUSE WHEEL
    if(E.type == SDL_MOUSEWHEEL)
    {
        mouse_wheel_roll_event(X,Y,in_view_x,in_view_y,E);
    }//////////////////////////////////////////// MOUSE WHEEL END


    //////////////////////////////////////////// MOUSE MOTION
    if(E.type == SDL_MOUSEMOTION)
    {
        //mouse_enter_even()
        //mouse_leave_event()
        //mouse_remote_motion_event()
        //are all called within mouse motion event

        mouse_motion_event(X,Y,in_view_x,in_view_y,E);

    }//////////////////////////////////////////// MOUSE MOTION END

}
//--------------------------------------------- MOUSE LEFT DOWN EVENT
void SoilUIElm::mouse_left_down_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(!engine_control_is_set){return;}

    if(scrollable)
    {

        if(scroll_w && in_view_y > content_view_rect.h)
        {
            scroll_w_mouse_left_down_event(in_view_x,in_view_y,E);
        }else{
            if(scroll_h && in_view_x > content_view_rect.w)
            {
                scroll_h_mouse_left_down_event(in_view_x,in_view_y,E);
            }
        }//else

    }//end if scrollable
    mouse_left_down_event_virt(X,Y,in_view_x,in_view_y,E);

    if(SIGNALS[MOUSE_DOWN].active && engine_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_DOWN],E.button.button,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_DOWN]);
        //button arg missing
    }
    
    set_as_mouse_left_down_elm(true);
    mouse_left_down_elm_x = in_view_x;
    mouse_left_down_elm_y = in_view_y;

    //set_as_focus_elm() calls set_as_focus_elm_virt()
    set_as_focus_elm(true);

}
//--------------------------------------------- MOUSE UP EVENT
void SoilUIElm::mouse_left_up_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(!engine_control_is_set){return;}

    mouse_left_up_event_virt(X,Y,in_view_x,in_view_y,E);

    if(SIGNALS[MOUSE_UP].active && engine_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_UP],E.button.button,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_UP]);
    }

    if(engine_control_is_set)
    {
        if(engine_control->mouse_left_down_elm != NULL)
        {
            engine_control->mouse_left_down_elm->set_as_mouse_left_down_elm(false);
        }
    }
}
//--------------------------------------------- MOUSE LEFT CLICK EVENT
void SoilUIElm::mouse_left_click_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(!engine_control_is_set){return;}

    mouse_left_click_event_virt(X,Y,in_view_x,in_view_y,E);

    if(SIGNALS[MOUSE_CLICK].active && external_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_CLICK],E.button.button,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_CLICK]);
    }
}

//--------------------------------------------- MOUSE WHEEL DOWN EVENT
void SoilUIElm::mouse_wheel_down_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{

    //fprintf(debug_file,"\n\nMOUSE WHEEL(%d , %d) ID: %d",X,Y,in_view_x,in_view_y,id);
    //MARKER: is it really to be a constant pixel value and not a fraction
    //of the total view size?

    if(!engine_control_is_set){return;}

    //This if returns if the wheel was pressed on the scrollbar
    if(in_view_x > content_view_rect.w || in_view_y > content_view_rect.h)
    {
        return;
    }
    if(engine_control->mouse_wheel_down_elm != NULL)
    {
        engine_control->mouse_wheel_down_elm->mouse_wheel_down_elm = false;
    }
    mouse_wheel_down_elm = true;
    engine_control->set_mouse_wheel_down_elm((SoilUIElm *)this,id);
    mouse_wheel_down_elm_x = in_view_x;
    mouse_wheel_down_elm_y = in_view_y;
    mouse_wheel_down_mx = 0;
    mouse_wheel_down_my = 0;

    mouse_wheel_down_event_virt(X,Y,in_view_x,in_view_y,E);
    if(SIGNALS[MOUSE_DOWN].active && engine_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_DOWN],E.button.button,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_DOWN]);
        //button arg missing
    }
}
//--------------------------------------------- MOUSE WHEEL UP EVENT
void SoilUIElm::mouse_wheel_up_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    
    if(!engine_control_is_set){return;}

    //MARKER: make the code below neater, I guess just call
    //engine_control->set_mouse_wheel...(false)
    if(engine_control->mouse_wheel_down_elm != NULL)
    {
        engine_control->mouse_wheel_down_elm->mouse_wheel_down_elm = false;
    }
    mouse_wheel_down_elm = false;
    mouse_wheel_down_mx = 0;
    mouse_wheel_down_my = 0;
    engine_control->set_mouse_wheel_down_elm(NULL,0);

    mouse_left_up_event_virt(X,Y,in_view_x,in_view_y,E);
    if(SIGNALS[MOUSE_UP].active && engine_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_UP],E.button.button,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_UP]);
    }
}
//--------------------------------------------- MOUSE WHEEL ROLL EVENT
void SoilUIElm::mouse_wheel_roll_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    scroll_h_wheel(E.wheel.y);
    mouse_wheel_roll_event_virt(X,Y,in_view_x,in_view_y,E);

    //SEND MOUSE SIGNAL
    if(SIGNALS[MOUSE_WHEEL].active && engine_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_WHEEL],0,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_WHEEL]);
    }
}

//--------------------------------------------- MOUSE MOTION EVENT
void SoilUIElm::mouse_motion_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{

    //fprintf(debug_file,"\n\nMOUSE MOTION(%d , %d) ID: %d",X,Y,in_view_x,in_view_y,id);
    if(!engine_control_is_set){return;}

    /////////////////////// ENTER / LEAVE EVENT
    if(engine_control->mouse_over_elm_id != id)
    {
        if(engine_control->mouse_over_elm != NULL)
        {
            engine_control->mouse_over_elm->mouse_leave_event(X,Y,in_view_x,in_view_y,E);
            //bool mouse_over_elm is set to false within
            //mouse_leave_event(X,Y)
        }

        mouse_enter_event(X,Y,in_view_x,in_view_y,E);
        //bool mouse_over_elm is set to true within
        //mouse_enter_event(X,Y)
        //engine_control->set_mouse_over_elm(elm,id) is also called
        //in SoilUIElm::mouse_enter_event(X,Y)

        //engine_control->set_mouse_over_elm(NULL,0) is only
        //called in SoilEngine::execute() when the event
        //SDL_WINDOWEVENT_LEAVE is triggered... because within the window
        //I guess everytime the mouse leaves a elm it enters another
        //so
        //calling engine_control->set_mouse_over_elm(elm,id) in the entered
        //elm should be enough to set the engine_control the right
        //mouse_over_elm....... or is it?.

    }

    mouse_over_elm_x = in_view_x;
    mouse_over_elm_y = in_view_y;

    //////////////////////// SENDING MOUSE MOTION SIGNAL
    if(SIGNALS[MOUSE_MOTION].active && engine_control_is_set)
    {
        load_mouse_coord_to_signal_arg(&SIGNALS[MOUSE_MOTION],0,X,Y,in_view_x,in_view_y);
        engine_control->push_SS(&SIGNALS[MOUSE_MOTION]);
    }

    //MARKER: check if scroll_move_flag.in_control is actually incompatible with the elm not being mouse_left_down_elm
    //meaning to move the scroll it needs to be the one where mouse left pressed down
    if(scrollable && scroll_move_flag.in_control)
    {
        check_scroll_move_mouse_motion();
    }

    
    mouse_motion_event_virt(X,Y,in_view_x,in_view_y,E);


    //////////////////////// SENDING MOTION TO MOUSE LEFT DOWN ELM
    if(engine_control->mouse_left_down_elm != NULL)
    {
        int remote_view_x = in_view_x;
        int remote_view_y = in_view_y;
        //CORRECTING IN_VIEW_X/Y FOR REMOTE MOTION ELM
        if(engine_control->mouse_over_elm_id != engine_control->mouse_left_down_elm_id)
        {
            //set in_view_x/y to the remote motion elm if it's not this one
            engine_control->mouse_left_down_elm->convert_win_to_view_coord(X,Y,&remote_view_x,&remote_view_y);
        }
        engine_control->mouse_left_down_elm->mouse_remote_motion_event(X,Y,remote_view_x,remote_view_y,E);
    }else{


        //////////////////// SENDING MOTION TO MOUSE WHEEL DOWN ELM
        if(engine_control->mouse_wheel_down_elm != NULL)
        {

            int remote_view_x = in_view_x;
            int remote_view_y = in_view_y;
            //CORRECTING IN_VIEW_X/Y FOR REMOTE MOTION ELM
            if(engine_control->mouse_over_elm_id != engine_control->mouse_wheel_down_elm_id)
            {
                //set in_view_x/y to the remote motion elm if it's not this one
                engine_control->mouse_wheel_down_elm->convert_win_to_view_coord(X,Y,&remote_view_x,&remote_view_y);
            }
            engine_control->mouse_wheel_down_elm->mouse_remote_motion_event(X,Y,remote_view_x,remote_view_y,E);
        }

    }
}
//--------------------------------------------- MOUSE REMOTE MOTION EVENT
void SoilUIElm::mouse_remote_motion_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(!engine_control_is_set){return;}

    //ATENTION! IN THIS FUNCTION in_view_x/y is the coord for the mouse motion
    //in the engine_control->mouse_over_elm. If that is not this elm, then
    //in_view_x/y need to be reset to the coord within this elm's view, as
    //it's done below:
    if(engine_control->mouse_over_elm_id != id)
    {
        //set in_view_x/y to the remote motion elm if it's not this one
        //convert_win_to_view_coord(X,Y,&in_view_x,&in_view_y);
    }
    //printf("\nscroll_w_bar_hold:");
    //if(scroll_w_bar_hold){printf("TRUE");}else{printf("FALSE");}
    //printf("\nscroll_h_bar_hold:");
    //if(scroll_h_bar_hold){printf("TRUE");}else{printf("FALSE");}
    //printf("\n.");

    if(scrollable)
    {
        
        //////////////////////////////// MOVING SCROLLBAR IF PRESSED
        if(mouse_wheel_down_elm)
        {
            int mx = mouse_wheel_down_elm_x - in_view_x;
            int my = mouse_wheel_down_elm_y - in_view_y;            

            SoilUIElm* elm = (SoilUIElm*) this;
            POINT motion;
            POINT res_motion;
            int rest = mx;
            bool first_elm = true;
            while(elm != NULL)
            {
                if(elm->bigger_content_w)
                {
                    motion = elm->move_content_view(rest,0);
                    rest -= motion.x;
                    mouse_wheel_down_elm_x -= motion.x;
                    if(!first_elm)
                    {
                        mouse_wheel_down_mx += motion.x;
                    }
                    if(rest == 0){break;}
                }
                elm = elm->parent;
                first_elm = false;
            }
            mouse_wheel_down_elm_x -= rest;
        
            res_motion.x = motion.x;

            rest = my;
            elm = (SoilUIElm*) this;
            first_elm = true;
            
            while(elm != NULL)
            {
                if(elm->bigger_content_h)
                {
                    motion = elm->move_content_view(0,rest);
                    rest -= motion.y;
                    mouse_wheel_down_elm_y -= motion.y;
                    if(!first_elm)
                    {
                        mouse_wheel_down_my += motion.y;
                    }
                    if(rest == 0){break;}
                }
                elm = elm->parent;
                first_elm = false;
            }
            mouse_wheel_down_elm_y -= rest;

            res_motion.y = motion.y;
        }else{
            if(scroll_w_bar_hold)
            {

                int m = in_view_x - scroll_hold_x;
                move_scrollbar_w(m);

            }
            if(scroll_h_bar_hold)
            {
                int m = in_view_y - scroll_hold_y;
                move_scrollbar_h(m);
            }
        }
    }//end if scrollable

    //IF NOT SCROLLABLE IT SHOULD PROBABLY SEND THE MOTION TO THE PARENT
    //LAYOUT

    mouse_remote_motion_event_virt(X,Y,in_view_x,in_view_y,E);
}

//--------------------------------------------- MOUSE ENTER EVENT
void SoilUIElm::mouse_enter_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{

    if(!engine_control_is_set){return;}
    //fprintf(debug_file,"\n\nMOUSE ENTER(%d , %d) ID: %d",X,Y,in_view_x,in_view_y,id);
    mouse_over_elm = true;


    engine_control->set_mouse_over_elm((SoilUIElm*)this,id);
    mouse_enter_event_virt(X,Y,in_view_x,in_view_y,E);
    if(SIGNALS[MOUSE_ENTER].active && external_control_is_set)
    {
        engine_control->push_SS(&SIGNALS[MOUSE_ENTER]);
    }
}


//--------------------------------------------- MOUSE LEAVE EVENT
void SoilUIElm::mouse_leave_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{

    if(!engine_control_is_set){return;}


    engine_control->set_mouse_over_elm(NULL,0);
    scroll_move_flag.value = 0;
    mouse_over_elm = false;

    //SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    //SDL_SetCursor(cursor);
    mouse_leave_event_virt(X,Y,in_view_x,in_view_y,E);
    if(SIGNALS[MOUSE_LEAVE].active && external_control_is_set)
    {
        engine_control->push_SS(&SIGNALS[MOUSE_LEAVE]);
    }
}
//--------------------------------------------- SET AS FOCUS ELM
void SoilUIElm::set_as_focus_elm(bool f)
{
    

    //MARKER: don't forget to make the keyboard interface for changing
    //focus_elm with tab and such stuff, I thought about using tab
    //to switch from widget to widget, enter to get into the editing area
    //of a SoilText, and esc to exit it. This way I could use tab within
    // the SoilText without changing the widget focus

    //This function already calls SoilEngineControl::set_as_focus_elm()

    //it's important that the focus elm be the only with
    //focus_elm set to true
    //same for mouse_left_down_elm

    bool prev_focus = focus_elm;
    if(!engine_control_is_set){return;}
    if(f)
    {
        //MARKER: make sure the variables you have for focus_elm
        //in engine_control and BOOL focus_elm in the elm are
        //always consistent with each other
        if(engine_control->focus_elm_id == id){return;}

        if(engine_control->focus_elm != NULL)
        {
            engine_control->set_ahead_focus_elm(this, id);
            engine_control->focus_elm->set_as_focus_elm(false);
        }
        engine_control->set_focus_elm((SoilUIElm*)this, id);
        //printf("\nSoilUIELM::(events)settting as focus elm  ID : %d", id);
        //printf("\n");
        engine_control->set_ahead_focus_elm(NULL, 0);
    }else{
        if(engine_control->focus_elm_id == id)
        {
            engine_control->set_focus_elm(NULL, 0);
        }
    }

    focus_elm = f;
    set_elm_ui_flag(1);

    set_as_focus_elm_virt(f);
    if(prev_focus != focus_elm && SIGNALS[FOCUS_CHANGED].active && engine_control_is_set)
    {
        SIGNALS[FOCUS_CHANGED].arguments.focus_changed_arg.focused = focus_elm;
        engine_control->push_SS(&SIGNALS[FOCUS_CHANGED]);
    }

}
//--------------------------------------------- SET AS MOUSE LEFT DOWN ELEMENT
void SoilUIElm::set_as_mouse_left_down_elm(bool f)
{
    if(!engine_control_is_set){return;}

    if(f)
    {
        if(engine_control->mouse_left_down_elm != NULL && engine_control->mouse_left_down_elm != this)//seems repetitive here...
        {
            engine_control->mouse_left_down_elm->set_as_mouse_left_down_elm(false);
        }
        engine_control->set_mouse_left_down_elm((SoilUIElm*)this,id);

    }else{
        if(mouse_left_down_elm && engine_control->mouse_left_down_elm != NULL)//...and here
        {
            engine_control->mouse_left_down_elm->scroll_move_flag.value = 0;
            engine_control->mouse_left_down_elm->scroll_w_bar_hold = false;
            engine_control->mouse_left_down_elm->scroll_h_bar_hold = false;

            engine_control->set_mouse_left_down_elm(NULL,0);
        }
    }
    mouse_left_down_elm = f;
    set_as_mouse_left_down_elm_virt(f);
}

//----------------------------------------------- PROCESS FLAG
void SoilUIElm::process_flag(int value)
{
    if(!engine_control_is_set){return;}
    if(scrollable && value > 0 && value < 11)
    {
        check_scroll_move_mouse_motion();

        //scroll_move_flag VALUES GO FROM 0 TO 10
        //it lacks 3 and 8 to make the scrollbar motion algorithms a bit simpler
        //
        //Each of these values, -1, match the indexes for
        //scroll_w_rects and scroll_h_bounds

        if(value == 1)//scroll_w_rect[0]
        {
            move_content_view(-SCR_MOV,0);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
        if(value == 2)//scroll_w_rect[1]
        {
            move_content_view(-SCR_BIG_MOV,0);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
        if(value == 4)//scroll_w_rect[3]
        {
            move_content_view(SCR_BIG_MOV,0);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
        if(value == 5)//scroll_w_rect[4]
        {
            move_content_view(SCR_MOV,0);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
        if(value == 6)//scroll_h_rect[0]
        {
            move_content_view(0,-SCR_MOV);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
        if(value == 7)//scroll_h_rect[1]
        {
            move_content_view(0,-SCR_BIG_MOV);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
        if(value == 9)//scroll_h_rect[3]
        {
            move_content_view(0,SCR_BIG_MOV);
            scroll_move_flag.interval = SCR_INTERVAL;

        }
        if(value == 10)//scroll_h_rect[4]
        {
            move_content_view(0,SCR_MOV);
            scroll_move_flag.interval = SCR_INTERVAL;
        }
    }

}
//----------------------------------------------- INIT FLAG
void SoilUIElm::init_flag(OBJ_FLAG* flag)
{


    flag->obj = (SoilObject*)this;
    flag->interval = 0;
    flag->in_control = false;
    flag->next = NULL;
    flag->prev = NULL;
    flag->value = 0;

}
//------------------------------ LOAD MOUSE COORD TO SIGNAL ARG
void SoilUIElm::load_mouse_coord_to_signal_arg(SOIL_SIGNAL_STRUCT * sig, Uint8 button, int X,int Y,int in_view_x,int in_view_y)
{
    sig->arguments.mouse_arg.button = button;
    sig->arguments.mouse_arg.win_coord.x = X;
    sig->arguments.mouse_arg.win_coord.y = Y;
    sig->arguments.mouse_arg.elm_coord.x = in_view_x;
    sig->arguments.mouse_arg.elm_coord.y = in_view_y;

    //button arg missing
}
