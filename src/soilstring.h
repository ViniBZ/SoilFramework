#ifndef SOILSTRING_H
#define SOILSTRING_H

#include "soilincstring.h"
#include "soilobject.h"

class SoilTextRecord;

class SoilString : public SoilObject
{
    friend class SoilText;
    friend class SoilTextRecord;
public:

    SoilString();
    SoilString(const char * str);
    SoilString(const std::string& str);
    SoilString(const SoilString& str);
    SoilString(SOIL_CHAR sch);
    SoilString(unsigned char uch);
    SoilString(char ch);
    SoilString(int num);
    ~SoilString();

private:
    int change_count;
    bool REC_ACTIVE;
    SoilTextRecord* REC;
    //if true it calls the soilobject_receiver[1] to set the undo_rec
    STR_REC_NODE last_rec;

    //multi_trans_tmp_str is to be used to code tha changes of search_replace_str()
    //for SoilTextRecord in case it is more efficient than just have undo replace all
    //from the first repalced occurrence to the last
    SoilIncString* multi_trans_inc_str;

    //if when adding or deleting series of same kind(space / non-space) char use one rec_action for the series instead of one per char
    //where each char is a SoilString action(insert / remove)

    //if update_cursor is TRUE it updates last_rec every time the string changes
    bool update_cursor;
    
    bool rec_action;//for when the action is being called by REC as to not create another undo rec

    SoilInfraString text[2];
    int text_i; //index for where the actual text is
    int alt_text_i; //used as auxiliary data holder

    int length;//char count
    int byte_n;//byte count

    void init();
    void process_utf8(bool from_file, FILE* file, const char * orig_str, bool line_limit, bool limit_orig, int orig_s, int orig_e, int * char_count, int * byte_count, SoilInfraString* dest_str, int dest_pos, bool do_copy);

    void utf8_str_len(const char * str, int orig_s, int orig_e, int* CN, int* BN);
    void utf8_str_len(const char * str, int* CN, int* BN);
    void utf8_str_line_len(const char * str, int* CN, int* BN);
    void utf8_file_len(FILE* file, int orig_s, int orig_e, int* CN, int* BN);
    void utf8_file_len(FILE* file, int* CN, int* BN);
    void utf8_file_line_len(FILE* file, int* CN, int* BN);

    void update_byte_n();

    int get_section_byte_n(int pos, int n);
    void copy_infra_to_infra(const SoilInfraString& orig, LINEAR_RANGE orig_range, SoilInfraString* dest,int dest_pos)const;
    void copy_infra_to_infra(const SoilInfraString* orig, LINEAR_RANGE orig_range, SoilInfraString* dest,int dest_pos);
    void copy_infra_to_infra(SoilInfraString* orig, LINEAR_RANGE orig_range, SoilInfraString* dest,int dest_pos);
    void copy_str_to_infra(const char * str, int s, int e, SoilInfraString* dest, int dest_pos);
    void copy_str_to_infra(const char * str, SoilInfraString* dest, int dest_pos);
    void copy_str_line_to_infra(const char * str, SoilInfraString* dest, int dest_pos);
    void copy_ascii_str_to_infra(const char * str, LINEAR_RANGE orig_range, SoilInfraString* dest, int dest_pos);
    void copy_infra_to_str(const SoilInfraString& str, LINEAR_RANGE orig_range, char* dest)const;
    void copy_infra_to_str(SoilInfraString* str, LINEAR_RANGE orig_range, char* dest);
    void copy_infra_to_str(const SoilInfraString& str, int str_len, char* dest)const;
    void copy_infra_to_str(SoilInfraString* str, int str_len, char* dest);
    void copy_inc_str_to_infra(SoilIncString * inc_str, int s, int e, SoilInfraString* dest, int dest_pos);
    void copy_inc_str_to_infra(SoilIncString * inc_str, SoilInfraString* dest, int dest_pos);
    void copy_infra_to_inc_str(const SoilInfraString& str, int s, int e, SoilIncString * dest);
    void copy_file_to_infra(FILE* orig_file, int s, int e, SoilInfraString* dest, int dest_pos);
    void copy_file_to_infra(FILE* orig_file, SoilInfraString* dest, int dest_pos);
    void copy_file_line_to_infra(FILE* orig_file, SoilInfraString* dest, int dest_pos);

