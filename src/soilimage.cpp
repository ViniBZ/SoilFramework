#include "soilimage.h"

SoilImage::SoilImage(): SoilWidget(IMAGE)
{

    ABSOLUTE_VIEW_MIN_SIZE.w = 10;
    ABSOLUTE_VIEW_MIN_SIZE.h = 10;
    view_min_size.w = ABSOLUTE_VIEW_MIN_SIZE.w;
    view_min_size.h = ABSOLUTE_VIEW_MIN_SIZE.h;

    set_auto_size(false,false);

    set_alignment_x(CENTER);
    set_alignment_y(CENTER);

    content_pad = 0;
    set_size(0,0);

    pixmap.set_dynamic_bound(true, 50,50);

    pixmap.alloc(view_size.w, view_size.h);
    pixmap.spx_fill(0xFF000000);

    FOLLOW_IMAGE_SIZE = true;
    FIT_IMAGE_TO_SIZE = false;
    KEEP_IMAGE_RATIO = true;
    scrollable = true;


    cur_img = 2;
}
//--------------------------------------------------- SET ALIGNMENT ALONG THE X AXIS
void SoilImage::set_alignment_x(ALIGNMENT_TYPE a)
{
    if(a == LEFT || a == CENTER || a == RIGHT)
    {
        alignment_x = a;
    }
}
//--------------------------------------------------- SET ALIGNMENT ALONG THE Y AXIS
void SoilImage::set_alignment_y(ALIGNMENT_TYPE a)
{
    if(a == TOP || a == CENTER || a == BOTTOM)
    {
        alignment_x = a;
    }
}
//--------------------------------------------------- SET FOLLOW IMAGE SIZE
void SoilImage::set_follow_image_size(bool v)
{
    FOLLOW_IMAGE_SIZE = v;
    internal_set_image("", NULL, 0);
}
//--------------------------------------------------- SET FIT IMAGE TO SIZE
void SoilImage::set_fit_image_to_size(bool v)
{
    FIT_IMAGE_TO_SIZE = v;
    internal_set_image("", NULL, 0);
}
//--------------------------------------------------- SET KEEP IMAGE RATIO
void SoilImage::set_keep_image_ratio(bool v)
{
    KEEP_IMAGE_RATIO = v;
    internal_set_image("", NULL, 0);
}
//--------------------------------------------------- ADJUST TO VIEW
SIZE SoilImage::adjust_to_view(SIZE img_size)
{
    image_rect.x = 0;
    image_rect.y = 0;
    image_rect.w = img_size.w;
    image_rect.h = img_size.h;
    SIZE new_alloc_size = img_size;
    //if view is bigger than image, set image_rect according to the right alignment
    //and establish the variable for pixmap.alloc to be the view size and not the image size
    if(view_size.w > img_size.w)
    {
        new_alloc_size.w = view_size.w;
        if(alignment_x == CENTER)
        {
            image_rect.x = (view_size.w - img_size.w) / 2;
        }
        if(alignment_x == RIGHT)
        {
            image_rect.x = (view_size.w - img_size.w);
        }
    }
    if(view_size.h > img_size.h)
    {
        new_alloc_size.h = view_size.h;
        if(alignment_y == CENTER)
        {
            image_rect.y = (view_size.h - img_size.h) / 2;
        }
        if(alignment_y == BOTTOM)
        {
            image_rect.y = (view_size.h - img_size.h);
        }
    }
    return new_alloc_size;
}
//--------------------------------------------------- SET SIZE
SIZE SoilImage::set_size_virt(int w, int h)
{
    if(orig_img_pixmap.loaded)
    {
        if(FOLLOW_IMAGE_SIZE){return view_size;}
        internal_set_image("", NULL, 0);
    }else{
        pixmap.alloc(view_size.w, view_size.h);
        pixmap.spx_fill(0xFF000000);
    }
    
    return view_size;
}
//--------------------------------------------------- INTERNAL SET IMAGE
bool SoilImage::internal_set_image(std::string filename, SoilPixmap* orig_px, int source)
{
    //source:
    //0: just reloads image
    //1: from file
    //2: from pixmap

    if(source < 0){source = 0;}
    if(source > 2){source = 2;}
    //load image's full data into orig_img_pixmap
    if(source == 0 && !orig_img_pixmap.loaded){return false;}
    if(source == 1)
    {
        orig_img_pixmap.alloc_from_file(filename.c_str());
    }
    if(source == 2)
    {
        if(orig_px == NULL){return false;}
        //orig_img_pixmap.alloc(orig_px->size.w, orig_px->size.h);
        orig_img_pixmap.alloc_from_pixmap(*orig_px);
    }

    if(!orig_img_pixmap.loaded){return false;}

    
    SIZE img_size = orig_img_pixmap.size;
    if(FOLLOW_IMAGE_SIZE)
    {
        //MARKER:are gif images being loaded black and white? why?

        //it won't be called from set_size because set_size only calls internal_set_image if FOLLOW_IMAGE_SIZE is false
        SoilUIElm::internal_set_size(orig_img_pixmap.size.w, orig_img_pixmap.size.h);
        set_min_size(orig_img_pixmap.size.w, orig_img_pixmap.size.h);
        set_max_size(orig_img_pixmap.size.w, orig_img_pixmap.size.h);

    }
    if(FIT_IMAGE_TO_SIZE)
    {
        img_size = view_size;
        if(KEEP_IMAGE_RATIO)
        {
            img_size = correct_image_ratio_in_view(orig_img_pixmap.size, view_size);   
        }
    }
    SIZE new_alloc_size = adjust_to_view(img_size);

    set_content_size(img_size.w, img_size.h);
    if(new_alloc_size != pixmap.size)
    {
        pixmap.alloc(new_alloc_size.w, new_alloc_size.h);
    }
    pixmap.spx_fill(0xFF000000);
    pixmap.spx_resize_paste_pixmap(orig_img_pixmap, orig_img_pixmap.get_full_rect(), image_rect);
    
    return true;
}
//--------------------------------------------------- SET IMAGE
bool SoilImage::set_image(std::string filename)
{
    return internal_set_image(filename, NULL, 1);
}
//--------------------------------------------------- SET IMAGE
bool SoilImage::set_image(SoilPixmap* orig_px)
{
    return internal_set_image("", orig_px, 2);
}
//--------------------------------------------------- RELEASE IMAGE
void SoilImage::release_image()
{
    orig_img_pixmap.release();

    bool realloc_pixmap = false;
    SIZE new_px_size = pixmap.size;
    if(pixmap.size.w > view_size.w)
    {
        new_px_size.w = view_size.w;
        realloc_pixmap = true;
    }
    if(pixmap.size.w > view_size.w)
    {
        new_px_size.h = view_size.h;
        realloc_pixmap = true;
    }
    if(realloc_pixmap)
    {
        pixmap.alloc(new_px_size.w, new_px_size.h);
    }

    pixmap.spx_fill(0xFF000000);
    image_rect.x = 0;
    image_rect.y = 0;
    image_rect.w = 0;
    image_rect.h = 0;

    set_content_size(view_size.w, view_size.h);

    if(elm_ui_flag < 2){elm_ui_flag = 2;}
    if(engine_control_is_set)
    {
        engine_control->set_ui_flag(2);
    }
}

