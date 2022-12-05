#ifndef SOILTEXTWIDGET_H
#define SOILTEXTWIDGET_H

#include "soilwidget.h"
#include "soiltext.h"


//There would be different approaches for resource usage for this class
//FOR FEW EDITS: everytime the text_px_size changes it reallocs its own pixmap and draws
//print the pixmap to dest_pixmap normally
//
//WITH FEWER ALLOCS: doesn't have its own pixmap, when print is called it draws the text 
//straight to the dest_pixmap according to win_cut_rect and win_pos_rect
//For now I'm just implementing WITH FEW EDITS

//IMPORTANT:this class no longer deals with TABS as larger characters, they are regarded as unknown, but they can easily be
//replaced for spaces, this class also doesn't input them

#define TEXT_COMMAND_MAP_N 664
#define STW_DEFAULT_FONT_SIZE 5

typedef enum STW_COMMAND{
        STW_NONE=0,
        STW_COPY,
        STW_CUT,
        STW_PASTE,
        STW_TAB,
        STW_SHIFT_TAB,
        STW_LINEBREAK,
        STW_REMOVE_CHAR_RIGHT,//delete
        STW_REMOVE_CHAR_LEFT,//backspace
        STW_REMOVE_WORD,
        STW_REMOVE_LINE,
        STW_MOVE_LINE_START,
        STW_MOVE_LINE_END,
        STW_MOVE_RIGHT,
        STW_MOVE_LEFT,
        STW_MOVE_UP,
        STW_MOVE_DOWN,
        STW_PAGE_UP,
        STW_PAGE_DOWN,
        STW_MOVE_PREV_WORD_BOUND,
        STW_MOVE_NEXT_WORD_BOUND,
        STW_DUPLICATE_LINE,
        STW_UNDO,
        STW_REDO
    }STW_COMMAND;

//SELECTABLE
//EDITABLE

//AUTOBREAK
//FOLLOW_TEXT_W

//HOME_TO_INDENT
//TAB_NORMAL_INDENT
//TAB_ALWAYS_INDENT
//REGULAR_ADD_TAB
//REGULAR_DEL_TAB
//BR_KEEP_INDENT
//KEEP_X_MOVING_VERTICALLY
//CODE_EDIT_TEXT_SHORTCUTS

//SOIL TEXT WIDGET TEXT FIELD PROPERTIES
typedef enum STW_TF_PROPERTIES{
    STW_TF_HOME_TO_INDENT=1,
    STW_TF_TAB_NORMAL_INDENT=2,
    STW_TF_TAB_ALWAYS_INDENT=4,
    STW_TF_REGULAR_ADD_TAB=8,
    STW_TF_REGULAR_DEL_TAB=16,
    STW_TF_BR_KEEP_INDENT=32,
    STW_TF_KEEP_X_MOVING_VERTICALLY=64,
    STW_TF_CODE_EDIT_TEXT_SHORTCUTS=128
}STW_TF_PROPERTIES;

//alignment


class SoilTextWidget : public SoilWidget
{
public:
    //BASICS
    int action_n;

    SoilString* soil_string;
    SoilText soil_text;
    SIZE char_size;
    int font_size;
    SIZE text_size;//in pixels
    bool text_size_changed;
    //edit_extra_w_content is so when reaching the limit of the text field the cursor doesn't disappear
    //but only when EDITABLE == true
    int edit_extra_w_content;

    SoilTextRecord REC;

    //no special keys
    //ctrl
    //ctrl + alt
    //ctrl + shift
    //ctrl + alt + shift
    //
    //alt
    //alt + shift
    //
    //shift

    //83 first sdl scancode values times the 8 possibilities of special keys above = 
    STW_COMMAND TEXT_COMMAND_MAP[TEXT_COMMAND_MAP_N];


    //0 is FOR FEW EDITS
    //1 is WITH FEWER ALLOCS
    //int RESOURCE_MODE;

    //PROPERTIES

    bool SELECTABLE;
    bool EDITABLE;
    bool PWD_MODE;

    bool REC_ACTIVE;

    //AUTOBREAK AND FOLLOW_TEXT_W WON'T WORK TOGETHER, EITHER ONE OR THE OTHER IS TRUE, NEVER BOTH.
    //AUTOBREAK DOESNT PLACE BREAK CHARACTERS IN THE TEXT
    bool AUTOBREAK;
    bool FOLLOW_TEXT_W;

    bool CLICKABLE_ITEM;
    bool MARKED;

    //------------------------- STW TF VARIABLES
    //takes cursor to the first non space char if it is after it or at the very beginning of the line already
    bool HOME_TO_INDENT;

