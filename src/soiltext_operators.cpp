#include "soiltext.h"
//---------------------------------------------- SET CURSOR FORM LAST REC
void SoilText::set_cursor_from_last_rec()
{
    STR_REC_NODE str_last_rec = soil_string.get_last_rec();
    set_cursor(str_last_rec.cursor_1_after);

}
//---------------------------------------------- SET CURSOR
void SoilText::set_cursor(int c, bool select)
{
    //select is when shift is pressed
    //MARKER:should I unselect even if c == CURSOR if select is false?I would say YES
    int prev_cursor = CURSOR;
    if(select)
    {
        internal_set_cursor(c);
        if(text_selected)
        {
            if(cursor_at_sel_s)
            {
                internal_select_text(SEL_E, CURSOR);
            }else{
                internal_select_text(SEL_S, CURSOR);
            }

        }else{
            internal_select_text(prev_cursor, CURSOR);
        }
    }else{
        if(text_selected)
        {
            unselect_text();
        }
        internal_set_cursor(c);
    }
    this_changed();

}
//---------------------------------------------- SET CURSOR
void SoilText::set_cursor(int c)
{
    set_cursor(c, false);
}

//---------------------------------------------- SET CURSOR
void SoilText::inc_cursor(int inc)
{
    int new_cursor = CURSOR + inc;
    //MARKER: do I really need to validate this?
    validate_abs_cursor(soil_string.get_length(), &new_cursor);
    set_cursor(new_cursor);
}
//---------------------------------------------- MOVE CURSOR
void SoilText::move_cursor(int dir, bool select)
{
    //dir:
    //0: line start
    //1: line end
    //2: left
    //3: right
    //4: up
    //5: down

    //printf("\nLINE INDENTATION VALUES");

    int i=0;
    while(i < line_n)
    {
        int ind = cursor_line_indent(line_index[i].first_char);
        //printf("\nLINE %d IND:%d", i, ind);
        //printf("\nLINE %d first non space:%d", i, line_index[i].first_non_space);
        i++;
    }

    int new_cursor;

    if(dir == 0)
    {
        new_cursor = cursor_line_indent(CURSOR);
        if(CURSOR == new_cursor)
        {
            new_cursor = cursor_line_start(CURSOR);
        }
    }

    if(dir == 1)
    {
        new_cursor = cursor_line_start(CURSOR);
    }

    if(dir == 2)
    {
        new_cursor = cursor_line_end(CURSOR);
    }

    if(dir == 3)
    {
        new_cursor = CURSOR - 1;
        if(text_selected && !select){new_cursor = SEL_S;}
    }
    if(dir == 4)
    {
        new_cursor = CURSOR + 1;
        if(text_selected && !select){new_cursor = SEL_E;}
    }

    if(dir == 5)
    {
        if(text_selected && !select)
        {
            new_cursor = cursor_above(SEL_S, true);
        }else{
            new_cursor = cursor_above(CURSOR, true);
        }
    }
    if(dir == 6)
    {
        if(text_selected && !select)
        {
            new_cursor = cursor_below(SEL_E, true);
        }else{
            new_cursor = cursor_below(CURSOR, true);
        }
    }
    if(dir == 7)
    {
        if(text_selected && !select)
        {
            new_cursor = soil_string.next_word_bound(SEL_S, -1);
        }else{
            new_cursor = soil_string.next_word_bound(CURSOR, -1);
        }
    }
    if(dir == 8)
    {
        if(text_selected && !select)
        {
            new_cursor = soil_string.next_word_bound(SEL_E, 1);
        }else{
            new_cursor = soil_string.next_word_bound(CURSOR, 1);
        }
    }

    set_cursor(new_cursor, select);

    //printf("\n");

}
//---------------------------------------------- SET CURSOR TO NEXT WORD BOUND
//select is true when shift is pressed
void SoilText::set_cursor_next_word(int dir, bool select)
{
    if(CURSOR < 0){return;}
    signal_receiver[0] = false;
    int new_cursor = soil_string.next_word_bound(CURSOR, dir);
    process_cursor_motion(new_cursor, new_cursor, select);
    signal_receiver[0] = true;
    this_changed();
}
//---------------------------------------------- SET TEXT
void SoilText::set_text(const char* str)
{
    soil_string = str;
    //this_changed();
}
//---------------------------------------------- SET TEXT
void SoilText::set_text(const SoilString& str)
{
    soil_string = str;
    //this_changed();
}
//---------------------------------------------- DUPLICATE LINE
void SoilText::duplicate_line()
{
    //undo rec
    int n;
    signal_receiver[0] = false;
    n = soil_string.duplicate_line(CURSOR);
    //MARKER, UNDO type 0
    signal_receiver[0] = true;
    inc_cursor(n);
}
//---------------------------------------------- REMOVE WORD
void SoilText::remove_word()
{
    //undo rec
    signal_receiver[0] = false;
    select_word(CURSOR, true);
    signal_receiver[0] = true;
    remove_selection();
}
//---------------------------------------------- REMOVE LINE
void SoilText::remove_line()
{
    //undo rec
    int res;
    signal_receiver[0] = false;
    int s = cursor_line_start(CURSOR);
    int e = cursor_line_end(CURSOR);
    if(e < soil_string.get_length()){e++;}
    select_text(s,e);
    signal_receiver[0] = true;

    //MARKER, UNDO type 1

    res = remove_selection();
}
//---------------------------------------------- TAB INPUT
/*
void SoilText::tab(bool always_indent, bool normal_indent, bool regular_tab)
{
    //undo rec
    if(always_indent || (normal_indent && valid_for_tab_selection_indent()))
    {
        add_indent(regular_tab);
        //MARKER: UNDO, make it so soil_text return data for the REC, it would be a type 2 (transformation)

    }else{
         
        if(regular_tab)
        {
            regular_tab_input();
            //MARKER: UNDO, make text removal node
        }else{
            //insert_space(TAB_W);
        }
    }

}
*/
//---------------------------------------------- IS SELECTION VALID FOR TAB SELECTION INDENT
//validate for conventional selection indent in text editors
//if the line is selected from line_start to line_end
//or any line break is selected
bool SoilText::valid_for_tab_selection_indent()
{
    if(!text_selected){return false;}
    int sel_s_line_start = cursor_line_start(SEL_S);
    int sel_e_line_start = cursor_line_start(SEL_E);

    if(sel_s_line_start != sel_e_line_start){return true;}

    int sel_e_line_end = cursor_line_end(SEL_E);
    if(SEL_S == sel_s_line_start && SEL_E == sel_e_line_end){return true;}

    return false;
}
//---------------------------------------------- INDENT
//means indenting the LINE where the CURSOR is, or the LINES selected
//this function doesn't place spaces in the middle of lines
bool SoilText::add_indent(bool regular)
{

    //undo rec

    int sp_before_pos_n = 0;
    int sp_middle_n = 0;
    int sp_last_line_n = 0;

    bool res;
    //this is for the return type of SoilString::regular_tab_input()
    int res_n;

    signal_receiver[0] = false;
    if(text_selected)
    {
        int sel_s = SEL_S;
        int sel_e = SEL_E;

        int orig_sel_s_line_indent = cursor_line_indent(SEL_S);

        int n = SEL_E - SEL_S;
        res = soil_string.indent_section(SEL_S, n, TAB_W, false, regular, &sp_before_pos_n, &sp_middle_n, &sp_last_line_n);
        if(!res)
        {
            //no indentation added
            signal_receiver[0] = true;
            return false;
        }

        //correcting SEL_S
        if(sel_s > orig_sel_s_line_indent)
        {
            sel_s += sp_before_pos_n;
        }
        //correcting SEL_E
        sel_e += (sp_before_pos_n + sp_middle_n + sp_last_line_n);
        if(cursor_at_sel_s)
        {
            select_text(sel_e, sel_s);
        }else{
            select_text(sel_s, sel_e);
        }

        signal_receiver[0] = true;
        this_changed();
        return true;

    }else{
        res = soil_string.indent_section(CURSOR, 0, TAB_W, false, regular, &sp_before_pos_n, &sp_middle_n, &sp_last_line_n);
        if(!res)
        {
            signal_receiver[0] = true;
            return false;
        }
        int new_cursor = CURSOR + sp_before_pos_n;
        set_cursor(new_cursor);
        signal_receiver[0] = true;
        this_changed();
        //MARKER: apparently there is a return true; missing here
    
    }
    return false;
}
//---------------------------------------------- INDENT
bool SoilText::remove_indent(bool regular)
{
    int sp_before_pos_n = 0;
    int sp_middle_n = 0;
    int sp_last_line_n = 0;

    //undo rec

    bool res;
    //this is for the return type of SoilString::regular_tab_input()
    int res_n;

    signal_receiver[0] = false;
    if(text_selected)
    {
        int sel_s = SEL_S;
        int sel_e = SEL_E;

        int orig_sel_s_line_indent = cursor_line_indent(SEL_S);
        int orig_sel_e_line_indent = cursor_line_indent(SEL_E);

        int n = SEL_E - SEL_S;
        res = soil_string.indent_section(SEL_S, n, TAB_W, true, regular, &sp_before_pos_n, &sp_middle_n, &sp_last_line_n);

        if(!res)
        {
            signal_receiver[0] = true;
            return false;
        }

        //correcting SEL_S
        int dif = orig_sel_s_line_indent - sel_s; 
        int sel_mov = sp_before_pos_n - dif;
        if(sel_mov < 0){sel_mov = 0;}
        if(sel_mov > sp_before_pos_n){sel_mov = sp_before_pos_n;}
        sel_s -= sel_mov;
        
        //correcting SEL_E
        dif = orig_sel_e_line_indent - sel_e;
        sel_mov = sp_last_line_n - dif;

        if(sel_mov < 0){sel_mov = 0;}
        if(sel_mov > sp_last_line_n){sel_mov = sp_last_line_n;}
        sel_e -= (sel_mov + sp_before_pos_n + sp_middle_n);
        
        if(cursor_at_sel_s)
        {
            select_text(sel_e, sel_s);
        }else{
            select_text(sel_s, sel_e);
        }

        signal_receiver[0] = true;
        this_changed();
        
        return true;

    }else{
        int orig_sel_s_line_start = cursor_line_start(CURSOR);
        res = soil_string.indent_section(CURSOR, 0, TAB_W, true, regular, &sp_before_pos_n, &sp_middle_n, &sp_last_line_n);  
        if(!res)
        {
            signal_receiver[0] = true;
            return false;
        }
        
        int new_cursor = CURSOR - sp_before_pos_n;
        if(new_cursor < orig_sel_s_line_start){new_cursor = orig_sel_s_line_start;}
        set_cursor(new_cursor);
        signal_receiver[0] = true;
        this_changed();
        return true;
    
    }
    return false;
}


