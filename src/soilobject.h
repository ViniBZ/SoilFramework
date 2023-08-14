#ifndef SOILOBJECT_H
#define SOILOBJECT_H

#include "global_definitions.h"

#define SOILOBJECT_RECEIVER_N 3


class SoilObject{
public:

    static int next_obj_id;
    int obj_id;
    bool signal_receiver[SOILOBJECT_RECEIVER_N];
    SoilObject* soilobject_receiver[SOILOBJECT_RECEIVER_N];

    SoilObject();
    virtual int soilobject_callback(int arg);
    virtual int soilobject_callback(int arg, int aux_arg);
    void set_soilobject_receiver(int receiver_i, SoilObject* receiver, bool signal_active=false);
    virtual void process_flag(int value);

};

#endif //SOILOBJECT_H