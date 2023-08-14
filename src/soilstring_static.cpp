#include "soilstring.h"

// ----------------------------------------------- FROM C STRING
SoilString SoilString::from_c_str(const char* str)
{
    SoilString soil_str(str);
    return soil_str;
}

// ----------------------------------------------- FROM NUMBER
SoilString SoilString::from_number(signed long long num, int base, int min_dig_n, int max_dig_n)
{
    SoilString str("");
    str.insert(num,base,min_dig_n,max_dig_n,0);
    return str;
}
// ----------------------------------------------- FROM NUMBER
SoilString SoilString::from_number(int num, int base, int min_dig_n, int max_dig_n)
{
    SoilString str("");
    str.insert(num,base,min_dig_n,max_dig_n,0);
    return str;
}
// ----------------------------------------------- FROM NUMBER
SoilString SoilString::from_number(signed long long num, int base)
{
    SoilString str("");
    str.insert(num,base,-1,-1,0);
    return str;
}
// ----------------------------------------------- FROM NUMBER
SoilString SoilString::from_number(int num, int base)
{
    SoilString str("");
    str.insert(num,base,-1,-1,0);
    return str;
}
// ----------------------------------------------- FROM NUMBER
SoilString SoilString::from_number(signed long long num)
{
    SoilString str("");
    str.insert(num,10,-1,-1,0);
    return str;
}
// ----------------------------------------------- FROM NUMBER
SoilString SoilString::from_number(int num)
{
    SoilString str("");
    str.insert(num,10,-1,-1,0);
    return str;
}
// ----------------------------------------------------------------- COUNT UTF8 CONTROL BITS
int SoilString::count_utf8_control_bits(unsigned char uch)
{
    //this counts just up to FIVE control bits
    //
    //UTF 8 coding:
    //0xxxxxxx :0 CONTROL BITS
    //10xxxxxx :1 CONTROL BIT
    //110xxxxx :2 CONTROL BITS
    //1110xxxx :3 CONTROL BITS
    //11110xxx :4 CONTROL BITS

    int res = 0;
    int i = 0;
    int div = 128;
    while(i < 5)
    {
        if(uch / div)
        {
            res++;
            uch -= div;
        }else{break;}

        div /= 2;
        i++;
    }
    return res;
}
// ----------------------------------------------------------------- INIT SOIL CHAR
SOIL_CHAR SoilString::init_soil_char(unsigned char n, unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3)
{
    SOIL_CHAR sch;
    if(n < 0){n=0;}else{if(n > 4){n=4;}}
    sch.N = n;
    sch.B[0] = b0;
    sch.B[1] = b1;
    sch.B[2] = b2;
    sch.B[3] = b3;
    
    return sch;
}
// ----------------------------------------------- EXTENDED ASCII INDEX TO SOIL CHAR
//maybe ind should be a unsigned char because it can only pass ext ascii values
SOIL_CHAR SoilString::ext_ascii_to_soil_char(int ind)
{
    SOIL_CHAR sch;
    if(ind < 128)
    {
        sch.N = 1;
        sch.B[0] = ind;
        return sch;
    }
    if(ind < 192)
    {
        sch.N = 2;
        sch.B[0] = 194;
        sch.B[1] = ind;
        return sch;
    }
    sch.N = 2;
    sch.B[0] = 195;
    sch.B[1] = ind - 64;

    return sch;
}
// ----------------------------------------------- C STR TO SOIL CHAR
SOIL_CHAR SoilString::utf8_c_str_to_soil_char(const char* str)
{
    SOIL_CHAR sch;
    sch.N = 0;

    int i = 0;
    int cb = 0;
    int set_i = 0;
    int set_n = 0;
    unsigned char uch;
    while(i < 5 && str[i] != 0)
    {
        uch = (unsigned char)str[i];
        cb = SoilString::count_utf8_control_bits(uch);
        //*************************************** IRREGULAR BYTES
        if(cb != 1 && set_n > 0){sch.N=0;return sch;}
        if(cb == 1 && set_n == 0){sch.N=0;return sch;}
        //--------------------------------------- SINGLE BYTE CHARS
        if(cb == 0 || cb > 4)
        {
            sch.N = 1;
            sch.B[0] = uch;
            return sch;
        }
        //--------------------------------------- AUX BYTES
        if(cb == 1 && set_n > 0)
        {
            sch.B[set_i] = uch;
            set_i++;
            if(set_i == set_n)
            {
            return sch;}   
        }
        //--------------------------------------- HEAD BYTES
        if(cb > 1 && cb < 5)
        {
            set_n = cb;
            sch.N = cb;
            sch.B[0] = uch;
            set_i = 1;
        }
        i++;
    }
    return sch;
}
// ----------------------------------------------- C CHAR TO SOIL CHAR
SOIL_CHAR SoilString::c_char_to_soil_char(unsigned char uch)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = uch;
    return sch;
}
// ----------------------------------------------- C CHAR TO SOIL CHAR
SOIL_CHAR SoilString::c_char_to_soil_char(char ch)
{
    SOIL_CHAR sch;
    sch.N = 1;
    sch.B[0] = (unsigned char)ch;
    return sch;
}
// ----------------------------------------------- SOIL CHAR TO EXTENDED ASCII INDEX
int SoilString::soil_char_to_ext_ascii(SOIL_CHAR sch)
{

    int index = (-1);
    if(sch.N == 1)
    {
        return sch.B[0];
    }
    if(sch.N == 2)
    {
        //0xc2
        if(sch.B[0] == 0xc2)
        {
            return sch.B[1];
        }
        //0xc3
        if(sch.B[0] == 0xc3)
        {
            return sch.B[1] + 64;
        }
    }
    return index;
}
// ----------------------------------------------- IS LOWER CASE
bool SoilString::is_lower_case(SOIL_CHAR sch)
{
    int ind = soil_char_to_ext_ascii(sch);
    if(utf8_case[ind] == 1){return true;}
    return false;
}
// ----------------------------------------------- IS UPPER CASE
bool SoilString::is_upper_case(SOIL_CHAR sch)
{
    int ind = soil_char_to_ext_ascii(sch);
    if(utf8_case[ind] == 2){return true;}
    return false;
}
// ----------------------------------------------- 
bool SoilString::is_letter(SOIL_CHAR sch)
{
    int ind = soil_char_to_ext_ascii(sch);
    if(utf8_opposite_case[ind] != 0){return true;}
    return false;
}
// ----------------------------------------------- IS DIGIT
bool SoilString::is_digit(SOIL_CHAR sch)
{
    if(sch.N != 1){return false;}
    if(sch.B[0] > 47 && sch.B[0] < 58){return true;}
    return false;
}

