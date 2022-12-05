#ifndef SOILOBJECT_H
#define SOILOBJECT_H

#include "global_definitions.h"


class SoilObject{
public:

    SoilObject();
    SoilObject* soilobject_receiver[3];
    virtual int soilobject_callback(int arg);
    virtual int soilobject_callback(int arg, int aux_arg);
    void set_soilobject_receiver(SoilObject* receiver);
    void set_soilobject_receiver(int receiver_i, SoilObject* receiver);

};

#endif //SOILOBJECT_H