#ifndef SOILINPUT_H
#define SOILINPUT_H

#include "soiltextwidget.h"

class SoilInput : public SoilTextWidget
{
public:
    SoilInput();
    SoilInput(const SoilString& str);
    void init();
    void set_font_size(int fs);
    void keydown_event_virt(SDL_Event E);
    void draw();
};

#endif // SOILINPUT_H
