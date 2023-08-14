#include "soilstring.h"


bool SoilString::utf8_loaded = false;
unsigned char SoilString::utf8_case[256];
unsigned char SoilString::utf8_opposite_case[256];
unsigned char SoilString::utf8_same_case[256];
unsigned char SoilString::utf8_same_accent[256];
unsigned char SoilString::utf8_same_case_accent[256];

SoilString::SoilString()
{
    init();
}
SoilString::SoilString(const char * str)
{
    init();
    *this = str;
}
SoilString::SoilString(const std::string& str)
{
    init();
    *this = str;
}
SoilString::SoilString(const SoilString& str)
{
    init();
    *this = str;
}
SoilString::SoilString(SOIL_CHAR sch)
{
    init();
    *this = sch;
}
SoilString::SoilString(unsigned char uch)
{
    init();
    *this = uch;
}
SoilString::SoilString(char ch)
{
    init();
    *this = ch;
}
SoilString::SoilString(int num)
{
    init();
    *this = num;
}
// ----------------------------------------------- DESTRUCTOR
SoilString::~SoilString()
{
    
}
// ----------------------------------------------- INITIATE VALUES
void SoilString::init()
{
    change_count = 0;

    update_cursor = false;
    REC_ACTIVE = false;
    REC = NULL;
    rec_action = false;

    if(!utf8_loaded){fill_utf8_case_arrays();}

    text_i = 0;
    alt_text_i = 1;
    length = 0;//CHAR 
    byte_n = 0;//BYTE count

    multi_trans_inc_str = NULL;
    
    //the object_receiver[0] is the SoilText that has this string as soil_string
    //MARKER:should signal_receiver[0] be true without an obj to call
    //I know it wont call if soilobject_receiver[0] is NULL... still...
}

// ----------------------------------------------- PROCESS UTF8 
//s and e are in chars
void SoilString::process_utf8(bool from_file, FILE* file, const char * orig_str,bool line_limit, bool limit_orig, int orig_s, int orig_e, int * char_count, int * byte_count, SoilInfraString* dest_str, int dest_pos, bool do_copy)
{
    //s , e concerns the relative origin count
    //
    //UTF 8 coding:
    //0xxxxxxx is 1 byte char
    //110xxxxx 10xxxxxx
    //1110xxxx 10xxxxxx 10xxxxxx
    //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
    //
    //110xxxxx , 1110xxxx , 11110xxx are the head(control) bytes
    //for 1 , 2 , 3 auxiliary bytes repectively
    //
    //any byte 10xxxxxx without the right control byte will be discarded
    //
    //any head(control?) byte that is not followed by the right number of
    //auxiliary bytes will be discarded
    if(!validate_rel_cursor(length, &dest_pos)){return;}
    
    *byte_count = 0;
    *char_count = 0;

    if(limit_orig && orig_e <= orig_s){return;}

    int byte_i = 0;
    int char_i = 0;
    unsigned char cb = 0;
    unsigned char uch;

    int set_i = 0;
    int set_n = 0;

    int ch = 0;
    int end = 0;
    if(from_file)
    {
        if(file == NULL){return;}
        end = EOF;
        //MARKER:make sure file is readable (?)
        ch = fgetc(file);
    }else{
        ch = orig_str[byte_i];
    }

    SOIL_CHAR sch;
    sch.N = 1;
    int i = 0;
    while(ch != end && i < 10000)
    {
        //printf("\nCH:%d. I:%d.", ch, i);
        if(line_limit && ch == 10)
        {
            if(do_copy)
            {
                sch.N = 1;
                sch.B[0] = 10;
                if(dest_pos == dest_str->alloc_len){return;}
                dest_str->str[dest_pos] = sch;
            }
            *char_count += 1;
            *byte_count += 1;
            return;
        }
        uch = (unsigned char)ch;
        cb = SoilString::count_utf8_control_bits(uch);
        //*************************************** IRREGULAR BYTES
        if(cb != 1 && set_n > 0)
        {   
            byte_i -= set_i;
            set_i = 0;
            set_n = 0;
        }
        //if(cb == 1 && set_n == 0){byte_i--;}
        //--------------------------------------- SINGLE BYTE CHARS
        if(cb == 0 || cb > 4)
        {
            //WRITE SINGLE BYTE CHAR
            if(!limit_orig || (char_i >= orig_s && char_i < orig_e))
            {
                
                if(do_copy)
                {
                    sch.N = 1;
                    sch.B[0] = uch;
                    if(dest_pos == dest_str->alloc_len){return;}
                    dest_str->str[dest_pos] = sch;
                    dest_pos++;                    
                }
                *char_count += 1;
                *byte_count += 1;
            }
            char_i++;
            if(limit_orig && char_i >= orig_e)
            {return;}
        }
        //--------------------------------------- AUX BYTES
        if(cb == 1 && set_n > 0)
        {
            sch.B[set_i] = uch;
            set_i++;
            if(set_i == set_n)
            {
                //WRITE MULTI BYTE CHAR
                if(!limit_orig || (char_i >= orig_s && char_i < orig_e))
                {
                    if(do_copy)
                    {
                        if(dest_pos == dest_str->alloc_len){return;}
                        dest_str->str[dest_pos] = sch;
                        dest_pos++;                        
                    }
                    *char_count += 1;
                    *byte_count += set_n;
                }
                set_n = 0;
                set_i = 0;
                char_i++;
                if(limit_orig && char_i >= orig_e)
                {
                    return;
                }
            }   
        }
        //--------------------------------------- HEAD BYTES
        if(cb > 1 && cb < 5)
        {
            sch.N = cb;
            sch.B[0] = uch;

            set_n = cb;
            set_i = 1;
        }


        //*************************************** INCREMENT
        if(from_file)
        {
            byte_i++;
            ch = fgetc(file);
        }else{
            byte_i++;
            ch = orig_str[byte_i];
        }
        //----------------------------- WRITE

    }

}