//---------------------------------------------- REGULAR TAB INPUT
//probably wont be using this
bool SoilText::regular_tab_input()
{
    if(CURSOR < 0){return false;}
    signal_receiver[0] = false;
    int sp_n = soil_string.regular_tab_input(CURSOR, TAB_W);
    signal_receiver[0] = true;
    set_cursor(CURSOR + sp_n);
    //undo rec

    return true;
}

//---------------------------------------------- REGULAR INDENT REMOVE
//This is for when you press BACKSPACE or DELETE to remove spaces in the indentation
//to the next multiple of TAB_W
int SoilText::regular_indent_backspace_del(bool backspace)
{
    if(CURSOR < 0){return (-1);}
    int indent_i = cursor_line_indent(CURSOR);
    int line_start = cursor_line_start(CURSOR);

    //undo rec

    if(backspace && CURSOR > line_start && CURSOR <= indent_i)
    {
        //removing regular indent spacing
        int pos_in_line = CURSOR - line_start;
        int mod = pos_in_line % TAB_W;
        if(mod == 0){mod = TAB_W;}
        signal_receiver[0] = false;
        soil_string.remove(CURSOR, (-1) * mod);
        //signal_change = true;
        //inc_cursor((-1) * mod);
        return mod;
    }

    if(!backspace && CURSOR < indent_i)
    {
        int pos_in_line = CURSOR - line_start;
        int mod = pos_in_line % TAB_W;
        int del_n = TAB_W - mod;
        int dif = indent_i - CURSOR;
        if(del_n > dif){del_n = dif;}
        soil_string.remove(CURSOR, del_n);

        return del_n;
    }
    return (-1);
}
//---------------------------------------------- SEARCH STR
int SoilText::search_str(const SoilString& str, int pos, int n, bool match_case, bool match_accent)
{
    int res = soil_string.search_str(str, pos, n, match_case, match_accent);
    return res;
}
//---------------------------------------------- SEARCH REPLACE STR
int SoilText::search_replace_str(const SoilString& str, const SoilString& rep, int pos, int n, bool match_case, bool match_accent)//return number of occurrences
{
    int res = soil_string.search_replace_str(str, rep, pos, n, match_case, match_accent);
    //undo rec
    return res;
}
//---------------------------------------------- EXTERNAL SET KEPT X
void SoilText::external_set_kept_x(int x)
{
    kept_x = x;
}
//---------------------------------------------- UNSELECT TEXT
bool SoilText::unselect_text()
{

    //check return type for every function in this class and SoilTextWidget
    bool prev_selected = text_selected;
    SEL_S = 0;
    SEL_E = 0;
    text_selected = false;
    if(prev_selected)
    {
        text_sel_changed = true;
        this_changed();
    }

return true;
}
//---------------------------------------------- SET SELELCTION
//accepts negative value
bool SoilText::select_text(int sel_a, int sel_b)
{

    internal_select_text(sel_a, sel_b);
    internal_set_cursor(sel_b);

    this_changed();
    
    return true;
}
//---------------------------------------------- SET SELECTION
bool SoilText::select_text(POINT mouse_a, POINT mouse_b, SIZE char_size)
{

    int sel_a;
    int sel_b;
    cursor_pos_from_mouse(mouse_a.x,mouse_a.y,char_size,&sel_a);
    cursor_pos_from_mouse(mouse_b.x,mouse_b.y,char_size,&sel_b);

    return select_text(sel_a,sel_b);
}
//---------------------------------------------- SET SELECTION
bool SoilText::select_text(int ax,int ay,int bx,int by, SIZE char_size)
{

    int sel_a;
    int sel_b;
    cursor_pos_from_mouse(ax,ay,char_size,&sel_a);
    cursor_pos_from_mouse(bx,by,char_size,&sel_b);

    return select_text(sel_a,sel_b);
}
//---------------------------------------------- SET WORD SELECTION
bool SoilText::select_word(int cursor_i, bool include_trailing_sp)
{
    int sel_a;
    int sel_b;

    soil_string.word_bounds(cursor_i, &sel_a, &sel_b, include_trailing_sp);

    return select_text(sel_a,sel_b);
}
//---------------------------------------------- SET WORD SELECTION
bool SoilText::select_word(int cursor_i)
{
    return select_word(cursor_i, false);
}
//---------------------------------------------- SET WORD SELECTION
bool SoilText::select_word(POINT mouse_coord, SIZE char_size, bool include_trailing_sp)
{
    int cursor_i;
    cursor_pos_from_mouse(mouse_coord.x,mouse_coord.y,char_size,&cursor_i);

    return select_word(cursor_i, include_trailing_sp);
}
//---------------------------------------------- SET WORD SELECTION
bool SoilText::select_word(POINT mouse_coord, SIZE char_size)
{

    int cursor_i;
    cursor_pos_from_mouse(mouse_coord.x,mouse_coord.y,char_size,&cursor_i);

    return select_word(cursor_i, false);
}
//---------------------------------------------- SET WORD SELECTION
bool SoilText::select_word(int mouse_coord_x,int mouse_coord_y, SIZE char_size, bool include_trailing_sp)
{
    int cursor_i;
    cursor_pos_from_mouse(mouse_coord_x,mouse_coord_y,char_size,&cursor_i);

    return select_word(cursor_i, include_trailing_sp);
}
//---------------------------------------------- SET WORD SELECTION
bool SoilText::select_word(int mouse_coord_x,int mouse_coord_y, SIZE char_size)
{

    int cursor_i;
    cursor_pos_from_mouse(mouse_coord_x,mouse_coord_y,char_size,&cursor_i);

    return select_word(cursor_i, false);
}

