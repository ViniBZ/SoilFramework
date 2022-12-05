#ifndef SOILTEXTBLOCK_H
#define SOILTEXTBLOCK_H

#include "soiltextwidget.h"

class SoilTextBlock : public SoilTextWidget
{
public:
    SoilTextBlock();
    SoilTextBlock(const SoilString& str);
    void init();
    void set_font_size(int fs);
};

#endif // SOILTEXTBLOCK_H
