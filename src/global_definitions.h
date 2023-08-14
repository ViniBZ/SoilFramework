//abcdefh
//  global_definitions.h
//  Soil Framework
// EDITED 2023_08_14
//  Created by Vinicius Baez on 16/11/2020.
//changed2


#ifndef global_definitions_h
#define global_definitions_h
#include <sys/time.h>

#include <algorithm>
#include <cstring>
#include <string>


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define DEFAULT_BKG_COLOR   0xFF000000
#define DEFAULT_CONTRAST_COLOR   0xFFFFFFFF
#define DEFAULT_DARK_LINE_COLOR  0xFF505050
#define DEFAULT_LIGHT_LINE_COLOR 0xFFB0B0B0
#define DEFAULT_DARK_ACTIVATION_1_COLOR   0xFF20459A
#define DEFAULT_LIGHT_ACTIVATION_1_COLOR   0xFF3065FF
#define DEFAULT_DARK_ACTIVATION_2_COLOR   0xFFA0206A
#define DEFAULT_LIGHT_ACTIVATION_2_COLOR   0xFFE2096
#define DEFAULT_DARK_SELECTION_COLOR   0xFFAC5710
#define DEFAULT_LIGHT_SELECTION_COLOR   0xFFD57610

#define PI 3.141592

//Usually the order for the bitmaps I've been using is BGRA but in SDL2 is ARGB

enum UI_TYPE{LAYOUT,WIDGET};
enum WIDGET_TYPE{LABEL,INPUT,TEXTBLOCK,TEXTFIELD,TEXTRULER,IMAGE,BUTTON,COLORBUTTON,DROPMENU,LIST,CANVAS,GRAPH,SLIDER,PROGRESSBAR,VIEW,RECT_VIS,NO_WIDGET};
enum LAYOUT_TYPE{VERTICAL,HORIZONTAL};
enum ALIGNMENT{CENTER,TOP,BOTTOM,LEFT,RIGHT};
enum LAYOUT_NEST_PROPERTY{ALL_PROPERTIES=0xFF,CONTROL=0x01,FONT=0x02,DEST_PIXMAP=0x04};

//typedef enum ENGINE_FLAG_TYPE{NO_FLAG=0x00,UI_CHANGED=0x01,UI_SIZE_CHANGED=0x02}ENGINE_FLAG_TYPE;

#define SOIL_SIGNAL_N 26
enum SOIL_SIGNAL{SIZE_CHANGED,FOCUS_CHANGED,DRAWN,SCROLL_H_SET,
                        SCROLL_V_SET,CONTENT_VIEW_MOTION,MOUSE_DOWN,
                        MOUSE_UP,MOUSE_WHEEL,MOUSE_MOTION,MOUSE_ENTER,
                        MOUSE_LEAVE,MOUSE_CLICK,KEY_DOWN,KEY_UP,TEXT_INPUT,
                        CHECK,TEXT_CHANGED,TEXT_SEL_CHANGED,TEXT_CURSOR_CHANGED,FONT_SIZE_CHANGED,VALUE_CHANGED,RANGE_CHANGED,
                        MODWIN_RESPONSE,SUBMIT,CUSTOM
                        };



#define MAIN_FONT_PX_W 6876
#define MAIN_FONT_PX_H 160
#define FONT_CHAR_W 36
#define FONT_CHAR_H 80
//amount of characters in the font px
#define FONT_LEN 191
//number of possible font sizes. 0 is the smallest, FONT_SIZE_N-1 is the biggest
//the biggest font will be FONT_CHAR_W by FONT_CHAR_H in size
//each font size will be 0.9*w and 0.9*h of the next bigger font size
//the font sizes can be calculated using a function in pixmap_functions.h
#define FONT_SIZE_N 18


#define CYCLE_LEN 5
//CURSOR_INTERVAL would be the time for EACH state, this much time with cursor visible and the same time with the cursor not visible
#define CURSOR_INTERVAL 400

//Neither SOIL_RECT or BOUND_RECT can negative values for any property
//SOIL_RECT.w / h would need to be positive
//BOUND_RECT.end_x / y would need to be bigger than start_x / y
//for that there is set_rect_abs() and set_bound_abs()

template<typename T>
struct LIST_NODE{
    T item;
    LIST_NODE* prev;
    LIST_NODE* next;
};

struct SOIL_RECT{
    int x;
    int y;
    int w;
    int h;
};

