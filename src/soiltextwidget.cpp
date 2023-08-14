#include "soiltextwidget.h"

SoilTextWidget::SoilTextWidget(WIDGET_TYPE widget_type) : SoilWidget(widget_type)
{
    //soil_string = &soil_text.soil_string;
    soil_string_pt = soil_text.get_soil_string_pt();
    //MARKER: make REC dynamic and what else not...
    init_flag(&cursor_flag);
    scrollable = true;
    PWD_MODE = false;
    mouse_left_down_cursor = (-1);

    //default_scrollbar_system is false because there is no pixmap for the entire text
    //if the text is bigger than the view
    default_scrollbar_system = false;
    font_size = 0;
    set_font_size(STW_DEFAULT_FONT_SIZE);
    //-------------------
    counter = 0;

    view_changed = false;
    text_changed = false;

    soil_text.set_soilobject_receiver(0, (SoilObject*)this, true);
}
//---------------------------------------------- 
SoilTextWidget::~SoilTextWidget()
{


}
//---------------------------------------------- VALIDATE FOR EXECUTION
void SoilTextWidget::validate_for_execution()
{

    SoilUIElm::validate_for_execution();
    process();

}
//---------------------------------------------- COMMAND MAP INDEX FROM KEYS
int SoilTextWidget::command_map_index_from_keys(bool ctrl, bool alt, bool shift, int key)
{
    //key would be the SCANCODE VALUE
    unsigned int ukey = (unsigned int) key;

    if(ukey > 83){return (-1);}

    //0: no special keys
    //1: ctrl
    //2: ctrl + alt
    //3: ctrl + shift
    //4: ctrl + alt + shift
    //5: alt
    //6: alt + shift
    //7: shift

    //the indexes above times 83 to determine the right offset in TEXT_COMMAND_MAP

    int sp_keys_i = 0;
    if(ctrl)
    {
        sp_keys_i = 83;
        if(alt)
        {
            sp_keys_i = 166;
            if(shift){sp_keys_i = 332;}
        }else{
            if(shift){sp_keys_i = 249;}
        }
    }else{
        if(alt)
        {
            sp_keys_i = 415;
            if(shift){sp_keys_i = 498;}
        }else{
            if(shift){sp_keys_i = 581;}
        }
    }
    return ukey + sp_keys_i;
}
//---------------------------------------------- SET COMMAND KEYS
bool SoilTextWidget::set_command_keys(bool ctrl, bool alt, int shift, int key, STW_COMMAND com)
{
    //key would be the SCANCODE VALUE
    int ind;
    //shift is the one that can be optional
    //0: no shift pressed
    //1: shift must be pressed
    //2: sets both shift not pressed and shift pressed, so the command is mapped in both key combinations

    shift = shift % 3;
    if(shift == 0 || shift == 2)
    {
        ind = command_map_index_from_keys(ctrl, alt, false, key);
        if(ind < 0){return false;}
        TEXT_COMMAND_MAP[ind] = com;
    }
    if(shift == 1 || shift == 2)
    {
        ind = command_map_index_from_keys(ctrl, alt, true, key);
        if(ind < 0){return false;}
        TEXT_COMMAND_MAP[ind] = com;
    }
    
    return true;

}
//---------------------------------------------- SET DEFAULT COMMAND MAP
int SoilTextWidget::set_default_command_map()
{
    int i = 0;
    while(i < TEXT_COMMAND_MAP_N)
    {
        TEXT_COMMAND_MAP[i] = STW_NONE;
        i++;
    }
    set_command_keys(true, false, 0, SDL_SCANCODE_C, STW_COPY);
    set_command_keys(true, false, 0, SDL_SCANCODE_X, STW_CUT);
    set_command_keys(true, false, 0, SDL_SCANCODE_V, STW_PASTE);
    set_command_keys(false, false, 0, SDL_SCANCODE_TAB, STW_TAB);
    set_command_keys(false, false, 1, SDL_SCANCODE_TAB, STW_SHIFT_TAB);//reverse_tab
    set_command_keys(false, false, 0, SDL_SCANCODE_RETURN, STW_LINEBREAK);
    set_command_keys(false, false, 0, SDL_SCANCODE_DELETE, STW_REMOVE_CHAR_RIGHT);//delete
    set_command_keys(false, false, 0, SDL_SCANCODE_BACKSPACE, STW_REMOVE_CHAR_LEFT);//backspace
    set_command_keys(true, false, 0, SDL_SCANCODE_W, STW_REMOVE_WORD);//backspace
    set_command_keys(true, false, 0, SDL_SCANCODE_L, STW_REMOVE_LINE);//backspace
    //STW_REMOVE_LINE);//
    set_command_keys(false, false, 2, SDL_SCANCODE_HOME, STW_MOVE_LINE_START);
    set_command_keys(false, false, 2, SDL_SCANCODE_END, STW_MOVE_LINE_END);
    set_command_keys(false, false, 2, SDL_SCANCODE_RIGHT, STW_MOVE_RIGHT);
    set_command_keys(false, false, 2, SDL_SCANCODE_LEFT, STW_MOVE_LEFT);
    set_command_keys(false, false, 2, SDL_SCANCODE_UP, STW_MOVE_UP);
    set_command_keys(false, false, 2, SDL_SCANCODE_DOWN, STW_MOVE_DOWN);
    set_command_keys(false, false, 2, SDL_SCANCODE_PAGEUP, STW_PAGE_UP);
    set_command_keys(false, false, 2, SDL_SCANCODE_PAGEDOWN, STW_PAGE_DOWN);
    set_command_keys(false, true, 2, SDL_SCANCODE_B, STW_MOVE_PREV_WORD_BOUND);
    set_command_keys(false, true, 2, SDL_SCANCODE_F, STW_MOVE_NEXT_WORD_BOUND);
    set_command_keys(true, false, 0, SDL_SCANCODE_D, STW_DUPLICATE_LINE);
    set_command_keys(true, false, 0, SDL_SCANCODE_Z, STW_UNDO);
    set_command_keys(true, false, 1, SDL_SCANCODE_Z, STW_REDO);

    return 0;
}
//---------------------------------------------- COMMAND FROM KEYS
STW_COMMAND SoilTextWidget::command_from_keys(bool ctrl, bool alt, bool shift, int key)
{
    int ind = command_map_index_from_keys(ctrl, alt, shift, key);
    if(ind < 0){return STW_NONE;}
    return TEXT_COMMAND_MAP[ind];
}


