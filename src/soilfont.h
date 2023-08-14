#ifndef SOILFONT_H
#define SOILFONT_H

#include "soilpixmap.h"
#include "soilstring.h"

//the values for the the char rects are in alloc_font
class SoilFont
{
public:
    SoilPixmap* pixmap;

    double char_ratio;//char H / char W
    SIZE* char_size;
    int size_n;//how many font sizes there is
    int font_len;//how many chars in font, 92 normally

    SOIL_RECT* font_coord;
    bool loaded;

    SoilFont();
    ~SoilFont();
    void init();
    void alloc_font();
    void draw_font_stub();
    int get_char_index(SOIL_CHAR ch);// receives two bytes for UTF8
    void print_font_to_pixmap(SOIL_CHAR ch, int font_size, bool inverted, SOIL_RECT char_rect, SoilPixmap* dest_pixmap, SOIL_RECT dest_rect);
    void print_font_to_pixmap(SOIL_CHAR ch, int font_size, bool inverted, SoilPixmap* dest_pixmap, SOIL_RECT dest_rect);

    void release_font();
    void make_font_display_file();

    static SIZE get_char_size_from_font_size(int font_size);
};

#endif // SOILFONT_H