// ----------------------------------------------- INVERT CASE
SOIL_CHAR SoilString::invert_case_ch(SOIL_CHAR sch)
{
    int ind = soil_char_to_ext_ascii(sch);
    if(ind <= 0){return sch;}
    if(utf8_case[ind] == 1 || utf8_case[ind] == 2)
    {
        unsigned char ret_ind = utf8_opposite_case[ind];
        sch = ext_ascii_to_soil_char(ret_ind);
    }
    return sch;
}
// ----------------------------------------------- TO LOWER
SOIL_CHAR SoilString::to_lower_ch(SOIL_CHAR sch)
{
    int ind = soil_char_to_ext_ascii(sch);
    if(ind <= 0){return sch;}
    if(utf8_case[ind] == 2)
    {
        unsigned char ret_ind = utf8_opposite_case[ind];
        sch = ext_ascii_to_soil_char(ret_ind);
    }
    return sch;
}

// ----------------------------------------------- TO UPPER
SOIL_CHAR SoilString::to_upper_ch(SOIL_CHAR sch)
{
    int ind = soil_char_to_ext_ascii(sch);
    if(ind <= 0){return sch;}
    if(utf8_case[ind] == 1)
    {
        unsigned char ret_ind = utf8_opposite_case[ind];
        sch = ext_ascii_to_soil_char(ret_ind);
    }
    return sch;
}
// ----------------------------------------------- INT CODED IN SOIL CHAR
//doesn't convert to text. uses little endian
SOIL_CHAR SoilString::code_int_in_soil_char(int num)
{
    //if num == 0 then sch.N = 0
    SOIL_CHAR sch = {0,0,0,0,0};
    sch.N = 1;
    sch.B[0] = (unsigned char)(num & 0x000000FF);
    if(sch.B[0] > 0){sch.N = 1;}
    num = num >> 8;

    sch.B[1] = (unsigned char)(num & 0x000000FF);
    if(sch.B[1] > 0){sch.N = 2;}
    num = num >> 8;

    sch.B[2] = (unsigned char)(num & 0x000000FF);
    if(sch.B[2] > 0){sch.N = 3;}
    num = num >> 8;

    sch.B[3] = (unsigned char)(num & 0x000000FF);
    if(sch.B[3] > 0){sch.N = 4;}

    return sch;
}
// ----------------------------------------------- CODE INT USING UNICODE
void SoilString::code_int_in_2_soil_char(int num, SOIL_CHAR* sch_1, SOIL_CHAR* sch_2)
{
    //sch_1 is the big end
    //both SOIL_CHAR will be encoded as 3 byte UNICODE chars because
    //3 byte char has 16 usable bits
    unsigned short short_1 = 0;
    unsigned short short_2 = num;
    num >>= 16;
    short_1 = num;

    // ----------------------- SCH 1
    unsigned char in_bits[3];
    in_bits[0] = short_1 >> 12;

    in_bits[1] = short_1 >> 4;
    in_bits[1] >>= 2;

    in_bits[2] = short_1 << 2;
    in_bits[2] >>= 2;

    unsigned char res_bits[3];
    res_bits[0] = 0xE0;
    res_bits[0] = res_bits[0] | in_bits[0];

    res_bits[1] = 0x80;
    res_bits[1] = res_bits[1] | in_bits[1];

    res_bits[2] = 0x80;
    res_bits[2] = res_bits[2] | in_bits[2];

    sch_1->N = 3;
    sch_1->B[0] = res_bits[0];
    sch_1->B[1] = res_bits[1];
    sch_1->B[2] = res_bits[2];

    // ----------------------- SCH 2

    in_bits[0] = short_2 >> 12;

    in_bits[1] = short_2 >> 4;
    in_bits[1] >>= 2;

    in_bits[2] = short_2 << 2;
    in_bits[2] >>= 2;

    res_bits[0] = 0xE0;
    res_bits[0] = res_bits[0] | in_bits[0];

    res_bits[1] = 0x80;
    res_bits[1] = res_bits[1] | in_bits[1];

    res_bits[2] = 0x80;
    res_bits[2] = res_bits[2] | in_bits[2];

    sch_2->N = 3;
    sch_2->B[0] = res_bits[0];
    sch_2->B[1] = res_bits[1];
    sch_2->B[2] = res_bits[2];
}
// ----------------------------------------------- GET INT FROM 2 SOIL CHAR
int SoilString::coded_int_from_2_soil_char(SOIL_CHAR sch_1, SOIL_CHAR sch_2)
{
    unsigned int res = 0;
    //sch_1 is the big end

    unsigned char in_bits[3];
    in_bits[0] = sch_1.B[0];
    in_bits[1] = sch_1.B[1];
    in_bits[2] = sch_1.B[2];
    
    //each 2 bytes of sch_1 that will be the big end of int(res)
    unsigned char res_bits[2];
    res_bits[0] = in_bits[0] << 4;
    res_bits[0] += (in_bits[1] & 0b00111100) >> 2;

    res += (int)res_bits[0];
    res <<= 8;

    //the 2 least significant bytes
    res_bits[1] = (in_bits[1] & 0b00000011) << 6;
    res_bits[1] += in_bits[2] & 0b00111111;
    res += (int)res_bits[1];
    res <<= 8;

    //same but sch_2
    in_bits[0] = sch_2.B[0];
    in_bits[1] = sch_2.B[1];
    in_bits[2] = sch_2.B[2];

    res_bits[0] = in_bits[0] << 4;
    res_bits[0] += (in_bits[1] & 0b00111100) >> 2;

    res += (int)res_bits[0];
    res <<= 8;

    //the 2 least significant bytes
    res_bits[1] = (in_bits[1] & 0b00000011) << 6;
    res_bits[1] += in_bits[2] & 0b00111111;

    res += (int)res_bits[1];

    int ret_res = res;

    return ret_res;
}