//---------------------------------------------- SET CODE EDIT SPECIAL SHORTCUTS
void SoilTextWidget::set_code_edit_special_shortcuts(bool activate)
{
    if(activate)
    {
        set_command_keys(true, false, 2, SDL_SCANCODE_A, STW_MOVE_LINE_START);
        set_command_keys(true, false, 2, SDL_SCANCODE_E, STW_MOVE_LINE_END);
        set_command_keys(true, false, 2, SDL_SCANCODE_F, STW_MOVE_RIGHT);
        set_command_keys(true, false, 2, SDL_SCANCODE_B, STW_MOVE_LEFT);
        set_command_keys(true, false, 2, SDL_SCANCODE_P, STW_MOVE_UP);
        set_command_keys(true, false, 2, SDL_SCANCODE_N, STW_MOVE_DOWN);
    }else{
        set_command_keys(true, false, 2, SDL_SCANCODE_A, STW_NONE);
        set_command_keys(true, false, 2, SDL_SCANCODE_E, STW_NONE);
        set_command_keys(true, false, 2, SDL_SCANCODE_F, STW_NONE);
        set_command_keys(true, false, 2, SDL_SCANCODE_B, STW_NONE);
        set_command_keys(true, false, 2, SDL_SCANCODE_P, STW_NONE);
        set_command_keys(true, false, 2, SDL_SCANCODE_N, STW_NONE);
    }
}
//---------------------------------------------- SET PWD MODE
void SoilTextWidget::set_pwd_mode(bool m)
{
    bool prev_mode = PWD_MODE;
    PWD_MODE = m;
    if(prev_mode != PWD_MODE)
    {
        soil_text.set_pwd_mode(PWD_MODE);
    }
}
//---------------------------------------------- SET TEXT RECORD
void SoilTextWidget::set_text_record(bool set)
{
    REC_ACTIVE = set;
    if(REC_ACTIVE)
    {
        REC.set_target_str(soil_string_pt);
    }else{
        REC.unset_target_str();
    }
}
//---------------------------------------------- SET TEXT RECORD
void SoilTextWidget::set_text_record(bool set, std::string storage_dir)
{
    REC_ACTIVE = set;
    if(REC_ACTIVE)
    {
        REC.set_target_str(soil_string_pt);
        REC.set_excess_storage(storage_dir);
        //REC.set_excess_storage(storage_dir);
    }else{
        REC.unset_target_str();
        
    }   
}
//---------------------------------------------- SET STR TEXT FIELDS
void SoilTextWidget::set_str_text_fields()
{
    /*
    if(soil_string_pt == NULL){return;}
    int cursor_2 = (-1);
    if(soil_text.text_selected)
    {
        if(soil_text.cursor_at_sel_s)
        {
            cursor_2 = soil_text.SEL_E;
        }else{
            cursor_2 = soil_text.SEL_S;
        }
    }

    soil_string_pt->set_last_rec_text_fields_before(soil_text.text_selected, soil_text.CURSOR, cursor_2);
    */
   //soil_text.set_str_text_fields();
}
//---------------------------------------------- SET AUTOBREAK
void SoilTextWidget::set_autobreak(bool a)
{
    AUTOBREAK = a;
    if(a)
    {
        FOLLOW_TEXT_W = false;
        int new_max_line_w = view_size.w / char_size.w;
        new_max_line_w--;
        soil_text.set_max_line_w(new_max_line_w);
    }else{
        soil_text.set_max_line_w(0);
    }
    
}
//---------------------------------------------- SET FOLLOW TEXT W
void SoilTextWidget::set_follow_text_w(bool f)
{
    FOLLOW_TEXT_W = f;
    if(f)
    {
        AUTOBREAK = false;
        soil_text.set_max_line_w(0);
    }
}
void SoilTextWidget::set_as_clickable_item(bool set)
{
    CLICKABLE_ITEM = set;
}
//---------------------------------------------- SET HORIZONTAL ALIGNMENT
void SoilTextWidget::set_horizontal_alignment(ALIGNMENT a)
{
    if(a == LEFT || a == CENTER || a == RIGHT)
    {
        ALIGNMENT prev_a = horizontal_alignment;
        horizontal_alignment = a;
        if(horizontal_alignment != prev_a)
        {
            process();
        }
    }
}
//---------------------------------------------- SET VERTICAL ALIGNMENT
void SoilTextWidget::set_vertical_alignment(ALIGNMENT a)
{
    if(a == TOP || a == CENTER || a == BOTTOM)
    {
        ALIGNMENT prev_a = vertical_alignment;
        vertical_alignment = a;
        if(vertical_alignment != prev_a)
        {
            process();
        }
    }
}
//---------------------------------------------- SET TEXT FIELD PROPERTIES
void SoilTextWidget::set_text_field_properties(Uint32 properties)
{
    if(properties & STW_TF_HOME_TO_INDENT)
    {
        HOME_TO_INDENT = true;
    }else{
        HOME_TO_INDENT = false;
    }

    if(properties & STW_TF_TAB_NORMAL_INDENT)
    {
        TAB_NORMAL_INDENT = true;
    }else{
        TAB_NORMAL_INDENT = false;
    }

    if(properties & STW_TF_TAB_ALWAYS_INDENT)
    {
        TAB_ALWAYS_INDENT = true;
    }else{
        TAB_ALWAYS_INDENT = false;
    }

    if(properties & STW_TF_REGULAR_ADD_TAB)
    {
        REGULAR_ADD_TAB = true;
    }else{
        REGULAR_DEL_TAB = true;
    }

    if(properties & STW_TF_REGULAR_DEL_TAB)
    {
        REGULAR_DEL_TAB = true;
    }else{
        REGULAR_DEL_TAB = false;
    }

    if(properties & STW_TF_BR_KEEP_INDENT)
    {
        BR_KEEP_INDENT = true;
    }else{
        BR_KEEP_INDENT = false;
    }

    if(properties & STW_TF_KEEP_X_MOVING_VERTICALLY)
    {
        KEEP_X_MOVING_VERTICALLY = true;
    }else{
        KEEP_X_MOVING_VERTICALLY = false;
    }

    if(properties & STW_TF_CODE_EDIT_TEXT_SHORTCUTS)
    {
        CODE_EDIT_TEXT_SHORTCUTS = true;
    }else{
        CODE_EDIT_TEXT_SHORTCUTS = false;
    }
}
//---------------------------------------------- GET TEXT LENGTH
int SoilTextWidget::get_text_length()
{
    return soil_string_pt->get_length();
}
//---------------------------------------------- GET SELECTION
bool SoilTextWidget::get_selection(int* sel_s, int* sel_e)
{
    bool result = soil_text.text_selected;
    *sel_s = soil_text.SEL_S;
    *sel_e = soil_text.SEL_E;

    return result;
}
//---------------------------------------------- IS EDITABLE
bool SoilTextWidget::is_editable()
{
    return EDITABLE;
}
//---------------------------------------------- IS MULTI LINE
bool SoilTextWidget::is_multi_line()
{
    return soil_text.MULTI_LINE;
}
//---------------------------------------------- IS REC ACTIVE
bool SoilTextWidget::is_rec_active()
{
    return REC_ACTIVE;
}
//---------------------------------------------- GET LINE N
int SoilTextWidget::get_line_n()
{
    return soil_text.line_n;
}
//---------------------------------------------- GET LINE N
int SoilTextWidget::get_font_size()
{
    return font_size;
}
//---------------------------------------------- SET RULER PACK
void SoilTextWidget::set_ruler_pack(int* line_n, int* font_s)
{
    *line_n = soil_text.line_n;
    *font_s = font_size;
}
//---------------------------------------------- SAVE STATE TO FILE
void SoilTextWidget::save_state_to_file(bool save_record, const SoilString & filename)
{
    //FORMAT:
    //CONTENT: 1B (if there is only text or text and record)
    //TEXT_LENGTH: 4B
    //TEXT
    //UNDO STACK
        //MEM_BLOCK_DATA_SIZE
        //MEM_BLOCK_N
        //EXCESS_FILE_SIZE
        //EXCESS_FILE_BLOCK_N
        //EXCESS_FILE_NODE_N
        //MEM_BLOCKS
        //EXCESS_FILE_CONTENT

    //REDO STACK    

    const char * fn = filename.c_str_alloc();

    FILE *fd = NULL;
    fd = fopen(fn, "w");
    if(fd == NULL){return;}

    unsigned int ch;

    if(save_record && REC_ACTIVE)
    {
        ch = 1;
    }else{
        ch = 0;
    }

    //CONTENT
    fputc(ch, fd);   

    
    //TEXT_LENGTH
    int len = soil_string_pt->get_byte_n();
    write_int_to_file(fd, len);

    //TEXT
    soil_string_pt->save_to_file(fd);

    if(!save_record || !REC_ACTIVE)
    {
        fclose(fd);
        delete[] fn;
        return;
    }
    REC.save_stack_to_file(UNDO_I, fd);
    REC.save_stack_to_file(REDO_I, fd);

    fclose(fd);
    delete[] fn;


    
}
//---------------------------------------------- LAOD STATE FROM FILE
void SoilTextWidget::load_state_from_file(const SoilString & filename)
{
    //FORMAT:
    //CONTENT: 1B (if there is only text or text and record)
    //TEXT_LENGTH: 4B
    //TEXT
    //UNDO STACK
        //MEM_BLOCK_DATA_SIZE
        //MEM_BLOCK_N
        //EXCESS_FILE_SIZE
        //EXCESS_FILE_BLOCK_N
        //EXCESS_FILE_NODE_N
        //MEM_BLOCKS
        //EXCESS_FILE_CONTENT
    //REDO STACK

    const char * fn = filename.c_str_alloc();

    FILE *fo = NULL;
    fo = fopen(fn, "r");
    if(fo == NULL){return;}

    unsigned int ch;
    int record_saved;
    int text_len;

    //CONTENT
    record_saved = fgetc(fo);
    
    //TEXT_LENGTH
    get_int_from_file(fo, &text_len);

    //TEXT
    REC.unset_target_str();
    //update_from_soil_text = false;
    soil_string_pt->load_from_file(fo, text_len);
    //update_from_soil_text = true;
    REC.set_target_str(soil_string_pt);
    
    //soil_string_pt->print_data();    

    //fclose(fo);
    //delete[] fn;
    //return;

    if(record_saved == 0 || !REC_ACTIVE)
    {
        fclose(fo);
        delete[] fn;
        return;
    }
    REC.load_stack_from_file(UNDO_I, fo);
    REC.load_stack_from_file(REDO_I, fo);


    fclose(fo);
    delete[] fn;
}