//for pixmap operations x = start_x;while(x < end_x)
//so in a 5x5 pixel square totally affected start would be 0,0 and end 6,6
struct BOUND_RECT
{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
};


struct SOIL_CHAR{
    //this is for UTF8 encoding
    unsigned char N;//number of bytes in the char
    unsigned char B[4];

    bool operator==(SOIL_CHAR other_ch)
    {
        if(N != other_ch.N){return false;}
        int i = 0;
        while(i < N)
        {
            if(B[i] != other_ch.B[i]){return false;}
            i++;
        }

        return true;
    }

    bool operator!=(SOIL_CHAR other_ch)
    {
        if(N != other_ch.N){return true;}
        int i = 0;
        while(i < N)
        {
            if(B[i] != other_ch.B[i]){return true;}
            i++;
        }

        return false;
    }

};

struct INC_STR_NODE{
    SOIL_CHAR* data;
    int len;//amount of SOIL_CHAR to be used from *data
    INC_STR_NODE* next;

    INC_STR_NODE()
    {
        data = NULL;
        len = 0;
        next = NULL;
    }
};


//class Widget;
struct PX_2x4b{
    unsigned char px_0 : 4;
    unsigned char px_1 : 4;
};

//this struct is originally meant to keep absolute values
//this way start, end and n(length) should be positive values
struct LINEAR_RANGE{
    int s;//start
    int e;//end
};


struct CALC_PIX_COLOR
{
    double b;
    double g;
    double r;
    double a;

    int n;
    bool flag;
    
};

struct PIXCOLOR
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};
union PIXDATA
{
    Uint32 pix;
    PIXCOLOR color;
};

struct PIXDATA_FLAG
{
    PIXDATA pixdata;
    bool flag;
};

typedef SOIL_RECT RECT;

struct SIZE_POLICY
{
    bool w;
    bool h;
};

struct SIZE
{
    int w;
    int h;

    bool operator==(SIZE other_size)
    {
        if(w != other_size.w){return false;}
        if(h != other_size.h){return false;}
        return true;
    }
    bool operator!=(SIZE other_size)
    {
        if(w != other_size.w){return true;}
        if(h != other_size.h){return true;}
        return false;
    }

};

struct RANGE
{
    int min;
    int max;

    bool operator==(RANGE other_range)
    {
        if(min != other_range.min){return false;}
        if(max != other_range.max){return false;}
        return true;
    }
    bool operator!=(RANGE other_range)
    {
        if(min != other_range.min){return true;}
        if(max != other_range.max){return true;}
        return false;
    }
};

struct D_SIZE
{
    double w;
    double h;
};

struct POINT
{
    int x;
    int y;
};

struct D_POINT
{
    double x;
    double y;
};

struct SQUARE_COORD
{
    D_POINT corner[4];
    
};


struct DRAW_POINTS
{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    int x;
    int y;
};

struct DRAW_D_POINTS
{
    double start_x;
    double start_y;
    double end_x;
    double end_y;
    double x;
    double y;
};

struct SCROLL_DATA{
    bool vertical;
    bool horizontal;
    int vertical_bar_len;
    int vertical_bar_pos;
    int horizontal_bar_len;
    int horizontal_bar_pos;
};

struct UI_SIZES{
    SIZE size;
    SIZE min_size;
    SIZE max_size;
};

class SoilUIElm;
class SoilFont;
class SoilControl;
class SoilPixmap;

//this flags are added to the SoilEngineControl calling process_flag in
//the SoilUIElm, it s for internal use only
class SoilObject;
struct OBJ_FLAG{
    bool in_control;//in control list
    SoilObject* obj;
    int value;
    int interval;
    OBJ_FLAG* prev;
    OBJ_FLAG* next;


};

//I have decided to have 2 kinds of SIGNALS, the ones connected directly
//to SDL_Event events and the SOIL ones
//for example, window events will be triggered straigth from SDL_WindowEvent
//and will have a SDL_Event as arguments
//
//a SoilUIElm sending a size_changed signal on the other hand has no
//counterpart in SDL events system, because the SoilUIElm doesn't exist
//in the SDL paradigm. So it will use Soil's internal SIGNAL system.
//The reason for that is that I would have to create structs and
//arguments for every kind of event SDL can possibly handle to transpose them
//into the Soil SIGNAL system. That makes no sense so unless the event is
//dependent on the individuality of SoilUI system they will be just handled
//as SDL_Events.

struct SDL_EVENT_SIGNAL{
    bool active;
    int response;
    int delay;
};

