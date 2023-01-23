#ifndef SOILCONTROL_H
#define SOILCONTROL_H

#include "soilobject.h"


class SoilControl : public SoilObject
{
public:
    
    SoilControl();
    virtual void execution_started_response();
    virtual void soil_signal_response(SOIL_SIGNAL_STRUCT signal);
    virtual void sdl_event_response(SDL_Event sdl_event, int response);
};

#endif // SOILCONTROL_H