// ----------------------------------------------- CODED INT FROM SOIL CHAR
int SoilString::coded_int_from_soil_char(SOIL_CHAR sch)
{
    int num = 0;
    int fac = 1;
    int i = 0;
    while(i < sch.N)
    {
        num += sch.B[i] * fac;
        fac *= 256;
        i++;
    }
    return num;
}

// ----------------------------------------------- COMPARE SOIL CHAR
int SoilString::compare_soil_char(SOIL_CHAR sch_1, SOIL_CHAR sch_2, bool match_case, bool match_accent)
{
    int res = 0;
    int ind_1 = soil_char_to_ext_ascii(sch_1);
    int ind_2 = soil_char_to_ext_ascii(sch_2);
    
    if((!match_case || !match_accent) && (ind_1 >= 0 && ind_2 >= 0))
    {

        unsigned char ch_val_1;
        unsigned char ch_val_2;
        if(!match_case)
        {
            if(!match_accent)
            {
                //MATCH NONE
                ch_val_1 = utf8_same_case_accent[ind_1];
                ch_val_2 = utf8_same_case_accent[ind_2];
            }else{
                //MATCH ACCENT
                ch_val_1 = utf8_same_case[ind_1];
                ch_val_2 = utf8_same_case[ind_2];
            }
        }else{
            //MATCH CASE
            ch_val_1 = utf8_same_accent[ind_1];
            ch_val_2 = utf8_same_accent[ind_2];
        }
        
        if(ch_val_1 < ch_val_2){return (-1);}
        if(ch_val_1 > ch_val_2){return 1;}
        if(ch_val_1 == ch_val_2){return 0;}
    }
    //MATCH BOTH
    if(sch_1.N < sch_2.N){return (-1);}
    if(sch_1.N > sch_2.N){return 1;}
    int i = 0;
    while(i < sch_1.N)
    {
        if(sch_1.B[i] < sch_2.B[i]){return (-1);}
        if(sch_1.B[i] > sch_2.B[i]){return 1;}
        i++;
    }

    return 0;
}
int SoilString::compare_soil_char(SOIL_CHAR sch_1, SOIL_CHAR sch_2)
{
    return compare_soil_char(sch_1, sch_2, true, true);
}
// ----------------------------------------------- SOIL CHAR IS EQUAL
bool SoilString::soil_char_are_equal(SOIL_CHAR sch_1, SOIL_CHAR sch_2, bool match_case, bool match_accent)
{
    int res = compare_soil_char(sch_1, sch_2, match_case, match_accent);
    if(res == 0){return true;}
    return false;
}
// ----------------------------------------------- SOIL CHAR IS EQUAL
bool SoilString::soil_char_are_equal(SOIL_CHAR sch_1, SOIL_CHAR sch_2)
{
    if(sch_1.N != sch_2.N){return false;}
    int i = 0;
    while(i < sch_1.N)
    {
        if(sch_1.B[i] != sch_2.B[i]){return false;}
        i++;
    }
    return true;
}
// ----------------------------------------------- SOIL CHAR IS SPACE
bool SoilString::soil_char_is_space(SOIL_CHAR sch, bool br_is_space)
{
    if(sch.N != 1){return false;}
    if(sch.B[0] == 32 || sch.B[0] == 9){return true;}
    if(br_is_space && sch.B[0] == 10){return true;}
    return false;
}
// ----------------------------------------------- SOIL CHAR IS ALPHANUM OR _
bool SoilString::soil_char_is_alphanum(SOIL_CHAR sch)
{
    if(sch.N != 1){return false;}
    if(sch.B[0] > 47 && sch.B[0] < 58){return true;}
    if(sch.B[0] > 64 && sch.B[0] < 91){return true;}
    if(sch.B[0] > 96 && sch.B[0] < 123){return true;}
    return false;
}
// ----------------------------------------------- SOIL CHAR IS ALPHANUM OR _
bool SoilString::soil_char_is_alphanum_or_(SOIL_CHAR sch)
{
    if(sch.N != 1){return false;}
    if(sch.B[0] > 47 && sch.B[0] < 58){return true;}
    if(sch.B[0] > 64 && sch.B[0] < 91){return true;}
    if(sch.B[0] > 96 && sch.B[0] < 123){return true;}
    if(sch.B[0] == 95){return true;}
    return false;
}
// ----------------------------------------------- SOIL CHAR ALPHANUM / DIGIT / _ / '/'
bool SoilString::soil_char_is_valid_for_path(SOIL_CHAR sch)
{
    if(sch.N != 1){return false;}
    if(sch.B[0] > 45 && sch.B[0] < 58){return true;}
    if(sch.B[0] > 64 && sch.B[0] < 91){return true;}
    if(sch.B[0] > 96 && sch.B[0] < 123){return true;}
    if(sch.B[0] == 95){return true;}
    return false;
}
// ----------------------------------------------- SOIL CHAR ALPHANUM / DIGIT / _ 
bool SoilString::soil_char_is_valid_for_path_entry(SOIL_CHAR sch)
{
    if(sch.N != 1){return false;}
    if(sch.B[0] == 46){return true;}
    if(sch.B[0] > 47 && sch.B[0] < 58){return true;}
    if(sch.B[0] > 64 && sch.B[0] < 91){return true;}
    if(sch.B[0] > 96 && sch.B[0] < 123){return true;}
    if(sch.B[0] == 95){return true;}
    return false;
}