//---------------------------------------------- SELECT ALL
bool SoilText::select_line(int line_i)
{

    if(line_i >= 0 && line_i < line_n)
    {
        int sel_a = line_index[line_i].first_char;
        int sel_b = sel_a + line_index[line_i].length;

        return select_text(sel_a,sel_b);
    }
   

    return false;
}
//---------------------------------------------- SELECT LINE
bool SoilText::select_line(POINT mouse_coord,SIZE char_size)
{

    POINT text_coord;
    text_coord = text_coord_from_mouse(mouse_coord.x,mouse_coord.y,char_size);

    return select_line(text_coord.y);
}

//---------------------------------------------- SELECT ALL
bool SoilText::select_all()
{
    return select_text(0,soil_string.get_length());
}
//---------------------------------------------- REMOVE SELECTION
int SoilText::remove_selection()
{

    if(SEL_S != SEL_E)
    {
        bool prev_signal_receiver_0 = signal_receiver[0];
        int n = SEL_E - SEL_S;
        signal_receiver[0] = false;
        soil_string.remove(SEL_S,n);
        
        set_cursor_from_last_rec();

        signal_receiver[0] = prev_signal_receiver_0;
        this_changed();
        return SEL_E - SEL_S;
    }
    //undo rec
    return 0;
}
//---------------------------------------------- GET SELECTION SOIL STR
SoilString SoilText::get_selection_soil_str()
{
    SoilString str = "";
    if(text_selected)
    {
        str = soil_string.section(SEL_S, SEL_E-SEL_S);
    }
    return str;
}
//---------------------------------------------- GET SELECTION C STR ALLOC
char* SoilText::get_selection_c_str_alloc()
{
    char *pt = NULL;
    if(text_selected)
    {
        pt = soil_string.section_c_str_alloc(SEL_S, SEL_E-SEL_S);
    }
    return pt;
}
//---------------------------------------------- PROCESS CURSOR MOTION
//select is true when shift is pressed
void SoilText::process_cursor_motion(int new_cursor, int new_cursor_sel, bool select)
{
  if(text_selected)
    {
        if(select)
        {   
            set_cursor(new_cursor);
        }else{
            unselect_text();
            set_cursor(new_cursor_sel);
        }
    }else{
        if(select)
        {
            select_text(CURSOR, new_cursor);
        }else{
            set_cursor(new_cursor);
        }
    }
}