    int load_from_file(FILE* fo, int n, bool line_limit);
    void prepare_text_str(int new_length, int pos, bool release_alt_text);
    void insert_soil_char(SOIL_CHAR ch,int pos);
    void remove_soil_char(int pos);
    void fill_section(SOIL_CHAR sch, int pos, int n);
    void fill_section(unsigned char uch, int pos, int n);
    void fill_section(char ch, int pos, int n);
    void section_itself(int pos, int n);//reduce to a section of itself
    void empty_itself();

    int str_parts(int str_len, int pos, int n, LINEAR_RANGE* str_p_1, LINEAR_RANGE* str_p_2, int* fill);
    //This (replace) is the base for most functions that alter the string
    int replace(int orig_type, const SoilString * soil_str, SoilIncString * soil_inc_str, const char * str, int pos, int n);
    //rec_op methods are for undo and redo
    int rec_op_double_insertion(STR_REC_NODE node, const SoilString& rec_str);//undo section_itself()
    int simplify(bool rec_redo_action, SoilIncString* rec_inc_str);
    int rec_op_simplify_undo(STR_REC_NODE node, const SoilString& rec_str);
    int rec_op_search_replace_str(STR_REC_NODE node, const SoilString& rec_str, SoilString* new_rec_str);
    int section_set_case(int pos, int n, SoilIncString* rec_inc_str, bool to_lower);
    int rec_op_section_set_case_undo(STR_REC_NODE node, const SoilString& rec_str);

    //used for marking the string without changing characters
    void alternate_text_i();
    int get_alloc_len();

    //This String Class doesn't realloc memory for every change in size
    //It allocs when its size passes certain limits, the bigger this arg
    //the larger the gap between those limits, that is better described in
    //SoilMemBounds
    void set_alloc_unit(Uint32 alloc_unit);
    void set_last_rec_node(int type, int str_pos, int ins_len, int rem_len);
    void set_last_rec_text_fields_before(bool selected_before, int cursor_1_before, int cursor_2_before);
    void set_last_rec_text_fields_after(bool selected_after, int cursor_1_after, int cursor_2_after);
    void set_update_cursor(bool update);

    void str_changed();
    void rec_callback(int arg);
public:
    ///////////////////////////////// OPERATORS
    //int to_backward_pos(int pos);
    long int get_total_size();
    int get_length()const;
    int get_byte_n()const;
    STR_REC_NODE get_last_rec();

    //MARKER: if n is negative, should it load backwards?
    int load_from_file(FILE* fo, int n);
    int load_from_file(FILE* fo);
    int load_from_file_line(FILE *fo);
    int load_from_file(const char* file_name);
    int load_from_file(const std::string& file_name);
    int load_from_file(const SoilString& file_name);
    
    //return false if pos in not a valid relative index or 
    //if the line has 0 chars, in which case it doesn't set this string
    int load_from_str_line(const SoilString& str, int pos); 
    int prepend_from_file(FILE *fo, int file_s, int file_e);

    bool save_section_to_file(FILE* fd, int pos, int n);
    bool save_section_to_file(const char* file_name, int pos, int n);
    bool save_section_to_file(const std::string& file_name, int pos, int n);
    bool save_section_to_file(const SoilString& file_name, int pos, int n);
    
    bool save_to_file(FILE *fd);
    bool save_to_file(const char* file_name);
    bool save_to_file(const std::string& file_name);
    bool save_to_file(const SoilString& file_name);

    SOIL_CHAR at(int ind)const;
    unsigned char ext_ascii_char_at(int ind);

    //doesn't change size, just change the char at ind
    bool set_ch_at(int ind, SOIL_CHAR sch);
    bool set_ch_at(int ind, char ch);

