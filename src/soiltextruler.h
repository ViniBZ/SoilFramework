#ifndef SOILTEXTRULER_H
#define SOILTEXTRULER_H

#include "soiltextwidget.h"

class SoilTextRuler : public SoilWidget
{
public:

    SoilTextWidget* target_widget;
    bool target_widget_set;

    int update_n;

    SoilTextRuler();
    void init();

    bool set_target_widget(SoilTextWidget* t);

    void update_from_target_widget();
    virtual void draw();

    virtual int soilobject_callback(int arg);
};

#endif // SOILTEXTRULER_H
