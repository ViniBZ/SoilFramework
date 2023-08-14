#ifndef SOILDROPMENU_H
#define SOILDROPMENU_H

#include "soilwidget.h"
#include "soillayout.h"
#include "soiltext.h"
#include "soillistwidget.h"

#define DROPMENU_IN_PAD 2

class SoilDropMenu : public SoilWidget
{
public:
    SoilText sel_str;
    SoilText * first_opt;
    int * first_val;

    SoilListWidget* hover_list;
    
    int value;
    int option_n;

    int font_size;
    SIZE char_size;

    SoilDropMenu();
    void internal_set_value(int v);
    void set_value(int v);
    virtual void set_engine_properties(SoilPixmap* d, SoilPixmap *a, int pix_d, SoilEngineControl *eng_c, SoilFont* f, SoilControl* ext_c);
    virtual void validate_for_execution();
    void add_option(const SoilString& opt_str, int opt_val);
    void add_option(const SoilString& opt_str);
    void remove_option(int i);
    virtual void draw();
    virtual void mouse_left_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void set_as_focus_elm_virt(bool f);
    virtual void keydown_event_virt(SDL_Event E);
    virtual int soilobject_callback(int arg);

};

#endif // SOILDROPMENU_H