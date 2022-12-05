#include "soilengine.h"

bool SoilEngine::the_one_engine_static = true;
SoilEngine::SoilEngine()
{
    the_one_engine = the_one_engine_static;
    the_one_engine_static = false;

    if(!the_one_engine){return;}

    active_ui = 0;

    sdl_reset_gui_pt(&sdl_gui_pt);

    window_size.w = 600;
    window_size.h = 400;

    WINDOW_ABSOLUTE_MIN_SIZE.w = 100;
    WINDOW_ABSOLUTE_MIN_SIZE.h = 100;
    WINDOW_ABSOLUTE_MAX_SIZE.w = 3000;
    WINDOW_ABSOLUTE_MAX_SIZE.h = 2000;

    window_min_size = WINDOW_ABSOLUTE_MIN_SIZE;
    window_max_size = WINDOW_ABSOLUTE_MAX_SIZE;

    WINDOW_USER_RESIZABLE = true;

    main_layout = NULL;
    
    external_control = NULL;
    external_control_is_set = false;
    execution_started_active = false;


    //////////////////////////////////////// this is a control
    /// so no more than one SoilEngine gets working
    if(!the_one_engine){return;}

    engine_control = new SoilEngineControl();

}

//--------------------------------------------------- INIT SDL EVENT SIGNALS
void SoilEngine::init_sdl_event_signals()
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}




    int i = 0;
    while(i < 65536)
    {
        SDL_EVENT_SIGNALS[i].active = false;
        SDL_EVENT_SIGNALS[i].response = 0;
        i++;
    }

}

//--------------------------------------------------- SET MAIN LAYOUT
void SoilEngine::set_main_layout(SoilLayout* l)
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}



    if(main_layout != NULL){return;}
    if(l == NULL){return;}
    main_layout = l;
    main_layout->set_as_main_layout();

}

//--------------------------------------------------- LOAD ALERT WINDOW
void SoilEngine::load_alert_window()
{
    active_ui = 1;
    engine_control->set_ui_flag(3);
}
//--------------------------------------------------- SET MAIN LAYOUT
void SoilEngine::set_control(SoilControl* c)
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}
 

    if(c != NULL)
    {
        external_control = c;
        external_control_is_set = true;
    }
}
//--------------------------------------------------- SET USER RESIZABLE
void SoilEngine::set_window_user_resizable(bool v)
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}

    if(sdl_gui_pt.sdl_gui_loaded){return;}
    WINDOW_USER_RESIZABLE = v;


}

