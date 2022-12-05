#ifndef SOILLIST_H
#define SOILLIST_H

#include "soilstring.h"



class SoilList : SoilObject
{
public:

    int length;
    int alloc_len;
    SoilString *list[2];
    int list_i;
    int alt_list_i;
    SoilMemBound mem_bound;


    SoilList();
    ~SoilList();

    void alternate_list_i();
    void empty_list();

    void insert(const SoilString& str, int pos);
    void append(const SoilString& str);
    void remove(int pos);
    SoilString pop();
    SoilString at(int pos);
};

#endif // SOILLIST_H