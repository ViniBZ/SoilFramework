#include "soiltext.h"

SoilText::SoilText()
{    

    draw_i = 0;

    PWD_MODE = false;
    MULTI_LINE = true;
    MAX_LINE_W = 0;

    SEL_S = 0;
    SEL_E = 0;

    CURSOR = 0;
    prev_drawn_CURSOR = (-1);

    set_str_text_fields_before();

    cursor_changed = false;
    text_changed = false;

    signal_receiver[0] = true;
    update_from_soil_string = true;

    print_in_cursor = false;

    ch_span = NULL;

    line_index = NULL;
    line_n = 0;
    line_index_alloc_len = 0;
    line_index_mem_bound.reset_unit(20);

    //MARKER:maybe get the ALLOC U used in SoilString infrastrings some other way
    //ch_span_mem_bound.reset_unit(soil_string.text[0].mem_bound.ALLOC_U);

    text_ch_size.w = 0;
    text_ch_size.h = 0;

    soil_string.set_update_cursor(true);

    set_tab_width(4);

    process_text();

    soil_string.set_soilobject_receiver(0, (SoilObject*)this, true);

}

SoilText::~SoilText()
{    
    
}
//---------------------------------------------- SET PWD MODE
void SoilText::set_pwd_mode(bool m)
{
    bool prev_mode = PWD_MODE;
    PWD_MODE = m;
    if(prev_mode != PWD_MODE)
    {
        this_changed();
    }
}
//---------------------------------------------- SET MULTI LINE
void SoilText::set_multi_line(bool m)
{

    bool prev_m = MULTI_LINE;
    MULTI_LINE = m;
    if(prev_m != MULTI_LINE && line_n > 1)
    {
        process_text();
    }

}
//---------------------------------------------- SET TAB WIDTH
void SoilText::set_tab_width(int w)
{

    if(w < 1){w = 1;}
    if(w > 8){w = 8;}
    TAB_W = w;
    //MARKER:make variable to tell if there is tab, to be set in process_text()

}
//---------------------------------------------- SET MAX LINE W
void SoilText::set_max_line_w(int w)
{
    int prev_max_line_w = MAX_LINE_W;
    MAX_LINE_W = w;
    if(prev_max_line_w != MAX_LINE_W)
    {
        process_text();
    }

}
//---------------------------------------------- ALLOC LINE INDEX
void SoilText::alloc_line_index(int n)
{

    //LINE INDEX, array with the line's width and first char index
    if(line_index_mem_bound.calculate(n))
    {
        
        if(line_index != NULL){delete[] line_index; line_index = NULL;}
        line_index_alloc_len = line_index_mem_bound.ALLOC_LEN;
        line_index = new LINE_INDEX_NODE[line_index_alloc_len];
    }

}
//---------------------------------------------- ALLOC CH SPAN
void SoilText::alloc_ch_span(int n)
{

    //CHAR SPAN, array with width, line and column of each character
    if(ch_span_mem_bound.calculate(n))
    {
        if(ch_span != NULL){delete[] ch_span; ch_span = NULL;}
        ch_span_alloc_len = ch_span_mem_bound.ALLOC_LEN;
        ch_span = new CHAR_SPAN[ch_span_alloc_len];
    }
    //MARKER: if there is ever a way to validate memory alloc...validate if ch_span was correctly alloc'ed

}
//---------------------------------------------- SET STRING TEXT FIELDS
void SoilText::set_str_text_fields_before()
{
    //for before the action
    int cursor_2 = CURSOR;
    if(text_selected)
    {
        if(cursor_at_sel_s)
        {
            cursor_2 = SEL_E;
        }else{
            cursor_2 = SEL_S;
        }
    }

    soil_string.set_last_rec_text_fields_before(text_selected, CURSOR, cursor_2);
}
//---------------------------------------------- PROCESS TEXT
void SoilText::process_text()
{
    if(MULTI_LINE)
    {
        process_multi_line_text();
    }else{
        process_single_line_text();
    }
    signal_receiver[0] = false;
    set_cursor_from_last_rec();
    signal_receiver[0] = true;

    text_changed = true;
    this_changed();
}