//--------------------------------------------------- EXECUTE
int SoilEngine::execute()
{

    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return -4;}

    //INIT WINDOW
    //window is being created as HIDDEN(just a reminder)
    int init_window_return = init_window();
    if(init_window_return != 0){return init_window_return;}
    
    texture_pixmap.spx_fill(0xFF000000);
    //CYCLE VARIABLES
    int cycle_cur_c=0;
    int cycle_prev_c=0;
    int cur_cycle_len=0;
    int cycle_comp=0;
    int max_cycle_len=0;
    int prev_cycle_len = 0;
    int cycle_average = 0;
    int cycle_n = 0;

    SDL_Event event;
    bool quit = false;

    
    //MARKER:The code below is an example of how to set the icon
    //Uint32 pxmask[4];
    //////  JPG mask
    //pxmask[0] = 0x00FF0000;
    //pxmask[1] = 0x0000FF00;
    //pxmask[2] = 0x000000FF;
    //pxmask[3] = 0xFF000000;
    //SoilPixmap icon_pixmap;
    //icon_pixmap.alloc_from_file("icon.jpg");
    
    //SDL_Surface* sdl_surface = SDL_CreateRGBSurface(0,icon_pixmap.size.w,icon_pixmap.size.h,32,pxmask[0],pxmask[1],pxmask[2],pxmask[3]);
    //paste_pixmap_to_sdl_surface(icon_pixmap.color_px_pt,icon_pixmap.size,icon_pixmap.get_full_rect(),sdl_surface,icon_pixmap.size,icon_pixmap.get_full_rect());
    //SDL_SetWindowIcon(sdl_gui_pt.sdl_window,sdl_surface);

    //MARKER: something weird happen here, even though
    //the window view rect is correct and the pixmap is all set
    //if window is set to a bigger size using set_window_size()
    //it only fills the window to the size it had before
    //I should probably call some SDL_.. function before
    //set_window_size(window_size.w,window_size.h);

    update_window_from_main_layout();
    SDL_ShowWindow(sdl_gui_pt.sdl_window);

    //main_layout->elm_ui_flag is set on the set_as_main_layout() method
    //in SoilLayout
    update_main_layout_from_window();

    reload_ui(3);
    update_view();


    //MARKER: make a soil_signal to trigger here for stuff that
    //should be executed as SoilEngine has just started executing

    if(external_control_is_set && execution_started_active)
    {
        external_control->execution_started_response();
    }

    SDL_GetWindowSize(sdl_gui_pt.sdl_window,&win_w,&win_h);


    int cycle_i = 0;
    int msg_i=0;

    int mouse_e=0;

    while(!quit)
    {

        cycle_prev_c=SDL_GetTicks();

        int event_i=0;
        while(SDL_PollEvent(&event))
        {
            
            ////////////////////////////// QUIT
            if(event.type == SDL_QUIT)
            {

                quit = true;
                continue;
            }

            ////////////////////////////// WINDOW EVENT
            if( event.type == SDL_WINDOWEVENT)
            {
                //MARKER: check which events will affect
                //states such as SoilUIElm.mouse_down_elm...
                if(event.window.event==SDL_WINDOWEVENT_NONE)
                {



                }
                if(event.window.event==SDL_WINDOWEVENT_SHOWN)
                {
                    //called when restoring minimized window
                    //called when SDL_ShowWindow() is called


                }
                if(event.window.event==SDL_WINDOWEVENT_HIDDEN)
                {
                    //called when window is minimized
                    //NOT called when another window gets in front of this
                    //called when SDL_HideWindow() is called


                }
                if(event.window.event==SDL_WINDOWEVENT_EXPOSED)
                {
                    //called when SDL_ShowWindow is used
                    //this is called when the window is maximized or restored
                    //and when the window is resized

                }
                if(event.window.event==SDL_WINDOWEVENT_MOVED)
                {
                    //for when the window is moved
                    //like when the user drags it
                    //it is usually called in other routines that show or hide
                    //the window, or when it is restored

                }
                if(event.window.event==SDL_WINDOWEVENT_RESIZED)
                {
                    //this is called when user drags window corner to resize it
                    //this is NOT called when you call SDL_SetWindowSize()
                    //this is also called when the window is maximized

                }

                if(event.window.event==SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    //this is called when user drags window corner to resize it
                    //this is also called when you call SDL_SetWindowSize()
                    //this is also called when the window is maximized


                    update_main_layout_from_window();
                }
                if(event.window.event==SDL_WINDOWEVENT_MINIMIZED)
                {


                }
                if(event.window.event==SDL_WINDOWEVENT_MAXIMIZED)
                {


                }
                if(event.window.event==SDL_WINDOWEVENT_RESTORED)
                {

                }
                if(event.window.event==SDL_WINDOWEVENT_ENTER)
                {
                    //it concerns the mouse pointer entering the window


                }
                if(event.window.event==SDL_WINDOWEVENT_LEAVE)
                {
                    //it concerns the mouse pointer leaving the window

                    if(engine_control->mouse_over_elm != NULL)
                    {
                        int y,x;
                        SDL_GetMouseState(&x,&y);

                        int in_view_x;
                        int in_view_y;
                        engine_control->mouse_over_elm->convert_win_to_view_coord(x,y,&in_view_x,&in_view_y);
                        engine_control->mouse_over_elm->mouse_leave_event(x,y,in_view_x,in_view_y,event);
                    }


                }
                if(event.window.event==SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    //exactly window focus as in the window selected in workspace


                }
                if(event.window.event==SDL_WINDOWEVENT_FOCUS_LOST)
                {

                    if(engine_control->mouse_left_down_elm != NULL)
                    {
                        engine_control->mouse_left_down_elm->mouse_left_down_elm = false;
                        engine_control->mouse_left_down_elm->scroll_w_bar_hold = false;
                        engine_control->mouse_left_down_elm->scroll_h_bar_hold = false;
                        engine_control->set_mouse_left_down_elm(NULL,0);
                    }
                    if(engine_control->mouse_wheel_down_elm != NULL)
                    {
                        engine_control->mouse_wheel_down_elm->mouse_wheel_down_elm = false;
                        engine_control->set_mouse_wheel_down_elm(NULL,0);
                    }

                    //event triggered in the moment the window loses focus


                }
                if(event.window.event==SDL_WINDOWEVENT_CLOSE)
                {


                }
                if(event.window.event==SDL_WINDOWEVENT_TAKE_FOCUS)
                {


                }
                if(event.window.event==SDL_WINDOWEVENT_HIT_TEST)
                {


                }

            }//END WINDOW EVENT
            ///////////////////////////////////// KEYBOARD EVENT
            if(event.type==SDL_KEYDOWN)
            {

                if(engine_control->focus_elm != NULL)
                {
                   engine_control->focus_elm->process_keydown_event(event);
                }



            }

            if(event.type==SDL_KEYUP)
            {

                if(engine_control->focus_elm != NULL)
                {
                   engine_control->focus_elm->process_keyup_event(event);
                }


            }
            ///////////////////////////////////// TEXT INPUT EVENT
            if(event.type==SDL_TEXTINPUT)
            {

                if(engine_control->focus_elm != NULL)
                {
                    engine_control->focus_elm->process_text_input_event(event);
                }

            }
            ///////////////////////////////////// MOUSE EVENT
            if(event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_MOUSEBUTTONUP ||
                    event.type==SDL_MOUSEMOTION || event.type==SDL_MOUSEWHEEL)
            {
                SoilUIElm* e;
                int y,x;
                if(event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_MOUSEBUTTONUP)
                {
                    x = event.button.x;
                    y = event.button.y;
                }
                if(event.type==SDL_MOUSEMOTION)
                {
                    x = event.motion.x;
                    y = event.motion.y;
                }
                if(event.type==SDL_MOUSEWHEEL)
                {
                    SDL_GetMouseState(&x,&y);
                }
                
                //MARKER: modal window
                //check the active_ui
                if(active_ui == 0)
                {
                    main_layout->recursive_process_mouse_event(x,y,x,y,event);
                }
            }

            ///////////////////////////////SDL EVENT USER SIGNAL(DIRECT CONNECTIONS)
            /// this checks if the user has activated any SDL_Event
            /// to trigger external_control.sdl_event_response(..., ...)
            ///
            Uint32 ET = event.type;
            if(ET >= 0 && ET < 65536)
            {
                if(SDL_EVENT_SIGNALS[ET].active && external_control_is_set)
                {
                    external_control->sdl_event_response(event,SDL_EVENT_SIGNALS[ET].response);
                }
            }


            event_i++;
        }


        //////////////////////////////////////////////////// SOIL EVENTS

        engine_control->reset_SS();
        SOIL_SIGNAL_STRUCT* signal = engine_control->next_SS();

        while(signal != NULL)
        {
            //PROCESSING
            if(external_control_is_set)
            {
                external_control->soil_signal_response(*signal);
            }
            engine_control->pop_SS();

            signal = engine_control->next_SS();
        }


        //////////////////////////////////////////////////// ELM FLAGS

        //MARKER: make sure to fine adjust the timing for interval based flags
        //elm flag processing
        //different flags from the same element shouldn't have the same value
    
        engine_control->reset_EF();
        ELM_FLAG* flag = engine_control->next_EF();
        while(flag != NULL)
        {
            if(flag->value == 0)
            {
                engine_control->pop_EF();
            }else{
                //PROCESSING
                if(flag->interval <= 0)
                {
                    flag->elm->process_flag(flag->value);

                }else{

                    flag->interval -= prev_cycle_len;

                }
            }

            flag = engine_control->next_EF();
        }

        //////////////////////////////////////////////////// RELOAD UI
        
        SDL_GetWindowSize(sdl_gui_pt.sdl_window,&win_w,&win_h);
        
        reload_ui(engine_control->ui_flag);
        
        update_view();
        
        //////////////////////////////////////////////////// CYCLE CONTROL
        cycle_cur_c=SDL_GetTicks();

        cur_cycle_len=cycle_cur_c-cycle_prev_c;
        prev_cycle_len = cur_cycle_len;
        if(cur_cycle_len>max_cycle_len)
        {
            max_cycle_len=cur_cycle_len;
            
        }
        cycle_comp=CYCLE_LEN-cur_cycle_len;
        if(cycle_comp>0)
        {
            SDL_Delay(cycle_comp);
            prev_cycle_len += cycle_comp;
        }

        SDL_GetWindowSize(sdl_gui_pt.sdl_window,&win_w,&win_h);

    }
    sdl_close(&sdl_gui_pt);

    return 0;
}
//---------------------------------------- SWITCH UI
void SoilEngine::switch_ui(int ui_id)
{
    active_ui = ui_id;
}
//--------------------------------------- ACTIVATE EXECUTION STARTED SIGNAL
void SoilEngine::activate_execution_started_signal(bool a)
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}
    execution_started_active = true;
}

