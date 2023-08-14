#include "soilstring.h"

// ----------------------------------------------- GET TOTAL SIZE
long int SoilString::get_total_size()
{
    long int mem = sizeof(SoilString);
    int alloc_size = text[text_i].alloc_len * sizeof(SOIL_CHAR);
    mem += alloc_size;

    return mem;
}
// ----------------------------------------------- GET LENGTH
int SoilString::get_length() const
{
    return length;
}
// ----------------------------------------------- GET BYTE Nf
int SoilString::get_byte_n() const
{
    return byte_n;
}
// ----------------------------------------------- GET LAST REC
STR_REC_NODE SoilString::get_last_rec()
{
    return last_rec;
}
// ----------------------------------------------- LOAD FROM FILE 1
int SoilString::load_from_file(FILE* fo, int n)
{
    return load_from_file(fo, n, false);
}
// ----------------------------------------------- LOAD FROM FILE 1
int SoilString::load_from_file(FILE* fo)
{
    return load_from_file(fo, -1, false);
}
// ----------------------------------------------- LOAD LINE FROM FILE
int SoilString::load_from_file_line(FILE* fo)
{
    
    return load_from_file(fo, -1, true);
}
// ----------------------------------------------- LOAD FROM FILE 2 call 1
int SoilString::load_from_file(const char* file_name)
{
    bool prev_signal_receiver_0 = signal_receiver[0];
    FILE* fo = NULL;
    fo = fopen(file_name,"rb");
    if(fo == NULL || fo == nullptr){return (-1);}

    signal_receiver[0] = false;

    int prev_length = length;
    int res = load_from_file(fo);
    signal_receiver[0] = prev_signal_receiver_0;
    fclose(fo);

    //undo, type 2

    if(prev_length == 0 && res == 0)
    {
        return res;
    }

    str_changed();
    return res;
}
// ----------------------------------------------- LOAD FROM FILE 3 call 2
int SoilString::load_from_file(const std::string& file_name)
{
    return load_from_file(file_name.c_str());
}
// ----------------------------------------------- LOAD FROM FILE 4 call 2
int SoilString::load_from_file(const SoilString& file_name)
{
    const char* fn_c_str = file_name.c_str_alloc();
    bool res = load_from_file(fn_c_str);
    delete[] fn_c_str;
    return res;
}
// ----------------------------------------------- LOAD FROM STR LINE
int SoilString::load_from_str_line(const SoilString& str, int pos)
{
    if(!validate_rel_index(str.length,&pos)){return (-1);}
    bool prev_signal_receiver_0 = signal_receiver[0];
    LINEAR_RANGE orig_range;
    int i = pos;
    int e = pos;
    SOIL_CHAR sch;
    while(i < str.length)
    {
        sch = str.at(i);
        i++;
        if(sch.N == 1 && sch.B[0] == 10){break;}
    }

    e = i;
    i = pos;
    if(e <= i){return 0;}

    if(this == &str)
    {
        section_itself(i, (e-i));
        str_changed();
        return length;
    }

    orig_range.s = i;
    orig_range.e = e;
    int n = e - i;

    if(length == 0 && n == 0){return 0;}

    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
    }

    //------------------------------------------ SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        //PROCESS POSSIBLE CURSOR CHANGE
        set_last_rec_node(20, 0, length, n);
        rec_callback(1);
    }
    //------------------------------------------ SoilTextRecord action END

    signal_receiver[0] = false;
    empty_itself();
    signal_receiver[0] = prev_signal_receiver_0;
    text[text_i].alloc(n);
    length = n;
    copy_infra_to_infra(str.text[str.text_i], orig_range, &text[text_i],0);
    update_byte_n();

    //undo, type 2

    str_changed();
    return length;

}
// ----------------------------------------------- SAVE SECTION TO FILE 1
int SoilString::prepend_from_file(FILE* fo, int file_s, int file_e)
{
    if(fo == NULL || fo == nullptr){return 0;}
    if(file_s < 0 || file_e < 0){return 0;}
    int bn = 0;
    int cn = 0;

    long int l=ftell(fo);
    utf8_file_len(fo, file_s, file_e, &cn, &bn);
    
    if(cn == 0){return 0;}
    fseek(fo,l,SEEK_SET);
    int orig_n = file_e - file_s;
    //THIS IS WHERE THE CHANGE WAS

    //printf("\nlength:%d",length);

    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        
        last_rec.cursor_1_after = last_rec.cursor_1_before + cn;
        last_rec.cursor_2_after = last_rec.cursor_2_before + cn;
        
        last_rec.selected_after = last_rec.selected_before;
    }

    //---------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        //PROCESS POSSIBLE CURSOR CHANGE
        set_last_rec_node(0, 0, 0, cn);
        rec_callback(1);
    }
    //---------------------------------------- SoilTextRecord action END
    //CHANGE
    text[alt_text_i].alloc(length + cn);
    copy_file_to_infra(fo, file_s, file_e, &text[alt_text_i], 0);
    LINEAR_RANGE copy_range;
    copy_range.s = 0;
    copy_range.e = length;
    copy_infra_to_infra(text[text_i], copy_range, &text[alt_text_i], cn);
    alternate_text_i();
    text[alt_text_i].release();
    length += cn;
    byte_n += bn;
    //CHANGE END

    str_changed();

    return cn;
}
// ----------------------------------------------- SAVE SECTION TO FILE 1
bool SoilString::save_section_to_file(FILE* fd, int pos, int n)
{
    //MARKER: validate if file is writable
    if(fd == NULL || fd == nullptr){return false;}

    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int e = pos+n;

    int c = 0;
    SOIL_CHAR sch;
    int ch;
    while(pos < e)
    {
        sch = at(pos);
        c = 0;
        while(c < sch.N)
        {
            ch = sch.B[c];
            fputc(ch,fd);
            c++;
        }
        pos++;
    }

    return true;
}
// ----------------------------------------------- SAVE SECTION TO FILE 2 call 1
bool SoilString::save_section_to_file(const char* file_name, int pos, int n)
{
  
    FILE* fd = NULL;
    fd = fopen(file_name,"wb");
    if(fd == NULL){return false;}

    bool result = save_section_to_file(fd, pos, n);
    fclose(fd);
    return result;
}
// ----------------------------------------------- SAVE SECTION TO FILE 3 call 2
bool SoilString::save_section_to_file(const std::string& file_name, int pos, int n)
{
    return save_section_to_file(file_name.c_str(), pos, n);
}
// ----------------------------------------------- SAVE SECTION TO FILE 4 call 2
bool SoilString::save_section_to_file(const SoilString& file_name, int pos, int n)
{
     //MARKER: check this limits
    if(file_name.length > 0)
    {
        char* fn = file_name.c_str_alloc();
        bool ret = save_section_to_file(fn, pos, n);

        if(fn != NULL){delete[] fn;}

        return ret;
    }else{
        return false;
    }
}
// ----------------------------------------------- SAVE FROM FILE 1
bool SoilString::save_to_file(FILE* fd)
{
    if(fd == NULL || fd == nullptr){return false;}
    return save_section_to_file(fd,0,length);
}

// ----------------------------------------------- SAVE FROM FILE 2 call 1
bool SoilString::save_to_file(const char* file_name)
{

    FILE* fd = NULL;
    fd = fopen(file_name,"wb");
    if(fd == NULL){return false;}

    bool result = save_to_file(fd);

    fclose(fd);
    return result;
}
// ----------------------------------------------- SAVE FROM FILE 3 call 2
bool SoilString::save_to_file(const std::string& file_name)
{

    return save_to_file(file_name.c_str());
}
// ----------------------------------------------- SAVE FROM FILE 4 call 2
bool SoilString::save_to_file(const SoilString& file_name)
{
    if(file_name.length > 0)
    {
        char* fn = file_name.c_str_alloc();
        bool ret = save_to_file(fn);

        if(fn != NULL){delete[] fn;}
        return ret;
    }else{
        return false;
    }
}


// ----------------------------------------------- AT
SOIL_CHAR SoilString::at(int ind)const
{
    SOIL_CHAR ch;
    ch.N = 0;
    if(!validate_rel_index(length, &ind))
    {
        return ch;
        
    }
    ch = text[text_i].str[ind];

    return ch;
}

// ----------------------------------------------- ASCII CHAR AT
unsigned char SoilString::ext_ascii_char_at(int ind)
{

    SOIL_CHAR soil_ch;
    unsigned char ch = (char)(-1);
    if(ind < 0 || ind >= length)
    {
        return ch;
    }
    soil_ch = at(ind);
    ch = SoilString::soil_char_to_ext_ascii(soil_ch);
    return ch;

}
// ----------------------------------------------- SET CHAR AT
bool SoilString::set_ch_at(int ind, SOIL_CHAR sch)
{

    if(!validate_rel_index(length,&ind))
    {
        return false;
    }
    //undo, type 3

    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
        last_rec.selected_after = last_rec.selected_before;
    }
    //---------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        set_last_rec_node(30, ind, 0, 0);
        rec_callback(1);
    }
    //---------------------------------------- SoilTextRecord action END
    SOIL_CHAR prev_sch = text[text_i].str[ind];
    text[text_i].str[ind] = sch;
    if(soil_char_are_equal(sch, prev_sch)){return false;}

    int dif = prev_sch.N - sch.N;
    byte_n -= dif;

    str_changed();

    return true;
}
// ----------------------------------------------- SET CHAR AT
bool SoilString::set_ch_at(int ind, char ch)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = ch;

    return set_ch_at(ind, sch);
}

