#ifndef SOILIMAGE_H
#define SOILIMAGE_H

#include "soilwidget.h"

class SoilImage : public SoilWidget
{
public:

    SoilPixmap orig_img_pixmap;//oh yes, original resolution image saved here

    bool FOLLOW_IMAGE_SIZE;
    bool FIT_IMAGE_TO_SIZE;
    bool KEEP_IMAGE_RATIO;

    //the portion of content corresponding to the image
    //if image is smaller than view, pixmap will then have view_size's size
    SOIL_RECT image_rect;
    ALIGNMENT_TYPE alignment_x;
    ALIGNMENT_TYPE alignment_y;

    SoilImage();
    void set_alignment_x(ALIGNMENT_TYPE a);
    void set_alignment_y(ALIGNMENT_TYPE a);

    void set_follow_image_size(bool v);
    void set_fit_image_to_size(bool v);
    void set_keep_image_ratio(bool v);

    SIZE adjust_to_view(SIZE img_size);
    virtual SIZE set_size_virt(int w, int h);
    //either set image from file or reset it after changing fit_image_to_size
    bool internal_set_image(std::string filename, SoilPixmap* orig_px, int source);

    bool set_image(std::string filename);
    bool set_image(SoilPixmap* orig_px);
    void release_image();
    virtual void print();

    int cur_img;
    virtual void keydown_event_virt(SDL_Event E);
};

#endif // SOILIMAGE_H
