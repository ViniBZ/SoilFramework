#include "soilobject.h"

SoilObject::SoilObject()
{
    //soilobject_receiver = NULL;

    soilobject_receiver[0] = NULL;
    soilobject_receiver[1] = NULL;
    soilobject_receiver[2] = NULL;
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
void SoilObject::set_soilobject_receiver(SoilObject* receiver)
{
    //soilobject_receiver = receiver;
}
// ----------------------------------------------- SET SOIL OBJECT RECEIVER
void SoilObject::set_soilobject_receiver(int receiver_i, SoilObject* receiver)
{
    soilobject_receiver[receiver_i] = receiver;
}