// ----------------------------------------------- OPERATOR = 1
int SoilString::operator= (const char* str)
{
    if(str == NULL)
    {
        *this = "";
        return 0;
    }
    int new_length = length;
    int new_byte_n = byte_n;
    utf8_str_len(str,&new_length,&new_byte_n);


    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
    }

    if(new_length == 0)
    {
        //no changes
        if(length == 0){return 0;}
        //--------------------------------------- SoilTextRecord action
        if(REC_ACTIVE && !rec_action)
        {
            last_rec.cursor_1_after = 0;
            last_rec.cursor_2_after = 0;
            last_rec.selected_after = false;
            set_last_rec_node(10, 0, length, 0);
            rec_callback(1);
        }
        empty_itself();
        str_changed();
        return 0;
    }
    //undo, type 2
    //------------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
        if(length == 0)
        {
            set_last_rec_node(0, 0, 0, new_length);
        }else{
            set_last_rec_node(20, 0, length , new_length);
        }
        
        rec_callback(1);
    }
    //-------------------------------------- SoilTextRecord action END
    byte_n = new_byte_n;
    length = new_length;
    text[text_i].alloc(length);

    copy_str_to_infra(str,&text[text_i],0);

    str_changed();

    return length;
}

// ----------------------------------------------- OPERATOR = 2 call 1
int SoilString::operator= (const std::string& str)
{
    *this = str.c_str();
    return length;
}

// ----------------------------------------------- OPERATOR = 3
int SoilString::operator= (const SoilString& str)
{
    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
    }
    if(&str == this){return length;}

    if(str.length == 0)
    {
        if(length == 0){return 0;}
        //undo, type 1
        //----------------------------------- SoilTextRecord action
        if(REC_ACTIVE && !rec_action)
        {
            set_last_rec_node(10, 0, length, 0);
            rec_callback(1);
            
        }
        //----------------------------------- SoilTextRecord action END

        empty_itself();
        str_changed();
        return 0;
    }
    //undo, type 2
    
    //------------------------------------ SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        if(length == 0)
        {
            set_last_rec_node(0, 0, 0, str.length);
        }else{
            set_last_rec_node(20, 0, length , str.length);
        }
        rec_callback(1);
    }
    //------------------------------------ SoilTextRecord action END

    length = str.length;
    byte_n = str.byte_n;
    text[text_i].alloc(length);

    LINEAR_RANGE orig_range;
    orig_range.s = 0;
    orig_range.e = str.length;
    copy_infra_to_infra(str.text[str.text_i],orig_range,&text[text_i],0);

    //undo, type 2

    str_changed();
    return length;
}

// ----------------------------------------------- OPERATOR = 5
int SoilString::operator= (SOIL_CHAR sch)
{
    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
    }
    //--------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        if(length == 0)
        {
            set_last_rec_node(0, 0, 0, 1);
        }else{
            set_last_rec_node(20, 0, length, 1);
        }
        rec_callback(1);
    }
    //--------------------------------------- SoilTextRecord action END
    bool changed = true;
    if(length == 1)
    {
        SOIL_CHAR prev_sch = text[text_i].str[0];
        if(soil_char_are_equal(prev_sch, sch))
        {
            changed = false;
        }
    }

    text[text_i].alloc(1);
    length = 1;
    byte_n = 1;
    text[text_i].str[0] = sch;

    //undo, type 2

    if(changed){str_changed();}
    return length;
}
// ----------------------------------------------- OPERATOR = 6 call 5
int SoilString::operator= (unsigned char uch)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = uch;
    *this = sch;
    return length;
}
// ----------------------------------------------- OPERATOR = 7 call 5
int SoilString::operator= (char ch)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = (unsigned char)ch;
    *this = sch;
    return length;
}
// ----------------------------------------------- OPERATOR = 8 call append(num...)
int SoilString::operator= (int num)
{
    byte_n = 0;
    length = 0;
    append(num,10,-1,-1);
    return length;
}
// ----------------------------------------------- IS EMPTY
bool SoilString::is_empty()
{
    if(length == 0){return true;}
    return false;
}
// ----------------------------------------------- COMPARE
int SoilString::compare(const SoilString& str, bool match_case, bool match_accent)
{
    if(this == &str){return 0;}
    int result = 0;
    int i = 0;
    int str_len = str.length;

    SOIL_CHAR this_ch;
    SOIL_CHAR str_ch;
    while(i < length && i < str_len)
    {
        this_ch = at(i);
        str_ch = str.at(i);

        result = compare_soil_char(this_ch,str_ch, match_case, match_accent);
        
        if(result != 0){return result;}    

        i++;
    }
    if(length < str_len){return (-1);}
    if(length > str_len){return 1;}

    return 0;
}
// ----------------------------------------------- EQUALS 1
bool SoilString::is_equal(const SoilString& str, bool match_case, bool match_accent)const
{
    if(this == &str){return true;}
    int i=0;
    if(length != str.length){return false;}
    while(i < length)
    {
        if(!soil_char_are_equal(text[text_i].str[i], str.text[str.text_i].str[i], match_case, match_accent))
        {
            return false;
        }
        i++;
    }
    return true;
}
// ----------------------------------------------- EQUALS 2 call 1
bool SoilString::is_equal(const SoilString& str)
{
    return is_equal(str, true, true);
}
// ----------------------------------------------- EQUALS 5 call 1
bool SoilString::is_equal(const char* str, bool match_case, bool match_accent)
{
    SoilString tmp_str;
    tmp_str = str;
    return is_equal(tmp_str, match_case, match_accent);
}
// ----------------------------------------------- EQUALS 6 call 5
bool SoilString::is_equal(const char* str)
{
    return is_equal(str, true, true);
}
// ----------------------------------------------- EQUALS 7 call 5
bool SoilString::is_equal(const std::string& str, bool match_case, bool match_accent)
{
    return is_equal(str.c_str(), match_case, match_accent);
}
// ----------------------------------------------- EQUALS 8 call 5
bool SoilString::is_equal(const std::string& str)
{
    return is_equal(str.c_str(), true, true);
}
// ----------------------------------------------- EQUALS
bool SoilString::is_equal(SOIL_CHAR sch, bool match_case, bool match_accent)
{
    SoilString str(sch);
    return is_equal(str, match_case, match_accent);
}
// ----------------------------------------------- EQUALS
bool SoilString::is_equal(SOIL_CHAR sch)
{
    SoilString str(sch);
    return is_equal(str, true, true);
}
// ----------------------------------------------- EQUALS 9 
bool SoilString::is_equal(unsigned char uch, bool match_case, bool match_accent)
{
    if(length == 0 && uch == 0){return true;}
    if(length != 1)
    {
        return false;
    }
    SOIL_CHAR sch_1 = text[text_i].str[0];
    SOIL_CHAR sch_2;
    sch_2.N = 1;
    sch_2.B[0] = uch;
    return soil_char_are_equal(sch_1, sch_2, match_case, match_accent);
}

// ----------------------------------------------- EQUALS 10 call 9
bool SoilString::is_equal(unsigned char uch)
{
    return is_equal(uch, true, true);
}
// ----------------------------------------------- EQUALS 11 call 9
bool SoilString::is_equal(char ch, bool match_case, bool match_accent)
{
    unsigned char uch = ch;
    return is_equal(uch, match_case, match_accent);
}
// ----------------------------------------------- EQUALS 12 call 9
bool SoilString::is_equal(char ch)
{
    unsigned char uch = ch;
    return is_equal(uch, true, true);
}
// ----------------------------------------------- OPERATOR ==
bool SoilString::operator== (const char* str)
{
    return is_equal(str, true, true);
}
// ----------------------------------------------- OPERATOR ==
bool SoilString::operator== (const std::string& str)
{
    return is_equal(str, true, true);
}
// ----------------------------------------------- OPERATOR ==
bool SoilString::operator== (const SoilString& str)
{
    return is_equal(str, true, true);
}
// ----------------------------------------------- OPERATOR ==
bool SoilString::operator== (SOIL_CHAR sch)
{
    return is_equal(sch, true, true);
}
// ----------------------------------------------- OPERATOR ==
bool SoilString::operator== (unsigned char uch)
{
    return is_equal(uch, true, true);
}
// ----------------------------------------------- OPERATOR ==
bool SoilString::operator== (char ch)
{
    return is_equal(ch, true, true);
}
// ----------------------------------------------- OPERATOR !=
bool SoilString::operator!= (const char* str)
{
    return !is_equal(str, true, true);
}
// ----------------------------------------------- OPERATOR !=
bool SoilString::operator!= (const std::string& str)
{
    return !is_equal(str, true, true);
}
// ----------------------------------------------- OPERATOR !=
bool SoilString::operator!= (const SoilString& str)const
{
    return !is_equal(str, true, true);
}
// ----------------------------------------------- OPERATOR !=
bool SoilString::operator!= (SOIL_CHAR sch)
{
    return !is_equal(sch, true, true);
}
// ----------------------------------------------- OPERATOR !=
bool SoilString::operator!= (unsigned char uch)
{
    return !is_equal(uch, true, true);
}
// ----------------------------------------------- OPERATOR !=
bool SoilString::operator!= (char ch)
{
    return !is_equal(ch, true, true);
}
// ----------------------------------------------- IS VALID FOR PATH NAME
bool SoilString::is_valid_for_path()
{
    int i = 0;
    while(i < length)
    {
        if(!soil_char_is_valid_for_path(text[text_i].str[i]))
        {
            return false;
        }
        i++;
    }
    return true;
}
// ----------------------------------------------- IS VALID FOR FILE / DIR NAME
bool SoilString::is_valid_for_path_entry()
{
    int i = 0;
    while(i < length)
    {
        if(!soil_char_is_valid_for_path_entry(text[text_i].str[i]))
        {
            return false;
        }
        i++;
    }
    return true;
}
// ----------------------------------------------- C STRING
char * SoilString::c_str_alloc()const
{
    char *pt = NULL;
    pt = new char[byte_n + 1];
    if(pt == NULL){return NULL;}
    

    copy_infra_to_str(text[text_i], length, pt);

    return pt;
}
// ----------------------------------------------- SECTION C STRING
char * SoilString::section_c_str_alloc(int pos, int n)
{
    char *pt = NULL;
    int sec_len = get_section_byte_n(pos,n);
    pt = new char[byte_n];
    if(pt == NULL){return NULL;}

    validate_rel_to_abs_cursor_range(length, &pos, &n);

    LINEAR_RANGE orig_range;
    orig_range.s = pos;
    orig_range.e = pos + n;

    copy_infra_to_str(&text[text_i], orig_range,pt);

    return pt;
}

