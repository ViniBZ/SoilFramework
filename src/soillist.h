#ifndef SOILLIST_H
#define SOILLIST_H

#include "soilobject.h"
#include "soilstring.h"

/*
template <typename T>
struct LIST_NODE{
    T item;
    LIST_NODE* next;

};
*/

template<typename T>
class SoilList : public SoilObject
{
public:
    int length;
    LIST_NODE<T>* first;
    LIST_NODE<T>* last;
    SoilList();

    void push_top(T item);
    void push_bottom(T item);
    

};

#endif //SOILLIST_H