// ----------------------------------------------- UTF8 STRING LENGTH
void SoilString::utf8_str_len(const char * str, int orig_s, int orig_e, int * CN, int * BN)
{
    process_utf8(false,NULL,str,false,true,orig_s,orig_e,CN,BN,NULL,0,false);
}
// ----------------------------------------------- UTF8 STRING LENGTH
void SoilString::utf8_str_len(const char * str, int * CN, int * BN)
{
    process_utf8(false,NULL,str,false,false,0,0,CN,BN,NULL,0,false);
}
// ----------------------------------------------- UTF8 STRING LENGTH
void SoilString::utf8_str_line_len(const char * str, int * CN, int * BN)
{
    process_utf8(false,NULL,str,true,false,0,0,CN,BN,NULL,0,false);
}
// ----------------------------------------------- UTF8 FILE LENGTH
void SoilString::utf8_file_len(FILE* file, int orig_s, int orig_e, int * CN, int * BN)
{
    process_utf8(true,file,NULL,false,true,orig_s,orig_e,CN,BN,NULL,0,false);
}
// ----------------------------------------------- UTF8 FILE LENGTH
void SoilString::utf8_file_len(FILE* file, int * CN, int * BN)
{
    process_utf8(true,file,NULL,false,false,0,0,CN,BN,NULL,0,false);
}
// ----------------------------------------------- UTF8 FILE LENGTH
void SoilString::utf8_file_line_len(FILE* file, int * CN, int * BN)
{
    process_utf8(true,file,NULL,true,false,0,0,CN,BN,NULL,0,false);
}
// ----------------------------------------------- UPDATE BYTE N
void SoilString::update_byte_n()
{
    int i = 0;
    int b = 0;
    while(i < length)
    {
        b += text[text_i].str[i].N;
        i++;
    }
    byte_n = b;
}
// ----------------------------------------------- GET SECTION BYTE N
int SoilString::get_section_byte_n(int pos, int n)
{
    validate_rel_to_abs_cursor_range(length,&pos,&n);
    int i = pos;
    int b = 0;
    int e = pos + n;
    while(i < e)
    {
        b += text[text_i].str[i].N;
        i++;
    }
    return b;
}
// ----------------------------------------------- COPY INFRA TO INFRA
void SoilString::copy_infra_to_infra(const SoilInfraString& orig, LINEAR_RANGE orig_range, SoilInfraString* dest,int dest_pos)const
{

    //to copy to the end LINEAR_RANGE.e should be str.length
    //because it is based on CURSOR position

    while(orig_range.s < orig_range.e)
    {
        dest->str[dest_pos] = orig.str[orig_range.s];

        dest_pos++;
        orig_range.s++;
    }

}
// ----------------------------------------------- COPY INFRA TO INFRA
void SoilString::copy_infra_to_infra(const SoilInfraString* orig, LINEAR_RANGE orig_range, SoilInfraString* dest,int dest_pos)
{
    copy_infra_to_infra(*orig, orig_range, dest, dest_pos);
}
// ----------------------------------------------- COPY INFRA TO INFRA
//only VALID absolute position should be passed to this method
void SoilString::copy_infra_to_infra(SoilInfraString* orig, LINEAR_RANGE orig_range, SoilInfraString* dest,int dest_pos)
{
    const SoilInfraString* orig_infra = orig;
    copy_infra_to_infra(*orig_infra, orig_range, dest, dest_pos);
}
// ----------------------------------------------- COPY STR TO INFRA
void SoilString::copy_str_to_infra(const char * str, int orig_s, int orig_e, SoilInfraString* dest, int dest_pos)
{
    int CN;
    int BN;
    process_utf8(false, NULL, str, false, true, orig_s, orig_e, &CN, &BN, dest, dest_pos, true);
}
// ----------------------------------------------- LOAD STR IN BK TEXT
void SoilString::copy_str_to_infra(const char * str, SoilInfraString* dest, int dest_pos)
{
    //copies the entire str to dest_pos
    int CN;
    int BN;    
    process_utf8(false, NULL, str, false, false, 0, 0, &CN, &BN, dest, dest_pos, true);
}
// ----------------------------------------------- LOAD STR IN BK TEXT
void SoilString::copy_str_line_to_infra(const char * str, SoilInfraString* dest, int dest_pos)
{
    //copies the entire str to dest_pos
    int CN;
    int BN;
    process_utf8(false, NULL, str, true, false, 0, 0, &CN, &BN, dest, dest_pos, true);
}
// ----------------------------------------------- COPY ASCII STR TO INFRA
void  SoilString::copy_ascii_str_to_infra(const char * str, LINEAR_RANGE orig_range, SoilInfraString* dest, int dest_pos)
{
    //this is meant to be ONE byte per character, therefore ASCII
    //and easier to parse
    //another internal function
    //the memory space for copying the string in SoilInfraString should
    //be alloc'ed already

    int i = orig_range.s;
    int c = dest_pos;//dest string index

    while(i < orig_range.e)
    {
        dest->str[c].N = 1;
        dest->str[c].B[0] = str[i];

        c++;
        i++;
    }

}

// ----------------------------------------------- COPY INFRA TO STRING
void SoilString::copy_infra_to_str(const SoilInfraString& str, LINEAR_RANGE orig_range, char* dest)const
{
    //str_len is the SoilInfraString length
    int i = orig_range.s;//SoilInfraString index
    int n = 0;//counter for byte number of each character
    int c = 0;//dest string index

    while(i < orig_range.e)
    {
        n = 0;
        while(n < str.str[i].N)
        {
            dest[c] = str.str[i].B[n];
            c++;
            n++;
        }
        i++;
    }
    dest[c] = 0;

}
// ----------------------------------------------- COPY INFRA TO STRING
void SoilString::copy_infra_to_str(SoilInfraString* str, LINEAR_RANGE orig_range, char* dest)
{
    copy_infra_to_str(*str, orig_range, dest);    
}
// ----------------------------------------------- COPY INFRA TO STRING
void SoilString::copy_infra_to_str(const SoilInfraString& str,int str_len, char* dest)const
{
    LINEAR_RANGE orig_range;
    orig_range.s = 0;
    orig_range.e = str_len;
    copy_infra_to_str(str, orig_range, dest);
}
// ----------------------------------------------- COPY INFRA TO STRING
void SoilString::copy_infra_to_str(SoilInfraString* str,int str_len, char* dest)
{
    LINEAR_RANGE orig_range;
    orig_range.s = 0;
    orig_range.e = str_len;
    copy_infra_to_str(*str, orig_range, dest);
}

