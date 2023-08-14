#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <iostream>
#include <cstring>
#include <string>

struct INT_DECIMAL_DIGITS{
    char dig_n;
    char dig[10];
};
// ----------------------------------- GET ABSOLUTE INDEX POSITION
int get_absolute_index_pos(int len, int pos);
// ----------------------------------- GET ABSOLUTE CURSOR POSITION
int get_absolute_cursor_pos(int len, int pos);
// ----------------------------------- GET ABSOLUTE RANGE POSITIONS
void get_absolute_range_pos(int len, int* pos, int* n);
// ----------------------------------- CONVERT RELATIVE INDEX TO ABSOLUTE
int rel_to_abs_index(int len, int i);
// ----------------------------------- CONVERT RELATIVE CURSOR TO ABSOLUTE
int rel_to_abs_cursor(int len, int i);
// ----------------------------------- CONVERT RELATIVE CURSOR TO ABSOLUTE
void rel_to_abs_cursor_range(int len, int* i, int* n);
//these 4 just return if it is valid or not
// ----------------------------------- IS VALID ABSOLUTE INDEX
bool is_valid_abs_index(int len, int i);
// ----------------------------------- IS VALID RELATIVE INDEX
bool is_valid_rel_index(int len, int i);
// ----------------------------------- IS VALID ABSOLUTE CURSOR
bool is_valid_abs_cursor(int len, int i);
// ----------------------------------- IS VALID RELATIVE CURSOR
bool is_valid_rel_cursor(int len, int i);

//these 4 return if valid or not and either way correct the value to the closest limit
// ----------------------------------- VALIDATE ABSOLUTE INDEX
bool validate_abs_index(int len, int* i);
// ----------------------------------- VALIDATE RELATIVE INDEX
bool validate_rel_index(int len, int* i);
// ----------------------------------- VALIDATE ABSOLUTE CURSOR
bool validate_abs_cursor(int len, int* i);
// ----------------------------------- VALIDATE RELATIVE CURSOR
bool validate_rel_cursor(int len, int* i);
// ----------------------------------- VALIDATE RELATIVE RANGE
//set s to be the smaller one regardless if e passed is smaller
//return 0 if no value was invalid, 1 if the smaller was invalid, 2 if the bigger was invalid
//and 3 if both were invalid
int validate_rel_to_abs_cursor_range(int len, int* i, int* n);
int validate_rel_cursor_range(int len, int* i, int* n);
bool hex_to_val(char d0, char d1, char *v);
int get_digit_n(int n);//return ammount of decimal digits needed
//These functions below consider 0 to have 1 digit
INT_DECIMAL_DIGITS set_int_decimal_digits_struct(int n);
void inc_int_decimal_digits_struct(INT_DECIMAL_DIGITS* digits);
void dec_int_decimal_digits_struct(INT_DECIMAL_DIGITS* digits);

#endif // STRING_FUNCTIONS