// ----------------------------------------------- REPLACE
int SoilString::replace(const char* str,int pos, int n)
{
    return replace(2, NULL, NULL, str, pos, n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(const std::string& str, int pos, int n)
{
    return replace(str.c_str(),pos,n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(const SoilString& str, int pos, int n)
{
    const SoilString* soil_str = &str;
    return replace(0, soil_str, NULL, NULL, pos, n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(SoilIncString* str, int pos, int n)
{
    return replace(1, NULL, str, NULL, pos, n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(SOIL_CHAR sch, int pos, int n)
{
    char ch[5];
    ch[0] = sch.B[0];
    ch[1] = sch.B[1];
    ch[2] = sch.B[2];
    ch[3] = sch.B[3];
    ch[4] = 0;
    if(sch.N < 0 || sch.N > 4)
    {
        ch[0] = 0;
    }else{
        ch[sch.N] = 0;
    }
    const char * ch_pt = ch;
    return replace(2,NULL,NULL,ch_pt,pos,n);
    
}
// ----------------------------------------------- REPLACE
int SoilString::replace(unsigned char uch, int pos, int n)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = uch;
    return replace(sch,pos,n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(char ch, int pos, int n)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = (unsigned char)ch;
    return replace(sch,pos,n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(signed long long num, int base, int min_dig_n, int max_dig_n, int pos, int n)
{
    //functions are:
    //
    //CONSTANT DIGIT NUMBER (dig_n)
    //the amount of digits that should be used, to be completed with zeroes
    //in case the number is smaller than the argument number of digits
    //if set below 1, it means the size will vary according to the value

    //if(!validate_rel_cursor(length, &pos)){return;}
    ///////////////// SETTING THE LIMITS

    if(base > 16){base = 16;}
    if(base < 2){base = 2;}

    const int len_lim = 32;

    if(min_dig_n > 32){min_dig_n = 32;}
    if(max_dig_n > 32){max_dig_n = 32;}

    if(max_dig_n > 0 && max_dig_n < min_dig_n){max_dig_n = min_dig_n;}
    ///////////////// SETTING VARIABLES

    //////////////////////////////////////// for negative values (neg)
    bool neg = false;//if number is negative
    bool only_zeroes = true;
    int first_non_zero = len_lim;
    int sign_inc = 0;
    ///////////////////////////////////////

    if(num < 0)
    {
        num *= (-1);
        neg = true;
    }

    char num_str[len_lim] = {'0','0','0','0','0','0','0','0','0','0','0','0',
                             '0','0','0','0','0','0','0','0','0','0','0','0',
                            '0','0','0','0','0','0','0','0'};

    char digits[16] = {'0','1','2','3','4','5','6','7',
                       '8','9','A','B','C','D','E','F'};

    ///////////////// PARSING THE DIGITS

    // n will be where the number ends to the left(most valuable digit)
    signed int dn = len_lim - 1;
    int i = 0;//neg

    while(dn >= 0)
    {
        num_str[dn] = digits[num % base];
        num /= base;

        //neg
        if(neg && only_zeroes && num_str[dn] != '0')
        {
            only_zeroes = false;
            first_non_zero = i;
        }
        i++;
        if(num == 0){break;}
        dn--;


    }

    ///////////////// LIMITS

    if(min_dig_n > 0 && i < min_dig_n)
    {
        dn = len_lim - min_dig_n;
        i = min_dig_n;
    }
    if(max_dig_n > 0 && i > max_dig_n)
    {
        dn = len_lim - max_dig_n;
        i = max_dig_n;
    }


    ///////////////// MAKING THE STRING

    //neg
    if(neg && !only_zeroes && first_non_zero < i)
    {
        sign_inc = 1;
    }


    const int res_len = (len_lim - dn) + sign_inc + 1;
    char res[res_len];

    int r = sign_inc;

    if(sign_inc == 1){res[0] = '-';}//neg

    while(dn < len_lim)
    {
        res[r] = num_str[dn];
        dn++;
        r++;
    }
    res[r] = 0;
    
    return replace(2,NULL,NULL,res,pos,n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(int num, int base, int min_dig_n, int max_dig_n, int pos, int n)
{
    signed long long NUM = (long long) num;
    return replace(NUM, base,min_dig_n,max_dig_n, pos, n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(signed long long num, int base, int pos, int n)
{
    return replace(num, base,-1,-1, pos, n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(int num, int base, int pos, int n)
{
    signed long long NUM = (long long) num;
    return replace(NUM, base,-1,-1, pos, n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(signed long long num, int pos, int n)
{
    return replace(num,10,-1,-1,pos,n);
}
// ----------------------------------------------- REPLACE
int SoilString::replace(int num, int pos, int n)
{
    signed long long NUM = (long long) num;
    return replace(NUM,10,-1,-1,pos,n);
}
// ----------------------------------------------- INSERT 1
int SoilString::insert(const char* str, int pos)
{
    return replace(str, pos, 0);
}
// ----------------------------------------------- INSERT 2 call 1
int SoilString::insert(const std::string& str, int pos)
{
    //insert(str.c_str(),pos);
    return replace(str, pos, 0);
}
// ----------------------------------------------- INSERT 3
int SoilString::insert(const SoilString& str, int pos)
{
    return replace(str, pos, 0);
}
// ----------------------------------------------- INSERT 3
int SoilString::insert(SoilIncString* str, int pos)
{
    return replace(str, pos, 0);
}
// ----------------------------------------------- INSERT 5
int SoilString::insert(SOIL_CHAR ch, int pos)
{    
    if(is_valid_rel_cursor(length, pos))
    {
        return insert_ch_in_range(ch, pos);
    }else{
        SoilString aux_str(ch);
        return insert(aux_str, pos);
    }
    return 0;
}
// ----------------------------------------------- INSERT 6 call 5
int SoilString::insert(unsigned char uch, int pos)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = uch;
    sch.B[1] = 0;
    if(is_valid_rel_cursor(length, pos))
    {
        return insert(sch,pos);
    }else{
        char str;
        //return 
        return 0;
    } 
}
// ----------------------------------------------- INSERT 7 call 5
int SoilString::insert(char ch, int pos)
{

    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = (unsigned char)ch;
    sch.B[1] = 0;
    return insert(sch,pos);
}
// ----------------------------------------------- INSERT 8

int SoilString::insert(signed long long num, int base, int min_dig_n, int max_dig_n, int pos)
{
    return replace(num, base, min_dig_n, max_dig_n, pos, 0);
}

// ----------------------------------------------- INSERT 9 call 8
int SoilString::insert(int num, int base, int min_dig_n, int max_dig_n, int pos)
{
    signed long long NUM = (long long) num;
    return insert(NUM,base,min_dig_n,max_dig_n,pos);
}
// ----------------------------------------------- INSERT 10 call 8
int SoilString::insert(signed long long num, int base, int pos)
{
    return insert(num, base, -1, -1, pos);
}
// ----------------------------------------------- INSERT 11 call 8
int SoilString::insert(int num, int base, int pos)
{
    signed long long NUM = (long long) num;
    return insert(NUM, base, -1, -1, pos);
}
// ----------------------------------------------- INSERT 12 call 8
int SoilString::insert(signed long long num, int pos)
{
    return insert(num, 10, -1, -1, pos);
}
// ----------------------------------------------- INSERT 13 call 8
int SoilString::insert(int num, int pos)
{
    signed long long NUM = (long long) num;
    return insert(NUM, 10, -1, -1, pos);
}
// ----------------------------------------------- INSERT SPACE
bool SoilString::insert_space(int pos, int space_n)
{
    if(!validate_rel_cursor(length, &pos)){return false;}

    SoilString space_str("");

    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = 32;
    int i = 0;
    while(i < space_n)
    {
        space_str.insert(sch,0);
        i++;
    }
    insert(space_str, pos);
    return true;
}
// ----------------------------------------------- INSERT 5
int SoilString::insert_ch_in_range(SOIL_CHAR ch, int pos)
{

    int prev_length = length;

    if(!validate_rel_cursor(length, &pos)){return 0;}
    int new_length = length + 1;
    if(!text[text_i].would_realloc(length + 1))
    {
        //insert_soil_char() changed length and byte_n
        insert_soil_char(ch, pos);
    }else{

        //prepare_str() alternates the text_i and updates length, but not byte_n
        prepare_text_str(new_length,pos,true);
        byte_n += ch.N;
        text[text_i].str[pos] = ch;
    }

    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
        last_rec.selected_after = false;
        //PROCESS POSSIBLE CURSOR CHANGE
        if(last_rec.cursor_1_after >= pos)
        {
            last_rec.cursor_1_after += 1;
        }
        //PROCESS POSSIBLE CURSOR CHANGE        
        if(last_rec.cursor_2_after >= pos)
        {
            last_rec.cursor_2_after += 1;
        }
    }


    if(REC_ACTIVE && !rec_action)
    {
        int rec_type = 2;
        if(SoilString::soil_char_is_space(ch, false))
        {
            rec_type = 1;
        }
        set_last_rec_node(rec_type, pos, 0, 1);
        rec_callback(1);
    }
    str_changed();
    return 1;
}
// ----------------------------------------------- REPLACE WITH SPACE
bool SoilString::replace_with_space(int pos, int n, int space_n)
{
    if(!validate_rel_cursor(length, &pos)){return false;}

    SoilString space_str("");

    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = 32;
    int i = 0;
    while(i < space_n)
    {
        space_str.insert(sch,0);
        i++;
    }
    //insert(space_str, pos);
    replace(space_str, pos, n);
    return true;
}
// ----------------------------------------------- DUPLICATE LINE
//returns amount of char added
int SoilString::duplicate_line(int pos)
{
    if(!validate_rel_cursor(length, &pos)){return 0;}
    if(length == 0){return 0;}
    SOIL_CHAR br_sch;
    br_sch.N = 1;
    br_sch.B[0] = 10;
    int s = next_char_pos(br_sch, pos, -1);
    s++;
    int e = next_char_pos(br_sch, pos, 1);
    if(e == (-1))
    {
        e = length;
    }

    int n = e - s;

    int new_length = length + n + 1;
    text[alt_text_i].alloc(new_length);
    int dest_pos = 0;

    LINEAR_RANGE orig_range;
    orig_range.s = 0;
    orig_range.e = e;
    copy_infra_to_infra(text[text_i], orig_range, &text[alt_text_i], 0);

    if(update_cursor)
    {
        //PROCESS POSSIBLE CURSOR CHANGE

        last_rec.cursor_1_after = last_rec.cursor_1_before;
        if(last_rec.cursor_1_after >= pos)
        {
            last_rec.cursor_1_after += n+1;
        }
    
        last_rec.cursor_2_after = last_rec.cursor_2_before;
        if(last_rec.cursor_2_after >= pos)
        {
            last_rec.cursor_2_after += n+1;
        }
        last_rec.selected_after = false;
    
    }

    //undo, type 2
    //------------------------------------------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        set_last_rec_node(0, e, 0 , n+1);
        rec_callback(1);
    }
    //------------------------------------------------------------------------- SoilTextRecord action END

    orig_range.s = s;
    dest_pos = orig_range.e;
    //PLACE LINE BREAK
    text[alt_text_i].str[dest_pos] = br_sch;
    dest_pos++;

    copy_infra_to_infra(text[text_i], orig_range, &text[alt_text_i], dest_pos);

    orig_range.s = orig_range.e;
    orig_range.e = length;
    dest_pos += n;

    copy_infra_to_infra(text[text_i], orig_range, &text[alt_text_i], dest_pos);

    length = new_length;
    alternate_text_i();
    text[alt_text_i].release();
    update_byte_n();

    //undo, type 0
    str_changed();

    return n+1;
}
// ----------------------------------------------- REMOVE
int SoilString::remove(int pos, int n)
{
    int dir = n;
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int new_length = length - n;

    if(new_length == length){return 0;}
    if(n == 0){return 0;}

    //undo, type 1
    if(update_cursor)
    {
        //PROCESS POSSIBLE CURSOR CHANGE

        last_rec.cursor_1_after = last_rec.cursor_1_before;
        if(last_rec.cursor_1_after > pos)
        {
            last_rec.cursor_1_after -= n;
            if(last_rec.cursor_1_after < pos)
            {
                last_rec.cursor_1_after = pos;
            }
        }

        last_rec.cursor_2_after = last_rec.cursor_2_before;
        if(last_rec.cursor_2_after > pos)
        {
            last_rec.cursor_2_after -= n;
            if(last_rec.cursor_2_after < pos)
            {
                last_rec.cursor_2_after = pos;
            }
        }
        last_rec.selected_after = false;
    }
    //------------------------------------------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        SOIL_CHAR ch = at(pos);
        bool rem_ch_space;
        bool rem_ch_right = false;
        int rec_type = 10;
        if(dir == 1 || dir == (-1))
        {
            if(dir == 1)
            {
                rem_ch_right = true;
                
                rec_type = 13;
            }else{
                
                rec_type = 11;
            }
            rem_ch_space = SoilString::soil_char_is_space(ch, false);
            if(!rem_ch_space){rec_type++;}
        }

        bool updated = false;
        set_last_rec_node(rec_type, pos, n, 0);
        rec_callback(1);

    }
    //------------------------------------------------------------------------- SoilTextRecord action END

    prepare_text_str(new_length,pos,true);
    update_byte_n();

    str_changed();
    return n;
}
// ----------------------------------------------- APPEND
int SoilString::append(const char* str)
{

    return insert(str,-1);

}
// ----------------------------------------------- APPEND
int SoilString::append(const std::string& str)
{
    return insert(str,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(const SoilString& str)
{
    return insert(str,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(SoilIncString* str)
{
    return insert(str,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(unsigned char uch)
{
    return insert(uch,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(char ch)
{
    return insert(ch,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(SOIL_CHAR ch)
{
    return insert(ch,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(signed long long num, int base, int min_dig_n, int max_dig_n)
{
    return insert(num,base,min_dig_n,max_dig_n,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(int num, int base, int min_dig_n, int max_dig_n)
{
    return insert(num,base,min_dig_n,max_dig_n,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(signed long long num, int base)
{
    return insert(num,base,-1,-1,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(int num, int base)
{
    return insert(num,base,-1,-1,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(signed long long num)
{
    return insert(num,10,-1,-1,-1);
}
// ----------------------------------------------- APPEND
int SoilString::append(int num)
{
    return insert(num,10,-1,-1,-1);
}
// ----------------------------------------------- PREPEND

int SoilString::prepend(const char* str)
{
    return insert(str,0);
}
// ----------------------------------------------- PREPEND

int SoilString::prepend(const std::string& str)
{
    return insert(str,0);
}
// ----------------------------------------------- PREPEND
int SoilString::prepend(const SoilString& str)
{
    return insert(str,0);
}
// ----------------------------------------------- PREPEND
int SoilString::prepend(SoilIncString* str)
{
    return insert(str,0);
}
// ----------------------------------------------- PREPEND
int SoilString::prepend(SOIL_CHAR ch)
{
    return insert(ch,0);
}

// ----------------------------------------------- PREPEND
int SoilString::prepend(unsigned char uch)
{
    return insert(uch,0);
}
// ----------------------------------------------- PREPEND
int SoilString::prepend(char ch)
{
    return insert(ch,0);
}

// ----------------------------------------------- APPEND
int SoilString::prepend(signed long long num, int base, int min_dig_n, int max_dig_n)
{
    return insert(num,base,min_dig_n,max_dig_n,0);
}
// ----------------------------------------------- APPEND
int SoilString::prepend(int num, int base, int min_dig_n, int max_dig_n)
{
    return insert(num,base,min_dig_n,max_dig_n,0);
}
// ----------------------------------------------- APPEND
int SoilString::prepend(signed long long num, int base)
{
    return insert(num,base,-1,-1,0);
}
// ----------------------------------------------- APPEND
int SoilString::prepend(int num, int base)
{
    return insert(num,base,-1,-1,0);
}
// ----------------------------------------------- APPEND
int SoilString::prepend(signed long long num)
{

    return insert(num,10,-1,-1,0);

}
// ----------------------------------------------- APPEND
int SoilString::prepend(int num)
{
    return insert(num,10,-1,-1,0);
}

// ----------------------------------------------- NEXT CHAR POS
int SoilString::next_char_pos(SOIL_CHAR ch, int start_pos, int dir)
{
    //IMPORTANT:if this function returns other value that is not (-1)
    //when not finding ch, indent_section() will need to be fixed

    //dir  1: to the right
    //dir -1: to the left
    validate_rel_cursor(length, &start_pos);

    int i = start_pos;
    if(dir > 0)
    {
        while(i < length)
        {
            if(soil_char_are_equal(text[text_i].str[i], ch, true, true))
            {

                return i;
            }
            i++;
        }
    }else{
        i--;
        while(i >= 0)
        {
            if(soil_char_are_equal(text[text_i].str[i], ch, true, true))
            {

                return i;
            }
            i--;
        }
    }

    return -1;

}

// ----------------------------------------------- NEXT CHAR POS
int SoilString::next_char_pos(unsigned char uch, int start_pos, int dir)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = uch;
    return next_char_pos(sch, start_pos, dir);
}

// ----------------------------------------------- NEXT CHAR POS
int SoilString::next_char_pos(char ch, int start_pos, int dir)
{

    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = (unsigned char)ch;
    return next_char_pos(sch, start_pos, dir);

}

// ----------------------------------------------- WORD BOUNDS
void SoilString::word_bounds(int i, int* s, int* e, bool include_trailing_sp)
{
    if(!validate_rel_cursor(length, &i)){return;}
    int count_i = i;
    int ind_s = 0;
    int ind_e = length;

    while(count_i < length)
    {
        if(soil_char_is_space(text[text_i].str[count_i],true))
        {
            ind_e = count_i;
            if(include_trailing_sp){ind_e++;}
            break;
        }
        count_i++;
    }
    *e = ind_e;
    count_i = i - 1;
    while(count_i >= 0)
    {
        if(soil_char_is_space(text[text_i].str[count_i],true))
        {
            ind_s = count_i + 1;
            break;
        }
        count_i--;
    }
    *s = ind_s;
}
// ----------------------------------------------- WORD BOUNDS
void SoilString::word_bounds(int i, int* s, int* e)
{
    word_bounds(i, s, e, false);
}
// ----------------------------------------------- NEXT WORD BOUND
int SoilString::next_word_bound(int i, int dir)
{
    validate_rel_cursor(length, &i);

    int c = i;
    bool first_is_space = false;
    bool cur_is_space = false;
    if(dir > 0)
    {
        if(c == length){return c;}
        first_is_space = SoilString::soil_char_is_space(at(c), true);

        c++;
        while(c < length)
        {
            cur_is_space = SoilString::soil_char_is_space(at(c), true);
            if(first_is_space != cur_is_space){return c;}

            c++;
        }
        
        if(first_is_space){return i;}else{return length;}
    }else{
        if(c == 0){return c;}
        c--;
        first_is_space = SoilString::soil_char_is_space(at(c), true);

        c--;
        while(c >= 0)
        {
            cur_is_space = SoilString::soil_char_is_space(at(c), true);
            if(first_is_space != cur_is_space){return c+1;}

            c--;
        }
        if(first_is_space){return i;}else{return 0;}
    }
    return i;//this line woul never get executed
}
// ----------------------------------------------- SIMPLIFY
//do the same as the trim function and replaces any sequence of
//space characters by a single space(ascii 32)
void SoilString::simplify()
{
    if(REC_ACTIVE)
    {
        multi_trans_inc_str = new SoilIncString;
        if(multi_trans_inc_str == NULL)
        {
            return;
        }
    }

    int rem_sp_n = simplify(false, multi_trans_inc_str);

    if(REC_ACTIVE)
    {
        if(multi_trans_inc_str != NULL)
        {
            delete multi_trans_inc_str;
        }
    }
    if(rem_sp_n > 0)
    {
        str_changed();
    }
}
// ----------------------------------------------- TRIM
//eliminates white spaces from the beginning and end of the string
void SoilString::trim()
{
    if(length == 0){return;}

    int new_length;
    int first_char_i = 0;
    int last_char_i = 0;
    bool found_non_space = false;

    SOIL_CHAR sch;
    while(first_char_i < length)
    {
        sch = at(first_char_i);
        if(!soil_char_is_space(sch,true))
        {
            found_non_space = true;
            break;
        }
        first_char_i++;
    }

    if(!found_non_space)
    {
        if(update_cursor)
        {
             //PROCESS POSSIBLE CURSOR CHANGE
            last_rec.cursor_1_after = last_rec.cursor_1_before;
            last_rec.cursor_2_after = last_rec.cursor_2_before;
            last_rec.selected_after = false;
        }
        //------------------------------------------------------------------------- SoilTextRecord action
        if(REC_ACTIVE && !rec_action)
        {
            set_last_rec_node(10, 0, length , 0);
            rec_callback(1);
        }
        //------------------------------------------------------------------------- SoilTextRecord action END

        empty_itself();
        str_changed();
        return;
    }

    last_char_i = length - 1;

    while(last_char_i >= first_char_i)
    {
        sch = at(last_char_i);
        if(!soil_char_is_space(sch,true))
        {
            break;
        }            
        last_char_i--;
    }

    //section_itself already deals with SoilTextRecord
    section_itself(first_char_i,((last_char_i + 1) - first_char_i));
    str_changed();   
}

// ----------------------------------------------- SECTION 1
SoilString SoilString::section(int pos, int n) const
{
    SoilString dest_str;
    validate_rel_to_abs_cursor_range(length, &pos, &n);

    dest_str = "";
    dest_str.text[dest_str.text_i].alloc(n);

    LINEAR_RANGE orig_range;
    orig_range.s = pos;
    orig_range.e = pos + n;

    copy_infra_to_infra(text[text_i],orig_range,&dest_str.text[dest_str.text_i],0);

    dest_str.length = n;
    dest_str.update_byte_n();    

    return dest_str;

}
// ----------------------------------------------- SECTION 2 call 1
//This function is not finished
//start and end tells how many sections are wanted
//start and end are inclusive to the sections
SoilString SoilString::section(const SoilString& sep, int start, int end, bool match_sep_case, bool match_sep_accent, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const
{


    int sep_n = 0;
    int pos = 0;
    int res = 0;
    while(res != (-1))
    {
        res = search_str(sep,pos,match_sep_case,match_sep_accent);
        if(res >= 0){sep_n++;}
        pos = res + sep.length;
    }
    validate_rel_cursor(sep_n, &start);
    validate_rel_cursor(sep_n, &end);

    if(start > end)
    {
        int aux = start;
        start = end;
        end = aux;
    }

    int s = 0;
    int start_pos = 0;
    int end_pos = length;
    pos = 0;
    res = 0;
    while(res != (-1))
    {
        if(s == start){start_pos = pos;}
        if(s == (end+1)){end_pos = pos - sep.length;}

        res = search_str(sep,pos,match_sep_case,match_sep_accent);
        pos = res + sep.length;
        
        s++;
    }

    if(start_pos >= sep.length && INCLUDE_START_SEP){start_pos -= sep.length;}
    if(end_pos <= (length - sep.length) && INCLUDE_END_SEP){end_pos += sep.length;}

    return section(start_pos,(end_pos - start_pos));

}
// ----------------------------------------------- SECTION 3 call 2
SoilString SoilString::section(const SoilString& sep, int start, int end, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const
{
    return section(sep, start, end, true, true, INCLUDE_START_SEP, INCLUDE_END_SEP);
}
// ----------------------------------------------- SECTION 4 call 2
SoilString SoilString::section(const SoilString& sep, int start, int end) const
{
    return section(sep, start, end, true, true, false, false);
}
// ----------------------------------------------- SECTION 5 call 2
SoilString SoilString::section(const char* sep, int start, int end, bool match_sep_case, bool match_sep_accent, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const
{
    SoilString tmp_str(sep);
    return section(tmp_str, start, end, match_sep_case, match_sep_accent, INCLUDE_START_SEP, INCLUDE_END_SEP);
}
// ----------------------------------------------- SECTION 6 call 2
SoilString SoilString::section(const char* sep, int start, int end, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const
{
    SoilString tmp_str(sep);
    return section(tmp_str, start, end, true, true, INCLUDE_START_SEP, INCLUDE_END_SEP);
}
// ----------------------------------------------- SECTION 7 call 2
SoilString SoilString::section(const char* sep, int start, int end) const
{
    SoilString tmp_str(sep);
    return section(tmp_str, start, end, true, true, false, false);
}

// ----------------------------------------------- SEARCH STR
// return the point where the first match was found
// or -1 if not found
int SoilString::search_str(const SoilString& str, int pos, int n, bool match_case, bool match_accent) const
{
    //MARKER:check if those should really be the return values for empty strings
    if(length == 0){return -2;}
    if(str.length == 0){return -3;}
    validate_rel_cursor_range(length, &pos, &n);
    if(this == &str){return 0;}

    int i = pos;
    int inc = 1;
    int end = i + n;
    
    int sub_start = 0;
    int sub_i = sub_start;
    int sub_c = 0;
    int sub_len = str.length;
    int sub_end = sub_len;

    int abs_n = n;
    if(abs_n < 0){abs_n *= (-1);}

    if(abs_n < sub_len){return -4;}

    if(n < 0)
    {
        end -= 1;
        i = pos-1;
        inc = (-1);
        sub_start = sub_len - 1;
        sub_end = (-1);
    }

    if(end > length){end = length;}
    if(end < (-1)){end = (-1);}

    sub_i = sub_start;

    bool found = false;

    int found_pos = 0;

    SOIL_CHAR ch = at(i);
    SOIL_CHAR sub_ch = str.at(sub_start);
    while(i != end)
    {
        ch = at(i);
        if(soil_char_are_equal(ch, sub_ch, match_case, match_accent))
        {
            //printf("\n%d match char (%c,%c)",i,ch.B[0],sub_ch.B[0]);
            if(sub_i == 0){found_pos = i;}
            sub_i += inc;
            sub_c++;
            sub_ch = str.at(sub_i);

            if(sub_i == sub_end)
            {
                return found_pos;
            }
        }else{
            //printf("\n%d NOT match (%c,%c)",i,ch.B[0],sub_ch.B[0]);
            if(n > 0){i -= sub_c;}else{i += sub_c;}
            sub_i = sub_start;
            sub_c = 0;
            sub_ch = str.at(sub_i);
        }
        i += inc;
    }
    return -1;
}
// ----------------------------------------------- SEARCH STR
// return the point where the first match was found
// or -1 if not found
//MARKER: revise parameter order for functions
int SoilString::search_str(const SoilString& str, int pos, bool match_case, bool match_accent) const
{
    validate_rel_cursor(length, &pos);
    int n = length - pos;    
    return search_str(str,pos,n,match_case,match_accent);
}
// ----------------------------------------------- SEARCH STR
// return the point where the first match was found
// or -1 if not found
int SoilString::search_str(const SoilString& str, int pos) const
{
    return search_str(str, pos, true, true);
}
// ----------------------------------------------- SEARCH STR
int SoilString::search_str(const char* str, int pos, int n, bool match_case, bool match_accent) const//return sub_str po
{
    SoilString soil_str(str);
    return search_str(soil_str, pos, n, match_case, match_accent);
}
// ----------------------------------------------- SEARCH STR
int SoilString::search_str(const char* str, int pos, bool match_case, bool match_accent) const//return sub_str po
{
    SoilString soil_str(str);
    return search_str(soil_str, pos, match_case, match_accent);
}
// ----------------------------------------------- SEARCH STR
int SoilString::search_str(const char* str, int pos) const//return sub_str po
{
    SoilString soil_str(str);
    return search_str(soil_str, pos, true, true);
}
// ----------------------------------------------- SEARCH REPLACE STR
//return number of occurrences
int SoilString::search_replace_str(const SoilString& str, const SoilString& rep, int pos, int n, bool match_case, bool match_accent)
{
    //MARKER:check if these are really the best way to handle 0 values in such variables
    if(str.length == 0 || str.length > length){return 0;}
    if(length == 0){return 0;}
    if(this == &str)
    {
        if(this == &rep){return 1;}
        replace(rep, 0, length);
        return 1;
    }

    const SoilString* rep_pt = &rep;
    bool copy_itself = false;
    if(this == &rep)
    {
        copy_itself = true;
        SoilString* pt = new SoilString(*this);
        rep_pt = pt;
    }
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int pos_added = 0;
    int cur_n = n;

    // --------------------- variables for SoilTextRecord
    int res_start = (-1);
    int res_end = (-1);

    int new_cursor_1 = 0;
    int new_cursor_2 = 0;
    int after_str_n = 0;
    if(update_cursor)
    {
        new_cursor_1 = last_rec.cursor_1_before;
        new_cursor_2 = last_rec.cursor_2_before;

        if(REC_ACTIVE)
        {
            multi_trans_inc_str = new SoilIncString;
            if(multi_trans_inc_str == NULL)
            {
                if(copy_itself){delete rep_pt;}
                return -2;
            }
            //placeholder for total_rec_n
            multi_trans_inc_str->append(c_char_to_soil_char('a'));
            multi_trans_inc_str->append(c_char_to_soil_char('a'));
        }
    }
    bool found_some = false;

    int str_len = str.length;
    int rep_len = rep_pt->length;
    int dif = rep_len - str_len;
    //printf("\ndif:%d",dif);
    int dif_sum = 0;
    int str_pos = 0;
    int res = pos;
    int found_n = 0;
    SOIL_CHAR int_sch_1, int_sch_2;

    while(res >= 0)
    {
        cur_n = n - pos_added;
        res = search_str(str, res, cur_n, match_case, match_accent);
        if(res >= 0)
        {

            if(res_start < 0){res_start = res;}
            res_end = res;

            //----------------------- if REC_ACTIVE
            if(REC_ACTIVE)
            {
                str_pos = res + dif_sum;
                //printf("\nstr_pos:%d",str_pos);
                //if match_case ant match_accent
                //copy the coords to multi_trans_inc_str
                if(match_case && match_accent)
                {
                    if(!found_some)
                    {
                        multi_trans_inc_str->append(text[text_i], res, str_len);
                        found_some = true;
                    }
                    code_int_in_2_soil_char(str_pos, &int_sch_1, &int_sch_2);
                    multi_trans_inc_str->append(int_sch_1);
                    multi_trans_inc_str->append(int_sch_2);                    
                }else{
                //else
                    //copy the coords and section to multi_trans_inc_str
                    multi_trans_inc_str->append(text[text_i], res, str_len);
                    code_int_in_2_soil_char(str_pos, &int_sch_1, &int_sch_2);
                    multi_trans_inc_str->append(int_sch_1);
                    multi_trans_inc_str->append(int_sch_2);   
                }
                dif_sum += dif;
            }
            //POSSIBLE CURSOR CHANGE
            if(update_cursor)
            {
                //NEW CURSOR 1
                if(last_rec.cursor_1_before > res)
                {
                    if(last_rec.cursor_1_before >= (res + str_len))
                    {
                        last_rec.cursor_1_after += dif;
                    }else{
                        int res_dif = last_rec.cursor_1_before - res;
                        if(res_dif > rep_len)
                        {
                            last_rec.cursor_1_after -= (res_dif - rep_len);
                        }
                        
                    }
                }
                //NEW CURSOR 2
                if(last_rec.cursor_2_before > res)
                {
                    if(last_rec.cursor_2_before >= (res + str_len))
                    {
                        last_rec.cursor_2_after += dif;
                    }else{
                        int res_dif = last_rec.cursor_2_before - res;
                        if(res_dif > rep_len)
                        {
                            last_rec.cursor_2_after -= (res_dif - rep_len);
                        }
                        
                    }
                }
            }

            found_n++;
            res += str_len;
            pos_added = res - pos;
        }
    }
    if(found_n == 0)
    {
        if(copy_itself){delete rep_pt;}
        if(multi_trans_inc_str != NULL)
        {
            delete multi_trans_inc_str;
            multi_trans_inc_str = NULL;
        }
        return 0;
    }
    

    int total_dif = found_n * dif;
    int new_length = length + (found_n*dif);

    if(update_cursor)
    {
        last_rec.cursor_1_after = new_cursor_1;
        last_rec.cursor_2_after = new_cursor_2;
        last_rec.selected_after = false;
    }
        
    //------------------------------------ UNDO / REDO RECORDS
    if(REC_ACTIVE && !rec_action)
    {
        //SOIL_CHAR total_rec_n_sch = code_int_in_soil_char(found_n);
        code_int_in_2_soil_char(found_n, &int_sch_1, &int_sch_2);
        multi_trans_inc_str->set_ch_at(0, int_sch_1);
        multi_trans_inc_str->set_ch_at(1, int_sch_2);
        if(match_case && match_accent)
        {
            set_last_rec_node(70, 0, str_len, rep_len);
        }else{
            set_last_rec_node(71, 0, str_len, rep_len);
        }
        
        rec_callback(1);        
    }
    //------------------------------------


    text[alt_text_i].alloc(new_length);

    res = pos;
    pos_added = 0;
    LINEAR_RANGE orig_range;
    orig_range.s = 0;
    orig_range.e = 0;
    int dest_pos = 0;
    while(res != (-1))
    {
        cur_n = n - pos_added;
        res = search_str(str, res, cur_n, match_case, match_accent);
        if(res != (-1))
        {
            orig_range.e = res;
            copy_infra_to_infra(text[text_i],orig_range,&text[alt_text_i],dest_pos);
            dest_pos += (orig_range.e - orig_range.s);
            orig_range.s = 0;
            orig_range.e = rep_len;
            copy_infra_to_infra(rep_pt->text[rep_pt->text_i],orig_range,&text[alt_text_i],dest_pos);
            orig_range.s = res + str_len;
            dest_pos += rep_len;

            res += str_len;

            pos_added = res - pos;
        }
    }

    orig_range.e = length;
    copy_infra_to_infra(text[text_i],orig_range,&text[alt_text_i],dest_pos);

    length = new_length;
    alternate_text_i();
    text[alt_text_i].release();
    update_byte_n();

    if(copy_itself){delete rep_pt;}
    if(multi_trans_inc_str != NULL)
    {
        delete multi_trans_inc_str;
        multi_trans_inc_str = NULL;
    }
    //undo, type 7

    if(signal_receiver[0] && found_n > 0 && str != rep){str_changed();}

    return found_n;
}
// ----------------------------------------------- SEARCH REPLACE STR
//return number of occurrences
int SoilString::search_replace_str(const SoilString& str, const SoilString& rep, int pos, bool match_case, bool match_accent)
{
    int n = length;
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    return search_replace_str(str, rep, pos, n, match_case, match_accent);
}
// ----------------------------------------------- SEARCH REPLACE STRING
int SoilString::search_replace_str(const char* str, const char* rep, int pos, bool match_case, bool match_accent)
{
    SoilString soil_str(str);
    SoilString soil_rep(rep);
    return search_replace_str(soil_str, soil_rep, pos, match_case, match_accent);
}
// ----------------------------------------------- SEARCH REPLACE STRING
int SoilString::search_replace_str(const SoilString& str, const char* rep, int pos, bool match_case, bool match_accent)
{
    SoilString soil_rep(rep);
    return search_replace_str(str, soil_rep, pos, match_case, match_accent);
}
// ----------------------------------------------- SEARCH REPLACE STRING
int SoilString::search_replace_str(const char* str, const SoilString& rep, int pos, bool match_case, bool match_accent)
{
    SoilString soil_str(str);
    return search_replace_str(soil_str, rep, pos, match_case, match_accent);
}
// ----------------------------------------------- COUNT SUB STR
int SoilString::count_sub_str(const SoilString& str, int pos, int n, bool match_case, bool match_accent)
{
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int res = pos;
    int pos_added = 0;
    int cur_n = n;
    int found_n = 0;
    int str_len = str.length;
    while(res != (-1))
    {
        cur_n = n - pos_added;
        res = search_str(str, res, cur_n, match_case, match_accent);
        if(res != (-1))
        {
            found_n++;
            res += str_len;
            pos_added = res - pos;
        }
    }
    return found_n;
}
// ----------------------------------------------- COUNT SUB STR
int SoilString::count_sub_str(const char* str, int pos, int n, bool match_case, bool match_accent)
{
    SoilString soil_str(str);
    return count_sub_str(soil_str, pos, n, match_case, match_accent);
}
// ----------------------------------------------- COUNT SUB STR
int SoilString::count_sub_str(const SoilString& str, bool match_case, bool match_accent)
{
    return count_sub_str(str,0,length,match_case,match_accent);
}
// ----------------------------------------------- COUNT SUB STR
int SoilString::count_sub_str(const char* str, bool match_case, bool match_accent)
{
    SoilString soil_str(str);
    return count_sub_str(soil_str, match_case, match_accent);
}
// ----------------------------------------------- CHANGE CASE SECTION
int SoilString::section_invert_case(int pos, int n)
{
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int i = pos;
    int e = pos + n;
    SOIL_CHAR sch;
    SOIL_CHAR sch_2;

    if(update_cursor)
    {
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
        last_rec.selected_after = false;
    }

    //undo, type 2
    if(REC_ACTIVE && !rec_action)
    {
        set_last_rec_node(40, pos, n, 0);
        rec_callback(1);
    }
    int changed_n = 0;
    while(i < e)
    {
        sch = at(i);
        sch_2 = SoilString::invert_case_ch(sch);
        text[text_i].str[i] = sch_2;
        if(!SoilString::soil_char_are_equal(sch, sch_2)){changed_n++;}

        i++;
    }

    str_changed();
    return changed_n;
}
// ----------------------------------------------- CHANGE CASE SECTION
int SoilString::section_set_case(int pos, int n, bool to_lower)
{
    if(REC_ACTIVE)
    {
        multi_trans_inc_str = new SoilIncString;
        if(multi_trans_inc_str == NULL)
        {
            return 0;
        }
    }
    int changed_n = section_set_case(pos, n, multi_trans_inc_str, to_lower);
    //undo, type 2
    if(REC_ACTIVE && !rec_action)
    {
        if(multi_trans_inc_str != NULL)
        {
            delete multi_trans_inc_str;
        }
    }
    
    if(changed_n > 0)
    {
        str_changed();
    }
    return changed_n;
}
// ----------------------------------------------- CHANGE CASE SECTION
int SoilString::section_to_lower_case(int pos, int n)
{
    return section_set_case(pos,n,true);
}
// ----------------------------------------------- CHANGE CASE SECTION
int SoilString::section_to_upper_case(int pos, int n)
{
   return section_set_case(pos,n,false);
}
// ----------------------------------------------- REPLACE TABS WITH SPACES
int SoilString::replace_tabs_with_spaces(int space_n)
{
    SoilString space_str("");
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = 32;
    int i = 0;
    while(i < space_n)
    {
        space_str.append(sch);
        i++;
    }
    bool prev_signal_receiver_0 = signal_receiver[0];
    signal_receiver[0] = false;
    int res = search_replace_str("\t", space_str, 0, true, true);
    signal_receiver[0] = prev_signal_receiver_0;

    if(signal_receiver[0] && res > 0){str_changed();}
    return res;
}

// ----------------------------------------------- INDENT
//this is supposed to indent the way a text editor would usually indent if tab is pressed while it is selected from pos to (pos + n)
//and remove indentation as well
bool SoilString::indent_section(int pos, int n, int space_n, bool reverse, bool regular, int* sp_before_pos_n, int* sp_middle_n, int* sp_last_line_n)
{
    //MARKER:make it in a way sp_ pointers can be NULL

    //regular means it keeps the tab aligned in multiples of space_n]
    
    //space_n is the one indent size(one tab) in spaces
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    //printf("\npos :%d n :%d",pos,n);
    if(space_n <= 0){return false;}
    
    //spaces added/removed to the first line of affected area, or to the only line if there is only one
    *sp_before_pos_n = 0;
    //spaces added/removed to neither the first nor the last line if there are more than 2 lines
    *sp_middle_n = 0;
    //spaces added/removed to the last line unless there is only one line in which case this remains 0
    *sp_last_line_n = 0;

    int i = pos;
    int e = pos + n;

    //---------- variables for SoilTextRecord
    int rec_str_pos = 0;//cursor pos before in orig to be indented
    int rec_ins_len = 0;//cursor pos after in orig after part to be indented

    //int c = 0;//cursor pos for in dest before indentation
    int rec_rem_len = 0;//cursor pos after indentation

    //POSSIBLE CURSOR CHANGE
    int cursor_inc = 0;
    int last_br = 0;
    bool got_indent_before = false;
    if(update_cursor)
    {
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
    }
    //small space adding loops counter
    int c = 0;

    int br_n = 0;
    int sp_n = 0;
    bool just_got_br = false;
    int remove_sp_n = 0;

    int indent = false;
    int line_start = 0;

    bool at_line_start = false;
    bool at_line_end = false;

    int new_length = length;
    int new_byte_n = byte_n;
    //////////////////////

    SOIL_CHAR sch;
    SOIL_CHAR br_sch = ext_ascii_to_soil_char(10);
    SOIL_CHAR sp_sch = ext_ascii_to_soil_char(32);

    //------------------------------------------ tracking br's ahead and counting chars to be added
    while(i < e)
    {
        sch = at(i);
        if(soil_char_are_equal(sch, br_sch))
        {
            //it doesn't consider the last br because it wouldn't indent the line after such br
            if(i < e-1)
            {
                just_got_br = true;
                br_n++;
            }
            last_br = i;
            sp_n = 0;
        }
        if(just_got_br)
        {
            //MARKER:place another limit here
            i++;
            rec_ins_len = i;  
            sch = at(i);
            while(soil_char_are_equal(sch, sp_sch))
            {
                sp_n++;
                i++;
                sch = at(i);
            }          

            if(reverse)
            {
                if(regular)
                {
                    remove_sp_n = sp_n % space_n;
                    if(remove_sp_n == 0){remove_sp_n = space_n;}
                }else{
                    remove_sp_n = space_n;
                }
                if(remove_sp_n > sp_n){remove_sp_n = sp_n;}
                new_length -= remove_sp_n;
                cursor_inc = 0 - remove_sp_n;

            }else{
                if(regular)
                {
                    new_length += space_n - (sp_n % space_n);
                    cursor_inc = space_n - (sp_n % space_n);
                }else{
                    new_length += space_n;
                    cursor_inc = space_n;
                }
            }
            
            if(update_cursor)
            {
                if(last_rec.cursor_1_before > last_br)
                {
                    last_rec.cursor_1_after += cursor_inc;
                }
                if(last_rec.cursor_2_before > last_br)
                {
                    last_rec.cursor_2_after += cursor_inc;
                }
            }


            //CHANGE FOR INVERT INDENT
            just_got_br = false;                

            //so it doesnt skip a br after the spaces
            if(soil_char_are_equal(sch, br_sch))
            {
                continue;
            }
        }
        
        i++;
    }
    //----------------------------------------- checking for single line indent
    line_start = next_char_pos(br_sch, pos, (-1));
    line_start++;

    //printf("\nPOINT 2");

    sp_n = 0;
    c = line_start;
    sch = at(c);
    //MARKER: place another limiter here
    while(soil_char_are_equal(sch, sp_sch))
    {
        sp_n++;
        c++;
        sch = at(c);
    }

    if(reverse)
    {
        if(regular)
        {
            remove_sp_n = sp_n % space_n;
            if(remove_sp_n == 0){remove_sp_n = space_n;}
        }else{
            remove_sp_n = space_n;
        }
        if(remove_sp_n > sp_n){remove_sp_n = sp_n;}
        new_length -= remove_sp_n;
        cursor_inc = 0 - remove_sp_n;

    }else{
        if(regular)
        {
            new_length += space_n - (sp_n % space_n);
            cursor_inc = space_n - (sp_n % space_n);
        }else{
            new_length += space_n;
            cursor_inc = space_n;
        }
    }
    //CHANGE FOR INVERT INDENT

    if(update_cursor)
    {
        last_rec.cursor_1_after += cursor_inc;
        last_rec.cursor_2_after += cursor_inc;
        last_rec.selected_after = last_rec.selected_before;
    }
    if(REC_ACTIVE && !rec_action)
    {
        rec_str_pos = line_start;
        if(line_start > rec_ins_len){rec_ins_len = line_start;}
        rec_ins_len -= line_start;
        rec_rem_len = rec_ins_len + (new_length - length);

        last_rec.selected_after = last_rec.selected_before;

        set_last_rec_node(20, rec_str_pos, rec_ins_len, rec_rem_len);
        rec_callback(1);
    }

    //-----------------------


    //----------------------------------------- alloc'ing and copying
    //printf("\nnew_length:%d",new_length);
    text[alt_text_i].alloc(new_length);
    LINEAR_RANGE orig_range;
    orig_range.s = 0;
    orig_range.e = line_start;
    int dest_pos = 0;

    int sp_add_n = space_n;
    //----------------------------------------- first line indent
    copy_infra_to_infra(&text[text_i], orig_range, &text[alt_text_i], dest_pos);
    //printf("\n(before pos)copying orig_range.s:%d orig_range.e:%d dest_pos:%d",orig_range.s, orig_range.e, dest_pos);
    orig_range.s = line_start;
    dest_pos = line_start;
    c = 0;
    //here sp_n still has the space number at line_start
    if(reverse)
    {
        orig_range.s += remove_sp_n;
        new_byte_n -= remove_sp_n;
        *sp_before_pos_n += remove_sp_n;
    }else{
        if(regular)
        {
            sp_add_n = space_n - (sp_n % space_n);
        }else{
            sp_add_n = space_n;
        }
        while(c < sp_add_n)
        {
            text[alt_text_i].str[dest_pos] = sp_sch;
            c++;
            dest_pos++;
            new_byte_n++;
            *sp_before_pos_n = (*sp_before_pos_n) + 1;
        }
    }
    //CHANGE FOR INVERT INDENT, increment orig_range_s instead of dest_pos

    //----------------------------------------- indent for next lines
    remove_sp_n = 0;
    sp_add_n = 0;

    i = pos;

    ///
    while(i < e)
    {
        sch = at(i);
        if(soil_char_are_equal(sch, br_sch))
        { 
            if(i < e-1)
            {
                just_got_br = true;
                br_n++;
            }
            sp_n = 0;
        }
        if(just_got_br)
        {
            //MARKER:place another limit here
            sp_n = 0;
            i++;
            sch = at(i);
            while(soil_char_are_equal(sch, sp_sch))
            {
                sp_n++;
                i++;
                sch = at(i);
            }

            if(reverse)
            {
                if(regular)
                {
                    remove_sp_n = sp_n % space_n;
                    if(remove_sp_n == 0){remove_sp_n = space_n;}
                }else{
                    remove_sp_n = space_n;
                }
                if(remove_sp_n > sp_n){remove_sp_n = sp_n;}

                *sp_middle_n += remove_sp_n;

                orig_range.e = i - remove_sp_n;
                copy_infra_to_infra(&text[text_i], orig_range, &text[alt_text_i], dest_pos);
                //printf("\ncopying orig_range.s:%d orig_range.e:%d dest_pos:%d",orig_range.s, orig_range.e, dest_pos);
                dest_pos += orig_range.e - orig_range.s;
                orig_range.s = i;

                new_byte_n -= remove_sp_n;

            }else{

                orig_range.e = i;
                copy_infra_to_infra(&text[text_i], orig_range, &text[alt_text_i], dest_pos);
                dest_pos += orig_range.e - orig_range.s;
                orig_range.s = orig_range.e;

                c = 0;
                if(regular)
                {
                    sp_add_n = space_n - (sp_n % space_n);
                }else{
                    sp_add_n = space_n;
                    //printf("\nsp_add irregular");
                }
                while(c < sp_add_n)
                {
                    text[alt_text_i].str[dest_pos] = sp_sch;
                    c++;
                    dest_pos++;
                    new_byte_n++;
                    *sp_middle_n = (*sp_middle_n) + 1;
                }
            }
            //CHANGE FOR INVERT INDENT, increment orig_range_s instead of dest_pos
            
            just_got_br = false;                

            //so it doesnt skip a br after the spaces
            if(soil_char_are_equal(sch, br_sch))
            {
                continue;
            }
        }
        
        i++;
    }

    if(reverse)
    {
        *sp_middle_n -= remove_sp_n;
        *sp_last_line_n = remove_sp_n;

    }else{
        *sp_middle_n -= sp_add_n;
        *sp_last_line_n = sp_add_n;
    }
    ///
    orig_range.e = length;
    copy_infra_to_infra(&text[text_i], orig_range, &text[alt_text_i], dest_pos);
    //printf("\ncopying orig_range.s:%d orig_range.e:%d dest_pos:%d",orig_range.s, orig_range.e, dest_pos);
    alternate_text_i();
    text[alt_text_i].release();

    length = new_length;
    byte_n = new_byte_n;

    //undo, type 2

    if(signal_receiver[0])
    {
        str_changed();
    }
    //printf("\n------------------------------------------");
    return true;
}
// ----------------------------------------------- REPALCE WITH REGULAR TAB
int SoilString::replace_with_regular_tab(int pos, int n, int space_n)
{
    //MARKER: this 8 should be global, like, MAX_TAB_W or something
    if(space_n < 0){space_n = 0;}
    if(space_n > 8){space_n = 8;}

    validate_rel_cursor(length, &pos);
    SOIL_CHAR br_sch;
    br_sch.N = 1;
    br_sch.B[0] = 10;

    SOIL_CHAR sp_sch;
    sp_sch.N = 1;
    sp_sch.B[0] = 32;

    SoilString str;

    int prev_br = next_char_pos(br_sch, pos, (-1));
    prev_br++;

    int pos_in_line = pos - prev_br;
    int tab_size = space_n - (pos_in_line % space_n);

    int i = 0;
    while(i < tab_size)
    {
        str.append(sp_sch);
        i++;
    }
    //----------- UNDO system
    //
    //insert(str, pos);
    replace(str, pos, n);

    return tab_size;
}
// ----------------------------------------------- REGULAR TAB
//indent in the line pos belongs to, doesn't need to be in the beginning of the line
int SoilString::regular_tab_input(int pos, int space_n)
{
    return replace_with_regular_tab(pos, 0, space_n);
}
// ----------------------------------------------- MKAE NUMBER SEQUENCE
void SoilString::make_number_sequence(int min, int max, bool with_separator, char sep)
{
    int digits_n = get_digit_n(max);
    int n = (max - min) + 1;
    int new_length = (n * digits_n);
    if(with_separator && n > 1)
    {
        new_length += (n - 1);
    }


    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
    }

    if(new_length == 0)
    {
        //no changes
        if(length == 0){return;}
        //--------------------------------------- SoilTextRecord action
        if(REC_ACTIVE && !rec_action)
        {
            last_rec.cursor_1_after = 0;
            last_rec.cursor_2_after = 0;
            last_rec.selected_after = false;
            set_last_rec_node(10, 0, length, 0);
            rec_callback(1);
        }
        empty_itself();
        str_changed();
        return;
    }
    //undo, type 2
    //------------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
        if(length == 0)
        {
            set_last_rec_node(0, 0, 0, new_length);
        }else{
            set_last_rec_node(20, 0, length , new_length);
        }
        
        rec_callback(1);
    }



    text[alt_text_i].alloc(new_length);

    INT_DECIMAL_DIGITS digits;
    digits = set_int_decimal_digits_struct(min);
    int num = min;

    SOIL_CHAR sch;
    sch.N = 1;
    int d = 10 - digits_n;
    int i = 0;
    int s = 0;//string
    while(num <= max)
    {

        if(num > min)
        {
            inc_int_decimal_digits_struct(&digits);
            if(with_separator)
            {
                sch.B[0] = sep;
                text[alt_text_i].str[s] = sch;
                s++;
            }
        }
        d = 10 - digits_n;
        while(d < 10)
        {
            sch.B[0] = 48 + digits.dig[d];
            text[alt_text_i].str[s] =  sch;
            s++;
            d++;
        }

        num++;
    }
    
    length = new_length;
    byte_n = new_length;
    alternate_text_i();
    text[alt_text_i].release();

    str_changed();

}
// ----------------------------------------------- PRINTF STRING
void SoilString::printf_str()const
{
    if(length > 0)
    {
        char * str = c_str_alloc();
        printf("%s",str);
        delete[] str;
    }
}
// ----------------------------------------------- PRINTF STRING
void SoilString::printf_str(int br_before, int br_after)const
{
    if(br_before < 0){br_before = 0;}
    if(br_after < 0){br_after = 0;}

    int i = 0;
    while(i < br_before)
    {
        printf("\n");
        i++;
    }
    if(length > 0)
    {

        char * str = c_str_alloc();
        printf("%s",str);
        delete[] str;
    }
    i=0;
    while(i < br_after)
    {
        printf("\n");
        i++;
    }
}

// ----------------------------------------------- PRINT DATA
void SoilString::print_data()const
{
    printf("\n\nLENGTH:%d",length);
    printf("\nBYTE_N:%d",byte_n);


    if(text[text_i].loaded)
    {
        printf("\nTEXT[TEXT_I] LOADED");
    }else{
         printf("\nTEXT[TEXT_I] NOT LOADED");
    }
    printf("\nTEXT[TEXT_I].ALLOC_LEN:%d",text[text_i].alloc_len);
    if(text[alt_text_i].loaded)
    {
        printf("\nTEXT[ALT_TEXT_I] LOADED");
    }else{
         printf("\nTEXT[ALT_TEXT_I] NOT LOADED");
    }
    printf("\nTEXT[ALT_TEXT_I].ALLOC_LEN:%d",text[alt_text_i].alloc_len);
    int len = 0;
    int l = 0;
    while(l < length)
    {
        printf("\n");
        printf("\nTEXT[%d].B[0]:%x",l,text[text_i].str[l].B[0]);
        printf("\nTEXT[%d].B[1]:%x",l,text[text_i].str[l].B[1]);
        printf("\nTEXT[%d].B[2]:%x",l,text[text_i].str[l].B[2]);
        printf("\nTEXT[%d].B[3]:%x",l,text[text_i].str[l].B[3]);
        printf("\nTEXT[%d].N:%x",l,text[text_i].str[l].N);
        l++;
        len++;
    }

}
// ----------------------------------------------- PRINT LAST REC
void SoilString::print_last_rec()
{
    printf("\n");
    if(last_rec.selected_before)
    {
        printf("\nlast_rec.selected_before: TRUE");
    }else{
        printf("\nlast_rec.selected_before: FALSE");
    }
    printf("\nlast_rec.cursor_1_before: %d",last_rec.cursor_1_before);
    printf("\nlast_rec.cursor_2_before: %d",last_rec.cursor_2_before);
    if(last_rec.selected_after)
    {
        printf("\nlast_rec.selected_after: TRUE");
    }else{
        printf("\nlast_rec.selected_after: FALSE");
    }
    printf("\nlast_rec.cursor_1_after: %d",last_rec.cursor_1_after);
    printf("\nlast_rec.cursor_2_after: %d",last_rec.cursor_2_after);
    printf("\n");
}
// ----------------------------------------------- PRINT DATA
void SoilString::print_log(int st)
{
    printf("\n");
    printf("\nPRINT_LOG: %d ",st);
    printf("\n");
    printf("\nLENGTH:%d ",length);
    printf("\nBYTE_N:%d ",byte_n);
    printf("\nTEXT_I:%d ",text_i);
    printf("\nTEXT[TEXT_I].ALLOC_LEN:%d ",text[text_i].alloc_len);
    printf("\nTEXT[ALT_TEXT_I].ALLOC_LEN:%d ",text[alt_text_i].alloc_len);

    printf("\n\nACTUAL TEXT:");
    int i = 0;
    int c = 0;
    SOIL_CHAR sch;
    char ch;
    while(i < length)
    {
        sch = at(i);
        printf("\nsch.N:%d. ",sch.N);
        while(c < sch.N)
        {
            ch = sch.B[c];
            printf("%c",ch);
            c++;
        }
        c = 0;
        i++;
    }


}
