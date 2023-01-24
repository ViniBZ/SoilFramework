#include "pixmap_functions.h"
#include "load_save_files.h"


// ------------------------------------------------------ CONVERT MM TO PIXELS
int mm_to_px(int dpi, int mm)
{
    double tmp = (double) dpi / 25.4;
    tmp *= (double) mm;
    int res = (int)tmp;
    return res;
}

// ------------------------------------------------------ RESET SIZE
void reset_size(SIZE* size)
{
    size->h=0;
    size->w=0;
}
// ------------------------------------------------------ PRINTF SDL RECT
void printf_soil_rect(SOIL_RECT rect, int id)
{
    printf("\n");
    if(id >= 0)
    {
        printf("\nRECT No:%d",id);
    }
    printf("\nRECT.X:%d",rect.x);
    printf("\nRECT.Y:%d",rect.y);
    printf("\nRECT.W:%d",rect.w);
    printf("\nRECT.H:%d",rect.h);
}
// ------------------------------------------------------ PRINTF SOIL BOUND
void printf_soil_bound(BOUND_RECT bound, int id)
{
    printf("\n");
    if(id >= 0)
    {
        printf("\nBOUND No:%d",id);
    }
    printf("\nBOUND.START_X:%d",bound.start_x);
    printf("\nBOUND.START_Y:%d",bound.start_y);
    printf("\nBOUND.END_X:%d",bound.end_x);
    printf("\nBOUND.END_Y:%d",bound.end_y);
}
// ------------------------------------------------------ PRINTF SIZE
void printf_size(SIZE size, int id)
{
    printf("\n");
    if(id >= 0)
    {
        printf("\nSIZE No:%d",id);
    }
    printf("\nSIZE.H:%d",size.h);
    printf("\nSIZE.W:%d",size.w);
}

// ------------------------------------------------------ RESET RECT
void reset_rect(SOIL_RECT* rect)
{
    rect->x = 0;
    rect->y = 0;
    rect->w = 0;
    rect->h = 0;
}
// ------------------------------------------------------ SET RECT ABSOLUTE
//it means w and h won't be negative
void set_rect_abs(SOIL_RECT* rect)
{
    if(rect->w < 0)
    {
        rect->x += rect->w;
        rect->w *= (-1);
    }
    if(rect->h < 0)
    {
        rect->y += rect->h;
        rect->h *= (-1);
    }

}
// ------------------------------------------------------ RESET BOUND
void reset_bound(BOUND_RECT* bound)
{
    bound->start_x = 0;
    bound->start_y = 0;
    bound->end_x = 0;
    bound->end_y = 0;
}
// ------------------------------------------------------ VALIDATE RECT WITH SIZE
//in relation to this pixmap's size
//return false if there is no intersection of the rect with size
//it doesn't accepts negative w/h in neither size nor rect
bool validate_rect_in_size(SOIL_RECT* rect, SIZE size)
{
    SOIL_RECT res_rect = (*rect);
    bool res = true;

    //if(res_rect.w < 0 || res_rect.h < 0 || size.w < 0 || size.h < 0){return false;}

    int res_rect_ex = res_rect.x + res_rect.w;
    int res_rect_ey = res_rect.y + res_rect.h;

    //
    if(res_rect.x < 0)
    {
        res_rect.x = 0;//change
    }else{
        if(res_rect.x >= size.w)
        {
            res_rect.x = size.w;
            return false;
        }
    }

    if(res_rect.y < 0)
    {
        res_rect.y = 0;//change
    }else{
        if(res_rect.y >= size.h)
        {
            res_rect.y = size.h;
            return false;
        }
    }

    if(res_rect_ex > size.w)
    {
        res_rect_ex = size.w;//change
    }else{
        if(res_rect_ex <= 0)
        {
            res_rect_ex = 0;
            return false;
        }
    }

    if(res_rect_ey > size.h)
    {
        res_rect_ey = size.h;//change
    }else{
        if(res_rect_ey < 0)
        {
            res_rect_ey = 0;
            return false;
        }
    }
    res_rect.w = res_rect_ex - res_rect.x;
    res_rect.h = res_rect_ey - res_rect.y;

    *rect = res_rect;

    return res;
}
// ------------------------------------------------------ ADJUST RECT TO SIZE
bool adjust_rect_to_size(SIZE size, SOIL_RECT* rect)
{
    set_rect_abs(rect);
    int ex = rect->x + rect->w;
    int ey = rect->y + rect->h;

    SOIL_RECT res_rect;
    res_rect = *rect;

    if(res_rect.x >= size.w || ex <= 0)
    {
        return false;
    }else{
        if(res_rect.x < 0)
        {
            res_rect.x = 0;
        }
        if(ex > size.w)
        {
            ex = size.w;
        }
        res_rect.w = ex - res_rect.x;
    }
    if(res_rect.y >= size.h || ey <= 0)
    {
        return false;
    }else{
        if(res_rect.y < 0)
        {
            res_rect.y = 0;
        }
        if(ey > size.h)
        {
            ey = size.h;
        }
        res_rect.h = ey - res_rect.y;
    }
    *rect = res_rect;
    
    return true;   

}
// ------------------------------------------------------ CUT RECT FROM RECT
SOIL_RECT rect_intersection(SOIL_RECT rect_1, SOIL_RECT rect_2)
{
    //the idea is to use the view_rect which is the rect relative to the content
    //as the limit rect and the rel_pos_rect from some in layout to be cut
    SOIL_RECT inter;
    inter.x = 0;
    inter.y= 0;
    inter.w = 0;
    inter.h = 0;

    //to return this if no intersection is found
    SOIL_RECT no_inter;
    no_inter.x = 0;
    no_inter.y= 0;
    no_inter.w = 0;
    no_inter.h = 0;

    if(rect_1.w == 0 || rect_1.h == 0 || rect_2.w == 0 || rect_2.h == 0)
    {
        return no_inter;
    }

    BOUND_RECT bound_1;
    bound_1.start_x = rect_1.x;
    bound_1.start_y = rect_1.y;
    bound_1.end_x = rect_1.x + rect_1.w;
    bound_1.end_y = rect_1.y + rect_1.h;

    BOUND_RECT bound_2;
    bound_2.start_x = rect_2.x;
    bound_2.start_y = rect_2.y;
    bound_2.end_x = rect_2.x + rect_2.w;
    bound_2.end_y = rect_2.y + rect_2.h;
    //horizontally
    if(bound_1.start_x < bound_2.start_x)
    {
        if(bound_1.end_x > bound_2.start_x)
        {
            if(bound_1.end_x > bound_2.end_x)
            {
                inter.x = bound_2.start_x;
                inter.w = bound_2.end_x - bound_2.start_x;
            }else{
                inter.x = bound_2.start_x;
                inter.w = bound_1.end_x - bound_2.start_x;
            }
        }else{
            return no_inter;
        }
    }else{
        if(bound_2.end_x > bound_1.start_x)
        {
            if(bound_2.end_x > bound_1.end_x)
            {
                inter.x = bound_1.start_x;
                inter.w = bound_1.end_x - bound_1.start_x;
            }else{
                inter.x = bound_1.start_x;
                inter.w = bound_2.end_x - bound_1.start_x;
            }
        }else{
            return no_inter;
        }
    }
    //vertically
    if(bound_1.start_y < bound_2.start_y)
    {
        if(bound_1.end_y > bound_2.start_y)
        {
            if(bound_1.end_y > bound_2.end_y)
            {
                inter.y = bound_2.start_y;
                inter.h = bound_2.end_y - bound_2.start_y;
            }else{
                inter.y = bound_2.start_y;
                inter.h = bound_1.end_y - bound_2.start_y;
            }
        }else{
            return no_inter;
        }
    }else{
        if(bound_2.end_y > bound_1.start_y)
        {
            if(bound_2.end_y > bound_1.end_y)
            {
                inter.y = bound_1.start_y;
                inter.h = bound_1.end_y - bound_1.start_y;
            }else{
                inter.y = bound_1.start_y;
                inter.h = bound_2.end_y - bound_1.start_y;
            }
        }else{
            return no_inter;
        }
    }
    return inter;
}
// ------------------------------------------------------ RECT INTERSECTION
BOUND_RECT bound_intersection(BOUND_RECT bound_1, BOUND_RECT bound_2)
{

    BOUND_RECT bound_3;
    BOUND_RECT no_inter;
    no_inter.start_x = 0;
    no_inter.start_y= 0;
    no_inter.end_y = 0;
    no_inter.end_x = 0;
    //horizontally
    if(bound_1.start_x < bound_2.start_x)
    {
        if(bound_1.end_x > bound_2.start_x)
        {
            if(bound_1.end_x > bound_2.end_x)
            {
                bound_3.start_x = bound_2.start_x;
                bound_3.end_x = bound_2.end_x;
            }else{
                bound_3.start_x = bound_2.start_x;
                bound_3.end_x = bound_1.end_x;
            }
        }else{
            return no_inter;
        }
    }else{
        if(bound_2.end_x > bound_1.start_x)
        {
            if(bound_2.end_x > bound_1.end_x)
            {
                bound_3.start_x = bound_1.start_x;
                bound_3.end_x = bound_1.end_x;
            }else{
                bound_3.start_x = bound_1.start_x;
                bound_3.end_x = bound_2.end_x;
            }
        }else{
            return no_inter;
        }
    }
    //vertically
    if(bound_1.start_y < bound_2.start_y)
    {
        if(bound_1.end_y > bound_2.start_y)
        {
            if(bound_1.end_y > bound_2.end_y)
            {
                bound_3.start_y = bound_2.start_y;
                bound_3.end_y = bound_2.end_y;
            }else{
                bound_3.start_y = bound_2.start_y;
                bound_3.end_y = bound_1.end_y;
            }
        }else{
            return no_inter;
        }
    }else{
        if(bound_2.end_y > bound_1.start_y)
        {
            if(bound_2.end_y > bound_1.end_y)
            {
                bound_3.start_y = bound_1.start_y;
                bound_3.end_y = bound_1.end_y;
            }else{
                bound_3.start_y = bound_1.start_y;
                bound_3.end_y = bound_2.end_y;
            }
        }else{
            return no_inter;
        }
    }
    return bound_3;
}

// ------------------------------------------------------ ADD TO RECT
SOIL_RECT add_to_rect_pos(SOIL_RECT rect_1, int w, int h)
{
    SOIL_RECT res;
    res.x = rect_1.x + w;
    res.y = rect_1.y + h;
    res.w = rect_1.w;
    res.h = rect_1.h;

    return res;
}
// ------------------------------------------------------ ADD TO BOUND
BOUND_RECT add_to_bound_pos(BOUND_RECT bound, int w, int h)
{
    BOUND_RECT res;
    res.start_x = bound.start_x + w;
    res.start_y = bound.start_y + h;
    res.end_x = bound.end_x + w;
    res.end_y = bound.end_y + h;

    return res;
}
// ------------------------------------------------------ SUBTRACT TO RECT
SOIL_RECT subtract_from_rect_pos(SOIL_RECT rect_1, int w, int h)
{
    SOIL_RECT res;
    res.x = rect_1.x - w;
    res.y = rect_1.y - h;
    res.w = rect_1.w;
    res.h = rect_1.h;

    return res;
}

// ------------------------------------------------------ SUBTRACT TO BOUND
BOUND_RECT subtract_from_bound_pos(BOUND_RECT bound, int w, int h)
{
    BOUND_RECT res;
    res.start_x = bound.start_x - w;
    res.start_y = bound.start_y - h;
    res.end_x = bound.end_x - w;
    res.end_y = bound.end_y - h;

    //MARKER: make sure it is really no problem this limit control 

    //if(res.start_x < 0){res.start_x = 0;}
    //if(res.start_y < 0){res.start_y = 0;}
    //if(res.end_x < 0){res.end_x = 0;}
    //if(res.end_y < 0){res.end_y = 0;}

    return res;
}
// ------------------------------------------------------ ADD TO RECT AREA
SOIL_RECT add_to_rect_area(SOIL_RECT rect_1, int w, int h)
{
    SOIL_RECT res;
    res.x = rect_1.x;
    res.y = rect_1.y;
    res.w = rect_1.w + w;
    res.h = rect_1.h + h;

    return res;
}

// ------------------------------------------------------ ADD TO BOUND AREA
BOUND_RECT add_to_bound_area(BOUND_RECT bound, int w, int h)
{
    BOUND_RECT res;
    res.start_x = bound.start_x;
    res.start_y = bound.start_y;
    res.end_x = bound.end_x + w;
    res.end_y = bound.end_y + h;

    return res;
}

// ------------------------------------------------------ SUBTRACT TO RECT AREA
SOIL_RECT subtract_from_rect_area(SOIL_RECT rect_1, int w, int h)
{
    SOIL_RECT res;
    res.x = rect_1.x;
    res.y = rect_1.y;
    res.w = rect_1.w - w;
    res.h = rect_1.h - h;

    if(res.w < 0){res.w = 0;}
    if(res.h < 0){res.h = 0;}

    return res;
}

// ------------------------------------------------------ SUBTRACT TO BOUND AREA
BOUND_RECT subtract_from_bound_area(BOUND_RECT bound, int w, int h)
{
    BOUND_RECT res;
    res.start_x = bound.start_x;
    res.start_y = bound.start_y;
    res.end_x = bound.end_x - w;
    res.end_y = bound.end_y - h;

    if(res.end_x < bound.start_x){res.end_x = bound.start_x;}
    if(res.end_y < bound.start_y){res.end_y = bound.start_y;}

    return res;
}
// ------------------------------------------------------ RECT TO BOUND
BOUND_RECT rect_to_bound(SOIL_RECT rect)
{
    BOUND_RECT res;
    res.start_x = rect.x;
    res.start_y = rect.y;
    res.end_x = rect.x + rect.w;
    res.end_y = rect.y + rect.h;

    return res;
}
// ------------------------------------------------------ BOUND TO RECT
SOIL_RECT bound_to_rect(BOUND_RECT bound)
{
    SOIL_RECT res;
    res.x = bound.start_x;
    res.y = bound.start_y;
    res.w = bound.end_x - bound.start_x;
    res.h = bound.end_y - bound.start_y;

    return res;
}
// ------------------------------------------------------ POINT IN BOUND
bool point_in_bound(BOUND_RECT B, POINT P)
{
    //MARKER: this kind of function might have to take
    //in consideration the limits to really be in a bound
    //probably be >= end
    bool res = true;
    if(P.x < B.start_x || P.x > B.end_x){res = false;}
    if(P.y < B.start_y || P.y > B.end_y){res = false;}
    return res;
}

// ------------------------------------------------------ POINT IN BOUND
bool point_in_bound(BOUND_RECT B, int X, int Y)
{
    bool res = true;
    if(X < B.start_x || X > B.end_x){res = false;}
    if(Y < B.start_y || Y > B.end_y){res = false;}
    return res;
}
// ------------------------------------------------------ POINT IN RECT
bool point_in_rect(SOIL_RECT R, POINT P)
{
    bool res = true;
    if(P.x < R.x || P.x > (R.x + R.w)){res = false;}
    if(P.y < R.y || P.y > (R.y + R.h)){res = false;}
    return res;
}

// ------------------------------------------------------ POINT IN RECT
bool point_in_rect(SOIL_RECT R, int X, int Y)
{
    bool res = true;
    if(X < R.x || X > (R.x + R.w)){res = false;}
    if(Y < R.y || Y > (R.y + R.h)){res = false;}
    return res;
}

// ------------------------------------------------------ ADJUST RECT TO PX SIZE
//MARKER: maybe change the name of this function to adjust_rect_to_size
void adjust_rect_to_px_size(SIZE size,SOIL_RECT *rect)
{
    BOUND_RECT bound = rect_to_bound(*rect);
    if(bound.start_x < 0){bound.start_x = 0;}
    if(bound.start_y < 0){bound.start_y = 0;}
    if(bound.start_x > size.w){bound.start_x = size.w;}
    if(bound.start_y > size.h){bound.start_y = size.h;}

    if(bound.end_x < 0){bound.end_x = 0;}
    if(bound.end_y < 0){bound.end_y = 0;}
    if(bound.end_x > size.w){bound.end_x = size.w;}
    if(bound.end_y > size.h){bound.end_y = size.h;}

    *rect = bound_to_rect(bound);
}
// ------------------------------------------------------ ADJUST ORIG RECT AND DEST RECT
void adjust_orig_and_dest_rect(SIZE orig_size,SOIL_RECT *orig_rect, SIZE dest_size, SOIL_RECT *dest_rect)
{
    adjust_rect_to_px_size(orig_size, orig_rect);
    adjust_rect_to_px_size(dest_size, dest_rect);

    if(orig_rect->w > dest_rect->w)
    {
        orig_rect->w = dest_rect->w;
    }else{
        dest_rect->w = orig_rect->w;
    }

    if(orig_rect->h > dest_rect->h)
    {
        orig_rect->h = dest_rect->h;
    }else{
        dest_rect->h = orig_rect->h;
    }
    
}
// ------------------------------------------------------ ADJUST LINE PATH TO PX SIZE
void adjust_line_path_to_px_size(SIZE size,POINT* point, int* end_x, int* end_y)
{
    if(point->y<0){point->y=0;}
    if(point->x<0){point->x=0;}
    if(point->y>=size.h){point->y=size.h-1;}
    if(point->x>=size.w){point->x=size.w-1;}
    //THIS IS NOT FINISHED YET
}
// ------------------------------------------------------ ADJUST HORIZONTAL LINE PATH TO PX SIZE
void adjust_h_line_path_to_px_size(SIZE size,POINT* point, int* end_x)
{
    if(point->y<0){point->y=0;}
    if(point->x<0){point->x=0;}
    if(point->y>=size.h){point->y=size.h-1;}
    if(point->x>=size.w){point->x=size.w-1;}

    if(*end_x<0){*end_x=0;}
    if(*end_x>=size.w){*end_x=size.w-1;}
}
// ------------------------------------------------------ ADJUST VERTICAL LINE PATH TO PX SIZE
void adjust_v_line_path_to_px_size(SIZE size,POINT* point, int* end_y)
{
    if(point->y<0){point->y=0;}
    if(point->x<0){point->x=0;}
    if(point->y>=size.h){point->y=size.h-1;}
    if(point->x>=size.w){point->x=size.w-1;}

    if(*end_y<0){*end_y=0;}
    if(*end_y>=size.h){*end_y=size.h-1;}
}