//---------------------------------------------- IS SELECTED
bool SoilText::is_selected()
{

    if(SEL_S != SEL_E){return true;}
    
    return false;
}
//---------------------------------------------- CURSOR IS AFTER INDENT
bool SoilText::cursor_is_after_indent()
{

    if(CURSOR < 0){return false;}
    int y = ch_span[CURSOR].y;
    if(CURSOR <= line_index[y].first_non_space)
    {
        return false;
    }else{
        return true;
    }
}
//---------------------------------------------- CURSOR IS AT LINE START
bool SoilText::cursor_is_at_line_start()
{
    if(CURSOR < 0){return false;}
    int y = ch_span[CURSOR].y;
    if(CURSOR == line_index[y].first_char)
    {
        return true;
    }
    
return false;
}
//---------------------------------------------- GET TEXT SIZE
SIZE SoilText::get_text_px_size(SIZE char_size)
{
    SIZE ret;
    ret.w = text_ch_size.w * char_size.w;
    ret.h = text_ch_size.h * char_size.h;
    
return ret;
}
//---------------------------------------------- GET TEXT SIZE
SIZE SoilText::get_text_px_size(int font_size)
{

    SIZE char_size = SoilFont::get_char_size_from_font_size(font_size);

    SIZE ret;
    ret.w = text_ch_size.w * char_size.w;
    ret.h = text_ch_size.h * char_size.h;
    
return ret;
}
//---------------------------------------------- DRAW TEXT RECT PIXMAP
//This is what SoilTextWidget currently uses in draw()
bool SoilText::draw_text_rect_px(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, SOIL_RECT dest_rect, SoilFont* font, int font_size)
{
    
    if(!dest_pixmap->loaded || font==NULL)
    {
        return false;
    }
    int ind = 0;
    SIZE char_size = SoilFont::get_char_size_from_font_size(font_size);
    
    //IMPORTANT:orig_rect is in pixels, the actual position in text depends on char_size/font_size
    if(!adjust_rect_to_size(dest_pixmap->size, &dest_rect)){return false;}

    SIZE text_size;
    text_size.w = char_size.w * text_ch_size.w;
    text_size.h = char_size.h * text_ch_size.h;
    if(!adjust_rect_to_size(dest_pixmap->size, &dest_rect)){return false;}
    if(orig_rect.w > dest_rect.w){orig_rect.w = dest_rect.w;}else{dest_rect.w = orig_rect.w;}
    if(orig_rect.h > dest_rect.h){orig_rect.h = dest_rect.h;}else{dest_rect.h = orig_rect.h;}


    dest_pixmap->spx_draw_solid_rect(dest_rect,0xFF000000);
    ///////////
  
    int orig_rect_ex = orig_rect.x + orig_rect.w;
    int orig_rect_ey = orig_rect.y + orig_rect.h;

    int coord_sx = orig_rect.x / char_size.w;
    int coord_sy = orig_rect.y / char_size.h;
    int coord_ex = orig_rect_ex / char_size.w;
    int coord_ey = orig_rect_ey / char_size.h;

    int x = coord_sx;
    int y = coord_sy;

    int margin_left = orig_rect.x % char_size.w;
    int margin_right = char_size.w - (orig_rect_ex % char_size.w);
    int margin_top = orig_rect.y % char_size.h;
    int margin_bottom = char_size.h - (orig_rect_ey % char_size.h);

    ///
    SOIL_RECT font_rect;
    SOIL_RECT paste_rect;
    paste_rect.x = dest_rect.x;
    paste_rect.y = dest_rect.y;
    paste_rect.w = char_size.w;
    paste_rect.h = char_size.h;
    ///

    int inc_x;
    int inc_y;

    //font_inverted is for when text is selected
    bool font_inverted;
    int char_i = 0;
    while(y <= coord_ey)
    {
        //printf("\n\nNEW LINE: y%d\n", y);
        font_rect.y = 0;
        
        font_rect.h = char_size.h;
        if(y == coord_sy){font_rect.y += margin_top;font_rect.h -= margin_top;}
        if(y == coord_ey){font_rect.h -= margin_bottom;}

        while(x <= coord_ex)
        {
            font_rect.x = 0;
            font_rect.w = char_size.w;
            if(x == coord_sx){font_rect.x += margin_left;font_rect.w -= margin_left;}
            if(x == coord_ex){font_rect.w -= margin_right;}

            //MARKER:check if cursor_pos_from_coord() is really adequate here as well
            //I mean, adequate for getting the char for printing
            bool char_at_pos = cursor_pos_from_coord(x,y,&char_i);
            
            SOIL_CHAR sch;
            if(char_at_pos)
            {
                if(PWD_MODE)
                {
                    sch = SoilString::c_char_to_soil_char('*');
                }else{
                    sch = soil_string.at(char_i);
                }
            }else{
                sch = SoilString::c_char_to_soil_char(' ');
            }
            if(text_selected && char_i >= SEL_S && char_i < SEL_E)
            {
                font_inverted = true;               
            }else{
                font_inverted = false;
            }
            font->print_font_to_pixmap(sch, font_size, font_inverted, font_rect, dest_pixmap, paste_rect);  
            paste_rect.x += font_rect.w;

            x++;
        }
        paste_rect.x = dest_rect.x;
        paste_rect.y += font_rect.h;


        x = coord_sx;
        y++;        
    }

    text_changed = false;
    text_sel_changed = false;

    //int ind = cursor_pos_from_coord()

    return true;

}

