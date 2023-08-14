#ifndef SOILLABEL_H
#define SOILLABEL_H

#include "soiltextwidget.h"

class SoilLabel : public SoilTextWidget
{
public:
    SoilLabel();
    SoilLabel(const SoilString& str);
    void init();
    void set_font_size(int fs);
    void mouse_motion_event_virt(int X, int Y, int in_view_x, int in_view_y, SDL_Event E);
};

#endif // SOILLABEL_H