// ------------------------------------------------------ RETURN UCHAR FROM PIXEL VALUE
// unsigned char is regarded as the monochromatic representation of the color
// based on simple average calculation
//MARKER: make it so the grayscale formula can be customized 
//as in giving different colors different weights on the average
unsigned char return_uchar_from_pix(Uint32 orig_pix)
{
    unsigned char mono_pix;
    PIXDATA pix_data;
    pix_data.pix = orig_pix;
    int total = 0;
    total += pix_data.color.r;
    total += pix_data.color.g;
    total += pix_data.color.b;
    total /= 3;
    mono_pix = total;

    return mono_pix;
}

// ------------------------------------------------------ RETURN UCHAR FROM PIXEL VALUE
unsigned char return_uchar_from_pix(PIXDATA orig_pix)
{
    unsigned char mono_pix;
    int total = 0;
    total += orig_pix.color.r;
    total += orig_pix.color.g;
    total += orig_pix.color.b;
    total /= 3;
    mono_pix = total;

    return mono_pix;
}

// ------------------------------------------------------ RETURN UINT32 FROM PIXEL VALUE
Uint32 return_uint32_from_pix(unsigned char orig_pix)
{
    Uint32 color_pix;
    PIXDATA pix_data;
    pix_data.color.a = 255;
    pix_data.color.r = orig_pix;
    pix_data.color.g = orig_pix;
    pix_data.color.b = orig_pix;
    color_pix = pix_data.pix;

    return color_pix;
}

// ------------------------------------------------------ RETURN UINT32 FROM PIXEL VALUE
Uint32 return_uint32_from_pix(PIXDATA orig_pix)
{
    return orig_pix.pix;
}

// ------------------------------------------------------ RETURN PIXDATA FROM PIXEL VALUE
PIXDATA return_pixdata_from_pix(unsigned char orig_pix)
{
    PIXDATA pix_data;
    pix_data.color.a = 255;
    pix_data.color.r = orig_pix;
    pix_data.color.g = orig_pix;
    pix_data.color.b = orig_pix;

    return pix_data;
}

