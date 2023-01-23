#ifndef SOILENGINE_H
#define SOILENGINE_H

#include "sdl_functions.h"
#include "soilpixmap.h"
#include "soillayout.h"

#define MARK_ARRAY_SIZE 100
//MARKER:might use a variable for CYCLE_LEN

class SoilEngine
{
public:

    static bool the_one_engine_static;
    bool the_one_engine;//means the instance is the first SoilEngine created

    //0 is the regular main_layout
    //1 is the current file window
    //2 is the current alert window
    int active_ui;
    int cur_modal;
    int win_w;
    int win_h;

    SDL_GUI_PT sdl_gui_pt;

    bool title_loaded;
    SoilString window_title;

    bool icon_loaded;
    SDL_Surface* icon_surface;

    SoilString resource_dir_str;

    SoilPixmap texture_pixmap;//it will have the maximum possible window size
    SoilPixmap arrows_pixmap;
    SoilEngineControl* engine_control;
    SoilFont font;
    SoilControl* external_control;//maybe change the class name to SoilExternalControl
    bool external_control_is_set;
    bool execution_started_active;

    //WINDOW SHAPE PROPERTIES
    SIZE window_size;
    SIZE window_min_size;
    SIZE window_max_size;

    SoilLayout* main_layout;
    
    //SoilLayout* modal_layout;
    //SoilAlertWindow* alert_window;

    bool WINDOW_USER_RESIZABLE;

    //-------------------- SDL_Event signals
    //MARKER: is it the name you want for this variable? really?
    SDL_EVENT_SIGNAL* SDL_EVENT_SIGNALS;

    //PUBLIC METHODS
    SoilEngine();
    ~SoilEngine();
    void init_sdl_event_signals();
    void set_main_layout(SoilLayout* l);

    //MARKER: review these and overflow with SoilString args
    void set_window_title(const char * title);
    void set_window_icon(const char * icon_fn);
    void set_resources_subdir(const char * sub_dir);
    
    void load_alert_window();
    void set_control(SoilControl* c);
    void set_window_user_resizable(bool v);
    int execute();
    void switch_ui(int ui_id);

    void activate_execution_started_signal(bool a);

    void activate_soil_signal(SoilUIElm* elm,SOIL_SIGNAL signal, int response);
    void activate_sdl_event(SDL_EventType event_type, int response);

    //MARKER:
    //make set_window size methods?

private:

    //WINDOW SHAPE PROPERTIES
    SIZE WINDOW_ABSOLUTE_MIN_SIZE;
    SIZE WINDOW_ABSOLUTE_MAX_SIZE;

    //PRIVATE METHODS

    void update_window_from_main_layout();
    void update_main_layout_from_window();
    void reload_ui(int opt);
    int init_window();
    void load_arrows_pixmap();
    void alt_load_arrows_pixmap();
    void update_view();
};

#endif // SOILENGINE_H