//--------------------------------------------------- ACTIVATE SOIL SIGNAL
void SoilEngine::activate_soil_signal(SoilUIElm *elm, SOIL_SIGNAL signal, int response)
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}
    elm->SIGNALS[signal].active = true;
    elm->SIGNALS[signal].response = response;
}
//--------------------------------------------------- ACTIVATE SDL EVENT
void SoilEngine::activate_sdl_event(SDL_EventType event_type, int response)
{
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return;}

    SDL_EVENT_SIGNALS[event_type].active = true;
    SDL_EVENT_SIGNALS[event_type].response = response;
}

//--------------------------------------------------- UPDATE WINDOW FROM MAIN LAYOUT
void SoilEngine::update_window_from_main_layout()
{
    if(!the_one_engine){return;}

    SDL_SetWindowSize(sdl_gui_pt.sdl_window,main_layout->view_size.w,main_layout->view_size.h);
}
//--------------------------------------------------- UPDATE WINDOW FROM MAIN LAYOUT
void SoilEngine::update_main_layout_from_window()
{
    if(!the_one_engine){return;}

    main_layout->updated_by_window = true;

    SIZE WIN_S;
    SDL_GetWindowSize(sdl_gui_pt.sdl_window,&WIN_S.w,&WIN_S.h);
    main_layout->set_size(WIN_S.w,WIN_S.h);

    if(engine_control != NULL)
    {
        engine_control->set_window_size(main_layout->view_size.w, main_layout->view_size.h);
    }

}
//--------------------------------------------------- RELOAD UI
void SoilEngine::reload_ui(int opt)
{
    if(opt > 0)
    {
        //printf("\n\n---------------------    RELOAD UI opt:%d", opt);
    }
    if(!the_one_engine){return;}

    if(main_layout->elm_ui_flag > 2 && !main_layout->updated_by_window)
    {
        update_window_from_main_layout();
    }
    main_layout->updated_by_window = false;


    //the argument passed to the recursive SoilLayout methods is false
    //but it changes depending on the ui_flag of the SoilLayout in question
    //the method will afect the SoilWidget elements inside based on their
    //ui_flag as well.
    //if a SoilLayout sets the argument to true all its elements will be
    //affected accordingly
    switch(opt)
    {
    case 0:
        break;

    case 1:
        //USUALLY WHEN SOME ELM CHANGES BUT NOT ITS SIZE
        main_layout->recursive_print(false);
        break;

    case 2:
        
        //WHEN SCROLLBAR MOVE IN A LAYOUT, IT CALLS
        //RECURSIVE_SET_ELM_WINDOW_AREA() ON THE SAID LAYOUT
        //AND PRINT JUST THE LAYOUT AS WELL
        main_layout->recursive_set_elm_window_area(false);
        main_layout->recursive_print(false);
        break;

    case 3:
        
        //WHEN ELM SIZE CHANGE IT CALLS SUCH SEQUENCE OF METHODS
        //IN ITS PARENT LAYOUT
        main_layout->recursive_load_auto_sizes(main_layout->view_size.w, main_layout->view_size.h,false);
        //main_layout->recursive_load_auto_sizes(main_layout->content_view_rect.w, main_layout->content_view_rect.h,false);
        main_layout->recursive_set_positions(false);
        main_layout->recursive_set_elm_window_area(false);
        main_layout->recursive_print(false);
        break;

    default:
        break;

    }

    engine_control->ui_flag = 0;

}