    int operator= (const char* str);
    int operator= (const std::string& str);
    int operator= (const SoilString& str);
    int operator= (SOIL_CHAR sch);
    int operator= (unsigned char uch);
    int operator= (char ch);
    int operator= (int num);

    bool is_empty();

    int compare(const SoilString& str, bool match_case, bool match_accent);
    
    bool is_equal(const SoilString& str, bool match_case, bool match_accent)const;
    bool is_equal(const SoilString& str);
    bool is_equal(const char* str, bool match_case, bool match_accent);
    bool is_equal(const char* str);
    bool is_equal(const std::string& str, bool match_case, bool match_accent);
    bool is_equal(const std::string& str);
    bool is_equal(SOIL_CHAR sch, bool match_case, bool match_accent);
    bool is_equal(SOIL_CHAR sch);
    bool is_equal(unsigned char uch, bool match_case, bool match_accent);
    bool is_equal(unsigned char uch);
    bool is_equal(char ch, bool match_case, bool match_accent);
    bool is_equal(char ch);

    bool operator== (const char* str);
    bool operator== (const std::string& str);
    bool operator== (const SoilString& str);
    bool operator== (SOIL_CHAR sch);
    bool operator== (unsigned char uch);
    bool operator== (char ch);

    bool operator!= (const char* str);
    bool operator!= (const std::string& str);
    bool operator!= (const SoilString& str)const;
    bool operator!= (SOIL_CHAR sch);
    bool operator!= (unsigned char uch);
    bool operator!= (char ch);

    //descent dir/file name, just letters, digts, underline and one dot
    //has nothing to do with checking if exist or what not
    //this is very arbitrary
    //uses SoilString::soil_char_is_valid_for_path()
    bool is_valid_for_path();

    //same as above but doesn't accept '/'
    bool is_valid_for_path_entry();

    char * c_str_alloc()const;
    char * section_c_str_alloc(int pos, int n);
    int replace(const char* str, int pos, int n);
    int replace(const std::string& str, int pos, int n);
    int replace(const SoilString& str, int pos, int n);
    int replace(SoilIncString* str, int pos, int n);
    int replace(SOIL_CHAR sch, int pos, int n);
    int replace(unsigned char uch, int pos, int n);
    int replace(char ch, int pos, int n);

    int replace(signed long long num, int base, int min_dig_n, int max_dig_n, int pos, int n);
    int replace(int num, int base, int min_dig_n, int max_dig_n, int pos, int n);
    int replace(signed long long num, int base, int pos, int n);
    int replace(int num, int base, int pos, int n);
    int replace(signed long long num, int pos, int n);
    int replace(int num, int pos, int n);
    //insert methods
    //if pos is beyond the size of the string, 
    //it extends the string filled with spaces(32) to reach pos
    int insert(const char* str, int pos);
    int insert(const std::string& str, int pos);
    int insert(const SoilString& str, int pos);
    int insert(SoilIncString* str, int pos);
    int insert(SOIL_CHAR ch, int pos);
    int insert(unsigned char uch, int pos);
    int insert(char ch, int pos);

    int insert(signed long long num, int base, int min_dig_n, int max_dig_n, int pos);
    int insert(int num, int base, int min_dig_n, int max_dig_n, int pos);
    int insert(signed long long num, int base, int pos);
    int insert(int num, int base, int pos);
    int insert(signed long long num, int pos);
    int insert(int num, int pos);

    int insert_ch_in_range(SOIL_CHAR ch, int pos);
    //int insert(double num, int )

    //MARKER: make the arg order standard, first pos then what is being inserted?
    //unlike insert() insert_space() / replace_with_space() dont add spaces to reach pos
    //if pos outside range it does nothing
    bool insert_space(int pos, int space_n);
    bool replace_with_space(int pos, int n, int space_n);
    int duplicate_line(int pos);

    int remove(int pos, int n);

    int append(const char* str);
    int append(const std::string& str);
    int append(const SoilString& str);
    int append(SoilIncString* str);
    int append(SOIL_CHAR ch);
    int append(unsigned char uch);
    int append(char ch);

