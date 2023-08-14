#include "soillist.h"

template<typename T>
SoilList<T>::SoilList()
{
    length = 0;
    first = NULL;
    last = NULL;
}

template<typename T>
void SoilList<T>::push_top(T item)
{
    
    if(first == NULL)
    {

    }
}



template class SoilList<int>;
template class SoilList<SoilString>;