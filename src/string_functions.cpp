#include "string_functions.h"
#include <iostream>
// ----------------------------------- COUNT UTF8 CONTROL BITS
// ----------------------------------- SWAP INT
void swap_int(int* a, int* b)
{
    int aux = (*a);
    *a = (*b);
    *b = aux;
}
// ----------------------------------- SWAP CHAR
void swap_char(char* a, char* b)
{
    char aux = (*a);
    *a = (*b);
    *b = aux;
}
// ----------------------------------- STRING LENGTH
int str_len(const char* s)
{
    if(s==NULL){return -1;}
    int len=0;
    while(s[len] != 0){len++;}
    return len;
}


// ----------------------------------- RETURNS WHETHER i IS A VALID INTDEX BASED ON LEN (IS_V)
bool is_valid_index(int len, int i)
{
    if(i < 0 || i >= len){return false;}
    return true;
}
// ----------------------------------- RETURNS WHETHER i IS A VALID RELATIVE INTDEX BASED ON LEN, NEGATIVE NUMBERS ARE POSSIBLE(IS_V)
bool is_valid_relative_index(int len, int i)
{
    if(i >= len){return false;}
    if(i < ((-1)*len)){return false;}
    return true;
}
// ----------------------------------- RETURNS WHETHER i IS A VALID CURSOR POS FOR TEXT(IS_V)
bool is_valid_cursor_pos(int len, int i)
{
    if(i < 0 || i > len){return false;}
    return true;
}
// ----------------------------------- RETURNS WHETHER i IS A VALID RELATIVE CURSOR POS FOR TEXT, NEGATIVE NUMBERS ARE POSSIBLE(IS_V)
bool is_valid_relative_cursor_pos(int len, int i)
{
    if(i > len){return false;}
    if(i < (-1)*(len+1)){return false;}
    return true;
}

// ----------------------------------- VALIDATE INDEX (return correct proximate)(VAL)
int validate_index(int len, int i)
{
    if(i < 0){i = 0;}
    if(i >= len){i = len - 1;}
    return i;
}
// ----------------------------------- VALIDATE CURSOR (return correct proximate)(VAL)
int validate_cursor(int len, int i)
{
    if(i < 0){i = 0;}
    if(i > len){i = len;}
    return i;
}
// ----------------------------------- VALIDATE INDEX IN RANGE (return correct proximate)(VAL)
int validate_index_in_range(int min,int max, int i)
{
    if(i < min){i = min;}
    if(i > max){i = max;}
    return i;
}

// ----------------------------------- COPY STRING
//those are internal functions, NO validation is done in it
//so make sure the arguments have valid values
//no negative values shall be passed as pos or n
void copy_str( const char* source_str,int source_pos,int n, char* dest_str,int dest_pos)
{
    n += source_pos;
    while(source_pos < n)
    {
        dest_str[dest_pos] = source_str[source_pos];
        source_pos++;
        dest_pos++;
    }

}
// ----------------------------------- COPY STD::STRING
//those are internal functions, NO validation is done in it
//so make sure the arguments have valid values
//no negative values shall be passed as pos or n
void copy_str( std::string source_str,int source_pos,int n, char* dest_str,int dest_pos)
{
    n += source_pos;
    while(source_pos < n)
    {
        dest_str[dest_pos] = source_str.at(source_pos);
        source_pos++;
        dest_pos++;
    }

}
// ----------------------------------- ALLOCATE COPY STRING
//no negative values shall be passed as pos or n
//the values passed to this function must be previously validated
char* alloc_copy_str(const char* source_str,int source_pos,int n)
{

    char* dest_str = NULL;
    int alloc_len=0;

    dest_str = new char[n];
    copy_str(source_str,source_pos,n,dest_str,0);

    return dest_str;
}

// ----------------------------------- GET ABSOLUTE POSITION( INDEX )(VAL)(ABS)
//len must be positive

//postitions in string
//POSITIVE pos : 0 V 1 I 2 N 3 I 4
//NEGATIVE pos index : -4 V -3 I -2 N -1 I
//NEGATIVE pos cursor: -5 V -4 I -3 N -2 I -1

//positive n counts the amount of char to the right
//negative n counts the amount of char to the left
int get_absolute_index_pos(int len, int pos)
{
    if(pos < 0)
    {
        pos = len + pos;
        if(pos < 0){return 0;}

    }else{
        if(pos > (len - 1)){return (len - 1);}
    }
    return pos;
}
// ----------------------------------- GET ABSOLUTE CURSOR POSITION(VAL)(ABS)
//len must be positive

