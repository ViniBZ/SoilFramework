#ifndef SOILOBJECT_H
#define SOILOBJECT_H

#include "global_definitions.h"


class SoilObject{
public:

    static int next_obj_id;
    int obj_id;
    SoilObject* soilobject_receiver[3];

    SoilObject();
    virtual int soilobject_callback(int arg);
    virtual int soilobject_callback(int arg, int aux_arg);
    void set_soilobject_receiver(SoilObject* receiver);
    void set_soilobject_receiver(int receiver_i, SoilObject* receiver);
    virtual void process_flag(int value);

};

#endif //SOILOBJECT_H