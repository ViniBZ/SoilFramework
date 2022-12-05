#include "load_save_files.h"
#include "pixmap_functions.h"

// -------------------------------------------------------------- GET INT FILE SIZE
bool get_int_from_file(FILE* f,int * num)
{
    if(f == NULL){return false;}
    int shift_n = 24;
    *num = 0;
    int val = 0;
    int i = 0;
    while(i < 4)
    {
        val = fgetc(f);
        if(val == EOF){return false;}
        *num += val << shift_n;
        shift_n -= 8;

        i++;
    }
    return true;

}
// -------------------------------------------------------------- GET INT FILE SIZE
bool write_int_to_file(FILE* f,int num)
{
    if(f == NULL){return false;}
    int ch;
    unsigned int val = 0;
    int shift_n = 8;
    unsigned int fac = 0xFF000000;
    int i = 0;
    while(i < 4)
    {
        val = num & fac;
        val = val >> (3-i)*8;
        fputc(val, f);
        fac = fac >> shift_n;    

        i++;
    }
    return true;

}
// -------------------------------------------------------------- GET IMG FILE SIZE
bool get_img_size_from_file(SIZE* img_size,std::string file_name)
{
    SDL_Surface * image = IMG_Load(file_name.c_str());

    if(!image)
	{
        //ERROR
        if(stderr != NULL)
        {
            fprintf(stderr, "LOAD IMG FAIL TO GET SIZE 1: %s",file_name.c_str());
        }
        return false;
	}
    img_size->w = image->w;
    img_size->h = image->h;

    SDL_FreeSurface(image);

    return true;
}
// -------------------------------------------------------------- SDL_SURFACE TO PIXMAP
bool sdl_surface_to_pixmap(SDL_Surface* surface, unsigned char* mono_px_pt, Uint32* color_px_pt, bool from_color, SIZE px_size)
{
    if(!surface)
	{
        return false;
	}
    if(from_color)
    {
        if(color_px_pt == NULL)
        {
            return false;
        }
    }else{
        if(mono_px_pt == NULL)
        {
            return false;
        }
    }

    if(px_size.w == 0 || px_size.h == 0)
    {
        return false;
    }

    int w=surface->w;
    int h=surface->h;

    SDL_PixelFormat* format=surface->format;
    SDL_Palette* palette= format->palette;

    unsigned char * img_pt=(unsigned char*)surface->pixels;

    int bit_f=format->BitsPerPixel;
    
    
    Uint32 bit_mask[4];
    Uint32 bit_fac[4];

    Uint32 bit_index[4]={0,1,2,3};

    unsigned char pix_value[4];

    int i=0;

    bit_mask[0]=format->Bmask;
    bit_mask[1]=format->Gmask;
    bit_mask[2]=format->Rmask;
    bit_mask[3]=format->Amask;
    bit_f/=8;


    if(bit_f > 1)
    {
        while(i<4)
        {
            if(bit_mask[i]!=0)
            {
                if(bit_mask[i]==0x000000FF){bit_index[i]=0;}
                if(bit_mask[i]==0x0000FF00){bit_index[i]=1;}
                if(bit_mask[i]==0x00FF0000){bit_index[i]=2;}
                if(bit_mask[i]==0xFF000000){bit_index[i]=3;}
            }
            i++;
        }
    }

    int y=0;
    int x=0;
    int p=0;
    int b=0;
    int pad=0;

    if(bit_f < 4)
    {
        pad=4-((w*bit_f)%4);
        if(pad==4){pad=0;}
    }

    int pix_i = 0;

    PIXDATA pixdata;
    while(y < h && y < px_size.h)
    {
        x=0;
        while(x<w && x < px_size.w)
        {
            int index=y*w+x;
            b=0;
            pixdata.pix=0;

            while(b<bit_f)
            {

                pix_value[b]=img_pt[p];
                p++;
                b++;
            }

            if(bit_f > 1)
            {
                if(bit_f > 2)
                {
                    pixdata.color.b=pix_value[bit_index[0]];
                    pixdata.color.g=pix_value[bit_index[1]];
                    pixdata.color.r=pix_value[bit_index[2]];
                    if(bit_f == 4)
                    {
                        pixdata.color.a=pix_value[bit_index[3]];
                    }else{
                        pixdata.color.a=255;
                    }
                }
                
            }else{
                pixdata.color.b=pix_value[0];
                pixdata.color.g=pix_value[0];
                pixdata.color.r=pix_value[0];
                pixdata.color.a=255;
            }
            pix_i+=4;
            if(from_color)
            {
                color_px_pt[index] = pixdata.pix;
            }else{
                mono_px_pt[index] = return_uchar_from_pix(pixdata.pix);
            }

            x++;
        }
        if(bit_f < 4){p+=pad;}
        
        y++;
    }

    return true;
}
// -------------------------------------------------------------- SDL_SURFACE TO PIXMAP
bool sdl_surface_to_pixmap(SDL_Surface* surface, unsigned char* mono_px_pt, SIZE px_size)
{
    return sdl_surface_to_pixmap(surface, mono_px_pt, NULL, false, px_size);
}
// -------------------------------------------------------------- SDL_SURFACE TO SOIL_PIXMAP
bool sdl_surface_to_pixmap(SDL_Surface* surface, Uint32* color_px_pt, SIZE px_size)
{
    return sdl_surface_to_pixmap(surface, NULL, color_px_pt, true, px_size);
}
// -------------------------------------------------------------- LOAD IMG FILE
bool load_img_file(unsigned char* pixmap,SIZE px_size,std::string file_name)
{
    bool res = true;
    return res;
}
// -------------------------------------------------------------- LOAD PNG FILE
bool load_img_file(Uint32* pixmap,SIZE px_size,std::string file_name)
{
    //MARKER: apparently it doesn't load well the 8 bit BMP that
    //save_img_file(unsigned char*, SIZE ,std::string ,int) creates

    if(pixmap == NULL){return false;}
    if(px_size.w == 0 || px_size.h == 0){return false;}

    SDL_Surface * surface = IMG_Load(file_name.c_str());

    if(!surface)
	{
        return false;
	}

    bool res = sdl_surface_to_pixmap(surface, pixmap, px_size);
    SDL_FreeSurface(surface);
    return res;
}
// -------------------------------------------------------------- LOAD PNG FILE PIXMAP ALLOC
// it will return the pointer to allcoated memory, make sure to delete somewhere else later
// px_size is a reference to a SIZE variable that will receive the values for the pixmap size
Uint32* load_img_file_px_alloc(SIZE* px_size,std::string file_name)
{
    //MARKER: apparently it doesn't load well the 8 bit BMP that
    //save_img_file(unsigned char*, SIZE ,std::string ,int) creates

    if(px_size == NULL){return NULL;}

    SDL_Surface * surface = IMG_Load(file_name.c_str());

    if(!surface)
	{
        return NULL;
	}

    int w=surface->w;
    int h=surface->h;

    Uint32* pixmap=NULL;
    pixmap=new Uint32[w*h];

    if(pixmap == NULL)
    {
        SDL_FreeSurface(surface);
        return NULL;
    }

    px_size->w = w;
    px_size->h = h;

    bool res = sdl_surface_to_pixmap(surface, pixmap, *px_size);
    SDL_FreeSurface(surface);
    return pixmap;
}

