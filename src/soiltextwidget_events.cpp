#include "soiltextwidget.h"

//---------------------------------------------- MOUSE LEFT DOWN EVENT
void SoilTextWidget::mouse_left_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(!coord_in_text_content(in_view_x, in_view_y)){return;}
    mouse_selected = false;
    if(SELECTABLE)
    {
        POINT in_text_coord;
        bool in_text_event = correct_coord_for_soil_text(in_view_x,in_view_y, &in_text_coord.x, &in_text_coord.y);
        if(in_text_event)
        {
            mouse_left_down_text = true;
            //WITH SHIFT
            int keys_pressed_n;
            const Uint8 * keys_pressed = SDL_GetKeyboardState(&keys_pressed_n);
            bool shift_pressed = false;
            if(keys_pressed[SDL_SCANCODE_LSHIFT] || keys_pressed[SDL_SCANCODE_LSHIFT])
            {
                shift_pressed = true;
            }
            //
            bool cursor_active_before = cursor_active;
            if(EDITABLE)
            {
                cursor_active = true;
                cursor_vis = true;
                
            }

            int new_cursor;
            soil_text.cursor_pos_from_mouse(in_text_coord.x, in_text_coord.y, char_size, &new_cursor);
            mouse_left_down_cursor = new_cursor;
            
            if(shift_pressed && cursor_active_before)
            {
                soil_text.select_text(soil_text.get_cursor_pos(), new_cursor);
                mouse_selected = true;
            }else{
                update_from_soil_text = false;
                if(soil_text.text_selected){soil_text.unselect_text();}
                update_from_soil_text = true;
                soil_text.set_cursor(new_cursor);
            }
        }
        //soil_text_changed();
    }

}
//---------------------------------------------- MOUSE LEFT CLICK EVENT
void SoilTextWidget::mouse_left_click_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(!coord_in_text_content(in_view_x, in_view_y)){return;}
    POINT in_text_coord;
    bool in_text_event = correct_coord_for_soil_text(in_view_x,in_view_y, &in_text_coord.x, &in_text_coord.y);

    if(SELECTABLE && in_text_event)
    {

        int click_n = E.button.clicks;

        if(click_n == 1)
        {
            //unselecting
            if(soil_text.is_selected() && !mouse_selected)
            {
                soil_text.unselect_text();
                //soil_text_changed();
            }
        }
        if(click_n == 2)
        {
            //select word

            
            soil_text.select_word(in_text_coord,char_size);
            //soil_text_changed();
        }
        if(click_n == 3)
        {
            //select all
            update_view_to_cursor = false;
            soil_text.select_all();
            //soil_text_changed();
        }
    }
    if(soilobject_receiver[0] != NULL)
    {
        soilobject_receiver[0]->soilobject_callback(0, id);
    }

}