//--------------------------------------------- PRINT
void SoilImage::print()
{

    SoilWidget::print();
}

//--------------------------------------------- PROCESS KEYDOWN EVENT
void SoilImage::keydown_event_virt(SDL_Event E)
{
    SoilUIElm::keydown_event_virt(E);
    SDL_Keycode key = E.key.keysym.sym;

    if(key == SDLK_y)
    {
        release_image();
    }
    if(key == SDLK_e)
    {
        set_follow_image_size(!FOLLOW_IMAGE_SIZE);
    }
    if(key == SDLK_r)
    {
        set_fit_image_to_size(!FIT_IMAGE_TO_SIZE);
    }
    if(key == SDLK_t)
    {
        set_keep_image_ratio(!KEEP_IMAGE_RATIO);
    }
    if(key == SDLK_a)
    {
        SIZE cur_size = view_size;
        set_size(cur_size.w - 20, cur_size.h);
    }
    if(key == SDLK_s)
    {
        SIZE cur_size = view_size;
        set_size(cur_size.w + 20, cur_size.h);
    }
    if(key == SDLK_q)
    {
        SIZE cur_size = view_size;
        set_size(cur_size.w, cur_size.h - 20);
    }
    if(key == SDLK_w)
    {
        SIZE cur_size = view_size;
        set_size(cur_size.w, cur_size.h + 20);
    }
}