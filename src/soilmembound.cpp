#include "soilmembound.h"

#include "iostream"

SoilMemBound::SoilMemBound()
{

    //a block of memory
    ALLOC_U = 0;
    //the minimum value for ALLOC_UPPER_MIN
    //it only allocs more memory if the number of unit
    //alloc is above this
    ALLOC_UPPER_MIN = 0;
    //the limit below which it deallocs some memory
    ALLOC_LOWER = 0;
    //the limit above which it allocs more memory
    ALLOC_UPPER = 0;
    //the current memory alloced
    ALLOC_LEN = 0;



    init();
}
// ----------------------------------------------- INIT
void SoilMemBound::init()
{

    //JUST CHANGE THIS TO INCREASE OR DECREASE
    //THE MEMORY USAGE BASED ON UNIT QUANTITY
    ALLOC_U = 50;
    ALLOC_UPPER_MIN = 4 * ALLOC_U;
    ALLOC_LOWER = 2 * ALLOC_U;
    ALLOC_UPPER = ALLOC_UPPER_MIN;
    ALLOC_LEN = 0;
}

// ----------------------------------------------- CALCULATE
//unit_n is the amount of memory units
//could be char types for a char dynamic array
//return true if ALLOC_LEN changes indicating the need to
//realloc memory
//
//any amount will set ALLOC_LEN to some amount bigger than 0, even 0
bool SoilMemBound::calculate(int unit_n)
{

    int prev_alloc_len = ALLOC_LEN;
    if(unit_n > ALLOC_UPPER)
    {
        ALLOC_UPPER = (unit_n - (unit_n % ALLOC_U)) + ALLOC_U;
        ALLOC_LOWER = ALLOC_UPPER - (2 * ALLOC_U);

    }else{
        if(unit_n < ALLOC_LOWER)
        {
            ALLOC_UPPER = (unit_n - (unit_n % ALLOC_U)) + 2 * ALLOC_U;
            //U = MEM - 
            if(ALLOC_UPPER < ALLOC_UPPER_MIN){ALLOC_UPPER = ALLOC_UPPER_MIN;}

            ALLOC_LOWER = ALLOC_UPPER - (2 * ALLOC_U);
        }

    }

    ALLOC_LEN = ALLOC_UPPER + ALLOC_U;

    if(ALLOC_LEN == prev_alloc_len){return false;}
    return true;
}
// ----------------------------------------------- SIMULATE
// simply shows if the values would change based on this unit number or not
// true if it would change values(need to reallocate)
bool SoilMemBound::simulate(int unit_n)
{

    int prev_alloc_len = ALLOC_LEN;
    int up = ALLOC_UPPER;
    int low;
    int new_len;
    if(unit_n > ALLOC_UPPER)
    {
        return true;

    }else{
        if(unit_n < ALLOC_LOWER)
        {
            up = (unit_n - (unit_n % ALLOC_U)) + 2 * ALLOC_U;
            if(up < ALLOC_UPPER_MIN){up = ALLOC_UPPER_MIN;}
        }

    }
    new_len = up + ALLOC_U;

    if(new_len == prev_alloc_len){return false;}
    return true;
}
// ----------------------------------------------- RESET
void SoilMemBound::reset()
{


    ALLOC_UPPER_MIN = 4 * ALLOC_U;
    ALLOC_LOWER = 2 * ALLOC_U;
    ALLOC_UPPER = ALLOC_UPPER_MIN;
    ALLOC_LEN = 0;

}
// ----------------------------------------------- RESET
//returns true if ALLOC_LEN changed
//which means you would need to reallocate whatever uses this class as memory control
//if ALLOC_LEN is 0 it returns false and keeps it 0
bool SoilMemBound::reset_unit(int u)
{
    if(u < 1){u = 1;}
    ALLOC_U = u;
    ALLOC_UPPER_MIN = 4 * ALLOC_U;
    ALLOC_LOWER = 2 * ALLOC_U;
    ALLOC_UPPER = ALLOC_UPPER_MIN;
    if(ALLOC_LEN > 0)
    {
        return calculate(ALLOC_LEN);
    }
    return false;
}
// ----------------------------------------------- PRINT LOG
void SoilMemBound::print_log()
{
        printf("\n\nSoilStringMemBounds::print_log()");
        printf( "\nALLOC_U:%d",ALLOC_U);
        printf( "\nALLOC_LOWER:%d",ALLOC_LOWER);
        printf( "\nALLOC_UPPER:%d",ALLOC_UPPER);
        printf( "\nALLOC_LEN:%d",ALLOC_LEN);        
        printf( "\n\nALLOC_UPPER_MIN:%d",ALLOC_UPPER_MIN);
        printf("\n\nSoilStringMemBounds::print_log()     END");
}