//postitions in string
//example: VINI
//POSITIVE pos : 0 V 1 I 2 N 3 I 4
//NEGATIVE pos index : -4 V -3 I -2 N -1 I
//NEGATIVE pos cursor: -5 V -4 I -3 N -2 I -1

//positive n counts the amount of char to the right
//negative n counts the amount of char to the left
int get_absolute_cursor_pos(int len, int pos)
{
    if(pos < 0)
    {
        pos = (len + 1) + pos;
        if(pos < 0){return 0;}

    }else{
        if(pos > len){return len;}
    }
    return pos;
}
// ----------------------------------- GET ABSOLUTE RANGE POSITIONS(VAL)(ABS)
//len must be positive
//it calculates the range based on CURSOR POSITION
//the result values will be validated based on len
//calling this you don't need to call get_absolute_cursor_pos
void get_absolute_range_pos(int len, int* pos, int* n)
{
    int p1 = *pos;
    p1 = get_absolute_cursor_pos(len,p1);
    int p2 = p1 + *n;

    if(p1 > p2)
    {
        int aux = p1;
        p1 = p2;
        p2 = aux;
    }
    if(p1 < 0){p1 = 0;}
    if(p2 < 0){p2 = 0;}

    if(p1 > len){p1 = len;}
    if(p2 > len){p2 = len;}

    *pos = p1;
    *n = p2 - p1;
}
// ----------------------------------- COMPARE STRINGS
bool are_strings_equal(char* s1, const char* s2)
{
    int i=0;
    while(s1[i] != 0 && s2[i] != 0)
    {
        if(s1[i] != s2[i]){return false;}
        i++;
    }
    if(s1[i] != s2[i]){return false;}
    return true;
}

// ----------------------------------- ARE STRINGS EQUAL
bool are_strings_equal(const char *s1, const char *s2)
{
    int i=0;
    while(s1[i] != 0 && s2[i] != 0)
    {
        if(s1[i] != s2[i]){return false;}
        i++;
    }
    if(s1[i] != s2[i]){return false;}
    return true;
}
// ----------------------------------- ARE STRINGS EQUAL
bool are_strings_equal(const char *s1, std::string s2)
{
    int i=0;
    int s2_len = s2.length();
    while(s1[i] != 0 && i < s2_len)
    {
        if(s1[i] != s2.at(i)){return false;}
        i++;
    }
    if(s1[i] ==0 && i == s2_len){return true;}
    return false;
}

// ----------------------------------- STRING 1 CONTAINS STRING 2
// see if s1 contains s2
// what if s2 is empty ?
bool str_contains(const char* s1, const char* s2)
{

    int s1_len = str_len(s1);
    int s2_len = str_len(s2);

    int s1_end = s1_len - s2_len;
    if(s1_end < 0){return false;}


    int b1 = 0;
    int i1 = b1;

    int i2 = 0;
    bool contains=false;
    while(b1 <= s1_end)
    {
        contains = true;
        while(i2 < s2_len)
        {
            if(s1[i1] != s2[i2])
            {
                contains = false;
                break;
            }
            i1++;
            i2++;
        }
        if(contains){return true;}

        i2 = 0;
        b1++;
        i1 = b1;
    }
    return false;

}
// ----------------------------------- STRING 1 CONTAINS STRING 2
// see if s1 contains s2
// what if s2 is empty ?
bool str_contains(const char* s1, std::string s2)
{
    int s1_len = str_len(s1);
    int s2_len = s2.length();

    int s1_end = s1_len - s2_len;
    if(s1_end < 0){return false;}

    int b1 = 0;
    int i1 = b1;

    int i2 = 0;
    bool contains=false;
    while(b1 <= s1_end)
    {
        contains = true;
        while(i2 < s2_len)
        {
            if(s1[i1] != s2.at(i2))
            {
                contains = false;
                break;
            }
            i1++;
            i2++;
        }
        if(contains){return true;}

        i2 = 0;
        b1++;
        i1 = b1;
    }
    return false;

}
// ----------------------------------- CONVERT RELATIVE INDEX TO ABSOLUTE
int rel_to_abs_index(int len, int i)
{
    if(i < 0){i += len;}
    return i;
}
// ----------------------------------- CONVERT RELATIVE CURSOR TO ABSOLUTE
int rel_to_abs_cursor(int len, int i)
{
    if(i < 0){i += (len + 1);}
    return i;
}
// ----------------------------------- CONVERT RELATIVE CURSOR TO ABSOLUTE
//it allows n to go beyond len
void rel_to_abs_cursor_range(int len, int* i, int* n)
{
    *i = rel_to_abs_cursor(len,*i);
    int e = *i + *n;
    if(e < *i)
    {
        int aux = *i;
        *i = e;
        e = aux;
    }
    *n = e - (*i);
}
//these 4 just return if it is valid or not
// ----------------------------------- IS VALID ABSOLUTE INDEX
bool is_valid_abs_index(int len, int i)
{
    if(i < 0){return false;}
    if(i >= len){return false;}
    return true;
}
// ----------------------------------- IS VALID RELATIVE INDEX
bool is_valid_rel_index(int len, int i)
{
    if(i < 0){i += len;}
    return is_valid_abs_index(len, i);
}
// ----------------------------------- IS VALID ABSOLUTE CURSOR
bool is_valid_abs_cursor(int len, int i)
{
    if(i < 0){return false;}
    if(i > len){return false;}
    return true;
}
// ----------------------------------- IS VALID RELATIVE CURSOR
bool is_valid_rel_cursor(int len, int i)
{
    if(i < 0){i += (len + 1);}
    return is_valid_abs_cursor(len, i);
}