// ----------------------------------------------- COPY INC STRING TO INFRA
void SoilString::copy_inc_str_to_infra(SoilIncString * inc_str, int s, int e, SoilInfraString* dest, int dest_pos)
{
    inc_str->set_mark(s);
    int i = s;

    SOIL_CHAR sch;
    while(inc_str->next_mark(&sch) && i < e)
    {
        dest->str[dest_pos] = sch;
        dest_pos++;
        i++;
    }
}
// ----------------------------------------------- COPY INC STRING TO INFRA
void SoilString::copy_inc_str_to_infra(SoilIncString * inc_str, SoilInfraString* dest, int dest_pos)
{
    copy_inc_str_to_infra(inc_str, 0, inc_str->length, dest, dest_pos);
}
// ----------------------------------------------- COPY INFRA TO INC STR
void SoilString::copy_infra_to_inc_str(const SoilInfraString& str, int s, int e, SoilIncString * dest)
{
    while(s < e)
    {
        dest->append(str.str[s]);
        s++;
    }
}
// ----------------------------------------------- COPY FILE TO INFRA
void SoilString::copy_file_to_infra(FILE* orig_file, int orig_s, int orig_e, SoilInfraString* dest, int dest_pos)
{
    //copies the entire str to dest_pos
    int CN;
    int BN;
    process_utf8(true,orig_file, NULL, false, true,orig_s,orig_e,&CN,&BN,dest,dest_pos,true);
}
// ----------------------------------------------- COPY FILE TO INFRA
void SoilString::copy_file_to_infra(FILE* orig_file, SoilInfraString* dest, int dest_pos)
{
    //copies the entire str to dest_pos
    int CN;
    int BN;
    process_utf8(true,orig_file, NULL, false, false,0,0,&CN,&BN,dest,dest_pos,true);
}
// ----------------------------------------------- COPY FILE TO INFRA
void SoilString::copy_file_line_to_infra(FILE* orig_file, SoilInfraString* dest, int dest_pos)
{
    //copies the entire str to dest_pos
    int CN;
    int BN;
    process_utf8(true,orig_file, NULL, true, false,0,0,&CN,&BN,dest,dest_pos,true);
}
// ----------------------------------------------- LOAD FROM FILE(BASE)
int SoilString::load_from_file(FILE* fo, int n, bool line_limit)
{
    if(fo == NULL || fo == nullptr){return (-1);}
    if(n == 0){return 0;}

    long int l = ftell(fo);

    int new_length = length;
    int new_byte_n = byte_n;
    int mode = 0;
    if(line_limit)
    {
        utf8_file_line_len(fo,&new_length,&new_byte_n);
    }else{
        if(n < 0)
        {
            mode = 1;
            utf8_file_len(fo,&new_length,&new_byte_n);
        }else{
            mode = 2;
            utf8_file_len(fo, 0, n, &new_length, &new_byte_n);
        }
    }
    if(length == 0 && new_length == 0){return 0;}
    //PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        last_rec.cursor_1_after = 0;
        last_rec.cursor_2_after = 0;
        last_rec.selected_after = false;
    }
    //------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        bool got_rec = false;
        int type = 0;
        int str_pos = 0;
        int ins_len = length;
        int rem_len = new_length;

        if(length > 0)
        {
            if(new_length > 0)
            {
                //type 2, transforming
                type = 20;
                got_rec = true;
            }else{
                //type 1, removing
                type = 10;
                rem_len = 0;
                got_rec = true;
            }
        }else{
            if(new_length > 0)
            {
                //type 0, inserting
                type = 0;
                ins_len = 0;
                got_rec = true;
            }
        }
        if(got_rec)
        {
            set_last_rec_node(type, str_pos, ins_len, rem_len);
            rec_callback(1);
        }
    }

    if(length == 0 && new_length == 0){return 0;}
    //------------------------------------ SoilTextRecord action END
    byte_n = new_byte_n;
    length = new_length;

    text[text_i].alloc(length);
    fseek(fo,l,SEEK_SET);

    if(mode == 0)
    {
        copy_file_line_to_infra(fo,&text[text_i],0);
    }
    if(mode == 1)
    {
        copy_file_to_infra(fo,&text[text_i],0);
    }
    if(mode == 2)
    {
        copy_file_to_infra(fo, 0, n, &text[text_i],0);
    }
    
    //undo, type 2

    str_changed();
    return length;
}
// ----------------------------------------------- PREPARE ALTERNATE STRING
// this is used to copy the part of the string that will not be altered
// could be used for both insert and remove
void SoilString::prepare_text_str(int new_length, int pos, bool release_alt_text)
{
    validate_abs_cursor(length, &pos);

    text[alt_text_i].alloc(new_length);
    alternate_text_i();//ALTERNATE TEXT_I INDEX HERE


    LINEAR_RANGE orig_range;
    int paste_pos;
    /////////////// COPYING FIRST PART
    orig_range.s = 0;
    orig_range.e = pos;

    paste_pos = 0;

    copy_infra_to_infra(&text[alt_text_i], orig_range, &text[text_i], paste_pos);
    /////////////// COPYING SECOND PART
    orig_range.s = pos;
    orig_range.e = length;

    paste_pos = pos;

    if(new_length > length)
    {
        paste_pos += (new_length - length);
    }else{
        orig_range.s += (length - new_length);
    }

    copy_infra_to_infra(&text[alt_text_i], orig_range, &text[text_i], paste_pos);

    length = new_length;

    if(release_alt_text)
    {
        text[alt_text_i].release();
    }
}

