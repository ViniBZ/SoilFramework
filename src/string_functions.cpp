#include "string_functions.h"

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
    if(i < 0 || i >= len){return false;}
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
    if(i < 0 || i > len){return false;}
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
//return 0 if the range was within the bounds of len
//return 1 if it passed beyond 0
//2 if passed len
//3 if passed both
//n is always >= 0 after this
int validate_rel_to_abs_cursor_range(int len, int* i, int* n)
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

// ----------------------------------- VALIDATE RELATIVE RANGE
//return 0 if the range was within the bounds of len
//return 1 if it passed beyond 0
//2 if passed len
//3 if passed both
int validate_rel_cursor_range(int len, int* i, int* n)
{
    int s = (*i);
    if(s < 0){s += len+1;}
    
    int e = s + (*n);

    int pass_s = 0;
    int pass_e = 0;
    if(s < 0)
    {
        s = 0;
        pass_s = 1;
    }
    if(e < 0){e = 0;pass_s = 1;}

    if(s > len){s = len;pass_e = 2;}
    if(e > len){e = len;pass_e = 2;}

    *i = s;
    *n = e - s;

    return pass_s+pass_e;

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
//---------------------------------------------- GET DIGIT N
//return ammount of decimal digits needed
//max 10 digits
int get_digit_n(int n)
{
    int abs_n = n;
    if(abs_n < 0){abs_n *= (-1);}

    int i = 0;
    int d = 1;
    int limit[9] = {9, 99, 999, 9999, 99999, 999999, 9999999, 99999999, 999999999};
    while(i < 9)
    {
        if(abs_n > limit[i])
        {
            d++;
        }else{
            break;
        }
        i++;
    }
    return d;
}
//---------------------------------------------- SET IN INT DEC DIGITS STRUCT
INT_DECIMAL_DIGITS set_int_decimal_digits_struct(int n)
{
    INT_DECIMAL_DIGITS aux;
    int div = 1000000000;
    int rest = n;
    int res = 0;
    int i = 0;    
    int dig_n = 10;
    bool got_first_non_zero = false;

    while(i < 10)
    {
        //printf("\nrest:%d. div:%d.", rest,div);
        res = rest / div;
        if(res == 0)
        {
            if(!got_first_non_zero)
            {
                dig_n--;
            }
        }else{
            got_first_non_zero = true;
        }
        //printf(" res:%d.",res);
        aux.dig[i] = (char)res;
        //printf(" aux.dig[%d]:%d",i,aux.dig[i]);
        rest %= div;
        div /= 10;
        i++;
    }
    if(dig_n == 0){dig_n = 1;}
    aux.dig_n = dig_n;
    return aux;
}
//---------------------------------------------- INC IN INT DEC DIGITS STRUCT
void inc_int_decimal_digits_struct(INT_DECIMAL_DIGITS* digits)
{
    //this donst validate for limits
    int i = 1;
    int d = 9;
    while(i <= 10)
    {
        if(digits->dig[d] < 9)
        {
            digits->dig[d] += 1;
            break;
        }else{
            digits->dig[d] = 0;
            if(i == digits->dig_n){digits->dig_n += 1;}
        }

        i++;
        d--;
    }
}
//---------------------------------------------- DEC IN INT DEC DIGITS STRUCT
void dec_int_decimal_digits_struct(INT_DECIMAL_DIGITS* digits)
{
    //this dont validate for limits, if the number is 0 and use this function
    //the result will be 9
    int i = 1;
    int d = 9;
    while(i <= digits->dig_n)
    {
        if(digits->dig[d] > 0)
        {
            digits->dig[d] -= 1;
            if(digits->dig[d] == 0 && i > 1 && i == digits->dig_n)
            {
                digits->dig_n -= 1;
            }
            break;
        }else{
            digits->dig[d] = 9;
        }

        i++;
        d--;
    }
}