//--------------------------------------------------- INIT WINDOW
int SoilEngine::init_window()
{    
    if(!the_one_engine || sdl_gui_pt.sdl_gui_loaded){return -4;}


    if(main_layout == NULL){return -3;}
    if(engine_control == NULL){return -4;}

    window_size.w = main_layout->view_size.w;
    window_size.h = main_layout->view_size.h;


    //HERE IS WHERE sdl_gui_pt.sdl_gui_loaded IS ACTIVATED!!!
    sdl_gui_pt = sdl_init(400,400);
    if(!sdl_gui_pt.sdl_gui_loaded){return -1;}

    load_arrows_pixmap();

    int TW = sdl_gui_pt.sdl_texture_size.w;
    int TH = sdl_gui_pt.sdl_texture_size.h;

    //TEXTURE PIXMAP
    texture_pixmap.alloc(TW,TH);
    if(!texture_pixmap.loaded)
    {
        //make some error treatment
        arrows_pixmap.release();
        sdl_close(&sdl_gui_pt);
        return -2;
    }
    //MARKER:CHECK TO SEE IF THERE ARE REPEATED UNECESSARY texture_pixmap.spx_fill(0xFF000000);
    texture_pixmap.spx_fill(0xFF000000);

    //WINDOW SIZE LIMITS AND SET BEFORE PROPERTIES
    WINDOW_ABSOLUTE_MAX_SIZE.w = TW;
    WINDOW_ABSOLUTE_MAX_SIZE.h = TH;
    //MARKER:set this as absolute max size for main_layout
    //and make sure the window size is the main_layout size

    engine_control->set_main_layout((SoilUIElm*)main_layout);
    main_layout->recursive_set_engine_properties(&texture_pixmap,&arrows_pixmap,sdl_gui_pt.pixel_density,engine_control,&font,external_control);

    //SoilUIElm::validate_for_execution() sets SoilUIElm::engine_running to true
    main_layout->recursive_validate_for_execution(sdl_gui_pt.sdl_texture_size.w,sdl_gui_pt.sdl_texture_size.h,150,150);

    SDL_SetWindowResizable(sdl_gui_pt.sdl_window,SDL_bool(WINDOW_USER_RESIZABLE));
    SDL_SetWindowMinimumSize(sdl_gui_pt.sdl_window,main_layout->view_min_size.w,main_layout->view_min_size.h);
    SDL_SetWindowMaximumSize(sdl_gui_pt.sdl_window,main_layout->view_max_size.w,main_layout->view_max_size.h);

    return 0;
}
//--------------------------------------------------- LOAD ARROWS PX
void SoilEngine::load_arrows_pixmap()
{
    if(!the_one_engine){return;}

    SoilPixmap tmp;
    tmp.alloc_from_file("images/arrows.bmp");
    int arrows_w = sdl_gui_pt.pixel_density / 7;
    arrows_pixmap.alloc(4*arrows_w,2*arrows_w);

    if(!tmp.loaded || !arrows_pixmap.loaded)
    {
       alt_load_arrows_pixmap();
       return;
    }

    SIZE TS = tmp.size;
    SIZE AS = arrows_pixmap.size;

    SOIL_RECT TR = tmp.get_full_rect();
    SOIL_RECT AR = arrows_pixmap.get_full_rect();
    arrows_pixmap.spx_resize_paste_pixmap(tmp,TR,AR);

}
//--------------------------------------------------- LOAD ARROWS PX
void SoilEngine::alt_load_arrows_pixmap()
{
    if(!the_one_engine){return;}

    int arrows_w = sdl_gui_pt.pixel_density / 10;
    arrows_pixmap.alloc(4*arrows_w,2*arrows_w);
    arrows_pixmap.spx_fill(0xFF000000);

    if(!arrows_pixmap.loaded || sdl_gui_pt.pixel_density < 80)
    {
       return;
    }

    SIZE AS = arrows_pixmap.size;
    int UN = arrows_w;
    int a_margin = 3;
    int d_margin = 2 * a_margin;
    SOIL_RECT alt_arrow_rect;
    alt_arrow_rect.x = a_margin;
    alt_arrow_rect.y = a_margin;
    alt_arrow_rect.w = arrows_w - (2*a_margin);
    alt_arrow_rect.h = arrows_w - (2*a_margin);

    Uint32 color = 0xFFFFFFFF;


    int i = 0;
    int mod;
    while(i < 8)
    {
        mod = i % 4;
        alt_arrow_rect.x = a_margin + (mod * (d_margin + alt_arrow_rect.w));
        if(i == 4)
        {
            alt_arrow_rect.y += d_margin + alt_arrow_rect.h;
            color = 0xFF000000;
        }
        arrows_pixmap.spx_draw_solid_rect(alt_arrow_rect,color);
        i++;
    }

}

