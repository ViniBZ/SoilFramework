#ifndef SOILMEMBOUNDS_H
#define SOILMEMBOUNDS_H

#include <iostream>

class SoilMemBound
{
public:
    int ALLOC_U;
    int ALLOC_UPPER_MIN;
    int ALLOC_LOWER;
    int ALLOC_UPPER;
    int ALLOC_LEN;

    SoilMemBound();
    void init();
    bool calculate(int unit_n);
    bool simulate(int unit_n);
    void reset();
    bool reset_unit(int u);//it resets and set the ALLOC_U
    void print_log();
};

#endif // SOILMEMBOUNDS_H