    int append(signed long long num, int base, int min_dig_n, int max_dig_n);
    int append(int num, int base, int min_dig_n, int max_dig_n);
    int append(signed long long num, int base);
    int append(int num, int base);
    int append(signed long long num);
    int append(int num);

    int prepend(const char* str);
    int prepend(const std::string& str);
    int prepend(const SoilString& str);
    int prepend(SoilIncString* str);
    int prepend(SOIL_CHAR ch);
    int prepend(unsigned char uch);
    int prepend(char ch);

    int prepend(signed long long num, int base, int min_dig_n, int max_dig_n);
    int prepend(int num, int base, int min_dig_n, int max_dig_n);
    int prepend(signed long long num, int base);
    int prepend(int num, int base);
    int prepend(signed long long num);
    int prepend(int num);

    int next_char_pos(SOIL_CHAR ch, int start_pos, int dir);
    int next_char_pos(unsigned char uch, int start_pos, int dir);
    int next_char_pos(char ch, int start_pos, int dir);
    void word_bounds(int i, int* s, int* e, bool include_trailing_sp);
    void word_bounds(int i, int* s, int* e);
    int next_word_bound(int i, int dir);

    //removes spaces from the beginning and end and
    //reduces multiple consecutive spaces for one space(32)
    void simplify();
    //removes spaces from the beginning and end and
    void trim();
    
