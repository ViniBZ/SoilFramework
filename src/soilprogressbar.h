#ifndef SOILPROGRESSBAR_H
#define SOILPROGRESSBAR_H

#include "soilwidget.h"
#include "soiltext.h"

class SoilProgressBar : public SoilWidget
{
public:

    int decimal_resolution;
    int value;
    SoilText value_text;

    SoilProgressBar();
    virtual void validate_for_execution();
    virtual SIZE set_size(int w,int h);
    void set_value(int v);

    void draw();
};


#endif // SOILPROGRESSBAR_H#