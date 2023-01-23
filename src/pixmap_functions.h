//
//  global_definitions.h
//  Soil Framework
//
//  Created by Vinicius Baez on 16/11/2020.

#ifndef pixmap_functions_h
#define pixmap_functions_h

#include <math.h>

#include "string_functions.h"
#include "load_save_files.h"

// ------------------------------------------------------ CONVERT MM TO PIXELS
int mm_to_px(int dpi, int mm);
// ------------------------------------------------------ RESET_SIZE
void reset_size(SIZE* size);
// ------------------------------------------------------ PRINTF SOIL RECT
void printf_soil_rect(SOIL_RECT rect, int id=(-1));
// ------------------------------------------------------ PRINTF SOIL BOUND
void printf_soil_bound(BOUND_RECT bound, int id=(-1));
// ------------------------------------------------------ PRINTF SIZE
void printf_size(SIZE size, int id=-1);
// ------------------------------------------------------ RESET RECT
void reset_rect(SOIL_RECT* rect);
// ------------------------------------------------------ SET RECT ABSOLUTE
void set_rect_abs(SOIL_RECT* rect);
// ------------------------------------------------------ RESET BOUND
void reset_bound(BOUND_RECT* bound);
// ------------------------------------------------------ VALIDATE RECT WITH SIZE
bool validate_rect_in_size(SOIL_RECT* rect, SIZE size);
// ------------------------------------------------------ ADJUST RECT TO SIZE
bool adjust_rect_to_size(SIZE size, SOIL_RECT* rect);
// ------------------------------------------------------ RECT INTERSECTION
SOIL_RECT rect_intersection(SOIL_RECT rect_1, SOIL_RECT rect_2);
// ------------------------------------------------------ RECT INTERSECTION
BOUND_RECT bound_intersection(BOUND_RECT bound_1, BOUND_RECT bound_2);
// ------------------------------------------------------ RECT INTERSECT TWO BASE REFERENCE
void rect_intersection_two_base_ref(SOIL_RECT rect_1, SOIL_RECT rect_2,SOIL_RECT* inter_rect_1, SOIL_RECT* inter_rect_2);
// ------------------------------------------------------ SUM TO RECT POS
SOIL_RECT add_to_rect_pos(SOIL_RECT rect_1, int w, int h);
// ------------------------------------------------------ SUM TO BOUND POS
BOUND_RECT add_to_bound_pos(BOUND_RECT bound, int w, int h);
// ------------------------------------------------------ SUBTRACT TO RECT POS
SOIL_RECT subtract_from_rect_pos(SOIL_RECT rect_1, int w, int h);
// ------------------------------------------------------ SUBTRACT TO BOUND POS
BOUND_RECT subtract_from_bound_pos(BOUND_RECT bound, int w, int h);
// ------------------------------------------------------ SUM TO RECT AREA
SOIL_RECT add_to_rect_area(SOIL_RECT rect_1, int w, int h);
// ------------------------------------------------------ SUM TO BOUND AREA
BOUND_RECT add_to_bound_area(BOUND_RECT bound, int w, int h);
// ------------------------------------------------------ SUBTRACT TO RECT AREA
SOIL_RECT subtract_from_rect_area(SOIL_RECT rect_1, int w, int h);
// ------------------------------------------------------ SUBTRACT TO BOUND AREA
BOUND_RECT subtract_from_bound_area(BOUND_RECT bound, int w, int h);
// ------------------------------------------------------ RECT TO BOUND
BOUND_RECT rect_to_bound(SOIL_RECT rect);
// ------------------------------------------------------ BOUND TO RECT
SOIL_RECT bound_to_rect(BOUND_RECT bound);
// ------------------------------------------------------ POINT IN BOUND
bool point_in_bound(BOUND_RECT B, POINT P);
// ------------------------------------------------------ POINT IN BOUND
bool point_in_bound(BOUND_RECT B, int X, int Y);
// ------------------------------------------------------ POINT IN RECT
bool point_in_rect(SOIL_RECT R, POINT P);
// ------------------------------------------------------ POINT IN RECT
bool point_in_rect(SOIL_RECT R, int X, int Y);
// ------------------------------------------------------ ADJUST RECT TO PX SIZE
void adjust_rect_to_px_size(SIZE size,SOIL_RECT *rect);
// ------------------------------------------------------ ADJUST ORIG RECT AND DEST RECT
void adjust_orig_and_dest_rect(SIZE orig_size,SOIL_RECT *orig_rect, SIZE dest_size, SOIL_RECT *dest_rect);
// ------------------------------------------------------ ADJUST LINE PATH TO PX SIZE
void adjust_line_path_to_px_size(SIZE size,POINT* point, int* end_x, int* end_y);
// ------------------------------------------------------ ADJUST HORIZONTAL LINE PATH TO PX SIZE
void adjust_h_line_path_to_px_size(SIZE size,POINT* point, int* end_x);
// ------------------------------------------------------ ADJUST VERTICAL LINE PATH TO PX SIZE
void adjust_v_line_path_to_px_size(SIZE size,POINT* point, int* end_y);
// ------------------------------------------------------ RETURN UCHAR FROM PIXEL VALUE
unsigned char return_uchar_from_pix(Uint32 orig_pix);
// ------------------------------------------------------ RETURN UCHAR FROM PIXEL VALUE
unsigned char return_uchar_from_pix(PIXDATA orig_pix);
// ------------------------------------------------------ RETURN UINT32 FROM PIXEL VALUE
Uint32 return_uint32_from_pix(unsigned char orig_pix);
// ------------------------------------------------------ RETURN UINT32 FROM PIXEL VALUE
Uint32 return_uint32_from_pix(PIXDATA orig_pix);
// ------------------------------------------------------ RETURN PIXDATA FROM PIXEL VALUE
PIXDATA return_pixdata_from_pix(unsigned char orig_pix);
// ------------------------------------------------------ RETURN PIXDATA FROM PIXEL VALUE
PIXDATA return_pixdata_from_pix(Uint32 orig_pix);
//--------------------------------------------------- CORRECT IMAGE RATIO IN VIEW
SIZE correct_image_ratio_in_view(SIZE orig_img_size, SIZE view_size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(Uint32* arr, Uint32 value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(int* arr, int value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(double* arr, double value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(POINT* arr, POINT value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(D_POINT* arr, D_POINT value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(CALC_PIX_COLOR* arr, CALC_PIX_COLOR value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_array(PIXDATA_FLAG* arr, PIXDATA_FLAG value, int size);
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(Uint32 *pixmap, SIZE size,Uint32 color);
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(Uint32 *pixmap, SIZE size, SOIL_RECT rect, Uint32 color);
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(unsigned char *pixmap, SIZE size,unsigned char color);
// ------------------------------------------------------ FILL PIXMAP
void fill_pixmap(unsigned char *pixmap, SIZE size, SOIL_RECT rect, unsigned char color);
// ------------------------------------------------------ FLIP PIXMAP
void flip_pixmap(Uint32 *orig_pixmap, Uint32 *dest_pixmap, SIZE size, int dir);
// ------------------------------------------------------ TRANSPOSE CHAR ARRAY TO UINT32 ARRAY
void transpose_char_to_uint32_arr_alloc(char* orig, int orig_h,int orig_w,Uint32* dest,SIZE * dest_size);
// ------------------------------------------------------ SET MAX ALPHA VALUE
void set_max_alpha_value(Uint32* pixmap, SIZE size,unsigned char max_value);
// ------------------------------------------------------ INVERT COLORS
void invert_colors(unsigned char *pixmap,SIZE size);
// ------------------------------------------------------ INVERT COLORS
void invert_colors(Uint32 *pixmap,SIZE size);
// ------------------------------------------------------ INVERT COLORS
void invert_colors(unsigned char *pixmap,SIZE size,SOIL_RECT rect);
// ------------------------------------------------------ INVERT COLORS
void invert_colors(Uint32 *pixmap,SIZE size,SOIL_RECT rect);
// ------------------------------------------------------ ALPHA BLEND
Uint32 alpha_blend_pixel(unsigned char base_pixel,unsigned char top_pixel);
// ------------------------------------------------------ ALPHA BLEND
Uint32 alpha_blend_pixel(Uint32 base_pixel,Uint32 top_pixel);
// ------------------------------------------------------ ALPHA BLEND
PIXDATA alpha_blend_pixdata(PIXDATA base_pixdata,PIXDATA top_pixdata);
// ------------------------------------------------------ ALPHA BLEND PIXMAP
void alpha_blend_pixmap(const Uint32 *base_pixmap,SIZE base_size, SOIL_RECT base_rect, const Uint32 *top_pixmap,SIZE top_size, SOIL_RECT top_rect,Uint32* dest_pixmap, SIZE dest_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ INVERT COLORS
void invert_opaque_colors(Uint32 *pixmap,SIZE size);

// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const unsigned char* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const unsigned char* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ COPY CONST PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ COPY CONST PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(const Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(PIXDATA* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(PIXDATA_FLAG* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP IN CERTAIN POSITION
void paste_pixmap_to_pixmap_in_pos(const Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect, int pos);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_sdl_surface(Uint32* orig_pixmap, SIZE orig_px_size,SOIL_RECT orig_rect,SDL_Surface* sdl_surface, SIZE surface_size, SOIL_RECT dest_rect);

//-------------- Here you have paste pixmap using BOUND_RECT instead of SOIL_RECT
//-------------- I created these for SoilText::draw()
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,unsigned char* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,Uint32* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(Uint32* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,unsigned char* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound);
// ------------------------------------------------------ COPY PIXMAP TO PIXMAP
void paste_pixmap_to_pixmap(Uint32* orig_pixmap, SIZE orig_px_size,BOUND_RECT orig_bound,Uint32* dest_pixmap, SIZE dest_px_size, BOUND_RECT dest_bound);



// ------------------------------------------------------ LOAD PIXMAP FROM BMP 32 FILE
// it returns the the amount of pixel copied
// file must be opened
int load_pixmap_from_bmp_32_file(Uint32* pixmap, SIZE px_size,FILE* file);
// ------------------------------------------------------ ROUND DOUBLE
//if you simply typecast the double to int it simply loses the fractional part
//2.8 becomes 2, so this function rounds it to the closest integer
int round_double(double num);
// ------------------------------------------------------ TRUNCATE DOUBLE
// works if num is negative
double trunc_double(double num,int dec_points);
// ------------------------------------------------------ DOUBLE MODULUS
//divide integer part of double by int and returns rest plus decimals
// it works if any or both of the arguments are negative
double double_modulus(double num, int div);
// ------------------------------------------------------ DOUBLE MODULUS
//divide double by double to find integer quocient and returns rest
// it works if any or both of the arguments are negative
double double_modulus(double num, double div);
// ------------------------------------------------------ THE DOUBLE TO NEXT INTEGER
// returns the difference to the next integer
// crescent indicates if the next int is in crecent or decrescent order
double double_to_next_int(double num, bool crescent=true);

// ------------------------------------------------------ RESIZE PIXMAP
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(unsigned char* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(Uint32* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, unsigned char* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// ------------------------------------------------------ RESIZE PIXMAP 5
//former resize_pixmap_5()
//this is the one to resize to any size
//and choose the rect from which take the image and to which resize it
void resize_pixmap(Uint32* orig_pixmap, SIZE orig_px_size, SOIL_RECT orig_rect, Uint32* dest_pixmap, SIZE dest_px_size, SOIL_RECT dest_rect);
// -------------------------------------------------------------- RETURN SINE(no look-up table)
double return_sine(double angle);
// -------------------------------------------------------------- RETURN SINE
double return_sine(int angle);
// -------------------------------------------------------------- POSITIVE
double turn_positive(double num);
// -------------------------------------------------------------- POSITIVE
int turn_positive(int num);
// -------------------------------------------------------------- SQUARE ROOT
//precision is defined in binary reduction from the original range
//every 10 of precision is the equivalent of 1024 times more precise
//the range is defined with up_lim and mn_lim
double square_root(double num,int precision=50);
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(POINT START,POINT END,SIZE* SIDE);
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(POINT START,POINT END,D_SIZE* SIDE);
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(D_POINT START,D_POINT END,SIZE* SIDE);
// -------------------------------------------------------------- GET TRIG SIDES
void get_trig_sides(D_POINT START,D_POINT END,D_SIZE* SIDE);
// -------------------------------------------------------------- GET HYPOTENUSE
//always positive
double get_hypotenuse(double cat_1,double cat_2,int precision=50);
// -------------------------------------------------------------- GET INCREMENT
void get_increment(SIZE SIDE,D_POINT* INC);
// -------------------------------------------------------------- GET INCREMENT
void get_increment(D_SIZE SIDE,D_POINT* INC);
// -------------------------------------------------------------- GET INCREMENT
void get_increment(D_SIZE SIDE,D_POINT* INC,double maj_inc);
// -------------------------------------------------------------- GET INCREMENT
// it algso giver the increment for the hypotenuse when
// rotating pixmap
// the hypotenuse increment will always be positive because
// it is meant to be used in the coords for the BUFFER
void get_tri_increment(D_SIZE SIDE,D_POINT* SIDE_INC, double* HYP_INC);
// -------------------------------------------------------------- GET TRIANGLE RATIO
// the hypotenuse woulb be 1
void get_ratio(double cat_1,double cat_2,double hyp,double* cat_1_ratio,double* cat_2_ratio,double* hyp_ratio);
// -------------------------------------------------------------- GET RATIO
//the idea is for the cat to be smaller than hyp so ratio wont be bigger than 1
double get_ratio(double cat,double hyp);
// -------------------------------------------------------------- IS PRIME
bool is_prime(unsigned long num);
// -------------------------------------------------------------- MMC
long mmc(long num_1,long num_2);
// -------------------------------------------------------------- MMC
double mmc(double num_1,double num_2);
// -------------------------------------------------------------- LINE THROUGH BASE
// the line can't end on this BASE
// it will return the number(0 to 7) corresponding to where the line went through
// 0 is te BASE[0] corner, 1 is the top side, 2 is the BASE[1] corner...
int line_through_base(POINT BASE[4],D_POINT T_pt,int T_CORN,double INC_rat_yx,double INC_rat_xy,D_POINT* INTER_pt);
// -------------------------------------------------------------- CORNER IS IN BASE
bool corner_is_in_base(POINT BASE_0, D_POINT T_pt);
// -------------------------------------------------------------- BASE CORNER IS IN TILT
bool base_corner_in_tilt(int step,D_POINT TILT[4],double INC_x,double INC_rat_yx,double INC_rat_xy,POINT B_pt);

// -------------------------------------------------------------- LINES CROSS
bool lines_cross(POINT A1, POINT A2, POINT B1, POINT B2);

// -------------------------------------------------------------- POLYGON AREA
double polygon_area(D_POINT* CORNERS,int corn_n);

// -------------------------------------------------------------- DRAW AA LINE
void draw_aa_line(unsigned char* pixmap,SIZE px_size,POINT START,POINT END,int bold,unsigned char color);
// -------------------------------------------------------------- DRAW AA LINE
void draw_aa_line(Uint32* pixmap,SIZE px_size,POINT START,POINT END,int bold,Uint32 color);

// -------------------------------------------------------------- DRAW LINE
void draw_straight_line(Uint32* pixmap,SIZE px_size,POINT START,POINT END,int bold,Uint32 color);
// -------------------------------------------------------------- DRAW HORIZONTAL LINE
void draw_h_line(unsigned char* pixmap, SIZE px_size, POINT start, int end_x, unsigned char color);
// -------------------------------------------------------------- DRAW HORIZONTAL LINE
void draw_h_line(Uint32* pixmap, SIZE px_size, POINT start, int end_x, Uint32 color);
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_v_line(unsigned char* pixmap,SIZE px_size,POINT start, int end_y, unsigned char color);
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_v_line(Uint32* pixmap,SIZE px_size,POINT start, int end_y, Uint32 color);
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_cursor_line(unsigned char* pixmap,SIZE px_size,POINT start, int end_y, unsigned char color);
// -------------------------------------------------------------- DRAW VERTICAL LINE
void draw_cursor_line(Uint32* pixmap,SIZE px_size,POINT start, int end_y, Uint32 color);
// -------------------------------------------------------------- ROTATE POINT TO AXIS
POINT rotate_point_in_axis(POINT SOURCE,int angle,POINT axis);
// -------------------------------------------------------------- ROTATE POINT TO AXIS
D_POINT rotate_d_point_in_axis(D_POINT SOURCE,double angle,D_POINT axis);
// -------------------------------------------------------------- DRAW LINE
void rotate_right_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int dir,POINT axis);
// -------------------------------------------------------------- COPY RECT TO UINT BUFFER
//auxiliary for rotation
//the buffer must already be allocated
void copy_rect_to_buffer(Uint32* pixmap,SIZE px_size,Uint32* buffer,SOIL_RECT rect, bool delete_area);
// -------------------------------------------------------------- COPY RECT TO CALC BUFFER
//auxiliary for rotation
//the buffer must already be allocated
void copy_rect_to_buffer(Uint32* pixmap,SIZE px_size,PIXDATA* buffer,SOIL_RECT rect, bool delete_area);
// -------------------------------------------------------------- COPY RECT TO CALC BUFFER
//auxiliary for rotation
//the buffer must already be allocated
void copy_rect_to_buffer(Uint32* pixmap,SIZE px_size,CALC_PIX_COLOR* buffer,SOIL_RECT rect,double fac, bool delete_area);

// -------------------------------------------------------------- MAKE CORNER SAMPLE FROM CORNER IMAGE
void make_corner_sample_from_border_img(Uint32* border_pixmap,SIZE border_px_size,Uint32* corner_pixmap,SIZE corner_px_size);
// -------------------------------------------------------------- LOAD BORDER FROM CORNER SAMPLE
void load_border_from_corner_sample(Uint32* corner_pixmap,SIZE corner_px_size,Uint32* border_pixmap,SIZE border_px_size);


// -------------------------------------------------------------- DRAW SOLID RECT
void draw_solid_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,unsigned char color);
// -------------------------------------------------------------- DRAW SOLID RECT
void draw_solid_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,Uint32 color);
// -------------------------------------------------------------- DRAW OUTLINE RECT
void draw_outline_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,int bold,unsigned char color);
// -------------------------------------------------------------- DRAW OUTLINE RECT
void draw_outline_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,Uint32 color);
// -------------------------------------------------------------- DRAW SOLID ROUND RECT
void draw_solid_round_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,int border_max_rad,Uint32 color);
// -------------------------------------------------------------- DRAW OUTLINE ROUND RECT
void draw_outline_round_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,int border_max_rad,Uint32 color);
// -------------------------------------------------------------- DRAW OUTLINE ROUND RECT
void draw_outline_round_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,int bold,Uint32 color);
// -------------------------------------------------------------- DRAW PATCH RECT
void draw_patch_rect(Uint32* pixmap,SIZE px_size,SOIL_RECT rect,Uint32 color);
// -------------------------------------------------------------- DRAW PATCH RECT
void draw_patch_rect(unsigned char* pixmap,SIZE px_size,SOIL_RECT rect,unsigned char color);
// -------------------------------------------------------------- DRAW PATCH RECT LINE
void draw_patch_rect_line(Uint32* pixmap,SIZE px_size,POINT orig,int length,int orient,Uint32 color);
// -------------------------------------------------------------- DRAW SCROLLBAR RECT
void draw_scroll_rect(unsigned char* pixmap, SIZE px_size, SOIL_RECT rect, bool dark, int border_displacement);
// -------------------------------------------------------------- DRAW SCROLLBAR RECT
void draw_scroll_rect(Uint32* pixmap, SIZE px_size, SOIL_RECT rect, bool dark, int border_displacement);
// -------------------------------------------------------------- DRAW SCROLLBAR RECT
void draw_slider_handle(unsigned char* pixmap, SIZE px_size,int handle_w, int handle_h, unsigned char color);


#endif /* pixmap_functions_h */