//---------------------------------------------- PROCESS SINGLE LINE TEXT
void SoilText::process_single_line_text()
{

    int text_len = soil_string.get_length();
    alloc_ch_span(text_len);

    line_n = 1;
    alloc_line_index(line_n);
    line_index[0].first_char = 0;
    line_index[0].length = text_len;

    text_ch_size.w = 0;//TEXT_CH_SIZE W to 0
    text_ch_size.h = 1;//TEXT_CH_SIZE H

    bool got_first_non_space = false;

    int char_i = 0;
    SOIL_CHAR ch;

    //MARKER: decide how to deal with line breaks in single_line mode, maybe filter them off in input

    while(char_i < text_len)
    {
        ch = soil_string.at(char_i);
        if(!got_first_non_space && !SoilString::soil_char_is_space(ch, false))
        {
            got_first_non_space = true;
            line_index[0].first_non_space = char_i;
        }
        if(ch.N == 1 && ch.B[0] == 9)
        {
            ch_span[char_i].w = TAB_W;
        }else{
            ch_span[char_i].w = 1;
        }

        ch_span[char_i].y = 0;
        ch_span[char_i].x = text_ch_size.w;

        ch_span[char_i].br = 0;

        text_ch_size.w += ch_span[char_i].w;//TEXT_CH_SIZE W


        char_i++;
    }

}