    //if selection goes from line_start to line_end, or a line break or more is selected
    //instead of removing the selection and adding spaces as tab, tab indents the selection
    //it also makes shift+tab remove indentation regardless if there is selected text or not
    bool TAB_NORMAL_INDENT;
    //tab always indent without removing chars regardless of selection
    //if this is true, TAB_NORMAL_INDENT is also true
    bool TAB_ALWAYS_INDENT;
    //tabs to next multiple of soil_text.TAB_W at any point in the line
    bool REGULAR_ADD_TAB;
    //when backspace or delete to remove spaces before the first_non_space char 
    //it deletes spaces until the next multiple of soil_text.TAB_W at line beginning
    bool REGULAR_DEL_TAB;
    //when ENTER input, it keeps the indentation of the previous line
    bool BR_KEEP_INDENT;
    //saves last x value change to apply when moving up or down
    //like when cursor is at the end of a long line, then press up, cursor goes to end of a shorter line, then up, it goes to the end of another long line
    bool KEEP_X_MOVING_VERTICALLY;

    //GNU_TEXT_SHORTCUTS activates some of the GNU Emacs text quick shortcuts, the cursor goes:
    //ctrl+a: line start
    //ctrl+e: line end
    //ctrl+f: right
    //ctrl+b: left
    //ctrl+n: down
    //ctrl+p: up
    bool CODE_EDIT_TEXT_SHORTCUTS;
    
    //-------------------

    ALIGNMENT horizontal_alignment;
    ALIGNMENT vertical_alignment;
    int margin_left;
    int margin_top;
    //FUNCTIONAL VARIABLES

    //to recognize the mouse left up as the one that has just selected text
    //and just select on the next mouse up
    bool mouse_selected;
    bool selected;

    ELM_FLAG cursor_flag;
    bool cursor_active;
    bool cursor_vis;

    //update_view_to_cursor was made just so the triple click won't tank the view
    //away to where the cursor is set in a big text
    bool update_view_to_cursor;

    bool update_from_soil_text;
    bool update_pixmap_from_size;
    
    int cursor_blink_n;
    int print_n;

    bool mouse_left_down_text;

    SoilTextWidget(WIDGET_TYPE widget_type);

    virtual void init() = 0;

    virtual void validate_for_execution();

    //key is the sdl scancode
    int command_map_index_from_keys(bool ctrl, bool alt, bool shift, int key);
    bool set_command_keys(bool ctrl, bool alt, int shift, int key, STW_COMMAND com);
    int set_default_command_map();
    STW_COMMAND command_from_keys(bool ctrl, bool alt, bool shift, int key);

    void set_code_edit_special_shortcuts(bool activate);
    void set_pwd_mode(bool m);
    void set_text_record(bool set);
    void set_text_record(bool set, std::string rec_excess_dir);
    void set_str_text_fields();
    void set_autobreak(bool a);
    void set_follow_text_w(bool f);
    void set_as_clickable_item(bool set);
    void set_horizontal_alignment(ALIGNMENT a);
    void set_vertical_alignment(ALIGNMENT a);
    void set_text_field_properties(Uint32 properties);
    int get_text_length();

    void save_state_to_file(bool save_record, const SoilString & filename);
    void load_state_from_file(const SoilString & filename);
    void log_state_file(const SoilString & state_filename, const SoilString & log_filename);

    virtual void set_text(const SoilString& str);
    //returns if font changed, doesnt call process()
    bool update_from_font_size(int fs);
    virtual void set_font_size(int fs);//
    virtual SIZE set_size_virt(int w, int h);//this is the one to be called by the programmer's code

    void adjust_content_view_to_cursor();//
    bool coord_in_text_content(int in_view_x,int in_view_y);

    virtual bool correct_coord_for_soil_text(int in_view_x,int in_view_y, int* in_text_x, int* in_text_y);
    SoilString* text();//
    bool process();
    virtual void draw();

    virtual int soilobject_callback(int arg);
    //----------------------------- EVENTS
    virtual void mouse_left_down_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_left_click_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void mouse_remote_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
    virtual void set_as_mouse_left_down_elm_virt(bool f);

    virtual void text_input_event_virt(SDL_Event E);
    virtual void keydown_event_virt(SDL_Event E);
    virtual void keyup_event_virt(SDL_Event E);

    virtual void set_as_focus_elm_virt(bool f);
    //return actual amount moved in POINT for x and y
    virtual POINT move_content_view(int mx, int my);//
    virtual void process_flag(int value);
};

#endif // SOILTEXTWIDGET_H