//SOIL SIGNAL ARGUMENTS
//to be used in a union and have the values set according to
//the case, ex: if it is a SOIL_SIGNAL for a Soil mouse event
//it will fill the MOUSE_ARG field


struct SIZE_CHANGED_ARG{
    SIZE size;
};

struct TEXT_SEL_CHANGED_ARG{
    bool selected;
    int sel_s;
    int sel_e;
};

struct TEXT_CURSOR_CHANGED_ARG{
    int new_cursor;
};

struct FOCUS_CHANGED_ARG{
    bool focused;
};

struct SCROLL_ARG{
    bool scroll_set;
    double scroll_ratio;
    int scroll_pos;
};

struct CONTENT_VIEW_ARG{
    POINT content_view_pos;

};

struct MOUSE_ARG{
    Uint8 button;
    POINT win_coord;
    POINT elm_coord;
};

struct VALUE_CHANGED_ARG{
    int value;
};

struct KEY_ARG{
    SDL_Keysym keysym;
};

struct BUTTON_CHECKED_ARG{
    bool checked;
};

struct RANGE_CHANGED_ARG{
    int new_range_min;
    int new_range_max;
};

//MARKER: might make it an union so different kinds of arguments can be passed
union SOIL_SIGNAL_ARGS{
    int gen_arg;
    SIZE_CHANGED_ARG size_changed_arg;
    TEXT_SEL_CHANGED_ARG text_sel_changed_arg;
    TEXT_CURSOR_CHANGED_ARG text_cursor_changed_arg;
    FOCUS_CHANGED_ARG focus_changed_arg;
    CONTENT_VIEW_ARG content_view_arg;
    SCROLL_ARG scroll_arg;
    MOUSE_ARG mouse_arg;
    VALUE_CHANGED_ARG value_changed_arg;
    KEY_ARG key_arg;
    BUTTON_CHECKED_ARG button_checked_arg;
    RANGE_CHANGED_ARG range_changed_arg;
};


//MARKER: maybe put a pointer to the SoilUIElm the SOIL_SIGNAL belongs
//to in the SOIL_SIGNAL struct

//this is for the signals the elements or window might send
//this signals are used to call the callback function in SoilControl
//this is for the user(developer) to use
//don't mistake this for the OBJ_FLAG above
struct SOIL_SIGNAL_STRUCT{
    SOIL_SIGNAL signal;
    SoilUIElm* elm;
    bool active;
    bool in_control;
    int response;
    SOIL_SIGNAL_ARGS arguments;

    SOIL_SIGNAL_STRUCT* prev;
    SOIL_SIGNAL_STRUCT* next;

    SOIL_SIGNAL_STRUCT()
    {
        signal = SIZE_CHANGED;
        active = false;
        in_control = false;
        response = 0;

        arguments.gen_arg = 0;
        arguments.size_changed_arg.size.w = 0;
        arguments.size_changed_arg.size.h = 0;
        arguments.focus_changed_arg.focused = false;
        arguments.scroll_arg.scroll_pos = 0;
        arguments.scroll_arg.scroll_ratio = 0;
        arguments.scroll_arg.scroll_set = false;
        arguments.mouse_arg.button = 0;
        arguments.mouse_arg.elm_coord.x = 0;
        arguments.mouse_arg.elm_coord.y = 0;
        arguments.mouse_arg.win_coord.x = 0;
        arguments.mouse_arg.win_coord.y = 0;
        //arguments.key_arg.keysym;
        arguments.value_changed_arg.value = (-1);
        arguments.button_checked_arg.checked = false;
        arguments.range_changed_arg.new_range_min = 0;
        arguments.range_changed_arg.new_range_max = 0;

        prev = NULL;
        next = NULL;
    }

};