//---------------------------------------------- LOAG STATE FILE
void SoilTextWidget::log_state_file(const SoilString & state_filename, const SoilString & log_filename)
{
    //FORMAT:
    //CONTENT: 1B (if there is only text or text and record)
    //TEXT_LENGTH: 4B
    //TEXT
    //UNDO_STACK LENGTH: 4B
    //UNDO_STACK
    //REDO_STACK LENGTH: 4B
    //REDO_STACK
    //UNDO_REC_STR_LENGTH: 4B
    //UNDO_REC_STR
    //REDO_REC_STR_LENGTH: 4B
    //REDO_REC_STR

    const char * state_filename_c = state_filename.c_str_alloc();
    const char * log_filename_c = log_filename.c_str_alloc();

    FILE* fo = NULL;
    fo = fopen(state_filename_c, "r");
    if(fo == NULL)
    {
        delete[] state_filename_c;
        delete[] log_filename_c;
        return;
    }

    FILE* fd = NULL;
    fd = fopen(log_filename_c, "w");
    if(fd == NULL)
    {
        delete[] state_filename_c;
        delete[] log_filename_c;
        fclose(fo);
        return;
    }


    //----------------------------- 
    unsigned int ch;
    SoilString aux_str;
    SoilString sub_str;
    unsigned char size_data[4];
    int len = 0;
    int i = 0;
    int n = 0;
    int node_n = 0;

    //CONTENT
    ch = fgetc(fo);
    aux_str = "Content type:";
    aux_str.save_to_file(fd);
    fputc(ch+48, fd);

    //TEXT_LENGTH
    i = 0;
    while(i < 4)
    {
        ch = fgetc(fo);
        if(ch == EOF)
        {
            delete[] state_filename_c;
            delete[] log_filename_c;
            fclose(fo);
            fclose(fd);
            break;
        }
        size_data[i] = ch;
        i++;
    }
    len = 16777216*size_data[0];
    len += 65536*size_data[1];
    len += 256*size_data[2];
    len += size_data[3];
    aux_str = "\nString length:";
    aux_str.append(len,10);
    aux_str.save_to_file(fd);    

    //TEXT
    aux_str = "\nString:";
    sub_str = "";
    sub_str.prepend_from_file(fo,0, len);
    aux_str.save_to_file(fd);
    sub_str.save_to_file(fd);

    //UNDO STACK LENGTH
    i = 0;
    while(i < 4)
    {
        ch = fgetc(fo);
        if(ch == EOF)
        {
            delete[] state_filename_c;
            delete[] log_filename_c;
            fclose(fo);
            fclose(fd);
            break;
        }
        size_data[i] = ch;
        i++;
    }
    len = 16777216*size_data[0];
    len += 65536*size_data[1];
    len += 256*size_data[2];
    len += size_data[3];
    aux_str = "\nUNDO stack length:";
    aux_str.append(len,10);
    aux_str.save_to_file(fd);

    node_n = len;

    //------------------------------ UNDO NODES
    n = 0;
    while(n < node_n)
    {
        aux_str = "\n\nUNDO node ";
        aux_str.append(n, 10);
        aux_str.append(" :");
        aux_str.save_to_file(fd);
        //--------- type
        ch = fgetc(fo);
        aux_str = "\ntype:";
        aux_str.append((int)ch, 10);
        aux_str.save_to_file(fd);

        //--------- rec_pos
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nrec_pos:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);
        //--------- str_pos
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nstr_pos:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- ins_len
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nins_len:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- rem_len
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nrem_len:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- selected before
        ch = fgetc(fo);
        aux_str = "\nselected_before:";
        aux_str.append((int)ch, 10);
        aux_str.save_to_file(fd);

        //--------- cursor_1_before
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_1_before:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- cursor_2_before
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_2_before:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);


        //--------- selected after
        ch = fgetc(fo);
        aux_str = "\nselected_after:";
        aux_str.append((int)ch, 10);
        aux_str.save_to_file(fd);

        //--------- cursor_1_after
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_1_after:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- cursor_2_after
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_2_after:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        n++;
    }



    //REDO STACK LENGTH
    i = 0;
    while(i < 4)
    {
        ch = fgetc(fo);
        if(ch == EOF)
        {
            delete[] state_filename_c;
            delete[] log_filename_c;
            fclose(fo);
            fclose(fd);
            break;
        }
        size_data[i] = ch;
        i++;
    }
    len = 16777216*size_data[0];
    len += 65536*size_data[1];
    len += 256*size_data[2];
    len += size_data[3];
    aux_str = "\nREDO stack length:";
    aux_str.append(len,10);
    aux_str.save_to_file(fd);

    node_n = len;

    //------------------------------ REDO NODES
    n = 0;
    while(n < node_n)
    {
        aux_str = "\n\nREDO node ";
        aux_str.append(n, 10);
        aux_str.append(" :");
        aux_str.save_to_file(fd);
        //--------- type
        ch = fgetc(fo);
        aux_str = "\ntype:";
        aux_str.append((int)ch, 10);
        aux_str.save_to_file(fd);

        //--------- rec_pos
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nrec_pos:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);
        //--------- str_pos
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nstr_pos:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- ins_len
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nins_len:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- rem_len
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\nrem_len:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- selected before
        ch = fgetc(fo);
        aux_str = "\nselected_before:";
        aux_str.append((int)ch, 10);
        aux_str.save_to_file(fd);

        //--------- cursor_1_before
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_1_before:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- cursor_2_before
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_2_before:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);


        //--------- selected after
        ch = fgetc(fo);
        aux_str = "\nselected_after:";
        aux_str.append((int)ch, 10);
        aux_str.save_to_file(fd);

        //--------- cursor_1_after
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_1_after:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        //--------- cursor_2_after
        i = 0;
        while(i < 4)
        {
            ch = fgetc(fo);
            if(ch == EOF)
            {
                delete[] state_filename_c;
                delete[] log_filename_c;
                fclose(fo);
                fclose(fd);
                break;
            }
            size_data[i] = ch;
            i++;
        }
        len = 16777216*size_data[0];
        len += 65536*size_data[1];
        len += 256*size_data[2];
        len += size_data[3];
        aux_str = "\ncursor_2_after:";
        aux_str.append(len,10);
        aux_str.save_to_file(fd);

        n++;
    }

    aux_str = "\n-------------------\n";
    aux_str.save_to_file(fd);

    //UNDO STR LENGTH
    i = 0;
    while(i < 4)
    {
        ch = fgetc(fo);
        if(ch == EOF)
        {
            delete[] state_filename_c;
            delete[] log_filename_c;
            fclose(fo);
            fclose(fd);
            break;
        }
        size_data[i] = ch;
        i++;
    }
    len = 16777216*size_data[0];
    len += 65536*size_data[1];
    len += 256*size_data[2];
    len += size_data[3];
    aux_str = "\nUNDO string length:";
    aux_str.append(len,10);
    aux_str.save_to_file(fd);    

    //UNDO STR
    aux_str = "\nUndo String:";
    sub_str = "";
    sub_str.prepend_from_file(fo,0, len);
    aux_str.save_to_file(fd);
    sub_str.save_to_file(fd);

    //REDO STR LENGTH
    i = 0;
    while(i < 4)
    {
        ch = fgetc(fo);
        if(ch == EOF)
        {
            delete[] state_filename_c;
            delete[] log_filename_c;
            fclose(fo);
            fclose(fd);
            break;
        }
        size_data[i] = ch;
        i++;
    }
    len = 16777216*size_data[0];
    len += 65536*size_data[1];
    len += 256*size_data[2];
    len += size_data[3];
    aux_str = "\nREDO string length:";
    aux_str.append(len,10);
    aux_str.save_to_file(fd);    

    //REDO STR
    aux_str = "\nRedo String:";
    sub_str = "";
    sub_str.prepend_from_file(fo,0, len);
    aux_str.save_to_file(fd);
    sub_str.save_to_file(fd);


    
    
    delete[] state_filename_c;
    delete[] log_filename_c;
    fclose(fo);
    fclose(fd);
}

