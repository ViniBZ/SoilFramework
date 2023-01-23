#include "soilinfrastring.h"

SoilInfraString::SoilInfraString()
{
    str = NULL;
    loaded = false;
    alloc_len = 0;
}

// ----------------------------------------------- DESTRUCTOR
SoilInfraString::~SoilInfraString()
{
    
    release();
}
// ----------------------------------------------- ALLOC
//only returns false upon memory alloc problem
//return true regardless if it needed to reallocate or not
bool SoilInfraString::alloc(int len)
{

    if(!mem_bound.calculate(len))
    {
        return true;
    }
    
    delete_memory();
    str = new SOIL_CHAR[mem_bound.ALLOC_LEN];
    if(str == NULL){return false;}
    loaded = true;
    alloc_len = mem_bound.ALLOC_LEN;
    return true;
}
// ----------------------------------------------- WOULD ALLOC
bool SoilInfraString::would_realloc(int len)
{
    return mem_bound.simulate(len);
}

// ----------------------------------------------- DELETE MEMORY
void SoilInfraString::delete_memory()
{

    if(str != NULL)
    {
        delete[] str;
        str = NULL;
    }
    loaded = false;
    alloc_len = 0;

}
// ----------------------------------------------- RELEASE
void SoilInfraString::release()
{
    
    delete_memory();
    mem_bound.reset();

}

// ----------------------------------------------- PRINT LOG
void SoilInfraString::print_log(int st)
{

    printf("\n");
    printf("\nSoilInfraString::print_log(int st:%d)",st);
    if(loaded)
    {
        printf("\nLOADED");
    }else{
        printf("\nNOT LOADED");
    }
    printf("\nALLOC LEN :%d",alloc_len);
    printf("\nSoilInfraString::print_log(int st:%d)     END",st);
}