// ------------------------------------------------------ RETURN PIXDATA FROM PIXEL VALUE
PIXDATA return_pixdata_from_pix(Uint32 orig_pix)
{
    PIXDATA pix_data;
    pix_data.pix = orig_pix;
    return pix_data;
}
//--------------------------------------------------- CORRECT IMAGE RATIO IN VIEW
SIZE correct_image_ratio_in_view(SIZE orig_img_size, SIZE view_size)
{
    
    double dw = (double)orig_img_size.w;
    double dh = (double)orig_img_size.h;
    double orig_img_ratio = dw / dh;

    dw = (double)view_size.w;
    dh = (double)view_size.h;

    double view_ratio = dw / dh;

    SIZE img_size = view_size;

    if(orig_img_ratio > view_ratio)
    {
        //view is taller
        img_size.h = img_size.w / orig_img_ratio;
    }
    if(orig_img_ratio < view_ratio)
    {
        //view is larger
        img_size.w = img_size.h * orig_img_ratio;
    }
    return img_size;
}
// ------------------------------------------------------ FILL PIXMAP
void fill_array(Uint32* arr, Uint32 value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_array(int* arr, int value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}

// ------------------------------------------------------ FILL PIXMAP
void fill_array(double* arr, double value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}

// ------------------------------------------------------ FILL PIXMAP
void fill_array(POINT* arr, POINT value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}

// ------------------------------------------------------ FILL PIXMAP
void fill_array(D_POINT* arr, D_POINT value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_array(CALC_PIX_COLOR* arr, CALC_PIX_COLOR value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_array(PIXDATA_FLAG* arr, PIXDATA_FLAG value, int size)
{
    int c=0;
    while(c<size)
    {
        arr[c] = value;
        c++;
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(Uint32 *pixmap, SIZE size,Uint32 color)
{
    int y=0;
    int x=0;
    if(pixmap!=NULL)
    {
        while(y<size.h)
        {
            while(x<size.w)
            {
                pixmap[(y*size.w)+x]=color;
                x++;
            }
            x=0;
            y++;
        }
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(Uint32 *pixmap, SIZE size,SOIL_RECT rect, Uint32 color)
{
    DRAW_POINTS DEST;

    long DEST_pre_index;
    long DEST_index;

    DEST.start_y=rect.y;
    DEST.start_x=rect.x;
    DEST.end_y=DEST.start_y+rect.h;
    DEST.end_x=DEST.start_x+rect.w;

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>size.h){DEST.end_y=size.h;}
    if(DEST.end_x>size.w){DEST.end_x=size.w;}


    DEST.y = DEST.start_y;
    DEST.x = DEST.start_x;
    if(pixmap!=NULL)
    {
        while(DEST.y < DEST.end_y)
        {
            DEST_pre_index = DEST.y * size.w;
            while(DEST.x < DEST.end_x)
            {
                DEST_index = DEST_pre_index + DEST.x;
                pixmap[DEST_index]=color;
                DEST.x++;
            }
            DEST.x=DEST.start_x;
            DEST.y++;
        }
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(unsigned char *pixmap, SIZE size,unsigned char color)
{
    int y=0;
    int x=0;
    if(pixmap!=NULL)
    {
        while(y<size.h)
        {
            while(x<size.w)
            {
                pixmap[(y*size.w)+x]=color;
                x++;
            }
            x=0;
            y++;
        }
    }
}
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(unsigned char *pixmap, SIZE size,SOIL_RECT rect, unsigned char color)
{
    DRAW_POINTS DEST;

    long DEST_pre_index;
    long DEST_index;

    DEST.start_y=rect.y;
    DEST.start_x=rect.x;
    DEST.end_y=DEST.start_y+rect.h;
    DEST.end_x=DEST.start_x+rect.w;

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>size.h){DEST.end_y=size.h;}
    if(DEST.end_x>size.w){DEST.end_x=size.w;}


    DEST.y = DEST.start_y;
    DEST.x = DEST.start_x;
    if(pixmap!=NULL)
    {
        while(DEST.y < DEST.end_y)
        {
            DEST_pre_index = DEST.y * size.w;
            while(DEST.x < DEST.end_x)
            {
                DEST_index = DEST_pre_index + DEST.x;
                pixmap[DEST_index]=color;
                DEST.x++;
            }
            DEST.x=DEST.start_x;
            DEST.y++;
        }
    }
}
// ------------------------------------------------------ FILL PIXMAP
void flip_pixmap(Uint32 *orig_pixmap, Uint32 *dest_pixmap, SIZE size, int dir)
{
    Uint32 *line_buffer=NULL;

    int flip_side;
    int other_side;
    if(dir==0)
    {
        //HORIZONTAL FLIP
        flip_side=size.w;
        other_side=size.h;
    }else{
        //VERTICAL FLIP
        flip_side=size.h;
        other_side=size.w;
    }
    line_buffer=new Uint32[flip_side];
    if(line_buffer==NULL || line_buffer==nullptr)
    {
        return;
    }

    int y=0;
    int x=0;
    int r=0;

    int index;
    while(y<other_side)
    {
        while(x<flip_side)
        {
            if(dir==0)
            {
                index=y*size.w+x;
            }else{
                index=x*size.w+y;
            }

            line_buffer[x]=orig_pixmap[index];
            x++;
        }
        x--;
        while(x>=0)
        {
            if(dir==0)
            {
                index=y*size.w+x;
            }else{
                index=x*size.w+y;
            }

            dest_pixmap[index]=line_buffer[r];
            x--;
            r++;
        }

        r=0;
        x=0;
        y++;
    }

    delete[] line_buffer;
}
// ------------------------------------------------------ TRANSPOSE CHAR ARRAY TO UINT32 ARRAY
//currently this function is not able to quite get the image format from the char* right
Uint32* transpose_char_to_uint32_arr_alloc(char* orig, int orig_h,int orig_w,SIZE * dest_size)
{
    if(orig==NULL){return NULL;}
    dest_size->h=orig_h;
    dest_size->w=orig_w;

    Uint32* dest=NULL;
    dest=new Uint32[dest_size->h*dest_size->w];
    if(dest==NULL){return NULL;}

    int orig_i=0;

    int dest_y=0;
    int dest_x=0;
    int dest_index=0;

    PIXDATA pixdata;
    char orig_ch[4];

    while(dest_y<dest_size->h)
    {
        while(dest_x<dest_size->w)
        {

            orig_ch[0]=orig[orig_i];
            orig_i++;
            orig_ch[1]=orig[orig_i];
            orig_i++;
            orig_ch[2]=orig[orig_i];
            orig_i++;
            orig_ch[3]=orig[orig_i];
            orig_i++;


            pixdata.color.a=orig_ch[0];
            pixdata.color.r=orig_ch[1];
            pixdata.color.g=orig_ch[2];
            pixdata.color.b=orig_ch[3];


            dest_index=(dest_y*dest_size->w)+dest_x;
            dest[dest_index]=pixdata.pix;

            dest_x++;
        }
        dest_x=0;
        dest_y++;
    }
    return dest;
}

// ------------------------------------------------------ SET MAX ALPHA VALUE
void set_max_alpha_value(Uint32* pixmap, SIZE size,unsigned char max_value)
{
    int total_size=size.w*size.h;
    PIXDATA pixdata;
    int c=0;
    while(c<total_size)
    {
        pixdata.pix=pixmap[c];
        if(pixdata.color.a>max_value)
        {
            pixdata.color.a=max_value;
            pixmap[c]=pixdata.pix;
        }
        c++;
    }

}
// ------------------------------------------------------ INVERT COLORS
void invert_colors(unsigned char *pixmap,SIZE size)
{
    int y=0;
    int x=0;
    int index=0;
    unsigned char inv_ch;
    while(y<size.h)
    {
        while(x<size.w)
        {
            index=y*size.w+x;
            inv_ch=255-pixmap[index];

            pixmap[index]=inv_ch;
            x++;
        }
        x=0;
        y++;
    }
}
// ------------------------------------------------------ INVERT COLORS
void invert_colors(Uint32 *pixmap,SIZE size)
{
    int y=0;
    int x=0;
    PIXDATA pixdata;
    int index=0;
    while(y<size.h)
    {
        while(x<size.w)
        {
            index=y*size.w+x;
            pixdata.pix=pixmap[index];

            pixdata.color.r=255-pixdata.color.r;
            pixdata.color.g=255-pixdata.color.g;
            pixdata.color.b=255-pixdata.color.b;

            pixmap[index]=pixdata.pix;
            x++;
        }
        x=0;
        y++;
    }
}
// ------------------------------------------------------ INVERT COLORS
void invert_colors(unsigned char *pixmap,SIZE size,SOIL_RECT rect)
{
    //MARKER:revise these limits, and in the function below
    if(rect.y<0){rect.y=0;}
    if(rect.x<0){rect.x=0;}
    if(rect.y>=size.h){rect.y=size.h-1;}
    if(rect.x>=size.w){rect.x=size.w-1;}

    int end_y;
    int end_x;
    end_y=rect.y+rect.h;
    end_x=rect.x+rect.w;

    if(end_y>=size.h){end_y=size.h;}
    if(end_x>=size.w){end_x=size.w;}


    PIXDATA pixdata;
    int index;
    int y=rect.y;
    int x=rect.x;

    unsigned char inv_ch;

    while(y<end_y)
    {
        while(x<end_x)
        {
            index=y*size.w+x;
            inv_ch = 255 - pixmap[index];
            pixmap[index]=inv_ch;
            x++;
        }
        x=rect.x;
        y++;
    }
}
// ------------------------------------------------------ INVERT COLORS
void invert_colors(Uint32 *pixmap,SIZE size,SOIL_RECT rect)
{
    //MARKER:revise these limits, and in the function below
    if(rect.y<0){rect.y=0;}
    if(rect.x<0){rect.x=0;}
    if(rect.y>=size.h){rect.y=size.h-1;}
    if(rect.x>=size.w){rect.x=size.w-1;}

    int end_y;
    int end_x;
    end_y=rect.y+rect.h;
    end_x=rect.x+rect.w;

    if(end_y>=size.h){end_y=size.h;}
    if(end_x>=size.w){end_x=size.w;}


    PIXDATA pixdata;
    int index;
    int y=rect.y;
    int x=rect.x;

    unsigned char inv_ch;

    while(y<end_y)
    {
        while(x<end_x)
        {
            index=y*size.w+x;
            pixdata.pix=pixmap[index];

            pixdata.color.r=255-pixdata.color.r;
            pixdata.color.g=255-pixdata.color.g;
            pixdata.color.b=255-pixdata.color.b;

            pixmap[index]=pixdata.pix;
            x++;
        }
        x=rect.x;
        y++;
    }
}
// ------------------------------------------------------ ALPHA BLEND
Uint32 alpha_blend_pixel(Uint32 base_pixel,Uint32 top_pixel)
{
    PIXDATA base_pixdata;
    PIXDATA top_pixdata;
    PIXDATA result_pixdata;
    double a1,r1,g1,b1;
    double a2,r2,g2,b2;
    double a3,r3,g3,b3;

    // -----------

    base_pixdata.pix=base_pixel;
    top_pixdata.pix=top_pixel;

    //------------ prepare blend
    a1=base_pixdata.color.a;
    r1=base_pixdata.color.r;
    g1=base_pixdata.color.g;
    b1=base_pixdata.color.b;

    a2=top_pixdata.color.a;
    r2=top_pixdata.color.r;
    g2=top_pixdata.color.g;
    b2=top_pixdata.color.b;


    //------------ blend


    a3=255-((255-a1)*(255-a2))/255;
    r3=r2+((((r1-r2)*a1)/255)*(255-a2))/255;
    g3=g2+((((g1-g2)*a1)/255)*(255-a2))/255;
    b3=b2+((((b1-b2)*a1)/255)*(255-a2))/255;


    result_pixdata.color.a=a3;
    result_pixdata.color.r=r3;
    result_pixdata.color.g=g3;
    result_pixdata.color.b=b3;

    return result_pixdata.pix;

}
// ------------------------------------------------------ ALPHA BLEND
PIXDATA alpha_blend_pixdata(PIXDATA base_pixdata,PIXDATA top_pixdata)
{
    PIXDATA result_pixdata;
    double a1,r1,g1,b1;
    double a2,r2,g2,b2;
    double a3,r3,g3,b3;

    //------------ prepare blend
    a1=base_pixdata.color.a;
    r1=base_pixdata.color.r;
    g1=base_pixdata.color.g;
    b1=base_pixdata.color.b;

    a2=top_pixdata.color.a;
    r2=top_pixdata.color.r;
    g2=top_pixdata.color.g;
    b2=top_pixdata.color.b;


    //------------ blend


    a3=255-((255-a1)*(255-a2))/255;
    r3=r2+((((r1-r2)*a1)/255)*(255-a2))/255;
    g3=g2+((((g1-g2)*a1)/255)*(255-a2))/255;
    b3=b2+((((b1-b2)*a1)/255)*(255-a2))/255;


    result_pixdata.color.a=a3;
    result_pixdata.color.r=r3;
    result_pixdata.color.g=g3;
    result_pixdata.color.b=b3;

    return result_pixdata;

}
// ------------------------------------------------------ ALPHA BLEND PIXMAP
void alpha_blend_pixmap(const Uint32 *base_pixmap,SIZE base_size, SOIL_RECT base_rect,const Uint32 *top_pixmap,SIZE top_size, SOIL_RECT top_rect,Uint32* dest_pixmap, SIZE dest_size, SOIL_RECT dest_rect)
{

    PIXDATA base_pixdata;
    PIXDATA top_pixdata;
    PIXDATA result_pixdata;

    double a1,r1,g1,b1;
    double a2,r2,g2,b2;
    double a3,r3,g3,b3;


    //--------------

    DRAW_POINTS BASE;
    DRAW_POINTS TOP;
    DRAW_POINTS DEST;

    long BASE_pre_index;
    long BASE_index;

    long TOP_pre_index;
    long TOP_index;

    long DEST_pre_index;
    long DEST_index;

    BASE.start_y=base_rect.y;
    BASE.start_x=base_rect.x;
    BASE.end_y=BASE.start_y+base_rect.h;
    BASE.end_x=BASE.start_x+base_rect.w;

    TOP.start_y=top_rect.y;
    TOP.start_x=top_rect.x;
    TOP.end_y=TOP.start_y+top_rect.h;
    TOP.end_x=TOP.start_x+top_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(BASE.start_y<0){BASE.start_y=0;}
    if(BASE.start_x<0){BASE.start_x=0;}
    if(BASE.end_y>base_size.h){BASE.end_y=base_size.h;}
    if(BASE.end_x>base_size.w){BASE.end_x=base_size.w;}

    if(TOP.start_y<0){TOP.start_y=0;}
    if(TOP.start_x<0){TOP.start_x=0;}
    if(TOP.end_y>top_size.h){TOP.end_y=top_size.h;}
    if(TOP.end_x>top_size.w){TOP.end_x=top_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_size.h){DEST.end_y=dest_size.h;}
    if(DEST.end_x>dest_size.w){DEST.end_x=dest_size.w;}

    BASE.y=BASE.start_y;
    BASE.x=BASE.start_x;

    TOP.y=TOP.start_y;
    TOP.x=TOP.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;

    while(BASE.y<BASE.end_y && TOP.y<TOP.end_y && DEST.y<DEST.end_y)
    {
        BASE_pre_index=BASE.y*base_size.w;
        TOP_pre_index=TOP.y*top_size.w;
        DEST_pre_index=DEST.y*dest_size.w;

        while(BASE.x<BASE.end_x && TOP.x<TOP.end_x)
        {
            BASE_index=BASE_pre_index+BASE.x;
            TOP_index=TOP_pre_index+TOP.x;
            DEST_index=DEST_pre_index+DEST.x;

            base_pixdata.pix=base_pixmap[BASE_index];
            top_pixdata.pix=top_pixmap[TOP_index];

            //------------ prepare blend
            a1=base_pixdata.color.a;
            r1=base_pixdata.color.r;
            g1=base_pixdata.color.g;
            b1=base_pixdata.color.b;

            a2=top_pixdata.color.a;
            r2=top_pixdata.color.r;
            g2=top_pixdata.color.g;
            b2=top_pixdata.color.b;


            //------------ blend


            a3=255-((255-a1)*(255-a2))/255;
            r3=r2+((((r1-r2)*a1)/255)*(255-a2))/255;
            g3=g2+((((g1-g2)*a1)/255)*(255-a2))/255;
            b3=b2+((((b1-b2)*a1)/255)*(255-a2))/255;


            result_pixdata.color.a=a3;
            result_pixdata.color.r=r3;
            result_pixdata.color.g=g3;
            result_pixdata.color.b=b3;

            dest_pixmap[DEST_index]=result_pixdata.pix;


            BASE.x++;
            TOP.x++;
            DEST.x++;
        }
        BASE.x=BASE.start_x;
        TOP.x=TOP.start_x;
        DEST.x=DEST.start_x;

        BASE.y++;
        TOP.y++;
        DEST.y++;
    }

}

// ------------------------------------------------------ INVERT COLORS
void invert_opaque_colors(Uint32 *pixmap,SIZE size)
{
    int y=0;
    int x=0;
    PIXDATA pixdata;
    int index=0;
    while(y<size.h)
    {
        while(x<size.w)
        {
            index=y*size.w+x;
            pixdata.pix=pixmap[index];


            if(pixdata.color.a>0)
            {
                pixdata.color.r=255-pixdata.color.r;
                pixdata.color.g=255-pixdata.color.g;
                pixdata.color.b=255-pixdata.color.b;
            }

            if(pixdata.color.a==0)
            {
                pixdata.color.r=0;
                pixdata.color.g=0;
                pixdata.color.b=0;
            }

            pixmap[index]=pixdata.pix;
            x++;
        }
        x=0;
        y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const unsigned char* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_px_size.h){DEST.end_y=dest_px_size.h;}
    if(DEST.end_x>dest_px_size.w){DEST.end_x=dest_px_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;

    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;

            dest_pixmap[DEST_index] = orig_pixmap[ORIG_index];

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const unsigned char* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_px_size.h){DEST.end_y=dest_px_size.h;}
    if(DEST.end_x>dest_px_size.w){DEST.end_x=dest_px_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;

    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;

            dest_pixmap[DEST_index] = return_uint32_from_pix(orig_pixmap[ORIG_index]);

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY CONST PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    PIXDATA color_pix;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_px_size.h){DEST.end_y=dest_px_size.h;}
    if(DEST.end_x>dest_px_size.w){DEST.end_x=dest_px_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;

    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;

            color_pix.pix = orig_pixmap[ORIG_index];
            dest_pixmap[DEST_index]=return_uchar_from_pix(color_pix);

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY CONST PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_px_size.h){DEST.end_y=dest_px_size.h;}
    if(DEST.end_x>dest_px_size.w){DEST.end_x=dest_px_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;

    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;
            dest_pixmap[DEST_index]=orig_pixmap[ORIG_index];

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(PIXDATA* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_px_size.h){DEST.end_y=dest_px_size.h;}
    if(DEST.end_x>dest_px_size.w){DEST.end_x=dest_px_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;
    //MARKER: make sure this funciton is working well
    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;

            dest_pixmap[DEST_index]=orig_pixmap[ORIG_index].pix;

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }

}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(PIXDATA_FLAG* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>dest_px_size.h){DEST.end_y=dest_px_size.h;}
    if(DEST.end_x>dest_px_size.w){DEST.end_x=dest_px_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;
    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;
            if(orig_pixmap[ORIG_index].flag)
            {
                dest_pixmap[DEST_index]=orig_pixmap[ORIG_index].pixdata.pix;
            }
            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }

}
//-------------- Here you have paste pixmap using BOUND_RECT instead of SOIL_RECT
//-------------- I created these for SoilText::draw()
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,unsigned char* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    POINT ORIG;
    POINT DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;


    if(orig_bound.start_y<0){orig_bound.start_y=0;}
    if(orig_bound.start_x<0){orig_bound.start_x=0;}
    if(orig_bound.end_y>orig_px_size.h){orig_bound.end_y=orig_px_size.h;}
    if(orig_bound.end_x>orig_px_size.w){orig_bound.end_x=orig_px_size.w;}

    if(dest_bound.start_y<0){dest_bound.start_y=0;}
    if(dest_bound.start_x<0){dest_bound.start_x=0;}
    if(dest_bound.end_y>dest_px_size.h){dest_bound.end_y=dest_px_size.h;}
    if(dest_bound.end_x>dest_px_size.w){dest_bound.end_x=dest_px_size.w;}

    ORIG.y=orig_bound.start_y;
    ORIG.x=orig_bound.start_x;

    DEST.y=dest_bound.start_y;
    DEST.x=dest_bound.start_x;

    while(ORIG.y<orig_bound.end_y && DEST.y<dest_bound.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<orig_bound.end_x && DEST.x<dest_bound.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;
            dest_pixmap[DEST_index]=orig_pixmap[ORIG_index];

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=orig_bound.start_x;
        DEST.x=dest_bound.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,Uint32* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    POINT ORIG;
    POINT DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;


    if(orig_bound.start_y<0){orig_bound.start_y=0;}
    if(orig_bound.start_x<0){orig_bound.start_x=0;}
    if(orig_bound.end_y>orig_px_size.h){orig_bound.end_y=orig_px_size.h;}
    if(orig_bound.end_x>orig_px_size.w){orig_bound.end_x=orig_px_size.w;}

    if(dest_bound.start_y<0){dest_bound.start_y=0;}
    if(dest_bound.start_x<0){dest_bound.start_x=0;}
    if(dest_bound.end_y>dest_px_size.h){dest_bound.end_y=dest_px_size.h;}
    if(dest_bound.end_x>dest_px_size.w){dest_bound.end_x=dest_px_size.w;}

    ORIG.y=orig_bound.start_y;
    ORIG.x=orig_bound.start_x;

    DEST.y=dest_bound.start_y;
    DEST.x=dest_bound.start_x;

    while(ORIG.y<orig_bound.end_y && DEST.y<dest_bound.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<orig_bound.end_x && DEST.x<dest_bound.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;

            dest_pixmap[DEST_index] = return_uint32_from_pix(orig_pixmap[ORIG_index]);


            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=orig_bound.start_x;
        DEST.x=dest_bound.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(Uint32* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,unsigned char* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    POINT ORIG;
    POINT DEST;

    PIXDATA color_pix;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;


    if(orig_bound.start_y<0){orig_bound.start_y=0;}
    if(orig_bound.start_x<0){orig_bound.start_x=0;}
    if(orig_bound.end_y>orig_px_size.h){orig_bound.end_y=orig_px_size.h;}
    if(orig_bound.end_x>orig_px_size.w){orig_bound.end_x=orig_px_size.w;}

    if(dest_bound.start_y<0){dest_bound.start_y=0;}
    if(dest_bound.start_x<0){dest_bound.start_x=0;}
    if(dest_bound.end_y>dest_px_size.h){dest_bound.end_y=dest_px_size.h;}
    if(dest_bound.end_x>dest_px_size.w){dest_bound.end_x=dest_px_size.w;}

    ORIG.y=orig_bound.start_y;
    ORIG.x=orig_bound.start_x;

    DEST.y=dest_bound.start_y;
    DEST.x=dest_bound.start_x;

    while(ORIG.y<orig_bound.end_y && DEST.y<dest_bound.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<orig_bound.end_x && DEST.x<dest_bound.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;

            color_pix.pix = orig_pixmap[ORIG_index];
            dest_pixmap[DEST_index]=return_uchar_from_pix(color_pix);


            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=orig_bound.start_x;
        DEST.x=dest_bound.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(Uint32* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,Uint32* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    POINT ORIG;
    POINT DEST;


    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;


    if(orig_bound.start_y<0){orig_bound.start_y=0;}
    if(orig_bound.start_x<0){orig_bound.start_x=0;}
    if(orig_bound.end_y>orig_px_size.h){orig_bound.end_y=orig_px_size.h;}
    if(orig_bound.end_x>orig_px_size.w){orig_bound.end_x=orig_px_size.w;}

    if(dest_bound.start_y<0){dest_bound.start_y=0;}
    if(dest_bound.start_x<0){dest_bound.start_x=0;}
    if(dest_bound.end_y>dest_px_size.h){dest_bound.end_y=dest_px_size.h;}
    if(dest_bound.end_x>dest_px_size.w){dest_bound.end_x=dest_px_size.w;}

    ORIG.y=orig_bound.start_y;
    ORIG.x=orig_bound.start_x;

    DEST.y=dest_bound.start_y;
    DEST.x=dest_bound.start_x;

    while(ORIG.y<orig_bound.end_y && DEST.y<dest_bound.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*dest_px_size.w;

        while(ORIG.x<orig_bound.end_x && DEST.x<dest_bound.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;
            dest_pixmap[DEST_index]=orig_pixmap[ORIG_index];

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=orig_bound.start_x;
        DEST.x=dest_bound.start_x;

        ORIG.y++;
        DEST.y++;
    }
}
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP IN CERTAIN POSITION
void paste_pixmap_to_pixmap_in_pos(const Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect, int pos)
{
    if(orig_pixmap == NULL || dest_pixmap == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps
    adjust_rect_to_size(orig_px_size, &orig_rect);
    adjust_rect_to_size(dest_px_size, &dest_rect);
    
    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;

    POINT ORIG_INC;

    int orig_pre_index;
    int orig_index;
    int dest_pre_index;
    int dest_index;

    //position changes the order the pixels will be taken from orig_pixmap,
    //it will always paste to the top left of the dest area regardless of the position


    DEST.start_x = dest_rect.x;
    DEST.start_y = dest_rect.y;
    DEST.end_x = dest_rect.x + dest_rect.w;
    DEST.end_y = dest_rect.y + dest_rect.h;
    DEST.x = DEST.start_x;
    DEST.y = DEST.start_y;

    bool y_loop_outside = true;


    // ------------------------------ NORMAL / same pos as in orig
    if(pos == 0)
    {
        ORIG.start_x = orig_rect.x;
        ORIG.start_y = orig_rect.y;
        ORIG.end_x = orig_rect.x + orig_rect.w;
        ORIG.end_y = orig_rect.y + orig_rect.h;
        ORIG.x = ORIG.start_x;
        ORIG.y = ORIG.start_y;

        ORIG_INC.x = 1;
        ORIG_INC.y = 1;
    }
    // ------------------------------ 90 degrees CLOCKWISE
    if(pos == 1)
    {
        //invert loop
        y_loop_outside = false;
        ORIG.start_x = orig_rect.x;
        ORIG.start_y = (orig_rect.y + orig_rect.h) - 1;//minus 1 to adjust limit
        ORIG.end_x = orig_rect.x + orig_rect.w;
        ORIG.end_y = orig_rect.y - 1;
        ORIG.x = ORIG.start_x;
        ORIG.y = ORIG.start_y;

        ORIG_INC.x = 1;
        ORIG_INC.y = (-1);
    }
    // ------------------------------ 180 degrees
    if(pos == 2)
    {
        ORIG.start_x = (orig_rect.x + orig_rect.w) - 1;
        ORIG.start_y = (orig_rect.y + orig_rect.h) - 1;
        ORIG.end_x = orig_rect.x - 1;
        ORIG.end_y = orig_rect.y - 1;
        ORIG.x = ORIG.start_x;
        ORIG.y = ORIG.start_y;

        ORIG_INC.x = (-1);
        ORIG_INC.y = (-1);
    }
    // ------------------------------ 90 degrees COUNTER CLOCKWISE
    if(pos == 3)
    {
        //invert loop
        y_loop_outside = false;
        ORIG.start_x = (orig_rect.x + orig_rect.w) - 1;
        ORIG.start_y = orig_rect.y;
        ORIG.end_x = orig_rect.x - 1;
        ORIG.end_y = orig_rect.y + orig_rect.h;
        ORIG.x = ORIG.start_x;
        ORIG.y = ORIG.start_y;

        ORIG_INC.x = (-1);
        ORIG_INC.y = 1;
    }
    // ------------------------------ invert horizontally
    if(pos == 4)
    {
        ORIG.start_x = (orig_rect.x + orig_rect.w) - 1;
        ORIG.start_y = orig_rect.y;
        ORIG.end_x = orig_rect.x - 1;
        ORIG.end_y = orig_rect.y + orig_rect.h;
        ORIG.x = ORIG.start_x;
        ORIG.y = ORIG.start_y;

        ORIG_INC.x = (-1);
        ORIG_INC.y = 1;
    }
    // ------------------------------ invert vertically
    if(pos == 5)
    {
        ORIG.start_x = orig_rect.x;
        ORIG.start_y = (orig_rect.y + orig_rect.h) - 1;
        ORIG.end_x = orig_rect.x + orig_rect.w;
        ORIG.end_y = orig_rect.y - 1;
        ORIG.x = ORIG.start_x;
        ORIG.y = ORIG.start_y;

        ORIG_INC.x = 1;
        ORIG_INC.y = (-1);
    }

    int orig_err = 0;
    int dest_err = 0;

    if(y_loop_outside)
    {
        while(ORIG.y != ORIG.end_y && DEST.y < DEST.end_y)
        {
            orig_pre_index = ORIG.y * orig_px_size.w;
            dest_pre_index = DEST.y * dest_px_size.w;
            while(ORIG.x != ORIG.end_x && DEST.x < DEST.end_x)
            {
                orig_index = orig_pre_index + ORIG.x;
                dest_index = dest_pre_index + DEST.x;

                dest_pixmap[dest_index] = orig_pixmap[orig_index];
                
                ORIG.x += ORIG_INC.x;
                DEST.x += 1;
            }
            ORIG.x = ORIG.start_x;
            ORIG.y += ORIG_INC.y;

            DEST.x = DEST.start_x;
            DEST.y += 1;
        }

    }else{
        while(ORIG.x != ORIG.end_x && DEST.y < DEST.end_y)
        {
            dest_pre_index = DEST.y * dest_px_size.w;
            while(ORIG.y != ORIG.end_y && DEST.x < DEST.end_x)
            {
                orig_index = (ORIG.y * orig_px_size.w) + ORIG.x;
                dest_index = dest_pre_index + DEST.x;

                dest_pixmap[dest_index] = orig_pixmap[orig_index];
                ORIG.y += ORIG_INC.y;
                DEST.x += 1;
            }
            ORIG.x += ORIG_INC.x;
            ORIG.y = ORIG.start_y;

            DEST.x = DEST.start_x;
            DEST.y += 1;
        }
    }
    printf("\norig err: %d", orig_err);
    printf("\ndest err: %d", dest_err);
    printf("\n.");

}
// ------------------------------------------------------ COPY PIXMAP TO SDL SURFACE
void paste_pixmap_to_sdl_surface(Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,SDL_Surface* sdl_surface, SIZE surface_size, SOIL_RECT dest_rect)
{
    if(orig_pixmap == NULL || sdl_surface == NULL){return;}
    //the orig_rect indicates the area to be copied
    //the dest_rect indicates the area to be pasted to and its limits
    //px_size values are the size to the entire pixmaps


    Uint32 *surface_pixels = (Uint32 *)sdl_surface->pixels;

    DRAW_POINTS ORIG;
    DRAW_POINTS DEST;

    long ORIG_pre_index;
    long ORIG_index;

    long DEST_pre_index;
    long DEST_index;

    ORIG.start_y=orig_rect.y;
    ORIG.start_x=orig_rect.x;
    ORIG.end_y=ORIG.start_y+orig_rect.h;
    ORIG.end_x=ORIG.start_x+orig_rect.w;

    DEST.start_y=dest_rect.y;
    DEST.start_x=dest_rect.x;
    DEST.end_y=DEST.start_y+dest_rect.h;
    DEST.end_x=DEST.start_x+dest_rect.w;

    if(ORIG.start_y<0){ORIG.start_y=0;}
    if(ORIG.start_x<0){ORIG.start_x=0;}
    if(ORIG.end_y>orig_px_size.h){ORIG.end_y=orig_px_size.h;}
    if(ORIG.end_x>orig_px_size.w){ORIG.end_x=orig_px_size.w;}

    if(DEST.start_y<0){DEST.start_y=0;}
    if(DEST.start_x<0){DEST.start_x=0;}
    if(DEST.end_y>surface_size.h){DEST.end_y=surface_size.h;}
    if(DEST.end_x>surface_size.w){DEST.end_x=surface_size.w;}

    ORIG.y=ORIG.start_y;
    ORIG.x=ORIG.start_x;

    DEST.y=DEST.start_y;
    DEST.x=DEST.start_x;

    while(ORIG.y<ORIG.end_y && DEST.y<DEST.end_y)
    {
        ORIG_pre_index=ORIG.y*orig_px_size.w;
        DEST_pre_index=DEST.y*surface_size.w;

        while(ORIG.x<ORIG.end_x && DEST.x<DEST.end_x)
        {
            ORIG_index=ORIG_pre_index+ORIG.x;
            DEST_index=DEST_pre_index+DEST.x;
            surface_pixels[DEST_index]=orig_pixmap[ORIG_index];
            //MARKER: apparently this goes wrong when saving this SDL_Surface to a file with IMG_SavePNG

            ORIG.x++;
            DEST.x++;
        }
        ORIG.x=ORIG.start_x;
        DEST.x=DEST.start_x;

        ORIG.y++;
        DEST.y++;
    }
}

// ------------------------------------------------------ LOAD PIXMAP FROM BMP 32 FILE
// it returns the the amount of pixel copied
// file must be opened
int load_pixmap_from_bmp_32_file(Uint32* pixmap, SIZE px_size,FILE* file)
{
    if(file==NULL && file==nullptr)
    {
        return -1;
    }

    int px_total_size=px_size.w*px_size.h;

    fseek(file, 0, SEEK_END);
    long file_size=ftell(file);
    fseek(file, 54, SEEK_SET);

    Uint32 pixel=0;


    int ch=0;
    int byte_c=0;
    int pixel_c=0;
    while(pixel_c<px_total_size && byte_c<file_size)
    {
        ch=fgetc(file);
        //alpha
        if(byte_c%4==3)
        {
            pixel+=16777216*ch;

        }
        //red
        if(byte_c%4==2)
        {
            pixel+=65536*ch;
        }
        //green
        if(byte_c%4==1)
        {
            pixel+=256*ch;
        }
        //blue
        if(byte_c%4==0)
        {
            pixel=ch;
        }

        if(byte_c%4==3)
        {
            pixmap[pixel_c]=pixel;
            pixel=0;
            pixel_c++;
        }
        byte_c++;
    }


    return 0;
}
// ------------------------------------------------------ ROUND DOUBLE
//if you simply typecast the double to int it simply loses the fractional part
//2.8 becomes 2, so this function rounds it to the closest integer
int round_double(double num)
{
    if(num > 0)
    {
        int integer = (int)num;
        double bigger_double = integer + 1;
        if((bigger_double - num) < 0.5)
        {
            return integer + 1;
        }else{
            return integer;
        }
    }else{
        int integer = (int)num;
        double bigger_double = integer - 1;
        double dif = bigger_double - num;
        if((bigger_double - num) > (-0.5))
        {
            return integer - 1;
        }else{
            return integer;
        }
    }
}

// ------------------------------------------------------ TRUNCATE DOUBLE
// works if num is negative
double trunc_double(double num,int dec_points)
{
    if(dec_points<0){dec_points=0;}
    if(dec_points>8){dec_points=8;}

    double ret;
    long div=1;
    while(dec_points>0)
    {
        div*=10;
        dec_points--;
    }

    long long_num=num*div;
    ret = (double)long_num/div;


    return ret;
}
// ------------------------------------------------------ DOUBLE MODULUS
//divide integer part of double by int and returns rest plus decimals
// it works if any or both of the arguments are negative
double double_modulus(double num, int div)
{
    if(div==0){return (double) (-1);}

    double ret;
    long long_num=(long)num;
    num-=long_num;
    long_num%=div;
    ret=(double)long_num+num;
    return ret;
}
// ------------------------------------------------------ DOUBLE MODULUS
//divide double by double to find integer quocient and returns rest
// it works if any or both of the arguments are negative
double double_modulus(double num, double div)
{
    if(div==0){return (double) (-1);}

    double quo=num/div;
    quo=trunc_double(quo, 0);
    return num-(quo*div);
}
// ------------------------------------------------------ THE DOUBLE TO NEXT INTEGER
// returns the difference to the next integer
// crescent indicates if the next int is in crecent or decrescent order
double double_to_next_int(double num, bool crescent)
{

    double dec=double_modulus(num, 1);

    if(crescent)
    {
        if(num>0)
        {
            return 1-dec;
        }else{
            if(dec==0)
            {
                return 1;
            }else{
                return dec*(-1);
            }
        }
    }else{
        if(num>0)
        {
            if(dec==0)
            {
                return (-1);
            }else{
                return dec*(-1);
            }
        }else{
            return (-1)-dec;
        }
    }
}

// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    // --------------------------------- VALIDATION
    if(orig_pixmap == NULL){return;}
    if(dest_pixmap == NULL){return;}

    SOIL_RECT orig_size_rect;
    orig_size_rect.x = 0;
    orig_size_rect.y = 0;
    orig_size_rect.w = orig_px_size.w;
    orig_size_rect.h = orig_px_size.h;

    orig_rect = rect_intersection(orig_size_rect,orig_rect);

    SOIL_RECT dest_size_rect;
    dest_size_rect.x = 0;
    dest_size_rect.y = 0;
    dest_size_rect.w = dest_px_size.w;
    dest_size_rect.h = dest_px_size.h;

    dest_rect = rect_intersection(dest_size_rect,dest_rect);

    if(orig_rect.w == 0 || orig_rect.h == 0 || dest_rect.w == 0 || dest_rect.h == 0)
    {
        return;
    }
    // --------------------------------

    // --------------------------------
    int e=0;

    D_POINT ratio;
    D_POINT inv_ratio;

    ratio.x = (double)orig_rect.w / dest_rect.w;
    ratio.y = (double)orig_rect.h / dest_rect.h;

    inv_ratio.x = 1/ratio.x;
    inv_ratio.y = 1/ratio.y;


    trunc_double(ratio.x,5);
    trunc_double(ratio.y,5);

    D_POINT SEG;
    double SEG_AREA;

    POINT ORIG; // for defining the int value or the ORIG_D coord
    DRAW_D_POINTS ORIG_D;

    ORIG_D.start_x = orig_rect.x;
    ORIG_D.start_y = orig_rect.y;


    DRAW_POINTS DEST;

    DEST.start_x = dest_rect.x;
    DEST.start_y = dest_rect.y;

    DEST.end_x = dest_rect.x + dest_rect.w;
    DEST.end_y = dest_rect.y + dest_rect.h;

    DEST.x = DEST.start_x;
    DEST.y = DEST.start_y;

    double PIX_CALC;//sum the ORIG colors in here
    unsigned char PIX;

    int orig_pre_index;
    int orig_index;

    int dest_pre_index;
    int dest_index;

    // ---------------------------------

    //1 LOOP THROUGH DEST PIXELS
    while(DEST.y < DEST.end_y)
    {

        dest_pre_index = DEST.y * dest_px_size.w;
        while(DEST.x < DEST.end_x)
        {

            dest_index = dest_pre_index + DEST.x;

            PIX_CALC = 0;

            ORIG_D.x = ORIG_D.start_x;
            ORIG_D.y = ORIG_D.start_y;
            ORIG_D.end_x = ORIG_D.start_x + ratio.x;
            ORIG_D.end_y = ORIG_D.start_y + ratio.y;

            while(ORIG_D.y < ORIG_D.end_y)
            {
                ORIG.y = ORIG_D.y;
                if(ORIG.y >= orig_px_size.h){break;}

                orig_pre_index = ORIG.y * orig_px_size.w;

                SEG.y = (double)ORIG.y + 1;
                if(SEG.y > ORIG_D.end_y)
                {
                    SEG.y = ORIG_D.end_y;
                }
                SEG.y -= ORIG_D.y;


                while(ORIG_D.x < ORIG_D.end_x)
                {


                    ORIG.x = ORIG_D.x;
                    if(ORIG.x >= orig_px_size.w){break;}

                    orig_index = orig_pre_index + ORIG.x;

                    SEG.x = (double)ORIG.x + 1;
                    if(SEG.x > ORIG_D.end_x)
                    {
                        SEG.x = ORIG_D.end_x;
                    }
                    SEG.x -= ORIG_D.x;



                    SEG_AREA = (SEG.y*inv_ratio.y) * (SEG.x*inv_ratio.x);

                    PIX = orig_pixmap[orig_index];
                    PIX_CALC += (double)SEG_AREA * PIX;

                    ORIG_D.x += SEG.x;
                }

                ORIG_D.x = ORIG_D.start_x;
                ORIG_D.y += SEG.y;
            }



            PIX = PIX_CALC;


            dest_pixmap[dest_index] = PIX;

        //2 END LOOP
            ORIG_D.start_x += ratio.x;
            DEST.x++;
        }
        ORIG_D.start_x = orig_rect.x;
        ORIG_D.start_y += ratio.y;

        DEST.x = DEST.start_x;
        DEST.y++;
    }
    
    //1 END LOOP
}

// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    // --------------------------------- VALIDATION
    if(orig_pixmap == NULL){return;}
    if(dest_pixmap == NULL){return;}

    SOIL_RECT orig_size_rect;
    orig_size_rect.x = 0;
    orig_size_rect.y = 0;
    orig_size_rect.w = orig_px_size.w;
    orig_size_rect.h = orig_px_size.h;

    orig_rect = rect_intersection(orig_size_rect,orig_rect);

    SOIL_RECT dest_size_rect;
    dest_size_rect.x = 0;
    dest_size_rect.y = 0;
    dest_size_rect.w = dest_px_size.w;
    dest_size_rect.h = dest_px_size.h;

    dest_rect = rect_intersection(dest_size_rect,dest_rect);

    if(orig_rect.w == 0 || orig_rect.h == 0 || dest_rect.w == 0 || dest_rect.h == 0)
    {
        return;
    }
    // --------------------------------

    // --------------------------------
    int e=0;

    D_POINT ratio;
    D_POINT inv_ratio;

    ratio.x = (double)orig_rect.w / dest_rect.w;
    ratio.y = (double)orig_rect.h / dest_rect.h;

    inv_ratio.x = 1/ratio.x;
    inv_ratio.y = 1/ratio.y;


    trunc_double(ratio.x,5);
    trunc_double(ratio.y,5);

    D_POINT SEG;
    double SEG_AREA;

    POINT ORIG; // for defining the int value or the ORIG_D coord
    DRAW_D_POINTS ORIG_D;

    ORIG_D.start_x = orig_rect.x;
    ORIG_D.start_y = orig_rect.y;


    DRAW_POINTS DEST;

    DEST.start_x = dest_rect.x;
    DEST.start_y = dest_rect.y;

    DEST.end_x = dest_rect.x + dest_rect.w;
    DEST.end_y = dest_rect.y + dest_rect.h;

    DEST.x = DEST.start_x;
    DEST.y = DEST.start_y;

    CALC_PIX_COLOR PIX_CALC;//sum the ORIG colors in here
    PIXDATA PIX;
    unsigned char ORIG_PIX;

    int orig_pre_index;
    int orig_index;

    int dest_pre_index;
    int dest_index;

    // ---------------------------------

    //1 LOOP THROUGH DEST PIXELS
    while(DEST.y < DEST.end_y)
    {

        dest_pre_index = DEST.y * dest_px_size.w;
        while(DEST.x < DEST.end_x)
        {

            dest_index = dest_pre_index + DEST.x;

            PIX_CALC.r = 0;
            PIX_CALC.g = 0;
            PIX_CALC.b = 0;
            PIX_CALC.a = 0;



            ORIG_D.x = ORIG_D.start_x;
            ORIG_D.y = ORIG_D.start_y;
            ORIG_D.end_x = ORIG_D.start_x + ratio.x;
            ORIG_D.end_y = ORIG_D.start_y + ratio.y;

            while(ORIG_D.y < ORIG_D.end_y)
            {
                ORIG.y = ORIG_D.y;
                if(ORIG.y >= orig_px_size.h){break;}

                orig_pre_index = ORIG.y * orig_px_size.w;

                SEG.y = (double)ORIG.y + 1;
                if(SEG.y > ORIG_D.end_y)
                {
                    SEG.y = ORIG_D.end_y;
                }
                SEG.y -= ORIG_D.y;


                while(ORIG_D.x < ORIG_D.end_x)
                {


                    ORIG.x = ORIG_D.x;
                    if(ORIG.x >= orig_px_size.w){break;}

                    orig_index = orig_pre_index + ORIG.x;

                    SEG.x = (double)ORIG.x + 1;
                    if(SEG.x > ORIG_D.end_x)
                    {
                        SEG.x = ORIG_D.end_x;
                    }
                    SEG.x -= ORIG_D.x;



                    SEG_AREA = (SEG.y*inv_ratio.y) * (SEG.x*inv_ratio.x);

                    ORIG_PIX = orig_pixmap[orig_index];
                    PIX_CALC.r += (double)SEG_AREA * ORIG_PIX;
                    PIX_CALC.g += (double)SEG_AREA * ORIG_PIX;
                    PIX_CALC.b += (double)SEG_AREA * ORIG_PIX;
                    PIX_CALC.a += (double)SEG_AREA * 255;


                    ORIG_D.x += SEG.x;
                }

                ORIG_D.x = ORIG_D.start_x;
                ORIG_D.y += SEG.y;
            }



            PIX.color.r = PIX_CALC.r;
            PIX.color.g = PIX_CALC.g;
            PIX.color.b = PIX_CALC.b;
            PIX.color.a = PIX_CALC.a;


            dest_pixmap[dest_index] = PIX.pix;

        //2 END LOOP
            ORIG_D.start_x += ratio.x;
            DEST.x++;
        }
        ORIG_D.start_x = orig_rect.x;
        ORIG_D.start_y += ratio.y;

        DEST.x = DEST.start_x;
        DEST.y++;
    }
    
    //1 END LOOP
}

// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(Uint32* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{
    // --------------------------------- VALIDATION
    if(orig_pixmap == NULL){return;}
    if(dest_pixmap == NULL){return;}

    SOIL_RECT orig_size_rect;
    orig_size_rect.x = 0;
    orig_size_rect.y = 0;
    orig_size_rect.w = orig_px_size.w;
    orig_size_rect.h = orig_px_size.h;

    orig_rect = rect_intersection(orig_size_rect,orig_rect);

    SOIL_RECT dest_size_rect;
    dest_size_rect.x = 0;
    dest_size_rect.y = 0;
    dest_size_rect.w = dest_px_size.w;
    dest_size_rect.h = dest_px_size.h;

    dest_rect = rect_intersection(dest_size_rect,dest_rect);

    if(orig_rect.w == 0 || orig_rect.h == 0 || dest_rect.w == 0 || dest_rect.h == 0)
    {
        return;
    }
    // --------------------------------

    // --------------------------------
    int e=0;

    D_POINT ratio;
    D_POINT inv_ratio;

    ratio.x = (double)orig_rect.w / dest_rect.w;
    ratio.y = (double)orig_rect.h / dest_rect.h;

    inv_ratio.x = 1/ratio.x;
    inv_ratio.y = 1/ratio.y;


    trunc_double(ratio.x,5);
    trunc_double(ratio.y,5);

    D_POINT SEG;
    double SEG_AREA;

    POINT ORIG; // for defining the int value or the ORIG_D coord
    DRAW_D_POINTS ORIG_D;

    ORIG_D.start_x = orig_rect.x;
    ORIG_D.start_y = orig_rect.y;


    DRAW_POINTS DEST;

    DEST.start_x = dest_rect.x;
    DEST.start_y = dest_rect.y;

    DEST.end_x = dest_rect.x + dest_rect.w;
    DEST.end_y = dest_rect.y + dest_rect.h;

    DEST.x = DEST.start_x;
    DEST.y = DEST.start_y;

    CALC_PIX_COLOR PIX_CALC;//sum the ORIG colors in here
    PIXDATA PIX;

    int orig_pre_index;
    int orig_index;

    int dest_pre_index;
    int dest_index;

    // ---------------------------------

    //1 LOOP THROUGH DEST PIXELS
    while(DEST.y < DEST.end_y)
    {

        dest_pre_index = DEST.y * dest_px_size.w;
        while(DEST.x < DEST.end_x)
        {

            dest_index = dest_pre_index + DEST.x;

            PIX_CALC.r = 0;
            PIX_CALC.g = 0;
            PIX_CALC.b = 0;
            PIX_CALC.a = 0;



            ORIG_D.x = ORIG_D.start_x;
            ORIG_D.y = ORIG_D.start_y;
            ORIG_D.end_x = ORIG_D.start_x + ratio.x;
            ORIG_D.end_y = ORIG_D.start_y + ratio.y;

            while(ORIG_D.y < ORIG_D.end_y)
            {
                ORIG.y = ORIG_D.y;
                if(ORIG.y >= orig_px_size.h){break;}

                orig_pre_index = ORIG.y * orig_px_size.w;

                SEG.y = (double)ORIG.y + 1;
                if(SEG.y > ORIG_D.end_y)
                {
                    SEG.y = ORIG_D.end_y;
                }
                SEG.y -= ORIG_D.y;


                while(ORIG_D.x < ORIG_D.end_x)
                {


                    ORIG.x = ORIG_D.x;
                    if(ORIG.x >= orig_px_size.w){break;}

                    orig_index = orig_pre_index + ORIG.x;

                    SEG.x = (double)ORIG.x + 1;
                    if(SEG.x > ORIG_D.end_x)
                    {
                        SEG.x = ORIG_D.end_x;
                    }
                    SEG.x -= ORIG_D.x;



                    SEG_AREA = (SEG.y*inv_ratio.y) * (SEG.x*inv_ratio.x);

                    PIX.pix = orig_pixmap[orig_index];
                    PIX_CALC.r += (double)SEG_AREA * PIX.color.r;
                    PIX_CALC.g += (double)SEG_AREA * PIX.color.g;
                    PIX_CALC.b += (double)SEG_AREA * PIX.color.b;
                    PIX_CALC.a += (double)SEG_AREA * PIX.color.a;


                    ORIG_D.x += SEG.x;
                }

                ORIG_D.x = ORIG_D.start_x;
                ORIG_D.y += SEG.y;
            }



            PIX.color.r = PIX_CALC.r;
            PIX.color.g = PIX_CALC.g;
            PIX.color.b = PIX_CALC.b;
            PIX.color.a = PIX_CALC.a;


            dest_pixmap[dest_index] = return_uchar_from_pix(PIX);

        //2 END LOOP
            ORIG_D.start_x += ratio.x;
            DEST.x++;
        }
        ORIG_D.start_x = orig_rect.x;
        ORIG_D.start_y += ratio.y;

        DEST.x = DEST.start_x;
        DEST.y++;
    }
    //1 END LOOP
    
}

// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect)
{

    // --------------------------------- VALIDATION
    if(orig_pixmap == NULL){return;}
    if(dest_pixmap == NULL){return;}

    SOIL_RECT orig_size_rect;
    orig_size_rect.x = 0;
    orig_size_rect.y = 0;
    orig_size_rect.w = orig_px_size.w;
    orig_size_rect.h = orig_px_size.h;

    orig_rect = rect_intersection(orig_size_rect,orig_rect);

    SOIL_RECT dest_size_rect;
    dest_size_rect.x = 0;
    dest_size_rect.y = 0;
    dest_size_rect.w = dest_px_size.w;
    dest_size_rect.h = dest_px_size.h;

    dest_rect = rect_intersection(dest_size_rect,dest_rect);

    if(orig_rect.w == 0 || orig_rect.h == 0 || dest_rect.w == 0 || dest_rect.h == 0)
    {
        return;
    }
    // --------------------------------

    // --------------------------------
    int e=0;

    D_POINT ratio;
    D_POINT inv_ratio;

    ratio.x = (double)orig_rect.w / dest_rect.w;
    ratio.y = (double)orig_rect.h / dest_rect.h;

    inv_ratio.x = 1/ratio.x;
    inv_ratio.y = 1/ratio.y;


    trunc_double(ratio.x,5);
    trunc_double(ratio.y,5);

    D_POINT SEG;
    double SEG_AREA;

    POINT ORIG; // for defining the int value or the ORIG_D coord
    DRAW_D_POINTS ORIG_D;

    ORIG_D.start_x = orig_rect.x;
    ORIG_D.start_y = orig_rect.y;


    DRAW_POINTS DEST;

    DEST.start_x = dest_rect.x;
    DEST.start_y = dest_rect.y;

    DEST.end_x = dest_rect.x + dest_rect.w;
    DEST.end_y = dest_rect.y + dest_rect.h;

    DEST.x = DEST.start_x;
    DEST.y = DEST.start_y;

    CALC_PIX_COLOR PIX_CALC;//sum the ORIG colors in here
    PIXDATA PIX;

    int orig_pre_index;
    int orig_index;

    int dest_pre_index;
    int dest_index;

    // ---------------------------------

    //1 LOOP THROUGH DEST PIXELS
    while(DEST.y < DEST.end_y)
    {

        dest_pre_index = DEST.y * dest_px_size.w;
        while(DEST.x < DEST.end_x)
        {

            dest_index = dest_pre_index + DEST.x;

            PIX_CALC.r = 0;
            PIX_CALC.g = 0;
            PIX_CALC.b = 0;
            PIX_CALC.a = 0;



            ORIG_D.x = ORIG_D.start_x;
            ORIG_D.y = ORIG_D.start_y;
            ORIG_D.end_x = ORIG_D.start_x + ratio.x;
            ORIG_D.end_y = ORIG_D.start_y + ratio.y;

            while(ORIG_D.y < ORIG_D.end_y)
            {
                ORIG.y = ORIG_D.y;
                if(ORIG.y >= orig_px_size.h){break;}

                orig_pre_index = ORIG.y * orig_px_size.w;

                SEG.y = (double)ORIG.y + 1;
                if(SEG.y > ORIG_D.end_y)
                {
                    SEG.y = ORIG_D.end_y;
                }
                SEG.y -= ORIG_D.y;


                while(ORIG_D.x < ORIG_D.end_x)
                {


                    ORIG.x = ORIG_D.x;
                    if(ORIG.x >= orig_px_size.w){break;}

                    orig_index = orig_pre_index + ORIG.x;

                    SEG.x = (double)ORIG.x + 1;
                    if(SEG.x > ORIG_D.end_x)
                    {
                        SEG.x = ORIG_D.end_x;
                    }
                    SEG.x -= ORIG_D.x;



                    SEG_AREA = (SEG.y*inv_ratio.y) * (SEG.x*inv_ratio.x);

                    PIX.pix = orig_pixmap[orig_index];
                    PIX_CALC.r += (double)SEG_AREA * PIX.color.r;
                    PIX_CALC.g += (double)SEG_AREA * PIX.color.g;
                    PIX_CALC.b += (double)SEG_AREA * PIX.color.b;
                    PIX_CALC.a += (double)SEG_AREA * PIX.color.a;


                    ORIG_D.x += SEG.x;
                }

                ORIG_D.x = ORIG_D.start_x;
                ORIG_D.y += SEG.y;
            }



            PIX.color.r = PIX_CALC.r;
            PIX.color.g = PIX_CALC.g;
            PIX.color.b = PIX_CALC.b;
            PIX.color.a = PIX_CALC.a;


            dest_pixmap[dest_index] = PIX.pix;

        //2 END LOOP
            ORIG_D.start_x += ratio.x;
            DEST.x++;
        }
        ORIG_D.start_x = orig_rect.x;
        ORIG_D.start_y += ratio.y;

        DEST.x = DEST.start_x;
        DEST.y++;
    }
    //1 END LOOP

}
// -------------------------------------------------------------- RETURN SINE(no look-up table)
double return_sine(double angle)
{
    double res_sine;
    if(angle < 0){angle = 0;}
    if(angle > 90){angle = 90;}
    res_sine = sin(angle*PI/180);
    return res_sine;
}
// -------------------------------------------------------------- RETURN SINE
double return_sine(int angle)
{
    if(angle<1 || angle>89)
    {
        return -1;
    }
    double sine_arr[10];
    int div=angle/10;
    int ind=angle%10;
    if(div==0)
    {
        sine_arr[1]=0.0174;
        sine_arr[2]=0.0349;
        sine_arr[3]=0.0523;
        sine_arr[4]=0.0698;
        sine_arr[5]=0.0872;
        sine_arr[6]=0.1045;
        sine_arr[7]=0.1219;
        sine_arr[8]=0.1392;
        sine_arr[9]=0.1564;

        return sine_arr[ind];
    }
    if(div==1)
    {
        sine_arr[0]=0.1736;
        sine_arr[1]=0.1908;
        sine_arr[2]=0.2079;
        sine_arr[3]=0.2249;
        sine_arr[4]=0.2419;
        sine_arr[5]=0.2588;
        sine_arr[6]=0.2756;
        sine_arr[7]=0.2924;
        sine_arr[8]=0.3090;
        sine_arr[9]=0.3256;

        return sine_arr[ind];
    }
    if(div==2)
    {
        sine_arr[0]=0.3420;
        sine_arr[1]=0.3584;
        sine_arr[2]=0.3746;
        sine_arr[3]=0.3907;
        sine_arr[4]=0.4067;
        sine_arr[5]=0.4226;
        sine_arr[6]=0.4384;
        sine_arr[7]=0.4540;
        sine_arr[8]=0.4695;
        sine_arr[9]=0.4848;

        return sine_arr[ind];
    }
    if(div==3)
    {
        sine_arr[0]=0.5000;
        sine_arr[1]=0.5150;
        sine_arr[2]=0.5299;
        sine_arr[3]=0.5446;
        sine_arr[4]=0.5592;
        sine_arr[5]=0.5736;
        sine_arr[6]=0.5878;
        sine_arr[7]=0.6018;
        sine_arr[8]=0.6157;
        sine_arr[9]=0.6293;

        return sine_arr[ind];
    }
    if(div==4)
    {
        sine_arr[0]=0.6428;
        sine_arr[1]=0.6561;
        sine_arr[2]=0.6691;
        sine_arr[3]=0.6820;
        sine_arr[4]=0.6947;
        sine_arr[5]=0.7071;
        sine_arr[6]=0.7193;
        sine_arr[7]=0.7314;
        sine_arr[8]=0.7431;
        sine_arr[9]=0.7547;

        return sine_arr[ind];
    }
    if(div==5)
    {
        sine_arr[0]=0.7660;
        sine_arr[1]=0.7772;
        sine_arr[2]=0.7880;
        sine_arr[3]=0.7986;
        sine_arr[4]=0.8090;
        sine_arr[5]=0.8191;
        sine_arr[6]=0.8290;
        sine_arr[7]=0.8387;
        sine_arr[8]=0.8480;
        sine_arr[9]=0.8571;

        return sine_arr[ind];
    }
    if(div==6)
    {
        sine_arr[0]=0.8660;
        sine_arr[1]=0.8746;
        sine_arr[2]=0.8829;
        sine_arr[3]=0.8910;
        sine_arr[4]=0.8988;
        sine_arr[5]=0.9063;
        sine_arr[6]=0.9135;
        sine_arr[7]=0.9205;
        sine_arr[8]=0.9272;
        sine_arr[9]=0.9336;

        return sine_arr[ind];
    }
    if(div==7)
    {
        sine_arr[0]=0.9397;
        sine_arr[1]=0.9455;
        sine_arr[2]=0.9511;
        sine_arr[3]=0.9563;
        sine_arr[4]=0.9613;
        sine_arr[5]=0.9659;
        sine_arr[6]=0.9703;
        sine_arr[7]=0.9744;
        sine_arr[8]=0.9781;
        sine_arr[9]=0.9816;

        return sine_arr[ind];
    }
    if(div==8)
    {
        sine_arr[0]=0.9848;
        sine_arr[1]=0.9877;
        sine_arr[2]=0.9903;
        sine_arr[3]=0.9926;
        sine_arr[4]=0.9945;
        sine_arr[5]=0.9962;
        sine_arr[6]=0.9976;
        sine_arr[7]=0.9986;
        sine_arr[8]=0.9994;
        sine_arr[9]=0.9998;

        return sine_arr[ind];
    }
    return -1;
}

// -------------------------------------------------------------- POSITIVE
double turn_positive(double num)
{
    if(num<0){num*=(-1);}
    return num;
}
// -------------------------------------------------------------- POSITIVE
int turn_positive(int num)
{
    if(num<0){num*=(-1);}
    return num;
}
// -------------------------------------------------------------- SQUARE ROOT
//precision is defined in binary reduction from the original range
//every 10 of precision is the equivalent of 1024 times more precise
//the range is defined with up_lim and mn_lim
double square_root(double num,int precision)
{
    num=turn_positive(num);
    if(num==0){return 0;}
    if(num==1){return 1;}

    double try_root;
    double dif;
    double squared;

    double up_lim;
    double mn_lim;

    int control=0;

    if(num<1)
    {
        up_lim=1;
        mn_lim=num;
    }
    if(num>1)
    {
        up_lim=num;
        mn_lim=1;
    }

    if(precision<20){precision=20;}
    if(precision>100){precision=100;}

    while(control<precision)
    {
        dif=up_lim-mn_lim;
        dif/=2;
        try_root=mn_lim+dif;
        squared=try_root*try_root;

        if(squared<num)
        {
            mn_lim=try_root;
            control++;
            continue;
        }
        if(squared>num)
        {
            up_lim=try_root;
            control++;
            continue;
        }
        if(squared==num)
        {
            break;
        }

        control++;
    }
    return try_root;
}
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(POINT START,POINT END,SIZE* SIDE)
{
    SIDE->w=END.x-START.x;
    SIDE->h=END.y-START.y;
}
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(POINT START,POINT END,D_SIZE* SIDE)
{
    SIDE->w=END.x-START.x;
    SIDE->h=END.y-START.y;
}
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(D_POINT START,D_POINT END,SIZE* SIDE)
{
    SIDE->w=END.x-START.x;
    SIDE->h=END.y-START.y;
}
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(D_POINT START,D_POINT END,D_SIZE* SIDE)
{
    SIDE->w=END.x-START.x;
    SIDE->h=END.y-START.y;
}
// -------------------------------------------------------------- GET HYPOTENUSE
//always positive
double get_hypotenuse(double cat_1,double cat_2,int precision)
{
    double hyp=(cat_1*cat_1)+(cat_2*cat_2);
    hyp=square_root(hyp,precision);
    return hyp;
}

// -------------------------------------------------------------- GET INCREMENT
void get_increment(SIZE SIDE,D_POINT* INC)
{
    D_SIZE SIDE_MOD;

    SIDE_MOD.h=SIDE.h;
    SIDE_MOD.w=SIDE.w;

    INC->y=0.4;
    INC->x=0.4;

    if(SIDE_MOD.h<0){SIDE_MOD.h*=(-1);INC->y=(-0.4);}
    if(SIDE_MOD.w<0){SIDE_MOD.w*=(-1);INC->x=(-0.4);}

    if(SIDE_MOD.h>SIDE_MOD.w)
    {
        INC->x*=SIDE_MOD.w/SIDE_MOD.h;
    }else{
        INC->y*=SIDE_MOD.h/SIDE_MOD.w;
    }
}
// -------------------------------------------------------------- GET INCREMENT
void get_increment(D_SIZE SIDE,D_POINT* INC)
{
    D_SIZE SIDE_MOD;

    SIDE_MOD.h=SIDE.h;
    SIDE_MOD.w=SIDE.w;

    INC->y=0.4;
    INC->x=0.4;

    if(SIDE_MOD.h<0){SIDE_MOD.h*=(-1);INC->y=(-0.4);}
    if(SIDE_MOD.w<0){SIDE_MOD.w*=(-1);INC->x=(-0.4);}

    if(SIDE_MOD.h>SIDE_MOD.w)
    {
        INC->x*=SIDE_MOD.w/SIDE_MOD.h;
    }else{
        INC->y*=SIDE_MOD.h/SIDE_MOD.w;
    }
}
// -------------------------------------------------------------- GET INCREMENT
void get_increment(D_SIZE SIDE,D_POINT* INC,double maj_inc)
{
    D_SIZE SIDE_MOD;

    SIDE_MOD.h=SIDE.h;
    SIDE_MOD.w=SIDE.w;

    INC->y=maj_inc;
    INC->x=maj_inc;

    if(SIDE_MOD.h<0){SIDE_MOD.h*=(-1);INC->y=(maj_inc*(-1));}
    if(SIDE_MOD.w<0){SIDE_MOD.w*=(-1);INC->x=(maj_inc*(-1));}

    if(SIDE_MOD.h>SIDE_MOD.w)
    {
        INC->x*=SIDE_MOD.w/SIDE_MOD.h;
    }else{
        INC->y*=SIDE_MOD.h/SIDE_MOD.w;
    }
}

// -------------------------------------------------------------- GET INCREMENT
// it algso giver the increment for the hypotenuse when
// rotating pixmap
// the hypotenuse increment will always be positive because
// it is meant to be used in the coords for the BUFFER
void get_tri_increment(D_SIZE SIDE,D_POINT* SIDE_INC, double* HYP_INC)
{
    D_SIZE SIDE_MOD;
    double HYP;

    SIDE_MOD.h=SIDE.h;
    SIDE_MOD.w=SIDE.w;

    HYP=get_hypotenuse(SIDE_MOD.h, SIDE_MOD.w);

    SIDE_INC->y=0.4;
    SIDE_INC->x=0.4;
    *HYP_INC=0.4;

    if(SIDE_MOD.h<0){SIDE_MOD.h*=(-1);SIDE_INC->y=(-0.4);}
    if(SIDE_MOD.w<0){SIDE_MOD.w*=(-1);SIDE_INC->x=(-0.4);}


    if(SIDE_MOD.h>SIDE_MOD.w)
    {
        SIDE_INC->x*=SIDE_MOD.w/SIDE_MOD.h;
        *HYP_INC*=HYP/SIDE_MOD.h;
    }else{
        SIDE_INC->y*=SIDE_MOD.h/SIDE_MOD.w;
        *HYP_INC*=HYP/SIDE_MOD.w;
    }

}
// -------------------------------------------------------------- GET TRIANGLE RATIO
// the hypotenuse woulb be 1
void get_ratio(double cat_1,double cat_2,double hyp,double* cat_1_ratio,double* cat_2_ratio,double* hyp_ratio)
{

}
// -------------------------------------------------------------- GET RATIO
//the idea is for the cat to be smaller than hyp so ratio wont be bigger than 1
double get_ratio(double cat,double hyp)
{
    double ratio=0;
    if(hyp!=0)
    {
        ratio=cat/hyp;
    }
    return ratio;
}
// -------------------------------------------------------------- IS PRIME
bool is_prime(unsigned long num)
{
    if(num<0){num*=(-1);}
    bool res=true;
    unsigned long div=2;
    unsigned long limit=num/2;

    while(div<limit)
    {
        if(num%div==0)
        {
            res=false;
            break;
        }
        limit=num/div;
        div++;
    }

    return res;

}
// -------------------------------------------------------------- MMC
long mmc(long num_1,long num_2)
{
    if(num_1==num_2)
    {
        return num_1;
    }
    int limit=100;

    long minor;
    long major;

    if(num_1<num_2)
    {
        minor=num_1;
        major=num_2;
    }else{
        minor=num_2;
        major=num_1;
    }

    long minor_fac=2;
    long major_fac=2;

    long minor_mul=minor*minor_fac;
    long major_mul=major;

    while(minor_fac<limit)
    {
        minor_mul=minor*minor_fac;
        minor_fac++;
        printf("\n\nMINOR MUL: %ld",minor_mul);
        printf("\nMINOR: %ld",minor);
        printf("\nMAJOR MUL: %ld",major_mul);
        printf("\nMAJOR: %ld",major);
        if(minor_mul==major_mul)
        {
            return minor_mul;
        }
        if(minor_mul>major_mul)
        {
            major_mul=major*major_fac;
            major_fac++;
            printf("\n---------------\n");
        }
    }




    return -1;

}
// -------------------------------------------------------------- MMC
double mmc(double num_1,double num_2)
{
    if(num_1==num_2)
    {
        return num_1;
    }
    int limit=100;

    double minor;
    double major;

    if(num_1<num_2)
    {
        minor=num_1;
        major=num_2;
    }else{
        minor=num_2;
        major=num_1;
    }

    long minor_fac=2;
    long major_fac=2;

    double minor_mul=minor*minor_fac;
    double major_mul=major;

    while(minor_fac<limit)
    {
        minor_mul=minor*minor_fac;
        minor_fac++;
        printf("\n\nMINOR MUL: %f",minor_mul);
        printf("\nMINOR: %f",minor);
        printf("\nMAJOR MUL: %f",major_mul);
        printf("\nMAJOR: %f",major);
        if(minor_mul==major_mul)
        {
            return minor_mul;
        }
        if(minor_mul>major_mul)
        {
            major_mul=major*major_fac;
            major_fac++;
            printf("\n---------------\n");
        }
    }




    return -1;

}
// -------------------------------------------------------------- LINE THROUGH BASE
// the line can't end on this BASE
// it will return the number(0 to 7) corresponding to where the line went through
// 0 is te BASE[0] corner, 1 is the top side, 2 is the BASE[1] corner...
int line_through_base(POINT BASE[4],D_POINT T_pt,int T_CORN,double INC_rat_yx,double INC_rat_xy,D_POINT* INTER_pt)
{
    D_POINT dist;
    double frac_y;
    double frac_x;

    if(T_CORN == 0)
    {
        dist.x = BASE[1].x - T_pt.x;
        dist.y = BASE[2].y - T_pt.y;

        frac_y = dist.x * INC_rat_yx;
        frac_x = dist.y * INC_rat_xy;
        //goes through vertical
        if(frac_y < dist.y)
        {
            INTER_pt->y = INTER_pt->y + frac_y;
            INTER_pt->x = BASE[2].x;

            return 3;
        }
        //goes through corner
        if(frac_y == dist.y)
        {
            INTER_pt->y = BASE[2].y;
            INTER_pt->x = BASE[2].x;
            return 4;

        }
        //goes through horizontal
        if(frac_y > dist.y)
        {
            INTER_pt->y = BASE[2].y;
            INTER_pt->x = INTER_pt->x + frac_x;
            return 5;
        }
    }
    if(T_CORN == 1)
    {
        dist.x = T_pt.x - BASE[0].x;
        dist.y = BASE[2].y - T_pt.y;

        frac_y = dist.x * INC_rat_yx;
        frac_x = dist.y * INC_rat_xy;

        //goes through vertical
        if(frac_y < dist.y)
        {
            INTER_pt->y = INTER_pt->y + frac_y;
            INTER_pt->x = BASE[3].x;

            return 7;
        }
        //goes through corner
        if(frac_y == dist.y)
        {
            INTER_pt->y = BASE[3].y;
            INTER_pt->x = BASE[3].x;
            return 6;

        }
        //goes through horizontal
        if(frac_y > dist.y)
        {
            INTER_pt->y = BASE[3].y;
            INTER_pt->x = INTER_pt->x - frac_x;
            return 5;
        }
    }
    if(T_CORN == 2)
    {
        dist.x = T_pt.x - BASE[0].x;
        dist.y = T_pt.y - BASE[0].y;

        frac_y = dist.x * INC_rat_yx;
        frac_x = dist.y * INC_rat_xy;

        //goes through vertical
        if(frac_y < dist.y)
        {
            INTER_pt->y = INTER_pt->y - frac_y;
            INTER_pt->x = BASE[0].x;

            return 7;
        }
        //goes through corner
        if(frac_y == dist.y)
        {
            INTER_pt->y = BASE[0].y;
            INTER_pt->x = BASE[0].x;
            return 0;

        }
        //goes through horizontal
        if(frac_y > dist.y)
        {
            INTER_pt->y = BASE[0].y;
            INTER_pt->x = INTER_pt->x - frac_x;
            return 1;
        }

    }
    if(T_CORN == 3)
    {
        dist.x = BASE[1].x - T_pt.x;
        dist.y = T_pt.y - BASE[0].y;

        frac_y = dist.x * INC_rat_yx;
        frac_x = dist.y * INC_rat_xy;

        //goes through vertical
        if(frac_y < dist.y)
        {
            INTER_pt->y = INTER_pt->y - frac_y;
            INTER_pt->x = BASE[1].x;

            return 3;
        }
        //goes through corner
        if(frac_y == dist.y)
        {
            INTER_pt->y = BASE[1].y;
            INTER_pt->x = BASE[1].x;
            return 2;

        }
        //goes through horizontal
        if(frac_y > dist.y)
        {
            INTER_pt->y = BASE[1].y;
            INTER_pt->x = INTER_pt->x + frac_x;
            return 1;
        }

    }


    return 0;
}
// -------------------------------------------------------------- CORNER IS IN BASE
bool corner_is_in_base(POINT BASE_0, D_POINT T_pt)
{
    double dist_x = T_pt.x - BASE_0.x;
    double dist_y = T_pt.y - BASE_0.y;

    if(dist_x < 0 || dist_x >= 1 || dist_y < 0 || dist_y >= 1)
    {
        return false;
    }

    return true;
}

// -------------------------------------------------------------- BASE CORNER IS IN TILT
bool base_corner_in_tilt(int step,D_POINT TILT[4],double INC_x,double INC_rat_yx,double INC_rat_xy,POINT B_pt)
{

    printf("\n\nIN FUNC:::");
    D_POINT D;
    double rat;

    D.x = B_pt.x - TILT[0].x;
    D.y = B_pt.y - TILT[0].y;

    rat = INC_rat_yx;
    if(D.x<0)
    {
        //rat = INC_rat_xy;
        //D.x*=(-1);
    }

    D.x *= rat;
    D.y -= D.x;
    D.y *= INC_x;

    if(D.y < 0 || D.y > 1){return false;}

    D.y = TILT[3].y - B_pt.y;
    D.x = B_pt.x - TILT[3].x;

    rat = INC_rat_yx;
    if(D.x<0)
    {
        rat = INC_rat_xy;
        D.x*=(-1);
    }

    if(D.x<0){rat = INC_rat_xy;}

    D.y *= rat;
    D.x -= D.y;
    D.x *= INC_x;

    //printf("\nD.x: %f", D.x);

    if(D.x < 0 || D.x > 1){return false;}

    return true;
}
// -------------------------------------------------------------- LINES CROSS
bool lines_cross(POINT A1, POINT A2, POINT B1, POINT B2)
{





    return false;
}

// -------------------------------------------------------------- POLYGON AREA
double polygon_area(D_POINT* CORNERS,int corn_n)
{
    double ret=0;

    int c=0;
    int n=1;

    while(c<corn_n)
    {
        ret+=CORNERS[c].x*CORNERS[n].y;
        ret-=CORNERS[c].y*CORNERS[n].x;
        c++;
        n++;
        if(n==corn_n){n=0;}
    }
    if(ret<0){ret*=(-1);}
    ret/=2;

    return ret;

}

// -------------------------------------------------------------- DRAW AA LINE
void draw_aa_line(unsigned char* pixmap,SIZE px_size,POINT START,POINT END,int bold,unsigned char color)
{
    
}
// -------------------------------------------------------------- DRAW AA LINE
void draw_aa_line(Uint32* pixmap,SIZE px_size,POINT START,POINT END,int bold,Uint32 color)
{	
    

}

// -------------------------------------------------------------- DRAW LINE
void draw_straight_line(Uint32* pixmap,SIZE px_size,POINT START,POINT END,int bold,Uint32 color)
{
    POINT P;
    int index;

    DRAW_D_POINTS L;
    D_POINT B;

    double L_hyp;

    D_SIZE L_len;
    D_SIZE B_len;

    D_POINT L_INC;
    D_POINT B_INC;
    //positive values for counting:
    D_SIZE L_len_pos;
    D_SIZE B_len_pos;
    D_SIZE B_len_pos_c;

    D_POINT L_INC_pos;
    D_POINT B_INC_pos;
    //-----------------------------------------
    //STABLISH POINTS AND DIMENSIONS AND VALUES
    get_trig_sides(START, END, &L_len);
    L_hyp=get_hypotenuse(L_len.h, L_len.w);

    //For the boldness w and h be the same sign as
    //the corresponding line vectors L_hyp must be positive
    B_len.h=get_ratio(L_len.w, L_hyp);
    B_len.w=get_ratio(L_len.h, L_hyp);

    B_len.h*=bold;
    B_len.w*=bold;

    //observe the signs for the vectors in the trigonometric
    //circle to understand this change in sign
    B_len.w*=(-1);

    L.start_y=START.y-B_len.h/2;
    L.start_x=START.x-B_len.w/2;
    L.end_y=END.y-B_len.h/2;
    L.end_x=END.x-B_len.w/2;
    L.y=L.start_y;
    L.x=L.start_x;

    B.y=0;
    B.x=0;

    L_len_pos.h=turn_positive(L_len.h);
    L_len_pos.w=turn_positive(L_len.w);

    B_len_pos.h=turn_positive(B_len.h);
    B_len_pos.w=turn_positive(B_len.w);

    B_len_pos_c.h=B_len_pos.h;
    B_len_pos_c.w=B_len_pos.w;

    //-------------------
    //STABLISH INCREMENTS

    get_increment(L_len, &L_INC);
    get_increment(B_len, &B_INC);

    L_INC_pos.y=turn_positive(L_INC.y);
    L_INC_pos.x=turn_positive(L_INC.x);

    B_INC_pos.y=turn_positive(B_INC.y);
    B_INC_pos.x=turn_positive(B_INC.x);

    //---------
    //DRAW LINE

    while(L_len_pos.h > 0 || L_len_pos.w > 0)
    {
        while(B_len_pos_c.h > 0 || B_len_pos_c.w > 0)
        {

            P.y=L.y+B.y;
            P.x=L.x+B.x;
            if(P.y<px_size.h && P.x<px_size.w)
            {
                index=(P.y*px_size.w)+P.x;
                pixmap[index]=color;
            }
            B.x+=B_INC.x;

            P.x=L.x+B.x;
            if(P.y<px_size.h && P.x<px_size.w)
            {
                index=(P.y*px_size.w)+P.x;
                pixmap[index]=color;
            }
            B.y+=B_INC.y;
            //-----
            B_len_pos_c.h-=B_INC_pos.y;
            B_len_pos_c.w-=B_INC_pos.x;
        }
        B.y=0;
        B.x=0;
        B_len_pos_c.h=B_len_pos.h;
        B_len_pos_c.w=B_len_pos.w;

        L.y+=L_INC.y;
        L.x+=L_INC.x;


        L_len_pos.h-=L_INC_pos.y;
        L_len_pos.w-=L_INC_pos.x;
    }


}
// -------------------------------------------------------------- DRAW HORIZONTAL LINE
void draw_h_line(unsigned char* pixmap,SIZE px_size,POINT start, int end_x,unsigned char color)
{
    adjust_h_line_path_to_px_size(px_size,&start,&end_x);

    int sx=start.x;
    int ex=end_x;

    if(end_x<start.x)
    {
        sx=end_x;
        ex=start.x;
    }

    int pre_index=start.y*px_size.w;
    int index;

    int x=sx;
    while(x<=ex)
    {
        index=pre_index+x;
        pixmap[index]=color;
        x++;
    }
}
// -------------------------------------------------------------- DRAW HORIZONTAL LINE
void draw_h_line(Uint32* pixmap,SIZE px_size,POINT start, int end_x,Uint32 color)
{
    adjust_h_line_path_to_px_size(px_size,&start,&end_x);

    int sx=start.x;
    int ex=end_x;

    if(end_x<start.x)
    {
        sx=end_x;
        ex=start.x;
    }

    int pre_index=start.y*px_size.w;
    int index;

    int x=sx;
    while(x<=ex)
    {
        index=pre_index+x;
        pixmap[index]=color;
        x++;
    }
}
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_v_line(unsigned char* pixmap,SIZE px_size,POINT start, int end_y, unsigned char color)
{
    adjust_v_line_path_to_px_size(px_size,&start,&end_y);

    int sy=start.y;
    int ey=end_y;

    if(end_y<start.y)
    {
        sy=end_y;
        ey=start.y;
    }

    int index;

    int y=sy;
    while(y<=ey)
    {
        index=(y*px_size.w)+start.x;
        pixmap[index]=color;
        y++;
    }
}
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_v_line(Uint32* pixmap,SIZE px_size,POINT start, int end_y, Uint32 color)
{
    adjust_v_line_path_to_px_size(px_size,&start,&end_y);

    int sy=start.y;
    int ey=end_y;

    if(end_y<start.y)
    {
        sy=end_y;
        ey=start.y;
    }

    int index;

    int y=sy;
    while(y<=ey)
    {
        index=(y*px_size.w)+start.x;
        pixmap[index]=color;
        y++;
    }
}
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_cursor_line(unsigned char* pixmap,SIZE px_size,POINT start, int end_y, unsigned char color)
{
    adjust_v_line_path_to_px_size(px_size,&start,&end_y);

    int sy=start.y;
    int ey=end_y;

    if(end_y<start.y)
    {
        sy=end_y;
        ey=start.y;
    }

    int index;

    int y=sy;
    while(y<=ey)
    {
        index=(y*px_size.w)+start.x;
        pixmap[index]=color;
        if(start.x < px_size.w)
        {
            index=(y*px_size.w)+start.x+1;
            pixmap[index]=255-color;
        }
        y++;
    }
}

// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_cursor_line(Uint32* pixmap,SIZE px_size,POINT start, int end_y, Uint32 color)
{
    adjust_v_line_path_to_px_size(px_size,&start,&end_y);

    int sy=start.y;
    int ey=end_y;

    if(end_y<start.y)
    {
        sy=end_y;
        ey=start.y;
    }

    int index;

    int y=sy;
    while(y<=ey)
    {
        index=(y*px_size.w)+start.x;
        pixmap[index]=color;
        if(start.x < px_size.w)
        {
            index=(y*px_size.w)+start.x+1;
            pixmap[index]=255-color;
        }
        y++;
    }
}

// -------------------------------------------------------------- ROTATE POINT TO AXIS
POINT rotate_point_in_axis(POINT SOURCE,int angle,POINT axis)
{
    int right_angle=angle/90;
    right_angle=right_angle%4;
    int acute_angle=angle%90;
    POINT dir_sign;
    POINT DEST=SOURCE;
    D_SIZE dist;

    double dist_hyp;

    double prev_sine;
    double prev_cosine;
    double added_sine;
    double added_cosine;

    D_POINT added;

    dist.h=SOURCE.y-axis.y;
    dist.w=SOURCE.x-axis.x;

    if(right_angle==1)
    {
        DEST.y=axis.y+dist.w;
        DEST.x=axis.x-dist.h;
    }
    if(right_angle==2)
    {
        DEST.y=axis.y-dist.h;
        DEST.x=axis.x-dist.w;
    }
    if(right_angle==3)
    {
        DEST.y=axis.y-dist.w;
        DEST.x=axis.x+dist.h;
    }

    if(acute_angle>0)
    {
        dist.h=DEST.y-axis.y;
        dist.w=DEST.x-axis.x;
        dir_sign.y=1;
        dir_sign.x=1;

        if(dist.h<0){dir_sign.y=(-1);}
        if(dist.w<0){dir_sign.x=(-1);}

        dist_hyp=get_hypotenuse(dist.h, dist.w);

        added_sine=return_sine(acute_angle);
        added_cosine=return_sine(90-acute_angle);

        //matching sides to vectors
        if(dir_sign.y==dir_sign.x)
        {
            prev_sine=get_ratio(dist.h,dist_hyp);
            prev_cosine=get_ratio(dist.w,dist_hyp);

            added.x=prev_sine*added_cosine+prev_cosine*added_sine;
            added.y=prev_cosine*added_cosine-prev_sine*added_sine;
        }else{
            prev_sine=get_ratio(dist.w,dist_hyp);
            prev_cosine=get_ratio(dist.h,dist_hyp);

            added_sine*=(-1);

            added.y=prev_sine*added_cosine+prev_cosine*added_sine;
            added.x=prev_cosine*added_cosine-prev_sine*added_sine;
        }

        DEST.y=axis.y;
        DEST.x=axis.x;
        DEST.y+=dist_hyp*added.x;
        DEST.x+=dist_hyp*added.y;

    }
    return DEST;
}
// -------------------------------------------------------------- ROTATE POINT TO AXIS
D_POINT rotate_d_point_in_axis(D_POINT SOURCE,double angle,D_POINT axis)
{
    int right_angle=angle/90;
    //right_angle=right_angle%4;
    //double acute_angle=angle%90;
    double acute_angle=angle;
    POINT dir_sign;
    D_POINT DEST=SOURCE;
    D_SIZE dist;

    double dist_hyp;

    double prev_sine;
    double prev_cosine;
    double added_sine;
    double added_cosine;

    D_POINT added;

    dist.h=SOURCE.y-axis.y;
    dist.w=SOURCE.x-axis.x;

    if(right_angle==1)
    {
        DEST.y=axis.y+dist.w;
        DEST.x=axis.x-dist.h;
    }
    if(right_angle==2)
    {
        DEST.y=axis.y-dist.h;
        DEST.x=axis.x-dist.w;
    }
    if(right_angle==3)
    {
        DEST.y=axis.y-dist.w;
        DEST.x=axis.x+dist.h;
    }

    if(acute_angle>0)
    {
        dist.h=DEST.y-axis.y;
        dist.w=DEST.x-axis.x;
        dir_sign.y=1;
        dir_sign.x=1;

        if(dist.h<0){dir_sign.y=(-1);}
        if(dist.w<0){dir_sign.x=(-1);}

        dist_hyp=get_hypotenuse(dist.h, dist.w);


        double angle=(acute_angle*PI/180);
        added_sine=sin(angle);
        angle=((90-acute_angle)*PI/180);
        added_cosine=sin(angle);

        //matching sides to vectors
        if(dir_sign.y==dir_sign.x)
        {
            prev_sine=get_ratio(dist.h,dist_hyp);
            prev_cosine=get_ratio(dist.w,dist_hyp);



            added.x=prev_sine*added_cosine+prev_cosine*added_sine;
            added.y=prev_cosine*added_cosine-prev_sine*added_sine;
        }else{
            prev_sine=get_ratio(dist.w,dist_hyp);
            prev_cosine=get_ratio(dist.h,dist_hyp);

            added_sine*=(-1);

            added.y=prev_sine*added_cosine+prev_cosine*added_sine;
            added.x=prev_cosine*added_cosine-prev_sine*added_sine;
        }

        DEST.y=axis.y;
        DEST.x=axis.x;
        DEST.y+=dist_hyp*added.x;
        DEST.x+=dist_hyp*added.y;

    }
    return DEST;
}

// -------------------------------------------------------------- DRAW LINE
void rotate_right_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int dir,POINT axis)
{
    int total_orig_size=rect.w*rect.h;
    Uint32 * buffer_arr=NULL;
    buffer_arr=new Uint32[total_orig_size];
    if(buffer_arr==NULL || buffer_arr==nullptr)
    {
        return;
    }

    POINT ORIG_COUNTER;
    int ORIG_INDEX;
    POINT ORIG_END_POINT;
    POINT BUFFER_COUNTER;
    int BUFFER_INDEX;

    ORIG_COUNTER.y=rect.y;
    ORIG_COUNTER.x=rect.x;

    ORIG_END_POINT.y=rect.y+rect.h;
    ORIG_END_POINT.x=rect.x+rect.w;

    BUFFER_COUNTER.y=0;
    BUFFER_COUNTER.x=0;
    //This loop copies the selected image to the buffer
    while(ORIG_COUNTER.y<ORIG_END_POINT.y && BUFFER_COUNTER.y<rect.h && ORIG_COUNTER.y<px_size.h)
    {

        while(ORIG_COUNTER.x<ORIG_END_POINT.x && BUFFER_COUNTER.x<rect.w && ORIG_COUNTER.x<px_size.w)
        {

            ORIG_INDEX=(ORIG_COUNTER.y*px_size.w)+ORIG_COUNTER.x;
            BUFFER_INDEX=(BUFFER_COUNTER.y*rect.w)+BUFFER_COUNTER.x;
            buffer_arr[BUFFER_INDEX]=pixmap[ORIG_INDEX];
            pixmap[ORIG_INDEX]=DEFAULT_BKG_COLOR;

            ORIG_COUNTER.x++;
            BUFFER_COUNTER.x++;
        }
        ORIG_COUNTER.x=rect.x;
        BUFFER_COUNTER.x=0;

        ORIG_COUNTER.y++;
        BUFFER_COUNTER.y++;
    }

    POINT BUFFER_START;
    POINT BUFFER_END;
    POINT BUFFER_C;
    POINT BUF_INC;
    POINT DEST_START;
    POINT DEST_END;
    POINT DEST_C;

    int DEST_INDEX;

    //90 degrees counter clockwise
    if(dir==3)
    {
        BUFFER_START.y=0;
        BUFFER_START.x=rect.w-1;

        DEST_START.y=axis.y+(axis.x-(rect.x+rect.w));
        DEST_START.x=axis.x-(axis.y-rect.y);

        DEST_END.y=DEST_START.y+rect.w;
        DEST_END.x=DEST_START.x+rect.h;

        BUF_INC.y=1;
        BUF_INC.x=(-1);
    }
    //90 degrees clockwise
    if(dir==1)
    {
        BUFFER_START.y=rect.h-1;
        BUFFER_START.x=0;

        DEST_START.y=axis.y-(axis.x-(rect.x));
        DEST_START.x=axis.x+(axis.y-(rect.y+rect.h));

        DEST_END.y=DEST_START.y+rect.w;
        DEST_END.x=DEST_START.x+rect.h;

        BUF_INC.y=(-1);
        BUF_INC.x=1;
    }
    //180 degrees
    if(dir==2)
    {
        BUFFER_START.y=rect.h;
        BUFFER_START.x=rect.w;

        DEST_START.y=0;
        DEST_START.x=0;

        DEST_END.y=DEST_START.y+rect.h;
        DEST_END.x=DEST_START.x+rect.w;

        BUF_INC.y=(-1);
        BUF_INC.x=(-1);
    }

    printf("axis Y: %d. X: %d.",axis.y,axis.x);
    printf("\n");
    printf("rect W: %d. H: %d.",rect.w,rect.h);
    printf("\n");
    printf("rect Y: %d. X: %d.",rect.y,rect.x);
    printf("\n");
    printf("BUFFER_START Y: %d. X: %d.",BUFFER_START.y,BUFFER_START.x);
    printf("\n");
    printf("DEST_START Y: %d. X: %d.",DEST_START.y,DEST_START.x);
    printf("\n");
    printf("DEST_END Y: %d. X: %d.",DEST_END.y,DEST_END.x);
    printf("\n");

    BUFFER_C=BUFFER_START;
    DEST_C=DEST_START;

    //both 90 degrees turns in which the buffer height becomes the
    // destiny width, that is why the BUFFER.y is incremented in the
    // inner loop
    if(dir==1 || dir==3)
    {
        while(DEST_C.y<DEST_END.y && DEST_C.y<px_size.h)
        {

            while(DEST_C.x<DEST_END.x && DEST_C.x<px_size.w && DEST_C.y>=0)
            {
                if(DEST_C.x>=0)
                {
                    BUFFER_INDEX=(BUFFER_C.y*rect.w)+BUFFER_C.x;
                    DEST_INDEX=(DEST_C.y*rect.h)+DEST_C.x;

                    pixmap[DEST_INDEX]=buffer_arr[BUFFER_INDEX];
                }

                BUFFER_C.y+=BUF_INC.y;
                DEST_C.x++;
            }
            BUFFER_C.y=BUFFER_START.y;
            BUFFER_C.x+=BUF_INC.x;

            DEST_C.x=DEST_START.x;
            DEST_C.y++;
        }
    }

    if(dir==2)
    {
        while(DEST_C.y<DEST_END.y && DEST_C.y<px_size.h)
        {

            while(DEST_C.x<DEST_END.x && DEST_C.x<px_size.w && DEST_C.y>=0)
            {
                if(DEST_C.x>=0)
                {
                    BUFFER_INDEX=(BUFFER_C.y*rect.w)+BUFFER_C.x;
                    DEST_INDEX=(DEST_C.y*rect.h)+DEST_C.x;

                    pixmap[DEST_INDEX]=buffer_arr[BUFFER_INDEX];
                }

                BUFFER_C.x+=BUF_INC.x;
                DEST_C.x++;
            }
            BUFFER_C.y+=BUF_INC.y;
            BUFFER_C.x+=BUFFER_START.x;

            DEST_C.x=DEST_START.x;
            DEST_C.y++;
        }
    }



    delete[] buffer_arr;


}
// -------------------------------------------------------------- COPY RECT TO UINT BUFFER
//auxiliary for rotation
//the buffer must already be allocated
void copy_rect_to_buffer(Uint32* pixmap,SIZE px_size,Uint32* buffer,SOIL_RECT rect, bool delete_area)
{
    int pixmap_pre_index=0;
    int pixmap_index=0;
    int buffer_pre_index=0;
    int buffer_index=0;

    int px_y=rect.y;
    int px_x=rect.x;
    int bf_y=0;
    int bf_x=0;
    while(bf_y<rect.h && px_y<px_size.h)
    {
        pixmap_pre_index=(px_y*px_size.w);
        buffer_pre_index=(bf_y*rect.w);
        while(bf_x<rect.w && px_x<px_size.w)
        {
            buffer_index=buffer_pre_index+bf_x;
            pixmap_index=pixmap_pre_index+px_x;

            buffer[buffer_index]=pixmap[pixmap_index];
            if(delete_area)
            {
               pixmap[pixmap_index]=0xFF000000;
            }

            bf_x++;
            px_x++;
        }
        bf_x=0;
        bf_y++;

        px_y++;
        px_x=rect.x;
    }
}
// -------------------------------------------------------------- COPY RECT TO CALC BUFFER
//auxiliary for rotation
//the buffer must already be allocated
void copy_rect_to_buffer(Uint32* pixmap,SIZE px_size,PIXDATA* buffer,SOIL_RECT rect, bool delete_area)
{
    int pixmap_pre_index=0;
    int pixmap_index=0;
    int buffer_pre_index=0;
    int buffer_index=0;

    int px_y=rect.y;
    int px_x=rect.x;
    int bf_y=0;
    int bf_x=0;
    while(bf_y<rect.h && px_y<px_size.h)
    {
        pixmap_pre_index=(px_y*px_size.w);
        buffer_pre_index=(bf_y*rect.w);
        while(bf_x<rect.w && px_x<px_size.w)
        {
            buffer_index=buffer_pre_index+bf_x;
            pixmap_index=pixmap_pre_index+px_x;

            buffer[buffer_index].pix=pixmap[pixmap_index];
            if(delete_area)
            {
               pixmap[pixmap_index]=0xFF000000;
            }

            bf_x++;
            px_x++;
        }
        bf_x=0;
        bf_y++;

        px_y++;
        px_x=rect.x;
    }
}

// -------------------------------------------------------------- COPY RECT TO CALC BUFFER
//auxiliary for rotation
//the buffer must already be allocated
void copy_rect_to_buffer(Uint32* pixmap,SIZE px_size,CALC_PIX_COLOR* buffer,SOIL_RECT rect,double fac, bool delete_area)
{
    int pixmap_pre_index=0;
    int pixmap_index=0;
    int buffer_pre_index=0;
    int buffer_index=0;

    int px_y=rect.y;
    int px_x=rect.x;
    int bf_y=0;
    int bf_x=0;

    PIXDATA pix_value;
    fac*=fac;

    while(bf_y<rect.h && px_y<px_size.h)
    {
        pixmap_pre_index=(px_y*px_size.w);
        buffer_pre_index=(bf_y*rect.w);
        while(bf_x<rect.w && px_x<px_size.w)
        {
            buffer_index=buffer_pre_index+bf_x;
            pixmap_index=pixmap_pre_index+px_x;

            pix_value.pix=pixmap[pixmap_index];
            buffer[buffer_index].r=pix_value.color.r*fac;
            buffer[buffer_index].g=pix_value.color.g*fac;
            buffer[buffer_index].b=pix_value.color.b*fac;
            buffer[buffer_index].a=pix_value.color.a*fac;

            if(delete_area)
            {
               pixmap[pixmap_index]=0xFF000000;
            }

            bf_x++;
            px_x++;
        }
        bf_x=0;
        bf_y++;

        px_y++;
        px_x=rect.x;
    }
}
// -------------------------------------------------------------- MAKE CORNER SAMPLE FROM CORNER IMAGE
void make_corner_sample_from_border_img(Uint32* border_pixmap,SIZE border_px_size, Uint32* corner_pixmap,SIZE corner_px_size)
{
    //if((2 * corner_px_size.h) > border_px_size.h || (2 * (corner_px_size.w - 1)) > border_px_size.w){return;}
    int end_x = (border_px_size.w / 3) + 1;
    int end_y = border_px_size.h / 3;
    if(corner_px_size.w < end_x || border_px_size.h < end_y){return;}
    int x = 0;
    int y = 0;

    int border_index;
    int corner_index;

    while(y < end_y)
    {
        while(x < end_x)
        {
            border_index = (y*border_px_size.w) + x;
            corner_index = (y*corner_px_size.w) + x;
            corner_pixmap[corner_index] = border_pixmap[border_index];
            x++;
        }
        x = 0;
        y++;
    }
}
// -------------------------------------------------------------- LOAD BORDER FROM CORNER SAMPLE
void load_border_from_corner_sample(Uint32* corner_pixmap,SIZE corner_px_size,Uint32* border_pixmap,SIZE border_px_size)
{
    //MARKER: have the validation somewhere else, or return a value from this function
    if((2 * corner_px_size.h) > border_px_size.h || (2 * (corner_px_size.w - 1)) > border_px_size.w){return;}


    DRAW_POINTS CP;//corner points
    DRAW_POINTS BP;//border points

    POINT CP_inc;
    POINT BP_inc;
    SOIL_RECT rect;
    int corner_pre_index = 0;
    int corner_index = 0;
    int border_pre_index = 0;
    int border_index = 0;


    //////////////////////////////// TOP LEFT
    CP.start_x = 0;
    CP.start_y = 0;
    CP.end_x = corner_px_size.w - 1;
    CP.end_y = corner_px_size.h;
    CP.x = CP.start_x;
    CP.y = CP.start_y;

    CP_inc.x = 1;
    CP_inc.y = 1;

    BP = CP;
    BP_inc = CP_inc;

    while(CP.y < CP.end_y)
    {
        //corner_pre_index = y*corner_px_size.w;
        //border_pre_index = y*border_px_size.w;
        //it's corner_px_size.h so I don't write corner_px_size.w-1;
        while(CP.x < CP.end_x)
        {
            //corner_index = corner_pre_index + x;
            //border_index = border_pre_index + x;
            border_pixmap[border_index] = alpha_blend_pixel(border_pixmap[border_index], corner_pixmap[corner_index]);
            CP.x += CP_inc.x;
        }
        CP.x = CP.start_x;
        CP.y += CP_inc.y;

        
    }

    




}
// -------------------------------------------------------------- DRAW SOLID RECT
void draw_solid_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,unsigned char color)
{
    int index=0;
    int px_arr_size=px_size.h*px_size.w;

    DRAW_POINTS R;

    R.start_y=rect.y;
    R.start_x=rect.x;

    if(R.start_y < 0){R.start_y = 0;}
    if(R.start_x < 0){R.start_x = 0;}

    if(R.start_y >= px_size.h){R.start_y = px_size.h;}
    if(R.start_x >= px_size.w){R.start_x = px_size.w;}

    R.end_y=rect.y+rect.h;
    R.end_x=rect.x+rect.w;

    if(R.end_y < 0){R.end_y = 0;}
    if(R.end_x < 0){R.end_x = 0;}

    if(R.end_y>=px_size.h){R.end_y=px_size.h;}
    if(R.end_x>=px_size.w){R.end_x=px_size.w;}

    R.y=R.start_y;
    R.x=R.start_x;

    while(R.y<R.end_y)
    {

        while(R.x<R.end_x)
        {
            index=(R.y*px_size.w)+R.x;
            if(index<px_arr_size)
            {
                pixmap[index]=color;
            }
            R.x++;
        }
        R.x=R.start_x;
        R.y++;
    }

}
// -------------------------------------------------------------- DRAW SOLID RECT
void draw_solid_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,Uint32 color)
{
    int index=0;
    int px_arr_size=px_size.h*px_size.w;

    DRAW_POINTS R;

    R.start_y=rect.y;
    R.start_x=rect.x;

    if(R.start_y < 0){R.start_y = 0;}
    if(R.start_x < 0){R.start_x = 0;}

    if(R.start_y >= px_size.h){R.start_y = px_size.h;}
    if(R.start_x >= px_size.w){R.start_x = px_size.w;}

    R.end_y=rect.y+rect.h;
    R.end_x=rect.x+rect.w;

    if(R.end_y < 0){R.end_y = 0;}
    if(R.end_x < 0){R.end_x = 0;}

    if(R.end_y>=px_size.h){R.end_y=px_size.h;}
    if(R.end_x>=px_size.w){R.end_x=px_size.w;}

    R.y=R.start_y;
    R.x=R.start_x;

    while(R.y<R.end_y)
    {

        while(R.x<R.end_x)
        {
            index=(R.y*px_size.w)+R.x;
            if(index<px_arr_size)
            {
                pixmap[index]=color;
            }
            R.x++;
        }
        R.x=R.start_x;
        R.y++;
    }

}

// -------------------------------------------------------------- DRAW OUTLINE RECT
void draw_outline_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,int bold,unsigned char color)
{
    int index=0;
    int px_arr_size=px_size.h*px_size.w;

    if(bold>(rect.w/2) || bold>(rect.h/2))
    {
        draw_solid_rect(pixmap, px_size, rect, color);
        return;
    }

    int mid_h=rect.h-2*bold;
    int mid_w=rect.w-2*bold;

    DRAW_POINTS sides[4];

    sides[0].start_y=rect.y;
    sides[0].start_x=rect.x;
    sides[0].end_y=rect.y+bold;
    sides[0].end_x=rect.x+rect.w;

    sides[1].start_y=rect.y+bold;
    sides[1].start_x=rect.x;
    sides[1].end_y=rect.y+bold+mid_h;
    sides[1].end_x=rect.x+bold;

    sides[2].start_y=rect.y+bold;
    sides[2].start_x=rect.x+bold+mid_w;
    sides[2].end_y=rect.y+bold+mid_h;
    sides[2].end_x=rect.x+rect.w;

    sides[3].start_y=rect.y+bold+mid_h;
    sides[3].start_x=rect.x;
    sides[3].end_y=rect.y+rect.h;
    sides[3].end_x=rect.x+rect.w;

    int sc=0;

    while(sc<4)
    {
        sides[sc].y=sides[sc].start_y;
        sides[sc].x=sides[sc].start_x;

        while(sides[sc].y<sides[sc].end_y && sides[sc].y<px_size.h)
        {
            while(sides[sc].x<sides[sc].end_x && sides[sc].x<px_size.w)
            {
                index=(sides[sc].y*px_size.w)+sides[sc].x;
                if(index<px_arr_size)
                {
                    pixmap[index]=color;
                }

                sides[sc].x++;
            }
            sides[sc].x=sides[sc].start_x;
            sides[sc].y++;
        }
        sc++;
    }

}
// -------------------------------------------------------------- DRAW OUTLINE RECT
void draw_outline_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,Uint32 color)
{
    int index=0;
    int px_arr_size=px_size.h*px_size.w;

    if(bold>(rect.w/2) || bold>(rect.h/2))
    {
        draw_solid_rect(pixmap, px_size, rect, color);
        return;
    }

    int mid_h=rect.h-2*bold;
    int mid_w=rect.w-2*bold;

    DRAW_POINTS sides[4];

    sides[0].start_y=rect.y;
    sides[0].start_x=rect.x;
    sides[0].end_y=rect.y+bold;
    sides[0].end_x=rect.x+rect.w;

    sides[1].start_y=rect.y+bold;
    sides[1].start_x=rect.x;
    sides[1].end_y=rect.y+bold+mid_h;
    sides[1].end_x=rect.x+bold;

    sides[2].start_y=rect.y+bold;
    sides[2].start_x=rect.x+bold+mid_w;
    sides[2].end_y=rect.y+bold+mid_h;
    sides[2].end_x=rect.x+rect.w;

    sides[3].start_y=rect.y+bold+mid_h;
    sides[3].start_x=rect.x;
    sides[3].end_y=rect.y+rect.h;
    sides[3].end_x=rect.x+rect.w;

    int sc=0;

    while(sc<4)
    {
        sides[sc].y=sides[sc].start_y;
        sides[sc].x=sides[sc].start_x;

        while(sides[sc].y<sides[sc].end_y && sides[sc].y<px_size.h)
        {
            while(sides[sc].x<sides[sc].end_x && sides[sc].x<px_size.w)
            {
                index=(sides[sc].y*px_size.w)+sides[sc].x;
                if(index<px_arr_size)
                {
                    pixmap[index]=color;
                }

                sides[sc].x++;
            }
            sides[sc].x=sides[sc].start_x;
            sides[sc].y++;
        }
        sc++;
    }

}
// -------------------------------------------------------------- DRAW SOLID ROUND RECT
void draw_solid_round_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,int border_max_rad,Uint32 color)
{


    int index=0;
    int px_arr_size=px_size.h*px_size.w;

    if(bold>(rect.w/2) || bold>(rect.h/2))
    {
        draw_solid_rect(pixmap, px_size, rect, color);
        return;
    }

    int mid_h=rect.h-2*bold;
    int mid_w=rect.w-2*bold;

    DRAW_POINTS sides[4];

    sides[0].start_y=rect.y;
    sides[0].start_x=rect.x;
    sides[0].end_y=rect.y+bold;
    sides[0].end_x=rect.x+rect.w;

    sides[1].start_y=rect.y+bold;
    sides[1].start_x=rect.x;
    sides[1].end_y=rect.y+bold+mid_h;
    sides[1].end_x=rect.x+bold;

    sides[2].start_y=rect.y+bold;
    sides[2].start_x=rect.x+bold+mid_w;
    sides[2].end_y=rect.y+bold+mid_h;
    sides[2].end_x=rect.x+rect.w;

    sides[3].start_y=rect.y+bold+mid_h;
    sides[3].start_x=rect.x;
    sides[3].end_y=rect.y+rect.h;
    sides[3].end_x=rect.x+rect.w;

    int sc=0;

    while(sc<4)
    {
        sides[sc].y=sides[sc].start_y;
        sides[sc].x=sides[sc].start_x;

        while(sides[sc].y<sides[sc].end_y && sides[sc].y<px_size.h)
        {
            while(sides[sc].x<sides[sc].end_x && sides[sc].x<px_size.w)
            {
                index=(sides[sc].y*px_size.w)+sides[sc].x;
                if(index<px_arr_size)
                {
                    pixmap[index]=color;
                }

                sides[sc].x++;
            }
            sides[sc].x=sides[sc].start_x;
            sides[sc].y++;
        }
        sc++;
    }


}
// -------------------------------------------------------------- DRAW OUTLINE ROUND RECT
void draw_outline_round_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,int border_max_rad,Uint32 color)
{
    int index=0;
    int px_arr_size=px_size.h*px_size.w;
    if(bold>(rect.w/2) || bold>(rect.h/2))
    {
        draw_solid_rect(pixmap, px_size, rect, color);
        return;
    }

    int mid_h=rect.h-2*bold;
    int mid_w=rect.w-2*bold;

    DRAW_POINTS sides[4];

    sides[0].start_y=rect.y;
    sides[0].start_x=rect.x;
    sides[0].end_y=rect.y+bold;
    sides[0].end_x=rect.x+rect.w;

    sides[1].start_y=rect.y+bold;
    sides[1].start_x=rect.x;
    sides[1].end_y=rect.y+bold+mid_h;
    sides[1].end_x=rect.x+bold;

    sides[2].start_y=rect.y+bold;
    sides[2].start_x=rect.x+bold+mid_w;
    sides[2].end_y=rect.y+bold+mid_h;
    sides[2].end_x=rect.x+rect.w;

    sides[3].start_y=rect.y+bold+mid_h;
    sides[3].start_x=rect.x;
    sides[3].end_y=rect.y+rect.h;
    sides[3].end_x=rect.x+rect.w;

    int sc=0;

    while(sc<4)
    {
        sides[sc].y=sides[sc].start_y;
        sides[sc].x=sides[sc].start_x;

        while(sides[sc].y<sides[sc].end_y && sides[sc].y<px_size.h)
        {
            while(sides[sc].x<sides[sc].end_x && sides[sc].x<px_size.w)
            {
                index=(sides[sc].y*px_size.w)+sides[sc].x;
                if(index<px_arr_size)
                {
                    pixmap[index]=color;
                }

                sides[sc].x++;
            }
            sides[sc].x=sides[sc].start_x;
            sides[sc].y++;
        }
        sc++;
    }
}
// -------------------------------------------------------------- DRAW OUTLINE ROUND RECT
void draw_outline_round_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,Uint32 color)
{
    if(rect.x < 0){rect.x = 0;}
    if(rect.y < 0){rect.y = 0;}

    if(rect.x >= px_size.w){return;}
    if(rect.y >= px_size.h){return;}

    int DIA = 16;
    int RAD = DIA / 2;

    if(rect.w < DIA){rect.w = DIA;}
    if(rect.h < DIA){rect.h = DIA;}

    int x = rect.x;
    int y = rect.y;

    int end_x = rect.x + rect.w;
    int end_y = rect.y + rect.h;

    double dist_center_x;
    double dist_center_y;



    while(y < end_y && y < px_size.h)
    {
        while(x < end_x && x < px_size.w)
        {
            //MARKER: unfinished
            x++;
        }
        x = rect.x;
        y++;
    }
}

// -------------------------------------------------------------- DRAW PATCH RECT
void draw_patch_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,Uint32 color)
{

    DRAW_POINTS DRAW;
    int point_w=2;
    int point_sqr=point_w*point_w;
    int p=0;
    int index;

    DRAW.start_y=rect.y;
    DRAW.start_x=rect.x;
    DRAW.end_y=rect.y+rect.h;
    DRAW.end_x=rect.x+rect.w;

    if(DRAW.start_y<0){DRAW.start_y=0;}
    if(DRAW.start_x<0){DRAW.start_x=0;}
    if(DRAW.start_y>px_size.h){DRAW.start_y=px_size.h;}
    if(DRAW.start_x>px_size.w){DRAW.start_x=px_size.w;}

    if(DRAW.end_y<0){DRAW.end_y=0;}
    if(DRAW.end_x<0){DRAW.end_x=0;}
    if(DRAW.end_y>px_size.h){DRAW.end_y=px_size.h;}
    if(DRAW.end_x>px_size.w){DRAW.end_x=px_size.w;}

    DRAW.y=DRAW.start_y;
    DRAW.x=DRAW.start_x;

    Uint32 extract;


    while(DRAW.x < DRAW.end_x)
    {
        p=0;
        while(p<point_sqr)
        {

            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;
            pixmap[index]=color;

            p++;
        }
        DRAW.x+= 2 * point_w;
    }
    DRAW.x-=point_w;

    while(DRAW.y < DRAW.end_y)
    {
        p=0;
        while(p<point_sqr)
        {

            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;
            pixmap[index]=color;

            p++;
        }
        DRAW.y+= 2 * point_w;
    }

    DRAW.y=DRAW.start_y;
    DRAW.x=DRAW.start_x;

    while(DRAW.y < DRAW.end_y)
    {
        p=0;
        while(p<point_sqr)
        {

            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;

            pixmap[index]=color;

            p++;
        }
        DRAW.y+= 2 * point_w;
    }
    DRAW.y-=point_w;
    while(DRAW.x < DRAW.end_x)
    {
        p=0;
        while(p<point_sqr)
        {

            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;
            pixmap[index]=color;

            p++;
        }
        DRAW.x+= 2 * point_w;
    }
}

// -------------------------------------------------------------- DRAW PATCH RECT
void draw_patch_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,unsigned char color)
{

    DRAW_POINTS DRAW;
    int point_w=2;
    int point_sqr=point_w*point_w;
    int p=0;
    int index;

    DRAW.start_y=rect.y;
    DRAW.start_x=rect.x;
    DRAW.end_y=rect.y+rect.h;
    DRAW.end_x=rect.x+rect.w;

    if(DRAW.start_y<0){DRAW.start_y=0;}
    if(DRAW.start_x<0){DRAW.start_x=0;}
    if(DRAW.start_y>px_size.h){DRAW.start_y=px_size.h;}
    if(DRAW.start_x>px_size.w){DRAW.start_x=px_size.w;}

    if(DRAW.end_y<0){DRAW.end_y=0;}
    if(DRAW.end_x<0){DRAW.end_x=0;}
    if(DRAW.end_y>px_size.h){DRAW.end_y=px_size.h;}
    if(DRAW.end_x>px_size.w){DRAW.end_x=px_size.w;}

    DRAW.y=DRAW.start_y;
    DRAW.x=DRAW.start_x;

    Uint32 extract;

    while(DRAW.x < DRAW.end_x)
    {
        p=0;
        while(p<point_sqr)
        {
            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;
            pixmap[index]=color;

            p++;
        }
        DRAW.x+= 2 * point_w;
    }
    DRAW.x-=point_w;

    while(DRAW.y < DRAW.end_y)
    {
        p=0;
        while(p<point_sqr)
        {
            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;
            pixmap[index]=color;

            p++;
        }
        DRAW.y+= 2 * point_w;
    }

    DRAW.y=DRAW.start_y;
    DRAW.x=DRAW.start_x;

    while(DRAW.y < DRAW.end_y)
    {
        p=0;
        while(p<point_sqr)
        {

            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;

            pixmap[index]=color;

            p++;
        }
        DRAW.y+= 2 * point_w;
    }
    DRAW.y-=point_w;
    while(DRAW.x < DRAW.end_x)
    {
        p=0;
        while(p<point_sqr)
        {

            index=((DRAW.y+p/point_w)*px_size.w)+DRAW.x+p%point_w;
            pixmap[index]=color;

            p++;
        }
        DRAW.x+= 2 * point_w;
    }
}

