#include "soildropmenu.h"

SoilDropMenu::SoilDropMenu() : SoilWidget(DROPMENU)
{
    sel_str.set_text("select...");
    first_opt = NULL;
    hover_elm = new SoilListWidget();
    hover_list = (SoilListWidget*)hover_elm;
    hover_open = false;

    font_size = 5;
    char_size = SoilFont::get_char_size_from_font_size(font_size);
    value = hover_list->get_value();
    option_n = 0;


    hover_list->set_soilobject_receiver(0, (SoilObject*)this);
    set_size(130, char_size.h + 4*DROPMENU_IN_PAD);
    draw();
}
//--------------------------------------------------- INTERNAL SET VALUE
void SoilDropMenu::internal_set_value(int v)
{
    value = v;
    sel_str.set_text(hover_list->at(v));
    if(engine_control_is_set && engine_control != NULL && SIGNALS[VALUE_CHANGED].active)
    {
        SIGNALS[VALUE_CHANGED].arguments.value_changed_arg.value = value;
        engine_control->push_SS(&SIGNALS[VALUE_CHANGED]);
    }
    set_elm_ui_flag(1);
    draw();
}
//--------------------------------------------------- SET VALUE
void SoilDropMenu::set_value(int v)
{
    //THIS IS JUST FOR EXTERNAL CALLS
    if(!validate_abs_index(option_n, &v)){return;}
    hover_list->internal_set_value(v);
    internal_set_value(v);
}
//--------------------------------------------------- SET ENGINE PROPERTIES
void SoilDropMenu::set_engine_properties(SoilPixmap* d, SoilPixmap *a, int pix_d, SoilEngineControl *eng_c, SoilFont* f, SoilControl* ext_c)
{
    SoilUIElm::set_engine_properties(d, a, pix_d, eng_c, f, ext_c);
    hover_list->recursive_set_engine_properties(d, a, pix_d, eng_c, f, ext_c);
}
//--------------------------------------------------- VALIDATE FOR EXECUTION
void SoilDropMenu::validate_for_execution()
{
    SoilUIElm::validate_for_execution();
    hover_list->recursive_validate_for_execution(0,0,0,0);
    draw();
}
//--------------------------------------------------- ADD OPTION
void SoilDropMenu::add_option(const SoilString& opt_str, int opt_val)
{
    hover_list->push(opt_str);
    option_n = hover_list->get_length();
}
//--------------------------------------------------- ADD OPTION
void SoilDropMenu::add_option(const SoilString& opt_str)
{
    hover_list->push(opt_str);
    option_n = hover_list->get_length();
}
//--------------------------------------------------- REMOVE OPTION
void SoilDropMenu::remove_option(int i)
{
    hover_list->remove(i);
    option_n = hover_list->get_length();
}
//--------------------------------------------------- DRAW
void SoilDropMenu::draw()
{

    if(font == NULL){return;}

    pixmap.alloc(view_size.w, view_size.h);
    pixmap.spx_fill(0xFF000000);
    //return;

    SOIL_RECT text_rect;
    text_rect.x = 2*DROPMENU_IN_PAD;
    text_rect.y = 2*DROPMENU_IN_PAD;
    text_rect.w = view_rect.w - (4*DROPMENU_IN_PAD);
    text_rect.h = view_rect.h - (4*DROPMENU_IN_PAD);

    sel_str.draw_text_rect_px(sel_str.get_full_rect(char_size), &pixmap, text_rect, font, font_size);

    draw_inner_border(0xFFFFFFFF, DROPMENU_IN_PAD);
}
//--------------------------------------------------- MOUSE LEFT DOWN EVENT VIRTUAL
void SoilDropMenu::mouse_left_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(option_n <= 0){return;}
    activate_hover_elm(!hover_open);
}
//--------------------------------------------------- SET AS FOCUS ELM VIRTUAL
void SoilDropMenu::set_as_focus_elm_virt(bool f)
{
    if(!f && engine_control != NULL && hover_open && engine_control->ahead_focus_elm != NULL)
    {
        if(!engine_control->ahead_focus_elm->in_hover())
        {
    
            activate_hover_elm(false);
        }          
    }
}
//--------------------------------------------------- KEY DOWN EVENT VIRTUAL
void SoilDropMenu::keydown_event_virt(SDL_Event E)
{
    SoilUIElm::keydown_event_virt(E);
    SDL_Keycode key = E.key.keysym.sym;
    
    int keys_pressed_n;
    const Uint8 * keys_pressed = SDL_GetKeyboardState(&keys_pressed_n);
}
//--------------------------------------------------- SOILOBJECT CALLBACK
int SoilDropMenu::soilobject_callback(int arg)
{
    if(arg == 0)
    {
        internal_set_value(hover_list->get_value());
        if(engine_control->hover_elm_id == hover_list->get_id())
        {
            activate_hover_elm(false);
        }
    }
    return 0;
}