//these 4 return if valid or not and either way correct the value to the closest limit
// ----------------------------------- VALIDATE ABSOLUTE INDEX
bool validate_abs_index(int len, int* i)
{
    if(*i < 0)
    {
        *i=0;
        return false;
    }
    if(*i >= len)
    {
        *i=len-1;
        return false;
    }
    return true;
}
// ----------------------------------- VALIDATE RELATIVE INDEX
bool validate_rel_index(int len, int* i)
{
    if(*i < 0){*i += len;}
    return validate_abs_index(len ,i);
}
// ----------------------------------- VALIDATE ABSOLUTE CURSOR
bool validate_abs_cursor(int len, int* i)
{
    if(*i < 0)
    {
        *i=0;
        return false;
    }
    if(*i > len)
    {
        *i=len;
        return false;
    }
    return true;
}
// ----------------------------------- VALIDATE RELATIVE CURSOR
bool validate_rel_cursor(int len, int* i)
{
    if(*i < 0){*i += (len+1);}
    return validate_abs_cursor(len, i);
}

// ----------------------------------- VALIDATE RELATIVE RANGE
//set s to be the smaller one regardless if e passed is smaller
//return 0 if no value was invalid, 1 if the smaller was invalid, 2 if the bigger was invalid
//and 3 if both were invalid
int validate_rel_cursor_range(int len, int* i, int* n)
{
    bool valid_i = validate_rel_cursor(len, i);
    int e = *i + *n;
    bool valid_e = validate_abs_cursor(len, &e);

    if(*i > e)
    {
        int aux = *i;
        *i = e;
        e = aux;
        bool baux = valid_i;
        valid_i = valid_e;
        valid_e = baux;
    }
    *n = e - *i;

    if(valid_i)
    {
        if(valid_e){return 0;}
        return 2;
    }else{
        if(valid_e){return 1;}
        return 3;
    }
    return 0;
}
//---------------------------------------------- HEX TO VAL
//returns false if d0,d1 don't form a valid hex number
bool hex_to_val(char d0, char d1, char* v)
{
    *v = 0;
    bool res = false;
    if(d0 > 47 && d0 < 58)
    {
        *v += (d0 - 48) * 16;
        res = true;
    }
    if(d0 > 64 && d0 < 71)
    {
        *v += (d0 - 55) * 16;
        res = true;
    }
    if(d0 > 96 && d0 < 103)
    {
        *v += (d0 - 87) * 16;
        res = true;
    }
    if(!res){return false;}
    res = false;

    if(d1 > 47 && d1 < 58)
    {
        *v += (d1 - 48);
        res = true;
    }
    if(d1 > 64 && d1 < 71)
    {
        *v += (d1 - 55);
        res = true;
    }
    if(d1 > 96 && d1 < 103)
    {
        *v += (d1 - 87);
        res = true;
    }
    if(!res){return false;}

    return true;
}