//---------------------------------------------- PROCESS MULTI LINE TEXT
void SoilText::process_multi_line_text()
{

    line_n = 1;//
    int text_len = soil_string.get_length();
    alloc_ch_span(text_len);
    text_ch_size.w = 0;
    text_ch_size.h = 0;
    int ln = 0; // temp line_n
    int line_w = 0;
    //----------------
    int char_i;
    SOIL_CHAR ch;
    int x;
    int y;
    bool NEW_LINE;
    bool AUTO_BROKE;
    int first_non_space_after_last_space = (-1);
    int last_space_x=0;
    int last_space_i;
    int INC_X;
    int DEC_X;
    int set_back_i;//for adjusting x when autobreaks

    bool got_first_non_space = false;

    int step_i = 0;
    //----------------
    //step 0: counts lines so that line_index can be alloc'ed and sets all char coords
    //step 1: sets lines first char and length
    //printf("\n\nPROCESS MULTI LINE\n");
    //line breaks, 10, stay as the last char in a line
    //printf("\n\nsoil_string.length:%d",soil_string.length);
    while(step_i < 2)
    {
        if(step_i == 1)
        {
            alloc_line_index(line_n);
            line_index[0].first_char = 0;
            line_index[0].first_non_space = (-1);
            line_index[0].length = 0;
        }

        //printf("\n********************************** STEP:%d",step_i);
        ln = 0;
        char_i = 0;
        x = 0;
        y = 0;
        NEW_LINE = true;
        AUTO_BROKE = false;

        while(char_i < text_len)
        {
            //printf("\n");
            
            
            ch = soil_string.at(char_i);
            INC_X = 1;

            //printf("\nCH n:%d",ch.B[0]);
            //printf("\nCH c:%c",ch.B[0]);
            //----------------------------------------- FIRST CHAR
            if(NEW_LINE)
            {
                if(step_i == 0)
                {
                    if(line_w > text_ch_size.w)
                    {
                        text_ch_size.w = line_w;
                    }
                    line_w = x;
                    //line_n++;
                }
                ln++;
                y = ln - 1;

                //---------------------------------- no non-space char in prev line
                if(step_i == 1 && y > 0 && !got_first_non_space)
                {
                    line_index[y-1].first_non_space = (-1);
                }

                last_space_x = 0;
                //printf("\nline_n:%d",line_n);
                //printf("\nY:%d",y);
                got_first_non_space = false;
                if(step_i == 1)
                {
                    if(AUTO_BROKE)
                    {
                        line_index[y].first_char = last_space_i + 1;
                        if(char_i > last_space_i + 1){got_first_non_space = true;}

                        AUTO_BROKE = false;
                    }else{
                        //printf("\nSET FIRST CH in LINE :%d", y);
                        line_index[y].first_char = char_i;
                    }
                    line_index[y].first_non_space = (-1);
                }
                NEW_LINE = false;
            }
            if(step_i == 1 && !got_first_non_space && !SoilString::soil_char_is_space(ch, false))
            {
                got_first_non_space = true;
                line_index[y].first_non_space = char_i;
            }
            //----------------------------------------- SPACES
            //MARKER: perhaps set the values below only once
            if(ch.N==1)
            {
                if(ch.B[0] == 32 || ch.B[0] == 9)
                {
                    last_space_i = char_i;
                    last_space_x = x;

                    //if(ch.B[0] == 9){INC_X = TAB_W;}
                }
            }

            if(step_i == 0)
            {
                //printf("\n\nchar i:%d",char_i);
                ch_span[char_i].y = y;
                ch_span[char_i].x = x;
                ch_span[char_i].w = INC_X;
                ch_span[char_i].br = 0;

                //MARKER:maybe don't set w and line_w when char is linebreak
                line_w += INC_X;
            }
            
            //----------------------------------------- LINE BREAK
            if(ch.N == 1 && ch.B[0] == 10)
            {

                NEW_LINE = true;
                if(step_i == 0){line_n++;}
                if(step_i == 1)
                {
                    line_index[y].length = (char_i - line_index[y].first_char) + 1;
                }

                ch_span[char_i].br = 1;

                INC_X = 0;
                x = 0;
            }
            //----------------------------------------- AUTO BREAK
            if(NEW_LINE == false && MAX_LINE_W > 0 && x >= MAX_LINE_W && last_space_x > 0)
            {
                NEW_LINE = true;
                if(step_i == 1)
                {
                    line_index[y].length = (last_space_i - line_index[y].first_char) + 1;
                    AUTO_BROKE = true;
                }

                if(step_i == 0)
                {
                    ch_span[last_space_i].br = 1;

                    line_n++;
                    line_w -= x - last_space_x;
                    set_back_i = last_space_i + 1;
                    
                    DEC_X = last_space_x + ch_span[last_space_i].w;
                    while(set_back_i <= char_i)
                    {
                        ch_span[set_back_i].y++;
                        ch_span[set_back_i].x -= DEC_X;
                        set_back_i++;
                    }
                }
                x -= last_space_x;
                
                INC_X = 0;
            }
            //----------------------------------------- INCREMENTS
            x += INC_X;
            
            char_i++;

        }

        if(step_i == 0 && line_w > text_ch_size.w)
        {
            text_ch_size.w = line_w;
        }


        step_i++;
    }

    line_index[y].length = char_i - line_index[y].first_char;

    //empty (last)line
    if(y < (line_n - 1))
    {
        line_index[line_n - 1].first_char = char_i;
        line_index[line_n - 1].length = 0;
        line_index[line_n - 1].first_non_space = (-1);
    }


    text_ch_size.h = line_n;
    if(line_w > text_ch_size.w)
    {
        text_ch_size.w = line_w;
    }
/*
    printf("\n\nMAX LINE W:%d",MAX_LINE_W);
    printf("\n\nLINE N:%d",line_n);
    int l = 0;
    while(l < line_n)
    {
        printf("\n\nLINE : %d",l);
        printf("\nFIRST CHAR:%d",line_index[l].first_char);
        printf("\nFIRST NON SPACE:%d",line_index[l].first_non_space);
        printf("\nLENGTH:%d",line_index[l].length);
        l++;
    }
*/
}
//---------------------------------------------- GET TEXT LENGTH
int SoilText::get_length()
{
    return soil_string.get_length();
}
//---------------------------------------------- PRINT TEXT LOG
void SoilText::print_text_log()
{

    printf("\n\n-------------------------------------------PRINT TEXT LOG");
    int char_i = 0;
    int text_len = soil_string.get_length();
    int line_i = 0;
    SOIL_CHAR sch;
    char ch;
    while(char_i < text_len)
    {
        if(line_i < line_n)
        {
            if(char_i == line_index[line_i].first_char)
            {
                printf("\n");
                printf("\n---------------------------");
                printf("\nLINE %d: FIRST CHAR I:%d",line_i,char_i);
                printf("\nLINE %d: FIRST NON SP I:%d",line_i,line_index[line_i].first_non_space);
                printf("\nLINE %d: LENGTH I:%d",line_i,line_index[line_i].length);
                printf("\n---------------------------");
                printf("\n");
                line_i++;

            }
        }
        sch = soil_string.at(char_i);
        ch = sch.B[0];
        
        printf("\n");
        printf("\nCH c:%c",ch);
        printf("\nCH n:%d",ch);
        printf("\nCHAR I :%d",char_i);
        printf("\nCHAR X :%d",ch_span[char_i].x);
        printf("\nCHAR Y :%d",ch_span[char_i].y);
        printf("\nCHAR W :%d",ch_span[char_i].w);
        printf("\nCHAR BR:%d",ch_span[char_i].br);


        char_i++;
    }
    if(line_i < line_n)
        {
                printf("\n");
                printf("\n---------------------------");
                printf("\nLINE %d: FIRST CHAR I:%d",line_i,line_index[line_i].first_char);
                printf("\nLINE %d: FIRST NON SP I:%d",line_i,line_index[line_i].first_non_space);
                printf("\nLINE %d: LENGTH I:%d",line_i,line_index[line_i].length);
                printf("\n---------------------------");
                printf("\n");
                line_i++;
            
        }

    printf("\n\n-------------------------------------------PRINT TEXT LOG END");


}
//---------------------------------------------- INTERNAL SET CURSOR
void SoilText::internal_set_cursor(int c)
{

    int prev_cursor = CURSOR;
    validate_abs_cursor(soil_string.get_length(),&c);
    CURSOR = c;

    POINT coord;
    coord_from_cursor_pos(c, &coord);
    kept_x = coord.x;

    if(CURSOR != prev_cursor)
    {
        cursor_changed = true;
    }
    set_str_text_fields_before();
}
//---------------------------------------------- INTERNAL SELECT
void SoilText::internal_select_text(int sel_a, int sel_b)
{
    //sel_b is where the cursor is supposed to be

    bool sel_changed = false;

    int text_len = soil_string.get_length();
    validate_rel_cursor(text_len,&sel_a);
    validate_rel_cursor(text_len,&sel_b);

    cursor_at_sel_s = false;

    if(sel_a > sel_b)
    {
        int aux = sel_a;
        sel_a = sel_b;
        sel_b = aux;

        cursor_at_sel_s = true;
    }
    if(SEL_S != sel_a || SEL_E != sel_b){text_sel_changed = true;}
    if(sel_a != sel_b)
    {
        text_selected = true;
    }else{
        text_selected = false;
    }
    SEL_S = sel_a;
    SEL_E = sel_b;
    set_str_text_fields_before();
}
//---------------------------------------------- GET FULL RECT
SOIL_RECT SoilText::get_full_rect(SIZE char_size)
{
    SOIL_RECT full_rect;
    full_rect.x = 0;
    full_rect.y = 0;
    full_rect.w = text_ch_size.w * char_size.w;
    full_rect.h = text_ch_size.h * char_size.h;

    return full_rect;
}
//---------------------------------------------- GET SOIL STRING PT
SoilString* SoilText::get_soil_string_pt()
{
    return &soil_string;
}
//---------------------------------------------- IS MULTILINE
bool SoilText::is_multi_line()
{
    return MULTI_LINE;
}
//---------------------------------------------- TEXT COORD FROM MOUSE COORD
//it doesn't take in consideration if there is char at coord(x,y) or not
POINT SoilText::text_coord_from_mouse(int x, int y, SIZE char_size)
{
    POINT text_coord;
    text_coord.x = (x + char_size.w / 2) / char_size.w;

    text_coord.y = y / char_size.h;

    return text_coord;
}