    SoilString section(int pos, int n) const;
    SoilString section(const SoilString& sep, int start, int end, bool match_sep_case, bool match_sep_accent, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const;
    SoilString section(const SoilString& sep, int start, int end, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const;
    SoilString section(const SoilString& sep, int start, int end) const;

    SoilString section(const char* sep, int start, int end, bool match_sep_case, bool match_sep_accent, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const;
    SoilString section(const char* sep, int start, int end, bool INCLUDE_START_SEP,bool INCLUDE_END_SEP) const;
    SoilString section(const char* sep, int start, int end) const;

    int search_str(const SoilString& str, int pos, int n, bool match_case, bool match_accent) const;//return sub_str pos
    int search_str(const SoilString& str, int pos, bool match_case, bool match_accent) const;//return sub_str pos
    int search_str(const SoilString& str, int pos) const;//return sub_str pos
    int search_str(const char* str, int pos, int n, bool match_case, bool match_accent) const;//return sub_str pos
    int search_str(const char* str, int pos, bool match_case, bool match_accent) const;//return sub_str pos
    int search_str(const char* str, int pos) const;//return sub_str pos
    int search_replace_str(const SoilString& str, const SoilString& rep, int pos, int n, bool match_case, bool match_accent);//return number of occurrences
    int search_replace_str(const SoilString& str, const SoilString& rep, int pos, bool match_case, bool match_accent);//return number of occurrences
    int search_replace_str(const char* str, const char* rep, int pos, bool match_case, bool match_accent);//return number of occurrences
    int search_replace_str(const SoilString& str, const char* rep, int pos, bool match_case, bool match_accent);//return number of occurrences
    int search_replace_str(const char* str, const SoilString& rep, int pos, bool match_case, bool match_accent);//return number of occurrences

    int count_sub_str(const SoilString& str, int pos, int n, bool match_case, bool match_accent);
    int count_sub_str(const char* str, int pos, int n, bool match_case, bool match_accent);
    int count_sub_str(const SoilString& str, bool match_case, bool match_accent);
    int count_sub_str(const char* str, bool match_case, bool match_accent);

    int section_invert_case(int pos, int n);
    int section_set_case(int pos, int n, bool to_lower);
    int section_to_lower_case(int pos, int n);
    int section_to_upper_case(int pos, int n);

    int replace_tabs_with_spaces(int space_n);
    bool indent_section(int pos, int n, int space_n, bool reverse, bool regular, int* sp_before_pos_n, int* sp_middle_n, int* sp_last_line_n);
    int replace_with_regular_tab(int pos, int n, int space_n);
    int regular_tab_input(int pos, int space_n);
    void make_number_sequence(int min, int max, bool with_separator, char sep);

    void printf_str()const;
    void printf_str(int br_before, int br_after)const;

    void print_data()const;
    void print_last_rec();
    void print_log(int st);  

    ///////////////////////////////// STATIC METHODS
    static bool utf8_loaded;
    static unsigned char utf8_same_case[256];
    static unsigned char utf8_same_accent[256];
    static unsigned char utf8_same_case_accent[256];
    static unsigned char utf8_case[256];
    static unsigned char utf8_opposite_case[256];
    static void fill_utf8_case_arrays()
    {
        if(!utf8_loaded)
        {
            utf8_loaded = true;
            //these are arrays that go as far as the extended ascii set
            //in utf8_case.h the chars that are the same with different case have the same value
            #include "utf8_same_case.h"
            //in utf8_accent.h the chars that are the same with different accentuation signs have the same value
            #include "utf8_same_accent.h"
            //in utf8_case_accent.h the chars that are the same with different case or accentuation sign have the same value
            #include "utf8_same_case_accent.h"
            //in utf8_... 0 means no case, 1 is lower case, 2 is upper case
            #include "utf8_case.h"
            //in utf8_opposite case each char entry in the array is either 0 if it has no case variation or the index of the opposite case char
            #include "utf8_opposite_case.h"
        }
    }
    static SoilString from_c_str(const char* str);

    static SoilString from_number(signed long long num, int base, int min_dig_n, int max_dig_n);
    static SoilString from_number(int num, int base, int min_dig_n, int max_dig_n);
    static SoilString from_number(signed long long num, int base);
    static SoilString from_number(int num, int base);
    static SoilString from_number(signed long long num);
    static SoilString from_number(int num);

    static int count_utf8_control_bits(unsigned char uch);
    static SOIL_CHAR init_soil_char(unsigned char n, unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
    static SOIL_CHAR ext_ascii_to_soil_char(int ind);
    static SOIL_CHAR utf8_c_str_to_soil_char(const char* str);
    static SOIL_CHAR c_char_to_soil_char(unsigned char ch);
    static SOIL_CHAR c_char_to_soil_char(char ch);
    static int soil_char_to_ext_ascii(SOIL_CHAR sch);
    static bool is_lower_case(SOIL_CHAR sch);
    static bool is_upper_case(SOIL_CHAR sch);
    static bool is_letter(SOIL_CHAR sch);
    static bool is_digit(SOIL_CHAR sch);
    static SOIL_CHAR invert_case_ch(SOIL_CHAR sch);
    static SOIL_CHAR to_lower_ch(SOIL_CHAR sch);
    static SOIL_CHAR to_upper_ch(SOIL_CHAR sch);
    static void code_int_in_2_soil_char(int num, SOIL_CHAR* sch_1, SOIL_CHAR* sch_2);//sch_1 is the big end
    static int coded_int_from_2_soil_char(SOIL_CHAR sch_1, SOIL_CHAR sch_2);//sch_1 is the big end
    static SOIL_CHAR code_int_in_soil_char(int num);//doesn't convert to text. uses little endian
    static int coded_int_from_soil_char(SOIL_CHAR sch);
    static int compare_soil_char(SOIL_CHAR sch_1, SOIL_CHAR sch_2, bool match_case, bool match_accent);
    static int compare_soil_char(SOIL_CHAR sch_1, SOIL_CHAR sch_2);
    static bool soil_char_are_equal(SOIL_CHAR sch_1, SOIL_CHAR sch_2, bool match_case, bool match_accent);
    static bool soil_char_are_equal(SOIL_CHAR sch_1, SOIL_CHAR sch_2);
    static bool soil_char_is_space(SOIL_CHAR sch, bool br_is_space);
    static bool soil_char_is_alphanum(SOIL_CHAR sch);
    static bool soil_char_is_alphanum_or_(SOIL_CHAR sch);
    static bool soil_char_is_valid_for_path(SOIL_CHAR sch);
    static bool soil_char_is_valid_for_path_entry(SOIL_CHAR sch);
};

#endif // SOILSTRING_H
