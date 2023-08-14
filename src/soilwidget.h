#ifndef SOILWIDGET_H
#define SOILWIDGET_H

#include "soiluielm.h"

class SoilWidget : public SoilUIElm
{
public:

    WIDGET_TYPE widget_type;
    SoilPixmap pixmap;
    int border;

    //if true, content is displayed in the pixmap and content_view_rect will basically be the offset
    //of view in the content
    //normally will be false for widgets that implement its own way of scrolling through content
    //for example SoilTextWidget, because a huge pixmap for a long text would spend too much memory
    bool default_scrollbar_system;

    SoilWidget(WIDGET_TYPE wt);
    virtual void draw_inner_border(Uint32 color, int padding);
    virtual void recursive_print(bool do_all);
    virtual void print();
};

#endif // SOILWIDGET_H