//---------------------------------------------- CHAR AT COORD
//if x,y lands exactly on a char returns true
//anyhow it returns the cursor position that would follow
//naturally based on mouse click on a text field
//WARNING: the mouse coord has to be corrected to the
//closest char coord prior to this function
//
//cursor_i represents the cursor in relation to the chars
bool SoilText::cursor_pos_from_coord(int x, int y, int* cursor_i)
{
    //MARKER:revise this algorithm
    int min;
    int max;
    int i;
    int text_len = soil_string.get_length();

    //MARKER: not sure if this should be here
    //if(y >= line_n){y = line_n - 1;if(print_in_cursor){printf("\nGOT FUCKING HERE!");}}

    if(x < 0){x = 0;}
    if(y < line_n)
    {
        if(y < 0)
        {
            *cursor_i = 0;
            return false;
        }

        min = line_index[y].first_char;
        max = min + line_index[y].length;

        int gap = max - min;
        int half = gap / 2;
        bool spotted = false;
        
        i = min + half;

        while(i < max)
        {
            //tried after
            if(ch_span[i].x > x)
            {
                max = i;
                spotted = false;
            }
            //tried before
            if((ch_span[i].x + ch_span[i].w) <= x)
            {
                min = i + 1;
                spotted = false;
            }
            //found
            if(x >= ch_span[i].x && x < ch_span[i].x + ch_span[i].w)
            {
                spotted = true;
            }

            if(spotted)
            {
                break;
            }else{
                gap = max - min;
                half = gap / 2;
                i = min + half;
            }

        }
        
        if(spotted)
        {
            if(ch_span[i-1].br == 1 && y == ch_span[i-1].y){i--;}
            *cursor_i = i;
            return true;
        }

        if(line_index[y].length == 0)
        {
            i = soil_string.get_length();
        }


        if(ch_span[i-1].br == 1 && y == ch_span[i-1].y){i -= 1;}
        *cursor_i = i;
        

        return false;

    }else{
        //MARKER:careful not to use this value as an index for the string
        *cursor_i = text_len;
        return false;
    }
}
//---------------------------------------------- CURSOR FROM MOUSE COORD
bool SoilText::cursor_pos_from_mouse(int x, int y, SIZE char_size, int * char_i)
{
    POINT text_coord = text_coord_from_mouse(x,y,char_size);
    bool on_char = cursor_pos_from_coord(text_coord.x, text_coord.y,char_i);
    return on_char;
}
//---------------------------------------------- COORD FROM CURSOR
//coord here wil be equivalent to the top left corner coord
//of the char
//if char_i >= text.length
//coord will be at the top right corner of the last char
//retrn false if char_i is not a valid cursor index
bool SoilText::coord_from_cursor_pos(int char_i, POINT* coord)
{
    bool result = is_valid_abs_cursor(soil_string.get_length(), char_i);

    if(!result)
    {
        return false;
    }

    if(char_i < soil_string.get_length())
    {
        coord->x = ch_span[char_i].x;
        coord->y = ch_span[char_i].y;
    }else{
        if(soil_string.get_length() == 0)
        {
            coord->x = 0;
            coord->y = 0;
        }else{
            if(ch_span[char_i - 1].br == 1)
            {
                coord->x = 0;
                coord->y = ch_span[char_i - 1].y + 1;
            }else{
                coord->x = ch_span[char_i - 1].x + 1;
                coord->y = ch_span[char_i - 1].y;
            }
        }
        
    }
    return true;
}
//---------------------------------------------- SOIL RECT FROM CURSOR POS
SOIL_RECT SoilText::soil_rect_from_cursor_pos(int char_i, SIZE char_size)
{
    POINT coord;
    SOIL_RECT rect;
    coord_from_cursor_pos(char_i, &coord);

    rect.x = coord.x * char_size.w;
    rect.y = coord.y * char_size.h;
    if(char_i < soil_string.get_length())
    {
        rect.w = ch_span[char_i].w * char_size.w;
    }else{
        rect.w = char_size.w;
    }
    rect.h = char_size.h;
    
    return rect;
}
//---------------------------------------------- GET LINE N
int SoilText::get_line_n()
{
    return line_n;
}
//---------------------------------------------- GET CURSOR POS
int SoilText::get_cursor_pos()
{
    return CURSOR;
}
//---------------------------------------------- CURSOR LINE INDENT
int SoilText::cursor_line_indent(int c)
{
    if(c < 0 || c > soil_string.get_length()){return -1;}
    int y;
    if(c == soil_string.get_length())
    {
        y = line_n-1;
    }else{
        y = ch_span[c].y;
    }
    int indent_i = line_index[y].first_non_space;

    if(indent_i == (-1))
    {
        //indent_i = line_index[y].first_char;
        indent_i = c;//for when there are just spaces
    }
    return indent_i;
}