//---------------------------------------------- DRAW FULL PX
//return true if it got after the drawing loop
//currently SoilTextWidget doesn't use this
bool SoilText::draw_full_px(SoilPixmap* dest_pixmap, POINT text_dest_offset, SoilFont* font, int font_size)
{
    
    SIZE char_size = SoilFont::get_char_size_from_font_size(font_size);
    if(!dest_pixmap->loaded || font==NULL)
    {
        return false;
    }
    Uint32 color_paint = 0xFFFFFFFF;
    Uint32 color_bkg = 0xFF000000;
    
    dest_pixmap->spx_fill(0xFF000000);
    SOIL_CHAR ch;
    int ind;
    SOIL_RECT font_rect;
    SOIL_RECT paste_rect;
    font_rect.w = char_size.w;
    font_rect.h = char_size.h;
    font_rect.y = 0;

    bool in_selection = false;

    int v = 0;
    int i = 0;
    int len = soil_string.get_length();
    while(i < len)
    {
        ch = soil_string.at(i);
        ind = font->get_char_index(ch);

        if(i == SEL_S){in_selection = true;}
        if(i == SEL_E){in_selection = false;}
        if(ind >= 0)
        {
            font_rect.x = ind*char_size.w;

            paste_rect.x = text_dest_offset.x + ch_span[i].x * char_size.w;
            paste_rect.y = text_dest_offset.y + ch_span[i].y * char_size.h;
            paste_rect.w = ch_span[i].w * char_size.w;
            paste_rect.h = char_size.h;

            //MARKER:This is for making line break selected narrower
            //if(ch_span[i].br == 1){paste_rect.w /= 2;}

            //CURSOR / SELECTION
            if(in_selection)
            {
                color_paint = 0xFF000000;
                color_bkg = 0xFFFFFFFF;
                font_rect.y = char_size.h;
            }else{
                color_paint = 0xFFFFFFFF;
                color_bkg = 0xFF000000;
                font_rect.y = 0;
            }
            v++;

            //printf("\npaste rect x:%d. y:%d. W:%d. H:%d. ",paste_rect.x,paste_rect.y,paste_rect.w,paste_rect.h);
            dest_pixmap->spx_paste_pixmap(font->pixmap[font_size], font_rect, paste_rect);
        }

        i++;
    }

    text_changed = false;
    text_sel_changed = false;
    
return true;
}

