#ifndef SOILRECTVIS_H
#define SOILRECTVIS_H

#include "soilwidget.h"

class SoilRectVis : public SoilWidget
{
public:

    int grid_unit_w;

    SoilPixmap field_pixmap;

    SoilRectVis();
    void draw();
};

#endif // SOILFONT_H