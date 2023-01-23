#include "soilinput.h"

SoilInput::SoilInput() : SoilTextWidget(INPUT)
{
    init();
}
SoilInput::SoilInput(const SoilString& str) : SoilTextWidget(INPUT)
{
    init();
    set_text(str);
}
//---------------------------------------------- INIT
void SoilInput::init()
{
    //init(widget_type, true, true);
    //init(widget_type, true, false);
    SELECTABLE = true;
    EDITABLE = true;
    horizontal_alignment = LEFT;    
    vertical_alignment = CENTER;    
    set_text_record(false);
    scrollable = false;
    set_auto_size(false,false);
    set_follow_text_w(false);
    soil_text.set_multi_line(false);
    set_as_clickable_item(false);
    set_default_command_map();
    set_code_edit_special_shortcuts(true);


    //soil_string_pt = &soil_text.soil_string;
    edit_extra_w_content=0;
    action_n=0;
    //DONE REGARDLESS OF WIDGET TYPE
    init_flag(&cursor_flag);
    
    scrollable = false;

    //default_scrollbar_system is false because there is no pixmap for the entire text
    //if the text is bigger than the view
    default_scrollbar_system = false;
    font_size = 0;
    set_font_size(STW_DEFAULT_FONT_SIZE);
    //-------------------
    HOME_TO_INDENT = false;

    TAB_NORMAL_INDENT = true;
    TAB_ALWAYS_INDENT = false;

    REGULAR_ADD_TAB = true;
    REGULAR_DEL_TAB = true;
    
    BR_KEEP_INDENT = true;
    KEEP_X_MOVING_VERTICALLY = true;

    CODE_EDIT_TEXT_SHORTCUTS = true;
    MARKED = false;
    //-------------------
    set_size(200,-1);

    cursor_active = false;
    cursor_vis = false;
    update_view_to_cursor = true;

    print_n = 0;

    update_from_soil_text = true;
    update_pixmap_from_size = true;

    mouse_left_down_text = false;

    soil_text.set_soilobject_receiver(0, (SoilObject*)this);
}
//---------------------------------------------- SET FONT SIZE

void SoilInput::set_font_size(int fs)
{
    
    bool font_changed = update_from_font_size(fs);
    if(!font_changed){return;}

    view_min_size.w = char_size.w * 6 + (2*content_pad);

    view_min_size.h = char_size.h + (2*content_pad);
    view_max_size.h = char_size.h + (2*content_pad);
    set_size(-1,-1);
    process();
}
//---------------------------------------------- KEYDOWN EVENT VIRT
void SoilInput::keydown_event_virt(SDL_Event E)
{
    SDL_Keycode key = E.key.keysym.sym;

    if(E.key.keysym.scancode == SDL_SCANCODE_KP_ENTER || E.key.keysym.scancode == SDL_SCANCODE_RETURN)
    {
        if(SIGNALS[SUBMIT].active && engine_control_is_set)
        {
            engine_control->push_SS(&SIGNALS[SUBMIT]);
        }
    }else{
        SoilTextWidget::keydown_event_virt(E);
    }
}
//---------------------------------------------- DRAW
void SoilInput::draw()
{
    SoilTextWidget::draw();
}
