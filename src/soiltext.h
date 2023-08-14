#ifndef SOILTEXT_H
#define SOILTEXT_H

#include "soilstring.h"
#include "soilfont.h"

#include "soiltextrecord.h"

//The principle behind this class is to be a text structure
//this won't deal with layout business, such as AUTO_SIZE...
//neither will it have properties such as SELECTABLE or EDITABLE
//that is all up to the widgets that will use this

//It will have MULTILINE option, SELECTION markers, CURSOR marker

class SoilText : public SoilObject
{
friend class SoilTextWidget;
private:
    //x,y,w,br for every char in soil_string

    int draw_i;

    CHAR_SPAN* ch_span;
    int ch_span_alloc_len;

    SoilMemBound ch_span_mem_bound;
    bool signal_change;
    bool update_from_soil_string;

    LINE_INDEX_NODE* line_index;
    int line_index_alloc_len;
    SoilMemBound line_index_mem_bound;

    SoilString soil_string;//
    int CURSOR;

    bool cursor_changed;
    bool text_changed;
    bool text_sel_changed;

    bool PWD_MODE;

    //for the cursor x when moving up and down
    int kept_x;

    int line_n;

    int TAB_W;

    int prev_drawn_CURSOR;

    bool MULTI_LINE;
    //if AUTOBREAK is on it will be the width of the text view
    int MAX_LINE_W;

    //marks selection in text
    int SEL_S;
    int SEL_E;
    bool text_selected;
    bool cursor_at_sel_s;

    bool print_in_cursor;

    //text w/h in char_size, not pixels, nor char number
    SIZE text_ch_size;

    void alloc_line_index(int n);//
    void alloc_ch_span(int n);//
    void set_str_text_fields_before();
    void process_text();//
    void process_single_line_text();//
    void process_multi_line_text();//
    void print_text_log();//
    void internal_set_cursor(int c);
    void internal_select_text(int sel_a, int sel_b);
    int this_changed();

public:
    int get_length();

    SoilText();
    ~SoilText();
    void set_pwd_mode(bool m);
    void set_multi_line(bool m);
    void set_tab_width(int w);
    void set_max_line_w(int w);
    //void print_text_log();

    //correct cursor position and selection if soil_string is changed directly
    SOIL_RECT get_full_rect(SIZE char_size);
    SoilString* get_soil_string_pt();
    bool is_multi_line();
    //mouse coord is in pixels, that's why it needs to provide char size
    //text coord is given in character amounts
    POINT text_coord_from_mouse(int x, int y, SIZE char_size);
    bool cursor_pos_from_coord(int x, int y, int * char_i);
    bool cursor_pos_from_mouse(int x, int y, SIZE char_size, int * char_i);

    //false if char_i is outside text bounds
    bool coord_from_cursor_pos(int char_i, POINT* coord);
    SOIL_RECT soil_rect_from_cursor_pos(int char_i, SIZE char_size);

    int get_line_n();
    int get_cursor_pos();
    int cursor_line_indent(int c);
    int cursor_line_start(int c);
    int cursor_line_end(int c);
    int cursor_above(int c, int n, bool use_kept_x);//returns the pos above c
    int cursor_below(int c, int n, bool use_kept_x);//returns the pos below c
    int cursor_above(int c, bool use_kept_x);//returns the pos above c
    int cursor_below(int c, bool use_kept_x);//returns the pos below c

    virtual int soilobject_callback(int arg);
    //--------------------------------------- OPERATORS
    void set_cursor_from_last_rec();
    void set_cursor(int c, bool select);
    void set_cursor(int c);
    void inc_cursor(int inc);
    void move_cursor(int dir, bool select);
    void set_cursor_next_word(int dir, bool select);

    void set_text(const char* str);
    void set_text(const SoilString& str);
    void duplicate_line();
    void remove_word();
    void remove_line();
    //void tab(bool always_indent, bool normal_indent, bool regular_tab);
    bool valid_for_tab_selection_indent();
    bool add_indent(bool regular);
    bool remove_indent(bool regular);
    //simply tab to the next multiple of TAB_W
    bool regular_tab_input();
    //delete spaces in indentation to the next multiple of TAB_W
    int regular_indent_backspace_del(bool backspace);

    int search_str(const SoilString& str, int pos, int n, bool match_case, bool match_accent);
    int search_replace_str(const SoilString& str, const SoilString& rep, int pos, int n, bool match_case, bool match_accent);//return number of occurrences

    void external_set_kept_x(int x);

    //return true if selection changed
    bool unselect_text();
    bool select_text(int sel_a, int sel_b);
    bool select_text(POINT mouse_a, POINT mouse_b, SIZE char_size);//from mouse coord
    bool select_text(int ax,int ay,int bx,int by, SIZE char_size);//from mouse coord
    
    bool select_word(int cursor_i, bool include_trailing_sp);
    bool select_word(int cursor_i);
    bool select_word(POINT mouse_coord, SIZE char_size, bool include_trailing_sp);//from mouse coord
    bool select_word(POINT mouse_coord, SIZE char_size);//from mouse coord
    bool select_word(int mouse_coord_x,int mouse_coord_y, SIZE char_size, bool include_trailing_sp);//from mouse coord
    bool select_word(int mouse_coord_x,int mouse_coord_y, SIZE char_size);//from mouse coord

    bool select_line(int line_i);
    bool select_line(POINT mouse_coord,SIZE char_size);
    bool select_all();
    int remove_selection();
    SoilString get_selection_soil_str();
    char* get_selection_c_str_alloc();
    //select is true when shift is pressed
    void process_cursor_motion(int new_cursor, int new_cursor_sel, bool select);
    bool is_selected();
    bool cursor_is_after_indent();
    bool cursor_is_at_line_start();

    SIZE get_text_px_size(SIZE char_size);
    SIZE get_text_px_size(int font_size);
    bool draw_text_rect_px(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, SOIL_RECT dest_rect, SoilFont* font, int font_size);
    //void draw_rect_px(SoilPixmap* dest_px, SDL_Rect orig_rect, SDL_Rect dest_rect, SoilFont* font, int font_size);//dont delete this
    bool draw_full_px(SoilPixmap* dest_pixmap, POINT text_dest_offset, SoilFont* font, int font_size);
    bool draw_full_px(SoilPixmap* dest_pixmap, SoilFont* font, int font_size);
    void draw_cursor(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, POINT text_dest_offset, SoilFont* font, int font_size, bool visible);//this calls set_cursor_img
    void draw_cursor(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, SoilFont* font, int font_size, bool visible);//this calls set_cursor_img
    void set_cursor_img(SOIL_RECT orig_rect, SoilPixmap* dest_pixmap, POINT text_dest_offset, SoilFont* font, int font_size, int cursor_pos, bool visible);
    //void draw_cursor(SoilPixmap* dest_px, SDL_Rect orig_rect, SDL_Rect dest_rect, SoilFont* font, int font_size,bool prev,bool visible);
    //--------------------------------------- EVENTS
    //These are not called by the SoilEngine/SoilLayout, this is called
    //by the SoilTextWidget that owns this SoilText
    //return true if the GUI needs update




};

#endif // SOILTEXT_H