//MARKER:finish this, there is still the double click and triple click missing
//and maybe deselect on click and not mouse left down
//---------------------------------------------- MOUSE MOTION EVENT
void SoilTextWidget::mouse_remote_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    if(SELECTABLE && mouse_left_down_text)
    {
        //POINT a;
        POINT b;
        //correct_coord_for_soil_text(mouse_left_down_elm_x,mouse_left_down_elm_y, &a.x, &a.y);
        correct_coord_for_soil_text(in_view_x,in_view_y, &b.x, &b.y);
        int new_cursor;
        soil_text.cursor_pos_from_mouse(b.x, b.y, char_size, &new_cursor);
        bool sel_changed = soil_text.select_text(mouse_left_down_cursor, new_cursor);
        //soil_text.set_cursor(b.x,b.y,char_size);

        if(sel_changed)
        {
            mouse_selected = true;
        }
    }
}
//---------------------------------------------- SET AS MOUSE LEFT DOWN ELM
void SoilTextWidget::set_as_mouse_left_down_elm_virt(bool f)
{
    bool prev_marked = MARKED;
    if(f)
    {
        //printf("\nSoilTextWidget::MARK TRUE");
        if(CLICKABLE_ITEM){MARKED = true;}
    }else{
        //printf("\nSoilTextWidget::MARK FALSE");
        mouse_left_down_cursor = (-1);
        mouse_left_down_text = false;
        if(CLICKABLE_ITEM){MARKED = false;}
    }
    if(prev_marked != MARKED)
    {
        //printf("\nSoilTextWidget::ITS DIFFERENT");
        draw();
        set_elm_ui_flag(1);
    }

}
//---------------------------------------------- PROCESS TEXT INPUT EVENT
void SoilTextWidget::text_input_event_virt(SDL_Event E)
{

    if(EDITABLE)
    {
        int keys_pressed_n;
        const Uint8 * keys_pressed = SDL_GetKeyboardState(&keys_pressed_n);
        if(keys_pressed[SDL_SCANCODE_LCTRL] || keys_pressed[SDL_SCANCODE_RCTRL] || keys_pressed[SDL_SCANCODE_LALT] || keys_pressed[SDL_SCANCODE_RALT])
        {
            return;
        }
        int input_len = strlen(E.text.text);
        int new_cursor_pos;
        //update_from_soil_text = false;
        //MARKER: Am I using soil_text.text_selected or soil_text.is_selected() ?
        SOIL_CHAR ch = SoilString::utf8_c_str_to_soil_char(E.text.text);
        if(REC_ACTIVE)
        {
            set_str_text_fields();
        }     
        int cursor_now = soil_text.get_cursor_pos();
        if(soil_text.text_selected)
        {
            //soil_text.remove_selection();
            //new_cursor_pos = soil_text.SEL_S + input_len;
            soil_string_pt->replace(ch, soil_text.SEL_S, (soil_text.SEL_E - soil_text.SEL_S));
            
            cursor_now = soil_text.get_cursor_pos();

        }else{
            soil_string_pt->insert_ch_in_range(ch, soil_text.get_cursor_pos());
           // new_cursor_pos = cursor + input_len;
        }
        
        update_from_soil_text = true;
        //soil_text.set_cursor_from_last_rec();
    }
}
//---------------------------------------------- PROCESS KEYDOWN EVENT
void SoilTextWidget::keydown_event_virt(SDL_Event E)
{
    //MARKER:revise cursor moving events

    //IMPORTANT: 
    //apparently SDL_Scancode (ex: SDL_SCANCODE_LSHIFT) maps to the keyboard key
    //while SDL_Keycode (ex: SDLK_c) is the one corresponding to the keyboard key in the "current layout"

    SDL_Keycode key = E.key.keysym.sym;
    
    int keys_pressed_n;
    const Uint8 * keys_pressed = SDL_GetKeyboardState(&keys_pressed_n);
    bool ctrl_pressed = false;
    bool alt_pressed = false;
    bool shift_pressed = false;

    if(keys_pressed[SDL_SCANCODE_LCTRL] || keys_pressed[SDL_SCANCODE_RCTRL])
    {
        ctrl_pressed = true;
    }
    if(keys_pressed[SDL_SCANCODE_LALT] || keys_pressed[SDL_SCANCODE_RALT])
    {
        alt_pressed = true;
    }
    if(keys_pressed[SDL_SCANCODE_LSHIFT] || keys_pressed[SDL_SCANCODE_LSHIFT])
    {
        shift_pressed = true;
    }
    
    int cursor = soil_text.get_cursor_pos();
    int cursor_before = cursor;
    int cursor_after = cursor;
    int prev_sel_s = soil_text.SEL_S;
    int prev_sel_e = soil_text.SEL_E;
    
    STW_COMMAND command = command_from_keys(ctrl_pressed, alt_pressed, shift_pressed, E.key.keysym.scancode);

    //MARKER:don't forget to put a return in each of these key cases
    if(EDITABLE)
    {
        //set cursor_2 and stuff(?)
        set_str_text_fields();
    }
    if(command == STW_COPY)
    {
        if(soil_text.text_selected)
        {
            
            //char* clipboard_text = str->section_c_str_alloc(soil_text.SEL_S,(soil_text.SEL_E - soil_text.SEL_S));
            char* clipboard_text = soil_text.get_selection_c_str_alloc();
            SDL_SetClipboardText(clipboard_text);
            if(clipboard_text != NULL){delete[] clipboard_text;}
        }
    }
    if(command == STW_CUT)
    {
        if(soil_text.text_selected)
        {
            char* clipboard_text = soil_string_pt->section_c_str_alloc(soil_text.SEL_S,(soil_text.SEL_E - soil_text.SEL_S));
            //char* clipboard_text = soil_text.get_selection_c_str_alloc();
            SDL_SetClipboardText(clipboard_text);
            if(clipboard_text != NULL){delete[] clipboard_text;}
            
            //MARKER: UNDO, new action remove (insertion node)
            //str->remove(soil_text.SEL_S, (soil_text.SEL_E - soil_text.SEL_S));
            //soil_text.remove_selection() already sets the cursor
            int res = soil_text.remove_selection();
        }
    }

    if(!EDITABLE)
    {
        return;
    }

    if(command == STW_PASTE)
    {
        char* clipboard_text = SDL_GetClipboardText();

        if(soil_text.text_selected)
        {
            if(clipboard_text != NULL)
            {
                soil_string_pt->replace(clipboard_text, soil_text.SEL_S, (soil_text.SEL_E - soil_text.SEL_S));

                SDL_free(clipboard_text);
            }
        }else{
            if(clipboard_text != NULL)
            {
                //update_from_soil_text = false;
                soil_string_pt->insert(clipboard_text, cursor);

                SDL_free(clipboard_text);
            }
        }
    }

    if(command == STW_TAB)
    {
        //soil_text.tab(TAB_ALWAYS_INDENT, TAB_NORMAL_INDENT, REGULAR_ADD_TAB);        
        if(TAB_ALWAYS_INDENT || (TAB_NORMAL_INDENT && soil_text.valid_for_tab_selection_indent()))
            {
                soil_text.add_indent(REGULAR_ADD_TAB);
            }else{
                //update_from_soil_text = false;
                int new_cursor_pos;

                if(REGULAR_ADD_TAB)
                {
                    int sp_n = 0;
                    if(soil_text.text_selected)
                    {
                        sp_n = soil_string_pt->replace_with_regular_tab(soil_text.SEL_S, (soil_text.SEL_E - soil_text.SEL_S), soil_text.TAB_W);

                    }else{
                        sp_n = soil_string_pt->regular_tab_input(cursor, soil_text.TAB_W);                        
                    }
                }else{
                    bool res = false;
                    if(soil_text.text_selected)
                    {
                        res = soil_string_pt->replace_with_space(soil_text.SEL_S, (soil_text.SEL_E - soil_text.SEL_S), soil_text.TAB_W);
                    }else{
                        res = soil_string_pt->insert_space(cursor, soil_text.TAB_W);
                    }
                    //update_from_soil_text = true;
                    //soil_text.set_cursor(str->last_rec.cursor_1_after);
                    //soil_text.set_cursor_from_last_rec();
                }
            }//!not valid for indentation
    }
    if(command == STW_SHIFT_TAB && TAB_NORMAL_INDENT)
    {
        bool res = soil_text.remove_indent(REGULAR_DEL_TAB);
    }

    if(command == STW_LINEBREAK)
    {
        //soil_text.line_break(BR_KEEP_INDENT);

        if(soil_text.MULTI_LINE)
        {
        
            SoilString br_str;
            br_str='\n';
            int cursor_inc = 1;
            //update_from_soil_text = false;
            if(BR_KEEP_INDENT)
            {
                int indent_i = soil_text.cursor_line_indent(cursor);
                int line_start = soil_text.cursor_line_start(cursor);
                int keep_indent_n = indent_i - line_start;
                cursor_inc += keep_indent_n;
                br_str.insert_space(1,keep_indent_n);
            }
            if(soil_text.is_selected())
            {
                soil_string_pt->replace(br_str,soil_text.SEL_S, (soil_text.SEL_E - soil_text.SEL_S));
            }else{
                soil_string_pt->insert(br_str,cursor);
            }
        }else{
            //MARKER:send submit signal
        }
    }
    
    //DELETE
    if(command == STW_REMOVE_CHAR_RIGHT)
    {
        int res = 0;
        if(soil_text.text_selected)
        {
            res = soil_text.remove_selection();
        }else{
            bool regular_indent_removed = false;
            if(REGULAR_DEL_TAB)
            {
                res = soil_text.regular_indent_backspace_del(false);
            }
            if(res <= 0)
            {
                res = soil_string_pt->remove(cursor,1);
            }
        }
    }
       
    //BACKSPACE
    if(command == STW_REMOVE_CHAR_LEFT)
    {
        
        int res = 0;
        if(soil_text.is_selected())
        {
            res = soil_text.remove_selection();
        }else{
            bool regular_indent_removed = false;
            if(REGULAR_DEL_TAB)
            {
                res = soil_text.regular_indent_backspace_del(true);
            }
            if(res <= 0)
            {
                res = soil_string_pt->remove(cursor,-1);
            }
        }
    }

    if(command == STW_REMOVE_WORD)
    {
        int res;
        update_from_soil_text = false;
        soil_text.select_word(cursor, true);
        update_from_soil_text = true;
        res = soil_text.remove_selection();
    }

    if(command == STW_REMOVE_LINE)
    {        
        int res;
        update_from_soil_text = false;
        int s = soil_text.cursor_line_start(cursor);
        int e = soil_text.cursor_line_end(cursor);
        if(e < soil_string_pt->get_length()){e++;}
        soil_text.select_text(s,e);
        update_from_soil_text = true;
        res = soil_text.remove_selection();
    }

    if(command == STW_DUPLICATE_LINE)
    {
        int res;
        res = soil_string_pt->duplicate_line(cursor);
    }

    if(command == STW_UNDO)
    {
        if(REC_ACTIVE)
        {
            STR_REC_NODE node = REC.undo();
            //return;
            if(node.type >= 0)
            {
                if(node.selected_before)
                {
                    soil_text.select_text(node.cursor_2_before, node.cursor_1_before);
                }else{
                    soil_text.unselect_text();
                    soil_text.set_cursor(node.cursor_1_before);
                }
            }
        }
    }
    if(command == STW_REDO)
    {
        if(REC_ACTIVE)
        {
            STR_REC_NODE node = REC.redo();
            if(node.type >= 0)
            {
                if(node.selected_before)
                {
                    soil_text.select_text(node.cursor_2_before, node.cursor_1_before);
                }else{
                    soil_text.unselect_text();
                    soil_text.set_cursor(node.cursor_1_before);
                }
            }
        }
    }

    if(command == STW_MOVE_LINE_START)
    {
        if(HOME_TO_INDENT)
        {
            soil_text.move_cursor(0, shift_pressed);
        }else{
            soil_text.move_cursor(1, shift_pressed);
        }
    }
    if(command == STW_MOVE_LINE_END)
    {
        soil_text.move_cursor(2, shift_pressed);
    }
    if(command == STW_MOVE_LEFT)
    {
        soil_text.move_cursor(3, shift_pressed);
    }
    if(command == STW_MOVE_RIGHT)
    {
        soil_text.move_cursor(4, shift_pressed);
    }
    if(command == STW_MOVE_UP)
    {
        soil_text.move_cursor(5, shift_pressed);
    }
    if(command == STW_MOVE_DOWN)
    {
        soil_text.move_cursor(6, shift_pressed);
    }

    if(command == STW_MOVE_PREV_WORD_BOUND)
    {
        soil_text.move_cursor(7, shift_pressed);
    }
    if(command == STW_MOVE_NEXT_WORD_BOUND)
    {
        soil_text.move_cursor(8, shift_pressed);
    }
}
//---------------------------------------------- PROCESS KEYUP EVENT
void SoilTextWidget::keyup_event_virt(SDL_Event E)
{
}
//---------------------------------------------- SET AS FOCUS ELM
void SoilTextWidget::set_as_focus_elm_virt(bool f)
{
    if(!f && cursor_active)
    {
        cursor_active = false;
        if(cursor_vis){draw();}
    }
}
//---------------------------------------------- MOVE CONTENT VIEW
//return actual amount moved in POINT for x and y
POINT SoilTextWidget::move_content_view_virt(int mx, int my)
{
    //POINT res = SoilUIElm::move_content_view(mx, my);
    soil_text.text_changed = true;
    draw();

    text_widget_changed(102);
    
    POINT res = {mx,my};
    return res;
}

//---------------------------------------------- PROCESS FLAG
void SoilTextWidget::process_flag(int value)
{
    SoilUIElm::process_flag(value);
    if(value == 100)
    {
        cursor_vis = !cursor_vis;
        draw();
        //soil_text.draw_cursor(&pixmap, font, font_size, true);
        //SETTING ELM UI FLAG
        if(elm_ui_flag < 1)
        {
            elm_ui_flag = 1;
        }
        if(engine_control_is_set)
        {
            engine_control->set_ui_flag(1);
        }
    }
    
}
