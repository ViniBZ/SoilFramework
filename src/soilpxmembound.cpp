#include "soilpxmembound.h"

SoilPxMemBound::SoilPxMemBound()
{
    W_MARGIN = 50;
    H_MARGIN = 50;
    ALLOC_SIZE.w = 0;
    ALLOC_SIZE.h = 0;
    ALLOC_LOWER.w = 0;
    ALLOC_LOWER.h = 0;
}
// ----------------------------------------------- SET MARGIN
void SoilPxMemBound::set_margin(int w_m, int h_m)
{
    W_MARGIN = w_m;
    H_MARGIN = h_m;
}
// ----------------------------------------------- CALCULATE
//w and h are the size the pixmap intends to use
bool SoilPxMemBound::calculate(int w, int h)
{
    bool sim = simulate(w,h);

    if(sim)
    {
        set_values(w + W_MARGIN, h + H_MARGIN);
        return true;
    }
    return false;
}

// ----------------------------------------------- SIMULATE
bool SoilPxMemBound::simulate(int w, int h)
{
    if(w <= 0 || h <=0){return false;}
    if(w > ALLOC_SIZE.w || h > ALLOC_SIZE.h)
    {
        return true;
    }

    if(w <= ALLOC_LOWER.w || h <= ALLOC_LOWER.h)
    {
        return true;
    }
    return false;
}
// ----------------------------------------------- SET VALUES
void SoilPxMemBound::set_values(int alloc_w, int alloc_h)
{
    if(alloc_w < 0){alloc_w = 0;}
    if(alloc_h < 0){alloc_h = 0;}

    //alloc_w - W_MARGIN and alloc_h - H_MARGIN would be the SoilPixmap.size

    ALLOC_SIZE.w = alloc_w;
    ALLOC_SIZE.h = alloc_h;

    ALLOC_LOWER.w = alloc_w - (2 * W_MARGIN);
    ALLOC_LOWER.h = alloc_h - (2 * H_MARGIN);

    if(ALLOC_LOWER.w < 0){ALLOC_LOWER.w = 0;}
    if(ALLOC_LOWER.h < 0){ALLOC_LOWER.h = 0;}
}
// ----------------------------------------------- ADAPT FROM SIZE
void SoilPxMemBound::adapt_from_size(int w, int h)
{
    set_values(w, h);
}
// ----------------------------------------------- RESET
void SoilPxMemBound::reset()
{
    ALLOC_SIZE.w = 0;
    ALLOC_SIZE.h = 0;
    ALLOC_LOWER.w = 0;
    ALLOC_LOWER.h = 0;
}