// -------------------------------------------------------------- SAVE BMP FILE
bool save_bmp_32(Uint32* pixmap,SIZE px_size,std::string file_name)
{
    
    FILE *fd = NULL;
    fd = fopen(file_name.c_str(), "w+");
    if(fd == NULL){return false;}

    int c=0;

    int offset=54;
    c=0;

    //in bmp file:
    //18-19: image width
    //22-23:image height

    //unsigned char meta_32[54]={66,77,54,9,61,0,0,0,0,0,54,0,0,0,40,0,0,0,232,3,0,0,24,252,255,255,1,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char meta_32[54]={66,77,54,9,61,0,0,0,0,0,54,0,0,0,40,0,0,0,232,3,0,0,24,252,255,255,1,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    bool write_from_arr=true;

    unsigned char out_uch;
    char out_ch;
    Uint32 out_ch_int;
    int file_size=((px_size.h*px_size.w)*4)+54;

    int height=px_size.h*(-1);
    unsigned int aux_int;


    while(c<offset)
    {
        write_from_arr=true;
        if(c==2)
        {
            aux_int=file_size%256;
            out_uch=aux_int;
            out_ch=out_uch;
            write_from_arr=false;
        }
        if(c==3)
        {
            aux_int=file_size&0x0000FF00;
            aux_int=aux_int >> 8;
            out_uch=aux_int;
            out_ch=out_uch;
            write_from_arr=false;
        }
        if(c==4)
        {
            aux_int=file_size&0x00FF0000;
            aux_int=aux_int >> 16;
            out_uch=aux_int;
            out_ch=out_uch;
            write_from_arr=false;
        }
        if(c==5)
        {
            aux_int=file_size&0xFF000000;
            aux_int=aux_int >> 24;
            out_uch=aux_int;
            out_ch=out_uch;
            write_from_arr=false;
        }
        if(c==18)
        {
            out_uch=px_size.w%256;
            out_ch=out_uch;
            write_from_arr=false;
        }
        if(c==19)
        {
            out_uch=px_size.w/256;
            out_ch=out_uch;
            write_from_arr=false;
        }

        if(c==22)
        {

            aux_int=height&0x000000FF;
            out_uch=aux_int;
            out_ch=out_uch;
            write_from_arr=false;

        }
        if(c==23)
        {
            aux_int=height&0x0000FF00;
            aux_int=aux_int >> 8;
            out_uch=aux_int;
            out_ch=out_uch;

            write_from_arr=false;

        }



        if(write_from_arr)
        {
            out_ch=meta_32[c];
        }
        out_ch_int = out_ch;
        fputc(out_ch_int,fd);
        c++;
    }



    int y=0;
    int x=0;
    while(y<px_size.h)
    {
        while(x<px_size.w)
        {
            PIXDATA pixdata;
            int index=y*px_size.w+x;
            pixdata.pix=pixmap[index];

            out_ch=pixdata.color.b;
            out_ch_int = out_ch;
            fputc(out_ch_int,fd);
            
            out_ch=pixdata.color.g;
            out_ch_int = out_ch;
            fputc(out_ch_int,fd);

            out_ch=pixdata.color.r;
            out_ch_int = out_ch;
            fputc(out_ch_int,fd);

            out_ch=pixdata.color.a;
            out_ch_int = out_ch;
            fputc(out_ch_int,fd);
            x++;
        }
        x=0;
        y++;
    }
    fclose(fd);

    return true;
}
// -------------------------------------------------------------- SAVE BMP FILE
bool save_img_file(unsigned char* pixmap,SIZE px_size,std::string file_name,int format, int cmpr)
{
    return false;
}
// -------------------------------------------------------------- SAVE BMP FILE
bool save_img_file(Uint32* pixmap,SIZE px_size,std::string file_name,int format, int cmpr)
{
    if(format == 0 || format == 1)
    {
        Uint32 pxmask[4];
        //////  JPG mask
        pxmask[0] = 0x00FF0000;
        pxmask[1] = 0x0000FF00;
        pxmask[2] = 0x000000FF;
        pxmask[3] = 0xFF000000;

        SOIL_RECT rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = px_size.w;
        rect.h = px_size.h;

        SDL_Surface* sdl_surface = SDL_CreateRGBSurface(0,px_size.w,px_size.h,32,pxmask[0],pxmask[1],pxmask[2],pxmask[3]);
        paste_pixmap_to_sdl_surface(pixmap,px_size,rect,sdl_surface,px_size,rect);


        if(format == 0)
        {
            //the bigger cmpr is the better the quality of the image
            if(cmpr < 1){cmpr = 1;}
            if(cmpr > 100){cmpr = 100;}
            IMG_SaveJPG(sdl_surface,file_name.c_str(),cmpr);
        }
        if(format == 1)
        {
            IMG_SavePNG(sdl_surface,file_name.c_str());
        }
        SDL_FreeSurface(sdl_surface);
    }
    

    if(format == 2)
    {
        save_bmp_32(pixmap,px_size,file_name.c_str());
    }


    return true;

}

