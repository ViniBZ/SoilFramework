#ifndef SOILLISTWIDGET_H
#define SOILLISTWIDGET_H


#include "soilwidget.h"
#include "soillabel.h"
#include "soillayout.h"

//MARKER: would SoilListWidget a misleading name, since it inherits from SoilLayout and not SoilWidget?

typedef struct STR_LIST_ITEM{
    SoilLabel label;
    STR_LIST_ITEM* next;
}STR_LIST_ITEM;

class SoilListWidget : public SoilLayout
{
public:
    int length;
    STR_LIST_ITEM* list;
    int value;

    SoilListWidget();
    ~SoilListWidget();
    void internal_set_value(int v);
    void set_value(int v);
    int get_value();
    int get_length();

    void insert(const SoilString& str, int pos);
    void push(const SoilString& str);
    void remove(int pos);
    SoilString pop();
    SoilString at(int pos);
    virtual int soilobject_callback(int arg, int aux_arg);
    void printf_list();
};



#endif // SOILLISTWIDGET_H