#ifndef LOAD_SAVE_FILES
#define LOAD_SAVE_FILES

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "global_definitions.h"

// -------------------------------------------------------------- GET INT FILE SIZE
bool get_int_from_file(FILE* f,int * num);
// -------------------------------------------------------------- WRITE INT TO FILE
bool write_int_to_file(FILE* f,int num);
// -------------------------------------------------------------- GET IMG FILE SIZE
bool get_img_size_from_file(SIZE* px_size,std::string file_name);
// -------------------------------------------------------------- SDL_SURFACE TO PIXMAP
bool sdl_surface_to_pixmap(SDL_Surface* surface, unsigned char* mono_px_pt, Uint32* color_px_pt, bool from_color, SIZE px_size);
// -------------------------------------------------------------- SDL_SURFACE TO PIXMAP
bool sdl_surface_to_pixmap(SDL_Surface* surface, unsigned char* px_pt, SIZE px_size);
// -------------------------------------------------------------- SDL_SURFACE TO PIXMAP
bool sdl_surface_to_pixmap(SDL_Surface* surface, Uint32* px_pt, SIZE px_size);
// -------------------------------------------------------------- LOAD IMG FILE
bool load_img_file(unsigned char* pixmap,SIZE px_size,std::string file_name);
// -------------------------------------------------------------- LOAD IMG FILE
bool load_img_file(Uint32* pixmap,SIZE px_size,std::string file_name);
// -------------------------------------------------------------- LOAD PNG FILE PIXMAP ALLOC
// it will return the pointer to allcoated memory, make sure to delete somewhere else later
// px_size is a reference to a SIZE variable that will receive the values for the pixmap size
Uint32* load_img_file_px_alloc(SIZE* px_size,std::string file_name);
// -------------------------------------------------------------- SAVE BMP FILE
//bool save_bmp_32(unsigned char* pixmap,SIZE px_size,std::string file_name);
// -------------------------------------------------------------- SAVE BMP FILE
bool save_bmp_32(Uint32* pixmap,SIZE px_size,std::string file_name);
// -------------------------------------------------------------- SAVE BMP FILE
bool save_img_file(unsigned char* pixmap,SIZE px_size,std::string file_name,int format, int cmpr);
// -------------------------------------------------------------- SAVE BMP FILE
bool save_img_file(Uint32* pixmap,SIZE px_size,std::string file_name,int format, int cmpr);
// -------------------------------------------------------------- CONCATENATE PIXMAPS HORIZONTALLY
Uint32* cat_pixmap_horizontal(Uint32* pixmap_1,SIZE px_1_size,Uint32* pixmap_2,SIZE px_2_size,SIZE* result_size);
// ------------------------------------------------------ WRITE QSTRING TO FILE
//void write_to_file(QString content, QFile * file);
// ------------------------------------------------------ BYTE NUMBER TO TEXT
void int_to_hex_char(Uint32 num, char *c1, char *c2);

#endif // LOAD_SAVE_FILES

