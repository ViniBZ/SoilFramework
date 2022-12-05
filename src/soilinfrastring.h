#ifndef SOILINFRASTRING_H
#define SOILINFRASTRING_H

#include "global_definitions.h"
#include "soilmembound.h"

class SoilInfraString
{
//private:
public:
    friend class SoilString;

    SOIL_CHAR* str;

    bool loaded;
    int alloc_len;// amount of alloc'ed memory
    SoilMemBound mem_bound;

    SoilInfraString();
    ~SoilInfraString();
    bool alloc(int len);
    bool would_realloc(int len);
    void delete_memory();//for internal use, doesn't reset mem_bound
    void release();//for external use, it does reset mem_bound

    void print_log(int st);
};

#endif // SOILINFRASTRING_H
