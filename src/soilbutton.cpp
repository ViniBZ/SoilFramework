#include "soilbutton.h"

SoilButton::SoilButton(): SoilWidget(BUTTON)
{
    scrollable = false;

    CHECKABLE = false;
    CHECKED = false;
    just_checked = false;
    MARKED = false;
    font_size = 4;

    hovering = false;

    soil_text.set_max_line_w(0);
    set_min_size(20,20);

    text_size = soil_text.get_text_px_size(font_size);
    //set_content_size(0,0);

    set_text("button");
}
//--------------------------------------------------- VALIDATE FOR EXECUTION
void SoilButton::validate_for_execution()
{
    SoilUIElm::validate_for_execution();
    draw();
}
//--------------------------------------------------- SET SIZE
SIZE SoilButton::set_size_virt(int w, int h)
{

    draw();
    if(elm_ui_flag < 3){elm_ui_flag = 3;}
    if(engine_running && engine_control_is_set)
    {
        engine_control->set_ui_flag(3);
    }
    
    return view_size;
}
//--------------------------------------------------- SET SIZE FROM CONTENT
void SoilButton::update_from_content()
{
    SIZE prev_size = view_size;

    int full_pad = 2*BUTTON_IN_PAD;     
    
    int content_w = full_pad;
    int content_h = full_pad;

    if(text_len > 0)
    {
        content_w += text_size.w;
        content_h += text_size.h;
    }

    if(image.loaded)
    {
        content_h += image.size.h;
        if(image.size.w > text_size.w)
        {
            content_w = image.size.w + full_pad;
        }
        if(text_len > 0)
        {
            content_h += BUTTON_IN_PAD;
        }
    }
    
    set_content_size(content_w, content_h);

    ABSOLUTE_VIEW_MIN_SIZE.w = content_size.w;
    ABSOLUTE_VIEW_MIN_SIZE.h = content_size.h;
    set_min_size(-1,-1);


    SoilUIElm::internal_set_size(content_size.w, content_size.h);
    //MARKER:I stopped here? ...
    draw();    

    if(elm_ui_flag < 3){elm_ui_flag = 3;}
    if(engine_running && engine_control_is_set)
    {
        engine_control->set_ui_flag(3);
    }
}
//--------------------------------------------------- SET CHECKABLE
void SoilButton::set_checkable(bool c)
{
    CHECKABLE = c;
    if(!c && CHECKED)
    {
        CHECKED = false;        
    }
}
//--------------------------------------------------- SET CHECKABLE
void SoilButton::set_checked(bool c)
{
    if(CHECKABLE)
    {
        bool prev_checked = CHECKED;
        CHECKED = c;
        if(CHECKED != prev_checked)
        {
            MARKED = CHECKED;
            draw();
        }
    }
}

//--------------------------------------------------- SET TEXT
void SoilButton::set_text(const char* str)
{
    soil_text.set_text(str);
    text_size = soil_text.get_text_px_size(font_size);
    text_len = soil_text.get_length();

    update_from_content();
}

//--------------------------------------------------- SET TEXT
void SoilButton::set_text(const SoilString& str)
{
    //MARKER:commented this because soil_text.soil_string should be SoilText's private property
    //soil_text.soil_string = str;
    soil_text.set_text(str);
    text_size = soil_text.get_text_px_size(font_size);

    update_from_content();
}
//--------------------------------------------------- SET IMAGE
void SoilButton::set_image(const SoilPixmap& img)
{
    image.alloc_from_pixmap(img);
    update_from_content();
}
//--------------------------------------------------- SET IMAGE
void SoilButton::set_image(const char* img_file_name)
{
    image.alloc_from_file(img_file_name);
    update_from_content();
}
//--------------------------------------------------- UNSET IMAGE
void SoilButton::unset_image()
{
    image.release();
    update_from_content();
}
//--------------------------------------------------- DRAW
void SoilButton::draw()
{
    if(!pixmap.loaded)
    {
        pixmap.alloc(view_size.w, view_size.h);
    }else{
        if(pixmap.size.w != view_size.w || pixmap.size.h != view_size.h)
        {
            pixmap.alloc(view_size.w, view_size.h);
        }
    }

    Uint32 front_color = 0xFFFFFFFF;
    Uint32 bk_color = 0x0FF000000;
    pixmap.spx_fill(bk_color);

    int half_pad = BUTTON_IN_PAD / 2;

    //border
    SOIL_RECT draw_rect;
    draw_rect.x = BUTTON_IN_PAD;
    draw_rect.y = BUTTON_IN_PAD;
    draw_rect.w = view_size.w - 2*(BUTTON_IN_PAD);
    draw_rect.h = view_size.h - 2*(BUTTON_IN_PAD);

    SIZE char_size = SoilFont::get_char_size_from_font_size(font_size);
    SOIL_RECT img_rect;
    if(image.loaded)
    {
        img_rect = draw_rect;
        if(draw_rect.w > image.size.w)
        {
            img_rect.x += (draw_rect.w - image.size.w) / 2;
        }
        
        if(draw_rect.h > content_size.h)
        {
            int in_size = image.size.h;
            if(text_len > 0)
            {
                in_size += text_size.h + BUTTON_IN_PAD;
            }
            int top_margin = (draw_rect.h - in_size) / 2;

            img_rect.y += top_margin;
        }
        pixmap.spx_paste_pixmap(image, image.get_full_rect(), img_rect);
    } 


    SOIL_RECT text_rect = draw_rect;
    if(image.loaded)
    {
        text_rect.y += img_rect.y + image.size.h;
    }
    if(text_rect.w > text_size.w)
    {
        text_rect.x += (text_rect.w - text_size.w) / 2;
        //text_rect.w -= (text_rect.w - text_size.w) / 2;
    }
    if(text_rect.h > text_size.h)
    {
        //text_rect.y += (text_rect.h - text_size.h) / 2;
        //text_rect.h -= (text_rect.h - text_size.h) / 2;
    }  

    //DRAWING TEXT
    SOIL_RECT rec = soil_text.get_full_rect(char_size);
    soil_text.draw_text_rect_px(soil_text.get_full_rect(char_size), &pixmap, text_rect, font, font_size);

    draw_rect.x -= half_pad;
    draw_rect.y -= half_pad;
    draw_rect.w += BUTTON_IN_PAD;
    draw_rect.h += BUTTON_IN_PAD;

    //DRAWING RECT
    pixmap.spx_draw_outline_rect(draw_rect, 1, front_color);

    if(MARKED)
    {
        pixmap.spx_invert_colors();
    }

    if(elm_ui_flag < 2){elm_ui_flag = 2;}
    if(engine_running && engine_control_is_set)
    {
        engine_control->set_ui_flag(elm_ui_flag);
    }
}
//--------------------------------------------------- MOUSE LEFT DOWN
void SoilButton::set_as_mouse_left_down_elm_virt(bool f)
{
    SoilUIElm* this_elm = (SoilUIElm*) this;

    bool prev_marked = MARKED;
    if(CHECKABLE)
    {
        if(f)
        {
            if(!CHECKED)
            {
                just_checked = true;
                set_checked(true);
            }			
        }else{
            if(!just_checked)
            {
                //set_checked() calls draw() if CHECKED changes
                set_checked(false);
            }
            just_checked = false;
        }
    }else{
        MARKED = f;
        if(prev_marked != MARKED)
        {
            draw();
        }
    }
}