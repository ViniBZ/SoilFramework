#ifndef SOILBUTTON_H
#define SOILBUTTON_H

#include "soilwidget.h"
#include "soiltext.h"
#include "soilimage.h"

#define BUTTON_IN_PAD 4

class SoilButton : public SoilWidget
{
public:

    bool CHECKABLE;
    bool CHECKED;//can only be true if CHECKABLE is true
    //true after MOUSE_DOWN that set
    bool just_checked;

    //MARKER: MARKED is for when the color is inverted when pressed or checked
    bool MARKED;

    bool hovering;

    SoilText soil_text;
    int text_len;
    SoilPixmap image;

    int font_size;
    SIZE text_size;

    SoilButton();
    virtual void validate_for_execution();
    
    virtual SIZE set_size_virt(int w, int h);
    void update_from_content();
    void set_checkable(bool c);
    void set_checked(bool c);
    void set_text(const char* str);
    void set_text(const SoilString& str);
    void set_image(const SoilPixmap& img);
    void set_image(const char* img_file_name);
    void unset_image();
    
    void draw();

    virtual void set_as_mouse_left_down_elm_virt(bool f);

    
};


#endif // SOILBUTTON_H