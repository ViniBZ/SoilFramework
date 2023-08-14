#include "soilobject.h"

int SoilObject::next_obj_id = 1;

SoilObject::SoilObject()
{
    //soilobject_receiver = NULL;

    obj_id = next_obj_id;
    next_obj_id++;

    int i = 0;
    while(i < SOILOBJECT_RECEIVER_N)
    {
        soilobject_receiver[i] = NULL;
        signal_receiver[i] = false;
        i++;
    }
}

// ----------------------------------------------- SOILOBJECT CALLBACK
int SoilObject::soilobject_callback(int arg)
{

    return 0;
}
// ----------------------------------------------- SOILOBJECT CALLBACK
int SoilObject::soilobject_callback(int arg, int aux)
{
    return 0;
}
// ----------------------------------------------- SET SOIL OBJECT RECEIVER
void SoilObject::set_soilobject_receiver(int receiver_i, SoilObject* receiver, bool signal_active)
{
    soilobject_receiver[receiver_i] = receiver;
    signal_receiver[receiver_i] = signal_active;
}
// ----------------------------------------------- PROCESS FLAG
void SoilObject::process_flag(int value)
{

}