//--------------------------------------------------- UPDATE VIEW
void SoilEngine::update_view()
{
    if(!the_one_engine){return;}

    if(sdl_gui_pt.sdl_gui_loaded)
    {
        //sdl_update_window(sdl_gui_pt.sdl_window,sdl_gui_pt.sdl_renderer,sdl_gui_pt.sdl_texture,&main_layout->win_pos_rect,&texture_pixmap.size,texture_pixmap.color_px_pt);
        //The texture should never be smaller than the window
        SDL_Rect window_sdl_rect;
        window_sdl_rect.x = main_layout->win_pos_rect.x;
        window_sdl_rect.y = main_layout->win_pos_rect.y;
        window_sdl_rect.w = main_layout->win_pos_rect.w;
        window_sdl_rect.h = main_layout->win_pos_rect.h;

        SDL_RenderClear( sdl_gui_pt.sdl_renderer );

        SDL_UpdateTexture(sdl_gui_pt.sdl_texture,&window_sdl_rect,texture_pixmap.color_px_pt,texture_pixmap.size.w*(sizeof(Uint32)));

        SDL_RenderCopy( sdl_gui_pt.sdl_renderer,sdl_gui_pt.sdl_texture,&window_sdl_rect,&window_sdl_rect);

        SDL_RenderPresent( sdl_gui_pt.sdl_renderer);

    }
}