// -------------------------------------------------------------- DRAW PATCH RECT LINE
void draw_patch_rect_line(Uint32* pixmap,SIZE px_size,POINT orig,int length,int orient,Uint32 color)
{
    POINT end;
    int dot_len=3;
    int dot_wid=3;
    int dot_gap=3;
    int d=0;//dot counter
    int g=0;//gap counter
    bool on_dot=true;

    int index;

    //VERTICAL
    if(orient==0)
    {
        end.y=orig.y+length;
        if(orig.y<0){orig.y=0;}
        if(end.y>px_size.h){end.y=px_size.h;}

        if(orig.x<0 || orig.x>px_size.w-dot_wid){return;}
        if(end.y<0){return;}
        length=end.y-orig.y;
        //---------draw vertical
        int i=orig.y;
        int j=0;

        while(j<dot_wid)
        {
            d=0;
            g=0;
            while(i<end.y)
            {
                if(on_dot)
                {
                    index=i*px_size.w+orig.x;
                    pixmap[index]=color;
                    d++;
                    if(d==dot_len){on_dot=false;d=0;}

                }else{
                    g++;
                    if(g==dot_gap){on_dot=true;g=0;}
                }
                i++;
            }
            orig.x++;
            j++;
        }
    }
    //HORIZONTAL
    if(orient==1)
    {
        end.x=orig.x+length;
        if(orig.x<0){orig.x=0;}
        if(end.x>px_size.w){end.x=px_size.w;}

        if(orig.y<0 || orig.y>px_size.h-dot_wid){return;}
        if(end.x<0){return;}
        length=end.x-orig.x;
        //---------draw horizontal
        int i=orig.x;
        int j=0;

        while(j<dot_wid)
        {
            d=0;
            g=0;
            while(i<end.x)
            {
                if(on_dot)
                {
                    index=orig.y*px_size.w+i;
                    pixmap[index]=color;
                    d++;
                    if(d==dot_len){on_dot=false;d=0;}

                }else{
                    g++;
                    if(g==dot_gap){on_dot=true;g=0;}
                }
                i++;
            }
            orig.y++;
            j++;
        }

    }
}


