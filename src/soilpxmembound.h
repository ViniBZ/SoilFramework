#ifndef SOILPXMEMBOUNDS_H
#define SOILPXMEMBOUNDS_H

//#include "global_definitions.h"

#include "pixmap_functions.h"
#include "soilobject.h"


//this class is for 4 byte(32 bits) pixel format mostly
//but the image data can be in 1 byte(8 bits) with mono_mode


class SoilPxMemBound : public SoilObject
{
public:
    int W_MARGIN;
    int H_MARGIN;
    SIZE ALLOC_SIZE;//currently alloced
    SIZE ALLOC_LOWER;//limit where it will alloc less
    //SIZE ALLOC_UPPER;//limit where it will alloc more
    
    SoilPxMemBound();
    void set_margin(int w_m, int h_m);
    bool calculate(int w, int h);
    bool simulate(int w, int h);
    void set_values(int alloc_w, int alloc_h);

    //adapt_from_size() serves to set working values for ALLOC_SIZE and ALLOC_LOWER from the SoilPixmap
    //when the pixmap is already alloc'ed and is set to dynamic_bound = true
    void adapt_from_size(int w, int h);
    void reset();
};

#endif // SOILPXMEMBOUNDS_H
