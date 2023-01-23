#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <string>
// ----------------------------------- COUNT UTF8 CONTROL BITS
//int count_utf8_control_bits(unsigned char uch);
// ----------------------------------- UTF8 STRING LENGTH (char count)
//int utf8_str_len(const char* s);
// ----------------------------------- SWAP INT
void swap_int(int* a, int* b);
// ----------------------------------- SWAP CHAR
void swap_char(char* a, char* b);
// ----------------------------------- STRING LENGTH
int str_len(const char* s);
// ----------------------------------- RETURNS WHETHER i IS A VALID INDEX
bool is_valid_index(int len, int i);
// ----------------------------------- RETURNS WHETHER i IS A VALID RELATIVE INDEX 
bool is_valid_relative_index(int len, int i);
// ----------------------------------- RETURNS WHETHER i IS A VALID CURSOR POS FOR TEXT
bool is_valid_cursor_pos(int len, int i);
// ----------------------------------- RETURNS WHETHER i IS A VALID RELATIVE CURSOR POS FOR TEXT
bool is_valid_relative_cursor_pos(int len, int i);
// ----------------------------------- VALIDATE INDEX (return correct proximate)
int validate_index(int len, int i);
// ----------------------------------- VALIDATE CURSOR (return correct proximate)
int validate_cursor(int len, int i);
// ----------------------------------- VALIDATE INDEX IN RANGE (return correct proximate)
int validate_index_in_range(int min,int max, int i);
// ----------------------------------- COPY STRING
void copy_str( const char* source_str,int source_pos,int n, char* dest_str,int dest_pos);
// ----------------------------------- COPY STD::STRING
void copy_str( std::string source_str,int source_pos,int n, char* dest_str,int dest_pos);
// ----------------------------------- ALLOCATE COPY STRING
char* alloc_copy_str( const char* source_str,int source_pos,int n);
// ----------------------------------- GET ABSOLUTE INDEX POSITION
int get_absolute_index_pos(int len, int pos);
// ----------------------------------- GET ABSOLUTE CURSOR POSITION
int get_absolute_cursor_pos(int len, int pos);
// ----------------------------------- GET ABSOLUTE RANGE POSITIONS
void get_absolute_range_pos(int len, int* pos, int* n);
// ----------------------------------- COMPARE STRINGS
bool are_strings_equal(char* s1, const char* s2);
// ----------------------------------- COMPARE STRINGS
bool are_strings_equal(const char* s1, const char* s2);
// ----------------------------------- COMPARE STRINGS
bool are_strings_equal(const char* s1, std::string s2);
// ----------------------------------- STRING 1 CONTAINS STRING 2
bool str_contains(const char* s1, const char* s2);
// ----------------------------------- STRING 1 CONTAINS STRING 2
bool str_contains(const char* s1, std::string s2);
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
int validate_rel_cursor_range(int len, int* i, int* n);
bool hex_to_val(char d0, char d1, char *v);

#endif // STRING_FUNCTIONS