//used for text UNDO and REDO actions
#define STR_REC_NODE_DATA_LEN 36
struct STR_REC_NODE{
    //type may be:
    //0:  text was added. UNDO will REMOVE
    //1:  one space ch was added. if prev was the same, UPDATE last rec
    //2:  one non-space ch was added. if prev was the same, UPDATE last rec
    //6:  two section added(at the beginning(0) and at the end(-1)) used for section_itself()
    //    UNDO will call section_itself()
    //    str_pos: section_itself() pos argument
    //    ins_len: section_itself() n argument
    //
    //10: text was removed. UNDO will INSERT
    //11: one space ch was removed left. if prev was the same UPDATE last rec
    //12: one non-space ch was removed left. if prev was the same UPDATE last rec
    //13: one space ch was removed right. if prev was the same UPDATE last rec
    //14: one non-space ch was removed right. if prev was the same UPDATE last rec
    //16: when section_itself() reduces the string to a section in its middle()
    //    ins_len: its total rec_str section length
    //    FORMAT: first_ins_len + ins_0 + second_ins_len + ins_1
    //
    //20: text was transformed. UNDO will REMOVE then INSERT
    //30: set ch
    //40: invert case
    //41: text was set to lower case
    //42: text was set to upper case
    //    41 and 42 work similarly to 70s
    //    ins_len: size of affected part ( n )
    //    REC STR FORMAT: total_rec_n...coord_0...sec_len_0...coord_1...sec_len_1...
    //    REC STR SIZE: 2 + (4*total_rec_n)
    //
    //70  text was transformed in multiple places(specially designed for search_replace_str())
    //    the coords of each transform will be coded in the rec_str[UNDO_I / REDO_I] SoilStrings
    //    It is called multi_transform
    //    match_case == true AND match_accent == true, which means the str to be replaced and the one
    //    to replace have no variation in each occurrence only needing to be stored once in undo_rec / redo_rec
    //    rec_pos: position in undo_rec / redo_rec
    //    str_pos: irrelevant
    //    ins_len: length of each individual section to be inserted
    //    rem_len: length of each individual section to be removed
    //    total_rec_n: number of sections to be transformed
    //    REC STR FORMAT: total_rec_n...ins_str...coord_0...coord_1...coord_2. . .
    //    REC STR SIZE: 2 + ins_len + (2 * total_rec_n)
    //71: either match_case == false OR match_accent == false 
    //    so it saves the replaced str section for every insertion to be made 
    //    rec_pos: position in undo_rec / redo_rec
    //    str_pos: irrelevant
    //    ins_len: length of each individual section to be inserted
    //    rem_len: length of each individual section to be removed
    //    total_rec_n: number of sections to be transformed
    //    REC STR FORMAT: total_rec_n...ins_str_0...coord_0...ins_str_1...coord_1. . .
    //    REC STR SIZE: 2 + (ins_len + 2) * total_rec_n
    //72: when UNDO is exec'ed it can create a node.type 70 if the original action was 70
    //    or 72 if the original created a node.type 71
    //    exec'ing REDO, if node.type is 70 it saves the replaced str only once
    //    if node.type is 72, it saves all replaced sections as search_replace_str does
    //    when match_case == false OR match_accent == false 
    //
    //    In summary, search_replace_str() makes either: node.type=70 OR node.type=71
    //    UNDO (rec_op_search_str()) makes either: node.type=70 OR node.type=72
    //    REDO (rec_op_search_str()) makes either: node.type=70 OR node.type=71
    //
    //80: space was removed in multiple places
    //    the 80's were specially made for simplify(), trim()
    //    when exec'ed it will insert the spaces
    //    REC STR FORMAT: total_rec_n...total_ins_len...coord_0..space_ins_len_0...coord_1...space_ins_len_1
    //    REC STR SIZE: 4 + (4*total_rec_n)
    //81: space was added in multiple places
    //    when exec'ed it will remove the spaces just like simplify
    //    
    //    each coded int in types 70, 71, 80 . . . is 2 SOIL_CHAR
    ////////
    char order_id;
    char type;
    //position in the rec string, not used for type 0
    int rec_pos;

    //position in the changed text string for all types
    int str_pos;
    //length of part to be inserted in UNDO, type 1, 2
    int ins_len;
    //length of part to be removed in UNDO, type 0, 2
    int rem_len;

    //below there are two possible cursor positions that may correspond to SEL_S and SEL_E in SoilText

    bool selected_before;
    int cursor_1_before;//cursor pos before action, to be restored when undone
    int cursor_2_before;
    bool selected_after;
    int cursor_1_after;//cursor pos after action, to be restored when redone
    int cursor_2_after;

    STR_REC_NODE* prev;
    STR_REC_NODE* next;

};

//for SoilText line index
struct LINE_INDEX_NODE{
    Uint32 first_char;
    Uint32 first_non_space;
    Uint32 length;//in number of chars
};

//for SoilText char position and size
struct CHAR_SPAN{
    Uint32 x;
    Uint32 y;
    unsigned char w;//in chars
    unsigned char br;//if this char is break, either 10 or a space in AUTO_BREAK
};

#endif /* global_definitions_h */