//---------------------------------------------- CURSOR LINE START
int SoilText::cursor_line_start(int c)
{
    if(c < 0 || c > soil_string.get_length()){return -1;}
    int y;
    if(c == soil_string.get_length())
    {
        y = line_n-1;
    }else{
        y = ch_span[c].y;
    }
    
    return line_index[y].first_char;
}
//---------------------------------------------- CURSOR LINE END
int SoilText::cursor_line_end(int c)
{
    if(c < 0 || c > soil_string.get_length()){return -1;}
    if(c == soil_string.get_length()){return c;}
    int y = ch_span[c].y;
    int line_end_i = line_index[y].first_char + line_index[y].length;
    if(line_index[y].length > 0 && ch_span[line_end_i - 1].br == 1){line_end_i--;}
    
    return line_end_i;
}
//---------------------------------------------- CURSOR ABOVE
int SoilText::cursor_above(int c, int n, bool use_kept_x)//returns the pos above c
{
    return 0;
}
//---------------------------------------------- CURSOR BELOW
int SoilText::cursor_below(int c, int n, bool use_kept_x)//returns the pos below c
{
    return 0;
}
//---------------------------------------------- CURSOR ABOVE
int SoilText::cursor_above(int c, bool use_kept_x)
{
    if(c < 0){return c;}
    POINT coord;
    coord_from_cursor_pos(c,&coord);
    coord.y--;
    if(use_kept_x){coord.x = kept_x;}
    
    int above_i;
    cursor_pos_from_coord(coord.x, coord.y, &above_i);
    return above_i;
}
//---------------------------------------------- CURSOR BELOW
int SoilText::cursor_below(int c, bool use_kept_x)
{
    if(c < 0){return c;}
    POINT coord;
    coord_from_cursor_pos(c,&coord);
    coord.y++;
    if(use_kept_x){coord.x = kept_x;}
    
    int below_i;
    cursor_pos_from_coord(coord.x, coord.y, &below_i);
    return below_i;
}
//---------------------------------------------- SOILOBJECT CALLBACK
int SoilText::soilobject_callback(int arg)
{

    if(update_from_soil_string)
    {
        process_text();
    }
    
    return 0;
}

//---------------------------------------------- THIS CHANGED
int SoilText::this_changed()
{
    if(soilobject_receiver[0] != NULL && signal_receiver[0])
    {
        soilobject_receiver[0]->soilobject_callback(1);
    }
    
    return 0;
}