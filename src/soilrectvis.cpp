#include "soilrectvis.h"

SoilRectVis::SoilRectVis() : SoilWidget(RECT_VIS)
{
    set_size(400,400);
    field_pixmap.alloc(400,400);
    grid_unit_w = 10;
    draw();
}


void SoilRectVis::draw()
{
    if(!pixmap.loaded){pixmap.alloc(view_size);}
    field_pixmap.spx_fill(0xFFA00050);

    SIZE field_size = field_pixmap.size;
    int i = 0;
    int y = 0;
    int x = 0;
    //while(i < field_size.y)

}