#include "soiltextruler.h"

SoilTextRuler::SoilTextRuler() : SoilWidget(TEXTRULER)
{
    init();
}
void SoilTextRuler::init()
{
    update_n = 0;   
}
//---------------------------------------------- SET TARGET WIDGET
bool SoilTextRuler::set_target_widget(SoilTextWidget* t)
{
    if(t->widget_type == TEXTBLOCK || t->widget_type == TEXTFIELD)
    {
        if(t->is_multi_line())
        {
            SoilObject* obj = (SoilObject*)this;
            t->set_soilobject_receiver(1, obj, true);
            target_widget = (SoilTextWidget*)t;
            update_from_target_widget();
            return true;
        }
    }
    return false;
}
//---------------------------------------------- UPDATE NUMBERS FROM TEXT WIDGET
void SoilTextRuler::update_from_target_widget()
{
    if(font == NULL){return;}
    SIZE target_size = target_widget->get_view_size();
    SIZE new_size{-1,-1};
    if(view_size.h != target_size.h)
    {
        new_size.h = target_size.h;
    }

    int widget_line_n = target_widget->get_line_n();
    int font_size = target_widget->get_font_size();
    int offset_y = target_widget->get_content_view_rect().y;

    SIZE char_size = SoilFont::get_char_size_from_font_size(font_size);

    int view_first_line = offset_y / char_size.h;
    offset_y %= char_size.h;
    /*
    printf("\n");
    printf("\nUPDATE N:%d",update_n);
    printf("\nLINE I:%d", line_i);
    printf("\nOFFSET Y:%d", offset_y);
    printf("\nLINE N:%d", line_n);
    */
    INT_DECIMAL_DIGITS digits = set_int_decimal_digits_struct(view_first_line);
    int total_digit_n = get_digit_n(widget_line_n);
    //printf("\nTOTAL DIGIT N:%d", total_digit_n);

    new_size.w = total_digit_n * char_size.w;


    //font->print_font_to_pixmap();
    set_size(new_size.w, new_size.h);

    pixmap.alloc(view_size);
    pixmap.spx_fill(0xFF000000);

    int view_line_n = view_size.h / char_size.h;
    if(offset_y > 0){view_line_n++;}

    SOIL_RECT char_rect{0,offset_y,char_size.w,(char_size.h-offset_y)};
    SOIL_RECT dest_rect{0,0,char_size.w,(char_size.h-offset_y)};

    SOIL_CHAR sch{1,32,0,0,0};
    int line_i = 0;
    while(line_i < view_line_n && line_i < widget_line_n)
    {
        
        dest_rect.x = (total_digit_n - digits.dig_n) * char_size.w;
        int i = 0;
        int d = 10 - digits.dig_n;
        while(i < digits.dig_n)
        {
            sch.B[0] = 48 + digits.dig[d];
            font->print_font_to_pixmap(sch, font_size, false, char_rect, &pixmap, dest_rect);
            dest_rect.x += char_size.w;
            d++;
            i++;
        }
        char_rect.y = 0;
        char_rect.h = char_size.h;

        dest_rect.y += dest_rect.h;
        if(dest_rect.h < char_size.h){dest_rect.h = char_size.h;}
        inc_int_decimal_digits_struct(&digits);
        line_i++;
    }

    draw();
    update_n++;
}
//---------------------------------------------- DRAW
void SoilTextRuler::draw()
{
    //draw_inner_border(0xFFFFFFFF, 1);
    set_elm_ui_flag(3);
}
//---------------------------------------------- SOILOBJECT CALLBACK
int SoilTextRuler::soilobject_callback(int arg)
{
    

    if(arg == 102)
    {
        //printf("\nUPDATING FROM TARGET WIDGET");
        update_from_target_widget();
    }   
    
    return 0;
}