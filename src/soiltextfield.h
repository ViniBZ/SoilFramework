#ifndef SOILTEXTFIELD_H
#define SOILTEXTFIELD_H

#include "soiltextwidget.h"

class SoilTextField : public SoilTextWidget
{
public:
    SoilTextField();
    SoilTextField(const SoilString& str);
    void init();
};

#endif // SOILTEXTFIELD_H