// -------------------------------------------------------------- DRAW SCROLLBAR RECT
void draw_scroll_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,bool dark, int border_displacement)
{
    //this function has no boundary control! ! !
    //if border_displacement is 0 the border will be at the edge.
    //the bigger border_displacement the more displaced to the inside border will be
    int pre_index=0;
    int index=0;

    int px_arr_size=px_size.h*px_size.w;

    Uint32 main_color;
    Uint32 border_color;

    if(dark)
    {
        main_color = 0;
        border_color = 255;
    }else{
        main_color = 255;
        border_color = 0;
    }

    DRAW_POINTS R;

    R.start_y=rect.y;
    R.start_x=rect.x;
    R.end_y=rect.y+rect.h;
    R.end_x=rect.x+rect.w;

    R.y=R.start_y;
    R.x=R.start_x;

    int B_sx = R.start_x + border_displacement;
    int B_sy = R.start_y + border_displacement;
    int B_ex = (R.end_x-1) - border_displacement;
    int B_ey = (R.end_y-1) - border_displacement;

    while(R.y < R.end_y)
    {
        pre_index = R.y * px_size.w;
        while(R.x < R.end_x)
        {
            index = pre_index + R.x;
            if(R.x == B_sx || R.x == B_ex || R.y == B_sy || R.y == B_ey)
            {
                pixmap[index]=border_color;
            }else{
                pixmap[index]=main_color;
            }            
            R.x++;
        }
        R.x=R.start_x;
        R.y++;
    }
}
// -------------------------------------------------------------- DRAW SCROLLBAR RECT
void draw_scroll_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,bool dark, int border_displacement)
{
    //this function has no boundary control! ! !
    //if border_displacement is 0 the border will be at the edge.
    //the bigger border_displacement the more displaced to the inside border will be
    int pre_index=0;
    int index=0;

    int px_arr_size=px_size.h*px_size.w;

    Uint32 main_color;
    Uint32 border_color;

    if(dark)
    {
        main_color = 0xFF000000;
        border_color = 0xFFFFFFFF;
    }else{
        main_color = 0xFFFFFFFF;
        border_color = 0xFF000000;
    }

    DRAW_POINTS R;

    R.start_y=rect.y;
    R.start_x=rect.x;
    R.end_y=rect.y+rect.h;
    R.end_x=rect.x+rect.w;

    R.y=R.start_y;
    R.x=R.start_x;

    int B_sx = R.start_x + border_displacement;
    int B_sy = R.start_y + border_displacement;
    int B_ex = (R.end_x-1) - border_displacement;
    int B_ey = (R.end_y-1) - border_displacement;


    while(R.y < R.end_y)
    {
        pre_index = R.y * px_size.w;
        while(R.x < R.end_x)
        {
            index = pre_index + R.x;
            if(R.x == B_sx || R.x == B_ex || R.y == B_sy || R.y == B_ey)
            {
                pixmap[index]=border_color;
            }else{
                pixmap[index]=main_color;
            }
            R.x++;
        }
        R.x=R.start_x;
        R.y++;
    }
}
// -------------------------------------------------------------- DRAW SCROLLBAR RECT
void draw_slider_handle(unsigned char* pixmap, SIZE px_size,int handle_w, int handle_h, unsigned char color)
{
    //this draws the slider handle at x:0 y:0

    int y = 0;
    int x = 0;

    int index = 0;

    while(y < px_size.h && y < handle_h)
    {
        while(x < px_size.w && x < handle_w)
        {
            //drawing
            if(y <= 1 || y <= handle_w || x <= 1 || x <= handle_w)
            {
                index = (y * px_size.w) + x;
                pixmap[index] = color;
            }

            x++;
        }

        x = 0;
        y++;
    }

}