// -------------------------------------------------------------- CONCATENATE PIXMAPS HORIZONTALLY
Uint32* cat_pixmap_horizontal(Uint32* pixmap_1,SIZE px_1_size,Uint32* pixmap_2,SIZE px_2_size,SIZE* result_size)
{
    Uint32* res_pt=NULL;
    SIZE res_size;
    res_size.h=px_1_size.h;
    if(px_2_size.h>px_1_size.h){res_size.h=px_2_size.h;}
    res_size.w=px_1_size.w+px_2_size.w;

    res_pt=new Uint32[res_size.h*res_size.w];

    if(res_pt==NULL){return NULL;}

    fill_pixmap(res_pt,res_size,0xFF000000);

    //PX 1 RECTS
    SOIL_RECT px_1_full_rect;
    px_1_full_rect.y=0;
    px_1_full_rect.x=0;
    px_1_full_rect.h=px_1_size.h;
    px_1_full_rect.w=px_1_size.w;

    //PX 1 pos rect is the same values as its full rect
    //PX 2 RECTS
    SOIL_RECT px_2_full_rect;
    px_2_full_rect.y=0;
    px_2_full_rect.x=0;
    px_2_full_rect.h=px_2_size.h;
    px_2_full_rect.w=px_2_size.w;

    SOIL_RECT px_2_pos_rect;
    px_2_pos_rect.y=0;
    px_2_pos_rect.x=px_1_size.w;
    px_2_pos_rect.h=px_2_size.h;
    px_2_pos_rect.w=px_2_size.w;


    paste_pixmap_to_pixmap(pixmap_1,px_1_size,px_1_full_rect,res_pt,res_size,px_1_full_rect);
    paste_pixmap_to_pixmap(pixmap_2,px_2_size,px_2_full_rect,res_pt,res_size,px_2_pos_rect);

    result_size->h=res_size.h;
    result_size->w=res_size.w;

    return res_pt;

}
// ------------------------------------------------------- WRITE TO FILE
// - file must be opened
/*
void write_to_file(QString content, QFile * file)
{
    QDataStream stream(file);
    long content_len=content.length();
    long c=0;
    uchar uch;
    char ch;
    if(file->isOpen() && file->isWritable())
    {
        while(c<content_len)
        {
            uch = content.at(c).unicode();
            ch=uch;
            stream.writeRawData(&ch,1);
            c++;
        }
    }
}
*/
// ------------------------------------------------------ BYTE NUMBER TO TEXT
void int_to_hex_char(Uint32 num, char *c1, char *c2)
{
    if(num>256){num=256;}
    int i1=num/16;
    int i2=num%16;

    char val_arr[16];
    val_arr[0]='0';
    val_arr[1]='1';
    val_arr[2]='2';
    val_arr[3]='3';
    val_arr[4]='4';
    val_arr[5]='5';
    val_arr[6]='6';
    val_arr[7]='7';
    val_arr[8]='8';
    val_arr[9]='9';
    val_arr[10]='A';
    val_arr[11]='B';
    val_arr[12]='C';
    val_arr[13]='D';
    val_arr[14]='E';
    val_arr[15]='F';


    *c1=val_arr[i1];
    *c2=val_arr[i2];

}