//---------------------------------------------- SET TEXT
void SoilTextWidget::set_text(const SoilString& str)
{
    soil_text.set_text(str);
    //soil_string_pt->replace_tabs_with_spaces();
}
//---------------------------------------------- SET TEXT
void SoilTextWidget::set_text(const char* str)
{
    soil_text.set_text(str);
    //soil_string_pt->replace_tabs_with_spaces();
}
//---------------------------------------------- UPDATE FROM FONT SIZE
bool SoilTextWidget::update_from_font_size(int fs)//doesnt call process()
{
    int prev_font_size = font_size;
    if(fs < 0){fs = 0;}
    if(fs >= FONT_SIZE_N){fs = FONT_SIZE_N - 1;}
    if(prev_font_size == fs){return false;}
    font_size = fs;
    char_size = SoilFont::get_char_size_from_font_size(font_size);
    text_size = soil_text.get_text_px_size(char_size);

    if(EDITABLE)
    {
        edit_extra_w_content = char_size.w;
    }
    text_size_changed = true;
    return true;
}
//---------------------------------------------- SET FONT SIZE
void SoilTextWidget::set_font_size(int fs)
{
    bool font_changed = update_from_font_size(fs);
    if(!font_changed){return;}

    view_min_size.w = (char_size.w * 6) + (2*content_pad);
    view_min_size.h = char_size.h + (2*content_pad);

    set_size(-1,-1);

    process();
}
//---------------------------------------------- SET SIZE
SIZE SoilTextWidget::set_size_virt(int w,int h)
{
    if(AUTOBREAK)
    {
        update_from_soil_text = false;
        int new_max_line_w = view_size.w / char_size.w;
        new_max_line_w--;
        soil_text.set_max_line_w(new_max_line_w);
        //soil_text.process_text();
        update_from_soil_text = true;
    }

    text_widget_changed(102);

    update_pixmap_from_size = true;
    
    process();
    
    return view_size;
}
//---------------------------------------------- ADJUST CONTENT VIEW TO CURSOR
void SoilTextWidget::adjust_content_view_to_cursor()
{
    SOIL_RECT cursor_rect = soil_text.soil_rect_from_cursor_pos(soil_text.get_cursor_pos(), char_size);
    int mov_x = 0;
    int mov_y = 0;

    //MARKER: ALIGN CORRECTION
    cursor_rect.x += margin_left;
    cursor_rect.y += margin_top;

    //MARKER:optimize this function
    int cursor_rect_ex = cursor_rect.x + cursor_rect.w;
    int cursor_rect_ey = cursor_rect.y + cursor_rect.h;

    //plus is a margin to make it work better so the user doesn't need to go so much
    //to the limit to move the view
    int plus_x = char_size.w;
    int plus_y = char_size.h / 2;

    //CENTRALIZE
    if(cursor_rect.x - plus_x < content_view_bound.start_x)
    {
        mov_x = ((cursor_rect.x - char_size.w) - content_view_bound.start_x) - plus_x;
    }
    if(cursor_rect_ex + plus_x > content_view_bound.end_x)
    {
        mov_x = (cursor_rect_ex - content_view_bound.end_x) + plus_x;
    }

    if(cursor_rect.y - plus_y < content_view_bound.start_y)
    {
        mov_y = (cursor_rect.y - content_view_bound.start_y) - plus_y;
    }
    if(cursor_rect_ey + plus_y > content_view_bound.end_y)
    {
        mov_y = (cursor_rect_ey - content_view_bound.end_y) + plus_y;
    }
    //MARKER: this will call draw(), avoid this repetition
    SoilUIElm::move_content_view(mov_x, mov_y);
}
//---------------------------------------------- COORD IN TEXT CONTENT
bool SoilTextWidget::coord_in_text_content(int in_view_x,int in_view_y)
{
    if(in_view_x < content_pad || in_view_x > (content_view_rect.w + content_pad))
    {
        return false;
    }
    if(in_view_y < content_pad || in_view_y > (content_view_rect.h + content_pad))
    {
        return false;
    }
    return true;
}
//---------------------------------------------- CORRECT COORD FOR SOIL TEXT
bool SoilTextWidget::correct_coord_for_soil_text(int in_view_x,int in_view_y, int* in_text_x, int* in_text_y)
{
    //MARKER: this widget may have an area that doesn't correspond to the 

    //text field
    //MARKER: ALIGN CORRECTION
    in_view_x += content_view_rect.x;
    in_view_x -= margin_left;
    //if(in_view_x < 0){in_view_x = 0;}

    in_view_y += content_view_rect.y;
    in_view_y -= margin_top;
    //if(in_view_y < 0){in_view_y = 0;}

    (*in_text_x) = in_view_x;
    (*in_text_y) = in_view_y;
    bool in_text = point_in_rect(content_view_rect, *in_text_x, *in_text_y);

    return in_text;
}
//---------------------------------------------- GET SOIL STRING PT
SoilString* SoilTextWidget::get_soil_string_pt()
{
    return soil_string_pt;
}
//---------------------------------------------- PROCESS (SOIL TEXT CHANGED)
bool SoilTextWidget::process()
{

    if(soil_text.text_changed || text_size_changed)
    {
        char_size = SoilFont::get_char_size_from_font_size(font_size);
        text_size = soil_text.get_text_px_size(char_size);
        if(FOLLOW_TEXT_W)
        {
            //fixed size
            //set_min_size(text_size.w+edit_extra_w_content,text_size.h);
            //set_max_size(text_size.w+edit_extra_w_content,text_size.h);
            SoilUIElm::internal_set_size(text_size.w+edit_extra_w_content,text_size.h);
            update_pixmap_from_size = true;
        }
    }
    if(update_pixmap_from_size)
    {
        int pix_w_inc = 2;//2 for the cursor
        if(text_size.w == 0){pix_w_inc += char_size.w;}
        //pixmap.alloc(text_size.w + pix_w_inc, text_size.h);
        pixmap.alloc(view_size.w, view_size.h);
        pixmap.spx_fill(0xFF000000);
        update_pixmap_from_size = false;
    }else{
        if(!pixmap.loaded)
        {
            pixmap.alloc(view_size.w, view_size.h);
            pixmap.spx_fill(0xFF000000);
        }
    }

    if(soil_text.text_changed)
    {
        text_changed = true;
        if(mouse_left_down_elm && mouse_left_down_cursor >= 0)
        {
            mouse_left_down_cursor = soil_text.get_cursor_pos();
        }
        set_content_size(text_size.w + edit_extra_w_content, text_size.h);
        //MARKER:check the conditions for TEXT_CHANGED to be sent
        if(SIGNALS[TEXT_CHANGED].active && engine_control_is_set)
        {
            engine_control->push_SS(&SIGNALS[TEXT_CHANGED]);
        }

    }
    if(soil_text.text_sel_changed)
    {
        if(SIGNALS[TEXT_SEL_CHANGED].active && engine_control_is_set)
        {
            SIGNALS[TEXT_SEL_CHANGED].arguments.text_sel_changed_arg.selected = soil_text.is_selected();
            SIGNALS[TEXT_SEL_CHANGED].arguments.text_sel_changed_arg.sel_s = soil_text.SEL_S;
            SIGNALS[TEXT_SEL_CHANGED].arguments.text_sel_changed_arg.sel_e = soil_text.SEL_E;
            engine_control->push_SS(&SIGNALS[TEXT_SEL_CHANGED]);
        }
    }

    if(soil_text.cursor_changed)
    {
        if(SIGNALS[TEXT_CURSOR_CHANGED].active && engine_control_is_set)
        {
            SIGNALS[TEXT_CURSOR_CHANGED].arguments.text_cursor_changed_arg.new_cursor = soil_text.get_cursor_pos();
            engine_control->push_SS(&SIGNALS[TEXT_CURSOR_CHANGED]);
        }
    }


    if(view_size.w > text_size.w)
    {
        if(horizontal_alignment == LEFT){margin_left = 0;}
        if(horizontal_alignment == RIGHT){margin_left = view_size.w - text_size.w;}
        if(horizontal_alignment == CENTER){margin_left = (view_size.w - text_size.w) / 2;}
    }else{
        margin_left = 0;
    }

    if(view_size.h > text_size.h)
    {
        if(vertical_alignment == TOP){margin_top = 0;}
        if(vertical_alignment == BOTTOM){margin_top = view_size.h - text_size.h;}
        if(vertical_alignment == CENTER){margin_top = (view_size.h - text_size.h) / 2;}
    }else{
        margin_top = 0;
    }


    if(soil_text.cursor_changed || soil_text.text_changed)
    {
        if(update_view_to_cursor)
        {
            adjust_content_view_to_cursor();
        }else{
            update_view_to_cursor = true;
        }
    }
    draw();

       
    //MARKER:check if this is really the best place for this
    text_size_changed = false;

    if(text_changed)
    {
        text_widget_changed(103);
        text_changed = false;
    }
    //MARKER:make sure every flag variable such as soil_text.cursor_changed gets reset accordingly
    soil_text.cursor_changed = false;
    
    return true;
}
//---------------------------------------------- DRAW
void SoilTextWidget::draw()
{
    if(!pixmap.loaded){return;}
    bool keep_cursor_vis = false;

    bool changed = false;

    SOIL_RECT orig_rect;
    orig_rect.x = content_view_rect.x;
    orig_rect.y = content_view_rect.y;
    orig_rect.w = content_view_rect.w;
    orig_rect.h = content_view_rect.h;

    //orig_rect.w = view_size.w;
    //orig_rect.h = view_size.h;
    if(soil_text.text_changed || soil_text.text_sel_changed || soil_text.cursor_changed)
    {        
        keep_cursor_vis = true;
        changed = true;
    }

    

    pixmap.spx_fill(0xFF000000);
    //margin_top = -12;
    
    //MARKER:IN_PAD:
    //MARKER: ALIGN CORRECTION
    SOIL_RECT draw_dest_rect = pixmap.get_full_rect();
    draw_dest_rect.x += margin_left + content_pad;
    draw_dest_rect.y += margin_top + content_pad;
    draw_dest_rect.w -= (2*content_pad);
    draw_dest_rect.h -= (2*content_pad);
    
    soil_text.draw_text_rect_px(orig_rect, &pixmap, draw_dest_rect, font, font_size);    

    //MARKER:is the cursor blinking when there is no text?

    POINT dest_offset;
    dest_offset.x = margin_left + content_pad;
    dest_offset.y = margin_top + content_pad;
    //soil_text.draw_text_rect_px(orig_rect, &pixmap, pixmap.get_full_rect(), font, font_size);    
    //printf("\n\ndraw::cursor changed");
    if(cursor_active)
    {
        if(keep_cursor_vis)
        {
            cursor_vis = true;
        }
        soil_text.draw_cursor(orig_rect, &pixmap, dest_offset, font, font_size, cursor_vis);
        //soil_text.draw_cursor(orig_rect, &pixmap, font, font_size, cursor_vis);
        //PUSH UI INTERVAL FLAG
        cursor_flag.interval = CURSOR_INTERVAL;
        if(!cursor_flag.in_control && engine_control_is_set)
        {
            cursor_flag.value = 100;
            engine_control->push_OF(&cursor_flag);
        }

    }else{
        //printf("\n\ndraw::cursor inactive");
        if(cursor_vis)
        {
            cursor_vis = false;
            soil_text.draw_cursor(orig_rect, &pixmap, dest_offset, font, font_size, cursor_vis);
            //soil_text.draw_cursor(orig_rect, &pixmap, font, font_size, false);        
        }
        cursor_flag.value = 0;
    }

    //MARKER:find a better way to invert the colors or mark the widget in the drawing action by literally swaping colors
    if(MARKED){pixmap.spx_invert_colors();}
    
    //dest_pixmap->spx_draw_patch_rect(win_pos_rect, 0xFFC02000);

    elm_ui_flag = 3;
    if(engine_control_is_set)
    {
        engine_control->set_ui_flag(3);
    }

       
    text_widget_changed(103);
    //MARKER: this_changed() will be called upon ANY change?
}

//---------------------------------------------- SOILOBJECT CALLBACK
int SoilTextWidget::soilobject_callback(int arg)
{

    if(arg == 1)
    {
        if(update_from_soil_text)
        {
            process();
        }
    }
    
    return 0;
}
//---------------------------------------------- THIS CHANGED
int SoilTextWidget::text_widget_changed(int arg)
{
    //printf("\nline view changed . . .");
    //printf("\n.");
    if(soilobject_receiver[1] != NULL && signal_receiver[1])
    {
        soilobject_receiver[1]->soilobject_callback(arg);
    }
    return 0;
}