//---------------------------------------------- DRAW FULL PX
//return true if it got after the drawing loop
bool SoilText::draw_full_px(SoilPixmap* dest_pixmap, SoilFont* font, int font_size)
{

    POINT text_dest_offset;
    text_dest_offset.x = 0;
    text_dest_offset.y = 0;
    
    return draw_full_px(dest_pixmap, text_dest_offset, font, font_size);
}

//---------------------------------------------- DRAW CURSOR
// this calls set_cursor_img
void SoilText::draw_cursor(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, POINT text_dest_offset, SoilFont * font, int font_size, bool visible)
{
    if(prev_drawn_CURSOR >= 0 && CURSOR != prev_drawn_CURSOR)
    {
        set_cursor_img(orig_rect, dest_pixmap, text_dest_offset, font, font_size, prev_drawn_CURSOR, false);
    }
    set_cursor_img(orig_rect, dest_pixmap, text_dest_offset, font, font_size, CURSOR, visible);
    if(visible)
    {
        prev_drawn_CURSOR = CURSOR;
    }else{
        prev_drawn_CURSOR = (-1);
    }
    cursor_changed = false;
}
//---------------------------------------------- DRAW CURSOR
// this calls set_cursor_img
void SoilText::draw_cursor(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, SoilFont * font, int font_size, bool visible)
{
    POINT text_dest_offset;
    text_dest_offset.x = 0;
    text_dest_offset.y = 0;
    draw_cursor(orig_rect, dest_pixmap, text_dest_offset, font, font_size, visible);
}
//---------------------------------------------- SET CURSOR IMG
void SoilText::set_cursor_img(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, POINT text_dest_offset, SoilFont * font, int font_size, int cursor_pos, bool visible)
{

    
    SIZE char_size = SoilFont::get_char_size_from_font_size(font_size);
    POINT cursor_coord;
    coord_from_cursor_pos(cursor_pos, &cursor_coord);

    if(visible)
    {
        Uint32 cursor_color = 0xFFFFFFFF;
        if(cursor_pos >= SEL_S && cursor_pos < SEL_E)
        {
            cursor_color = 0xFF000000;
        }

        SOIL_RECT cursor_rect;
        cursor_rect.x = cursor_coord.x * char_size.w;
        cursor_rect.y = cursor_coord.y * char_size.h + 2;
        cursor_rect.w = 2;
        cursor_rect.h = char_size.h - 4;

        cursor_rect = rect_intersection(cursor_rect, orig_rect);
        if(cursor_rect.w == 0 || cursor_rect.h == 0){return;}

        cursor_rect.x += text_dest_offset.x;
        cursor_rect.y += text_dest_offset.y;
        
        cursor_rect.x -= orig_rect.x;
        cursor_rect.y -= orig_rect.y;

        dest_pixmap->spx_draw_solid_rect(cursor_rect,cursor_color);
    }else{

        //if cursor will not be visible, it takes the char and covers the area
        SOIL_RECT font_rect;
        font_rect.y = 0;
        font_rect.w = char_size.w;
        font_rect.h = char_size.h;

        if(cursor_pos >= SEL_S && cursor_pos < SEL_E)
        {
            font_rect.y = char_size.h;
        }

        SOIL_CHAR ch;
        int ind;
        SOIL_RECT paste_rect = soil_rect_from_cursor_pos(cursor_pos, char_size);
        paste_rect = rect_intersection(paste_rect, orig_rect);
        if(paste_rect.w == 0 || paste_rect.h == 0){return;}

        paste_rect.x += text_dest_offset.x;
        paste_rect.y += text_dest_offset.y;

        paste_rect.x -= orig_rect.x;
        paste_rect.y -= orig_rect.y;

        font_rect.x += (char_size.w - paste_rect.w);
        font_rect.y += (char_size.h - paste_rect.h);

        if(cursor_pos < soil_string.get_length())
        {
            if(PWD_MODE)
            {
                ind = font->get_char_index(SoilString::c_char_to_soil_char('*'));
            }else{
                ch = soil_string.at(cursor_pos);
                ind = font->get_char_index(ch);
            }
            if(ind < 0){ind = 0;}        
            font_rect.x = ind * char_size.w;
            dest_pixmap->spx_paste_pixmap(font->pixmap[font_size],font_rect,paste_rect);
            
        }else{
            dest_pixmap->spx_draw_solid_rect(paste_rect, 0xFF000000);
        }
    }
}