// ----------------------------------------------- INSERT CHAR
// this function should only be called by another internal one
// that verifies if there is a need to alloc more memory in
// text[text_i] beforehand
void SoilString::insert_soil_char(SOIL_CHAR ch,int pos)
{
    SOIL_CHAR input_ch = ch;
    SOIL_CHAR next_ch;
    int i = pos;

    int add_bn = ch.N;

    while(i < length)
    {
        next_ch = text[text_i].str[i];
        text[text_i].str[i] = input_ch;

        input_ch = next_ch;
        i++;
    }
    text[text_i].str[i] = input_ch;

    byte_n += add_bn;
    length++;

}
// ----------------------------------------------- REMOVE SOIL CHAR
// like insert_soil_char() with this you should also 
// validate the need to reallocate memory before hand
void SoilString::remove_soil_char(int pos)
{
    int i = pos;
    int j = pos + 1;

    int rem_bn = text[text_i].str[i].N;

    while(i < length)
    {
        if(j < length)
        {
            text[text_i].str[i] = text[text_i].str[j];
        }else{
            text[text_i].str[i].N=0;
        }
        i++;
        j++;
    }
    byte_n -= rem_bn;
    length-=1;

}
// ----------------------------------------------- FILL SECTION
void SoilString::fill_section(SOIL_CHAR sch, int pos, int n)
{
    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int e = pos + n;
    while(pos < e)
    {
        text[text_i].str[pos]=sch;
        pos++;
    }
}
// ----------------------------------------------- FILL SECTION
void SoilString::fill_section(unsigned char uch, int pos, int n)
{
    
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = uch;
    fill_section(sch,pos,n);
}
// ----------------------------------------------- FILL SECTION
void SoilString::fill_section(char ch, int pos, int n)
{   
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = (unsigned char)ch;
    fill_section(sch,pos,n);
}
// ----------------------------------------------- SECTION ITSELF
//reduce to a section of itself
void SoilString::section_itself(int pos, int n)
{
    if(length == 0){return;}

    validate_rel_to_abs_cursor_range(length,&pos,&n);

    LINEAR_RANGE range;
    range.s = pos;
    range.e = pos+n;

    if(range.s == 0 && range.e == length){return;}

    //if(range.s == range.e){empty_itself();return;}

    text[alt_text_i].alloc(n);

    if(update_cursor)
    {
        //PROCESS POSSIBLE CURSOR CHANGE
        if(last_rec.cursor_1_before <= range.s)
        {
            last_rec.cursor_1_after = 0;
        }else{
            last_rec.cursor_1_after = last_rec.cursor_1_before - range.s;
            if(last_rec.cursor_1_after > n){last_rec.cursor_1_after = n;}
        }
        if(last_rec.cursor_2_before <= range.s)
        {
            last_rec.cursor_2_after = 0;
        }else{
            last_rec.cursor_2_after = last_rec.cursor_2_before - range.s;
            if(last_rec.cursor_2_after > n){last_rec.cursor_2_after = n;}
        }
        last_rec.selected_after = last_rec.selected_before;
        if(last_rec.selected_after && last_rec.cursor_1_after == last_rec.cursor_2_after)
        {
            last_rec.selected_after = false;
        }
        
    }

    //------------------------------------------------------------------------- SoilTextRecord action
    if(REC_ACTIVE && !rec_action)
    {
        bool got_rec = false;
        if(n > 0)
        {
            
            if(range.s == 0)
            {
                //type 1, removing
                set_last_rec_node(10,range.e, (length - range.e), 0);
                got_rec = true;
            }
            if(range.e == length)
            {
                //type 1, removing
                set_last_rec_node(10, 0, range.s, 0);
                got_rec = true;
            }
            if(!got_rec)
            {
                //type 16, double insertion
                multi_trans_inc_str = NULL;
                multi_trans_inc_str = new SoilIncString;
                SOIL_CHAR sch_1, sch_2;
                int second_sec = length - range.e;

                code_int_in_2_soil_char(range.s, &sch_1, &sch_2);
                multi_trans_inc_str->append(sch_1);
                multi_trans_inc_str->append(sch_2);
                copy_infra_to_inc_str(text[text_i], 0, range.s, multi_trans_inc_str);

                code_int_in_2_soil_char(second_sec, &sch_1, &sch_2);
                multi_trans_inc_str->append(sch_1);
                multi_trans_inc_str->append(sch_2);
                copy_infra_to_inc_str(text[text_i], range.e, length, multi_trans_inc_str);

                set_last_rec_node(16, 0, 4 + (range.s + second_sec), 0);
                got_rec = true;
            }
            
        }else{
            //type 1, removing
            set_last_rec_node(10, 0, length, 0);
            got_rec = true;
        }
        
        if(got_rec)
        {
            rec_callback(1);
            if(multi_trans_inc_str != NULL)
            {
                delete multi_trans_inc_str;
            }
        }
    }

    //------------------------------------------------------------------------- SoilTextRecord action END
    length = n;
    copy_infra_to_infra(&text[text_i], range, &text[alt_text_i],0);
    alternate_text_i();
    text[alt_text_i].release();
    update_byte_n();
    //str_changed();
}
// ----------------------------------------------- EMPTY ITSELF
void SoilString::empty_itself()
{
    int prev_length = length;
    length = 0;
    byte_n = 0;
    text[text_i].release();
}
// ----------------------------------------------- STR PARTS
int SoilString::str_parts(int str_len, int pos, int n, LINEAR_RANGE* part_1, LINEAR_RANGE* part_2, int* fill)
{
    //this function is auxiliar to replace in soilstring.cpp
    //
    //str_len is the length of string to be inserted to pos
    //n is the number of chars to be removed from pos
    //part_1 the part of this str that remains
    //part_2 the other part of this if the insertion is in the middle
    //fill is in case the insertion is beyond this str size and the gap needs to be filled

    //pos and n should be absolute already
    int e = pos + n;
    part_1->s = 0;
    part_1->e = str_len;
    part_2->s = 0;
    part_2->e = str_len;
    *fill = 0;

    if(pos <= 0)
    {
        if(e <= 0)
        {
            //part_1->e = str_len;
            *fill = 0 - e;
            return 0;
        }else{
            if(e < str_len)
            {
                part_1->s = e;
                //part_1->e = str_len;
                return 1;
            }else{
                return 2;
            }
        }
    }else{
        if(pos < str_len)
        {
            part_1->e = pos;
            if(e < str_len)
            {
                part_2->s = e;
                //part_2->e = str_len;
                return 3;
            }else{
                return 4;
            }
        }else{
            //part_1->e = str_len;
            *fill = pos - str_len;
            return 5;
        }
    }

    //shouldnt reach this point
    return 0;
}
// ----------------------------------------------- REPLACE 2
int SoilString::replace(int orig_type, const SoilString * soil_str, SoilIncString * soil_inc_str, const char * str, int pos, int n)
{
    int prev_length = length;

    int str_len;//char_count
    int str_bn;//byte_count
    int new_length;

    //before was bool from_soil_str

    //orig_type
    //0: SoilString*
    //1: SoilIncString*
    //2: const char*
    orig_type %= 3;
    if(orig_type < 0){orig_type *= (-1);}

    if(orig_type == 0)
    {
        if(soil_str == NULL){return 0;}
        str_len = soil_str->length;
        str_bn = soil_str->byte_n;
    }
    if(orig_type == 1)
    {
        if(soil_inc_str == NULL){return 0;}
        str_len = soil_inc_str->length;
        str_bn = soil_inc_str->byte_n;
    }
    if(orig_type == 2)
    {
        if(str == NULL){return 0;}
        utf8_str_len(str,&str_len,&str_bn);
    }

    //n should always be positive after this function:
    rel_to_abs_cursor_range(length, &pos, &n);

    new_length = length;
    int end_n = pos + n;
    if(pos >= length)
    {
        new_length += pos - length;
        n = 0;
    }else{
        if(pos < 0)
        {
            //increase new_length
            new_length -= pos;
        }

        if(end_n > length){n -= end_n - length;}
        //if(n < 0){n = 0;}//n should never be less than 0 here since pos < length
    }
    
    new_length -= n;

    if(new_length < 0){new_length = 0;}
    new_length += str_len;

    //--------------------------------------     

    LINEAR_RANGE this_range[2];
    int fill = 0;
    int position_case = str_parts(length, pos, n, &this_range[0], &this_range[1], &fill);

    //0:input str
    //1:fill
    //2:copy this str
    int order[3]={0,1,2};
    int order_n = 3;
    int this_i = 0;

    switch(position_case)
    {
        case 1:
            order[1] = 2;
            order_n = 2;
            break;
        case 2:
            order_n = 1;
            break;
        case 3:
            order[0] = 2;
            order[1] = 0;
            order[2] = 2;
            break;
        case 4:
            order[0] = 2;
            order[1] = 0;
            order_n = 2;
            break;
        case 5:
            order[0] = 2;
            order[1] = 1;
            order[2] = 0;
            break;

        default:
            break;
    }
    //-------------------------------------- VERIFY IF IT ACTUALLY CHANGED
    //empty insertion with no removal or filling
    if(position_case == 0 && str_len == 0 && (pos+n) == 0)
    {
        return 0;
    }
    if(position_case == 3 && str_len == 0 && n == 0)
    {
        return 0;
    }
    if(position_case == 5 && str_len == 0 && pos == length)
    {
        return 0;
    }


    //-------------------------------------- PROCESS POSSIBLE CURSOR CHANGE
    if(update_cursor)
    {
        //data for the undo rec node
        int rec_str_pos = pos;
        int rec_ins_len = 0;
        int rec_rem_len = str_len;

        int dif;
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
        switch(position_case)
        {
            case 0:
                rec_str_pos = 0;
                rec_rem_len = str_len + fill;

                last_rec.cursor_1_after += str_len + fill;
                last_rec.cursor_2_after += str_len + fill;
                break;
            case 1:
                rec_str_pos = 0;
                rec_ins_len = end_n;

                dif = last_rec.cursor_1_before - end_n;
                if(dif < 0){dif = 0;}
                last_rec.cursor_1_after = str_len + dif;

                dif = last_rec.cursor_2_before - end_n;
                if(dif < 0){dif = 0;}
                last_rec.cursor_2_after = str_len + dif;
                break;
            case 2:
                rec_str_pos = 0;
                rec_ins_len = length;
                rec_rem_len = str_len;

                //I added these two lines below to fix it for input
                //when selecting the whoel string and typing the cursor would go 
                //to position 0, before the first inputed char
                last_rec.cursor_1_after = str_len;
                last_rec.cursor_2_after = str_len;

                validate_abs_cursor(new_length, &last_rec.cursor_1_after);
                validate_abs_cursor(new_length, &last_rec.cursor_2_after);
                break;
            case 3:
                rec_ins_len = n;

                if(last_rec.cursor_1_before >= pos)
                {
                    dif = last_rec.cursor_1_before - end_n;
                    if(dif < 0){dif = 0;}
                    last_rec.cursor_1_after = pos + str_len + dif;
                }
                if(last_rec.cursor_2_before >= pos)
                {
                    dif = last_rec.cursor_2_before - end_n;
                    if(dif < 0){dif = 0;}
                    last_rec.cursor_2_after = pos + str_len + dif;
                }
                break;
            case 4:
                rec_ins_len = length - pos;

                if(last_rec.cursor_1_before >= pos)
                {
                    last_rec.cursor_1_after = new_length;
                }
                if(last_rec.cursor_2_before >= pos)
                {
                    last_rec.cursor_2_after = new_length;
                }
                break;
            case 5:
                rec_str_pos = pos - fill;
                rec_rem_len = fill + str_len;
                if(last_rec.cursor_1_before == length)
                {
                    last_rec.cursor_1_after = new_length;
                }
                if(last_rec.cursor_2_before == length)
                {
                    last_rec.cursor_2_after = new_length;
                }
                break;
            default:
                break;
        }
        if(last_rec.selected_before && last_rec.cursor_1_after != last_rec.cursor_2_after)
        {
            last_rec.selected_after = true;
        }else{
            last_rec.selected_after = false;
        }

        if(REC_ACTIVE && !rec_action)
        {
            set_last_rec_node(20, rec_str_pos, rec_ins_len , rec_rem_len);
            rec_callback(1);
        }
    }
    //-------------------------------------- 
    text[alt_text_i].alloc(new_length);
    alternate_text_i();
    //length is updated here so fill_section() can work with the new value
    int length_dif = new_length - length;
    length = new_length;

    int dest_pos = 0;
    int o = 0;
    while(o < order_n)
    {
        switch(order[o])
        {
            //input str
            case 0:

                if(orig_type == 0)
                {
                    LINEAR_RANGE orig_range;
                    orig_range.s = 0;
                    orig_range.e = str_len;
                    if(this == soil_str)
                    {
                        copy_infra_to_infra(&text[alt_text_i], orig_range, &text[text_i], dest_pos);
                    }else{
                        copy_infra_to_infra(&soil_str->text[soil_str->text_i], orig_range, &text[text_i], dest_pos);
                    }
                }
                if(orig_type == 1)
                {
                    
                    copy_inc_str_to_infra(soil_inc_str, &text[text_i], dest_pos);
                }
                if(orig_type == 2)
                {
                    copy_str_to_infra(str, &text[text_i], dest_pos);
                }
                dest_pos += str_len;
            break;

            //fill space
            case 1:
                fill_section('X', dest_pos, fill);
                dest_pos += fill;
            break;

            //copy from this
            case 2: 
                copy_infra_to_infra(&text[alt_text_i], this_range[this_i], &text[text_i], dest_pos);
                dest_pos += this_range[this_i].e - this_range[this_i].s;
                this_i++;
            
            break;

            default:
            break;
        }
        o++;
    }
    update_byte_n();

    text[alt_text_i].release();

    str_changed();
    
    return length_dif;
}
// ----------------------------------------------- RECORD OP DOUBLE INSERTION
//UNDO section_itself() when the section is in the middle
int SoilString::rec_op_double_insertion(STR_REC_NODE node, const SoilString& rec_str)
{
    int new_length = length + (node.ins_len - 4);
    text[alt_text_i].alloc(new_length);
    alternate_text_i();

    int rec_str_i = 0;
    int ins_len;
    LINEAR_RANGE orig_range;
    int dest_pos = 0;
    int total_copied = 0;

    //copy ins 1
    ins_len = coded_int_from_2_soil_char(rec_str.at(rec_str_i), rec_str.at(rec_str_i + 1));
    orig_range.s = 2;
    orig_range.e = 2 + ins_len;
    copy_infra_to_infra(rec_str.text[rec_str.text_i], orig_range, &text[text_i], dest_pos);
    rec_str_i = orig_range.e;
    dest_pos += ins_len;
    total_copied += ins_len;

    //copy this.text
    orig_range.s = 0;
    orig_range.e = length;
    copy_infra_to_infra(text[alt_text_i], orig_range, &text[text_i], dest_pos);
    dest_pos += length;

    //copy ins 2
    ins_len = coded_int_from_2_soil_char(rec_str.at(rec_str_i), rec_str.at(rec_str_i + 1));
    orig_range.s = rec_str_i + 2;
    orig_range.e = orig_range.s + ins_len;
    copy_infra_to_infra(rec_str.text[rec_str.text_i], orig_range, &text[text_i], dest_pos);
    total_copied += ins_len;
    
    text[alt_text_i].release();

    length = new_length;
    update_byte_n();

    return total_copied;
}
// ----------------------------------------------- SIMPLIFY
int SoilString::simplify(bool rec_redo_action, SoilIncString* rec_inc_str)
{
    
    //amount of segments of blank spaces to remove
    if(update_cursor && !rec_redo_action)
    {
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
    }

    if(REC_ACTIVE)
    {
        //placeholders for total_rec_n and total_ins_len
        rec_inc_str->append(c_char_to_soil_char('a'));
        rec_inc_str->append(c_char_to_soil_char('a'));
        rec_inc_str->append(c_char_to_soil_char('a'));
        rec_inc_str->append(c_char_to_soil_char('a'));
    }

    //---------------------------------- counting spaces loop
    int rec_coord;
    int rec_ins_len;

    int seg_i_at_end = 0;
    int space_seg_n = 0;
    int rem_space_n = 0;//returned value
    int seg_i = 0;
    bool is_space = false;
    int sn = 0;
    int dec = 0;
    bool increment = false;
    int last_i = length - 1;
    int i = 0;
    SOIL_CHAR sch;
    SOIL_CHAR int_sch_1, int_sch_2;
    while(i < length)
    {
        sch = at(i);
        if(soil_char_is_space(sch, false))
        {
            if(!is_space){seg_i = i;}
            is_space = true;
            sn++;
        }else{
            is_space = false;
        }
        
        //--------------------
        if(!is_space)
        {
            if(seg_i == 0)
            {
                if(sn > 0)
                {
                    //printf("\ninc 1sn:%d",sn);
                    increment = true;
                }
            }else{
                if(sn > 1)
                {
                    //printf("\ninc 2");
                    increment = true;
                    seg_i++;
                    dec = 1;
                }else{
                    sn = 0;
                }
            }

        }else{
            if(i == last_i)
            {
                //printf("\ninc 3");
                seg_i_at_end = seg_i;
                increment = true;
            }
        }
        //--------------------
        if(increment)
        {            
            // ---------------------------- CURSOR / RECORD

            if(update_cursor && !rec_redo_action)
            {
                int dif = 0;
                //NEW CURSOR 1
                if(last_rec.cursor_1_before > seg_i)
                {
                    dif = last_rec.cursor_1_before - seg_i;
                    if(dif > rem_space_n){dif = rem_space_n;}
                    last_rec.cursor_1_after -= dif;
                }
                //NEW CURSOR 2
                if(last_rec.cursor_2_before > seg_i)
                {
                    dif = last_rec.cursor_2_before - seg_i;
                    if(dif > rem_space_n){dif = rem_space_n;}
                    last_rec.cursor_2_after -= dif;
                }
            }
            if(REC_ACTIVE)
            {
                rec_coord = seg_i - rem_space_n;
                rec_ins_len = sn - dec;
                //SOIL_CHAR sch = code_int_in_soil_char(rec_coord);
                code_int_in_2_soil_char(rec_coord, &int_sch_1, &int_sch_2);
                rec_inc_str->append(int_sch_1);
                rec_inc_str->append(int_sch_2);
                code_int_in_2_soil_char(rec_ins_len, &int_sch_1, &int_sch_2);
                rec_inc_str->append(int_sch_1);
                rec_inc_str->append(int_sch_2);
            }
            // ---------------------------- CURSOR / RECORD END
            space_seg_n++;
            increment = false;
            rem_space_n += (sn - dec);
            sn = 0;
            dec = 0;
        }

        i++;
    }
    //---------------------------------- 
    if(rem_space_n == 0)
    {
        return 0;
    }
    //---------------------------------- text record
    if(REC_ACTIVE)
    {
        code_int_in_2_soil_char(space_seg_n, &int_sch_1, &int_sch_2);
        rec_inc_str->set_ch_at(0, int_sch_1);
        rec_inc_str->set_ch_at(1, int_sch_2);
        code_int_in_2_soil_char(rem_space_n, &int_sch_1, &int_sch_2);
        rec_inc_str->set_ch_at(2, int_sch_1);
        rec_inc_str->set_ch_at(3, int_sch_2);

        if(!rec_redo_action && !rec_action)
        {
            set_last_rec_node(80, 0, 0, 0);
            rec_callback(1);
        }
    }

    int new_length = length - rem_space_n;
    int new_byte_n = byte_n - rem_space_n;
    text[alt_text_i].alloc(new_length);
    //---------------------------------- copying string loop
    bool copy_ch = true;
    i = 0;
    int d = 0;
    
    while(i < length)
    {
        sch = at(i);
        if(soil_char_is_space(sch, false))
        {
            if(is_space || i == 0 || i == seg_i_at_end)
            {
                copy_ch = false;
            }
            is_space = true;
        }else{
            is_space = false;
        }

        if(copy_ch)
        {
            //printf("\nCOPY:%d",i);
            text[alt_text_i].str[d] = text[text_i].str[i];
            d++;
        }else{
            //printf("\nSKIP:%d",i);
            copy_ch = true;
        }

        i++;
    }
    alternate_text_i();
    text[alt_text_i].release();
    length = new_length;
    byte_n = new_byte_n;
    
    return rem_space_n;
    //printf("\nREM SPACE N:%d",rem_space_n);
}
// ----------------------------------------------- RECORD OP SIMPLIFY STR
int SoilString::rec_op_simplify_undo(STR_REC_NODE node, const SoilString& rec_str)
{
    int total_rec_n = coded_int_from_2_soil_char(rec_str.at(0), rec_str.at(1));
    int total_ins_len = coded_int_from_2_soil_char(rec_str.at(2), rec_str.at(3));

    int new_length = length + total_ins_len;
    text[alt_text_i].alloc(new_length);
    alternate_text_i();

    LINEAR_RANGE orig_range;
    orig_range.s = 0;

    SOIL_CHAR sp_sch;
    sp_sch = c_char_to_soil_char(' ');
    int ins_sp_n = 0;
    int s = 2;
    int i = 0;
    int r = 4;
    int dest_pos = 0;
    while(i < total_rec_n)
    {

        orig_range.e = coded_int_from_2_soil_char(rec_str.at(r), rec_str.at(r + 1));
        copy_infra_to_infra(text[alt_text_i], orig_range, &text[text_i],dest_pos);
        dest_pos += orig_range.e - orig_range.s;
        
        ins_sp_n = coded_int_from_2_soil_char(rec_str.at(r + 2), rec_str.at(r + 3));

        s = 0;
        while(s < ins_sp_n)
        {
            text[text_i].str[dest_pos] = sp_sch;
            dest_pos++;
            s++;
        }
        orig_range.s = orig_range.e;
        i++;
        r += 4;
    }
    if(orig_range.s < length)
    {
        orig_range.e = length;
        copy_infra_to_infra(text[alt_text_i], orig_range, &text[text_i],dest_pos);
    }

    length = new_length;
    byte_n == ins_sp_n;

    text[alt_text_i].release();
    return ins_sp_n;
}
// ----------------------------------------------- RECORD OP SEARCH REPLACE STR
//for undo and redo
int SoilString::rec_op_search_replace_str(STR_REC_NODE node, const SoilString& rec_str, SoilString* new_rec_str)
{
    timeval time_prev, time_after;
    gettimeofday(&time_prev, NULL);

    int dif = node.ins_len - node.rem_len;
    //int total_rec_n = coded_int_from_soil_char(rec_str.at(0));
    int total_rec_n = coded_int_from_2_soil_char(rec_str.at(0), rec_str.at(1));
    
    //MARKER: is total_rec_n <= 0 even possible? is it the best approach to this?
    if(total_rec_n <=0){return 0;}

    int new_length = length + total_rec_n*dif;
    
    text[alt_text_i].alloc(new_length);
    alternate_text_i();

    new_rec_str->empty_itself();
    int new_rec_str_len = 2;
    if(node.type == 72){
        new_rec_str_len += total_rec_n*(node.rem_len+2);
    }else{
        new_rec_str_len += (2*total_rec_n) + node.rem_len;
    }
    new_rec_str->text[new_rec_str->text_i].alloc(new_rec_str_len);
    SOIL_CHAR sch_1, sch_2;
    code_int_in_2_soil_char(total_rec_n, &sch_1, &sch_2);
    new_rec_str->text[new_rec_str->text_i].str[0] = sch_1;
    new_rec_str->text[new_rec_str->text_i].str[1] = sch_2;

    int new_rec_pos = 2;

    bool copied_some = false;

    LINEAR_RANGE rec_orig_range;
    rec_orig_range.s = 2;
    rec_orig_range.e = rec_orig_range.s + node.ins_len;

    LINEAR_RANGE this_orig_range;
    this_orig_range.s = 0;
    this_orig_range.e = 0;
    
    int dest_pos = 0;
    int str_pos = 0;
    int p = 2 + node.ins_len;
    int i = 0;
    while(i < total_rec_n)
    {

        str_pos = coded_int_from_2_soil_char(rec_str.at(p), rec_str.at(p+1));
        this_orig_range.e = str_pos;
        
        // COPYING ITSELF
        copy_infra_to_infra(text[alt_text_i], this_orig_range, &text[text_i], dest_pos);
        dest_pos += this_orig_range.e - this_orig_range.s;

        this_orig_range.s = this_orig_range.e;
        this_orig_range.e = this_orig_range.s + node.rem_len;
        if(node.type == 72 || !copied_some)
        {
            // SAVING REPLACED SECTION TO NEW_REC_STR
            copy_infra_to_infra(text[alt_text_i], this_orig_range, &new_rec_str->text[new_rec_str->text_i], new_rec_pos);
            new_rec_pos += this_orig_range.e - this_orig_range.s;
            copied_some = true;
        }

        code_int_in_2_soil_char(dest_pos, &sch_1, &sch_2);
        new_rec_str->text[new_rec_str->text_i].str[new_rec_pos] = sch_1;
        new_rec_str->text[new_rec_str->text_i].str[new_rec_pos+1] = sch_2;
        new_rec_pos += 2;

        this_orig_range.s = this_orig_range.e;
        // INSERTING
        copy_infra_to_infra(rec_str.text[rec_str.text_i], rec_orig_range, &text[text_i], dest_pos);
        if(node.type == 71)
        {
            rec_orig_range.s += node.ins_len + 2;
            rec_orig_range.e = rec_orig_range.s + node.ins_len;
            p += node.ins_len + 2;
        }else{
            p += 2;
        }
        dest_pos += node.ins_len;
        i++;
    }
    if(this_orig_range.s < length)
    {
        this_orig_range.e = length;
        copy_infra_to_infra(text[alt_text_i], this_orig_range, &text[text_i], dest_pos);
    }

    new_rec_str->length = new_rec_str_len;
    new_rec_str->update_byte_n();

    length = new_length;
    text[alt_text_i].release();
    update_byte_n();
    return 0;
}
// ----------------------------------------------- SECTION TO LOWER CASE
int SoilString::section_set_case(int pos, int n, SoilIncString* rec_inc_str, bool to_lower)
{
    //This method is used by SoilTextRecord to REDO(not undo) section_set_case()
    //and used by section_set_case()(operator) itself
    //str_changed() call is in section_set_case()(operator)

    validate_rel_to_abs_cursor_range(length, &pos, &n);
    int i = pos;
    int e = pos + n;
    SOIL_CHAR sch;
    SOIL_CHAR sch_2;
    bool is_different = false;

    bool first_in_sequence = true;
    int total_rec_n = 0;
    int rec_pos = 0;
    int rec_len = 0;
    SOIL_CHAR int_sch_1;
    SOIL_CHAR int_sch_2;


    if(REC_ACTIVE)
    {
        //placeholders for total_rec_n and total_rec_n
        rec_inc_str->append(c_char_to_soil_char('a'));
        rec_inc_str->append(c_char_to_soil_char('a'));
    }

    int changed_n = 0;
    while(i < e)
    {
        sch = at(i);
        if(REC_ACTIVE)
        {
            if(to_lower)
            {
                is_different = SoilString::is_upper_case(sch);
            }else{
                is_different = SoilString::is_lower_case(sch);
            }
            if(is_different)
            {
                if(first_in_sequence)
                {
                    first_in_sequence = false;
                    rec_pos = i;
                }
                rec_len++;
            }else{
                if(rec_len > 0)
                {
                    code_int_in_2_soil_char(rec_pos, &int_sch_1, &int_sch_2);
                    rec_inc_str->append(int_sch_1);
                    rec_inc_str->append(int_sch_2);
                    code_int_in_2_soil_char(rec_len, &int_sch_1, &int_sch_2);
                    rec_inc_str->append(int_sch_1);
                    rec_inc_str->append(int_sch_2);
                    total_rec_n++;
                }
                first_in_sequence = true;
                rec_len = 0;
            }
        }
        if(to_lower)
        {
            sch_2 = SoilString::to_lower_ch(sch);
        }else{
            sch_2 = SoilString::to_upper_ch(sch);
        }
        text[text_i].str[i] = sch_2;
        if(!SoilString::soil_char_are_equal(sch, sch_2)){changed_n++;}
    
        i++;
    }

    if(rec_len > 0)
    {
        code_int_in_2_soil_char(rec_pos, &int_sch_1, &int_sch_2);
        rec_inc_str->append(int_sch_1);
        rec_inc_str->append(int_sch_2);
        code_int_in_2_soil_char(rec_len, &int_sch_1, &int_sch_2);
        rec_inc_str->append(int_sch_1);
        rec_inc_str->append(int_sch_2);
        total_rec_n++;
    }

    if(update_cursor)
    {
        last_rec.cursor_1_after = last_rec.cursor_1_before;
        last_rec.cursor_2_after = last_rec.cursor_2_before;
        last_rec.selected_after = false;
    }

    //undo, type 2
    if(REC_ACTIVE && total_rec_n > 0)
    {
        code_int_in_2_soil_char(total_rec_n, &int_sch_1, &int_sch_2);
        rec_inc_str->set_ch_at(0, int_sch_1);
        rec_inc_str->set_ch_at(1, int_sch_2);

        if(!rec_action)
        {
            int node_type = 41;
            if(!to_lower)
            {
                node_type = 42;
            }
            set_last_rec_node(node_type, pos, n, 0);
            rec_callback(1);
        }
    }
    return changed_n;

}
// ----------------------------------------------- REC OP SECTION SET CASE
int SoilString::rec_op_section_set_case_undo(STR_REC_NODE node, const SoilString& rec_str)
{    
    //This method is used by SoilTextRecord to UNDO(not redo) section_set_case()
    
    int total_rec_n = coded_int_from_2_soil_char(rec_str.at(0), rec_str.at(1));
 
    int rec_i = 2;//index for rec_str
    int rec_pos;
    int rec_len;
    SOIL_CHAR sch;

    int i = 0;
    while(i < total_rec_n && i < 20)
    {
        rec_pos = coded_int_from_2_soil_char(rec_str.at(rec_i), rec_str.at(rec_i+1));
        rec_i += 2;
        rec_len = coded_int_from_2_soil_char(rec_str.at(rec_i), rec_str.at(rec_i+1));
        rec_i += 2;

        int c = 0;
        while(c < rec_len)
        {
            if(node.type == 41)
            {
                text[text_i].str[rec_pos + c] = SoilString::to_upper_ch(text[text_i].str[rec_pos + c]);
                
            }else{
                text[text_i].str[rec_pos + c] = SoilString::to_lower_ch(text[text_i].str[rec_pos + c]);
                
            }
            c++;            
        }
        i++;
    }
    return i;
}
// ----------------------------------------------- ALTERNATE TEXT INDEX
void SoilString::alternate_text_i()
{
    int aux = text_i;
    text_i = alt_text_i;
    alt_text_i = aux;
}
// ----------------------------------------------- GET ALLOC LENGTH
int SoilString::get_alloc_len()
{
    return text[text_i].alloc_len;
}
// ----------------------------------------------- CHANGE ALLOC UNIT
void SoilString::set_alloc_unit(Uint32 alloc_unit)
{
    text[text_i].mem_bound.reset_unit(alloc_unit);
    text[alt_text_i].mem_bound.reset_unit(alloc_unit);

    if(length > 0)
    {
        LINEAR_RANGE range;
        range.s = 0;
        range.e = length - 1;

        text[alt_text_i].alloc(length);
        copy_infra_to_infra(&text[text_i],range,&text[alt_text_i],0);

        alternate_text_i();

        text[alt_text_i].release();
    }

}
// ----------------------------------------------- SET LAST REC NODE
void SoilString::set_last_rec_node(int type, int str_pos, int ins_len, int rem_len)
{
    last_rec.type = type;
    last_rec.str_pos = str_pos;
    last_rec.ins_len = ins_len;
    last_rec.rem_len = rem_len;
}
// ----------------------------------------------- SET LAST REC TEXT FIELD BEFORE
void SoilString::set_last_rec_text_fields_before(bool selected_before, int cursor_1_before, int cursor_2_before)
{
    last_rec.selected_before = selected_before;
    last_rec.cursor_1_before = cursor_1_before;
    last_rec.cursor_2_before = cursor_2_before;
}
// ----------------------------------------------- SET LAST REC TEXT FIELD AFTER
void SoilString::set_last_rec_text_fields_after(bool selected_after, int cursor_1_after, int cursor_2_after)
{
    last_rec.selected_after = selected_after;
    last_rec.cursor_1_after = cursor_1_after;
    last_rec.cursor_2_after = cursor_2_after;
}
// ----------------------------------------------- SET UPDATE CURSOR
void SoilString::set_update_cursor(bool update)
{
    update_cursor = true;
}

// ----------------------------------------------- SOILOBJECT CALLBACK
void SoilString::str_changed()
{
    
    if(soilobject_receiver[0] != NULL && signal_receiver[0])
    {
        change_count++;
        soilobject_receiver[0]->soilobject_callback(1);
    }
}
// ----------------------------------------------- SOILTEXTRECORD CALLBACK
void SoilString::rec_callback(int arg)
{
    //arg:
    //0 : simply make rec
    //2 : 
    
    soilobject_receiver[1]->soilobject_callback(arg);
}