#include "soilstringlist.h"

SoilStringList::SoilStringList()
{
    length = 0;
    list[0] = NULL;
    list[1] = NULL;
    mem_bound.reset_unit(20);
    alloc_len = mem_bound.ALLOC_LEN;

    list_i = 0;
    alt_list_i = 1;
}
SoilStringList::~SoilStringList()
{

}
//--------------------------------------------- ALTERNATE LIST I
void SoilStringList::alternate_list_i()
{
    if(list_i == 0)
    {
        list_i = 1;
        alt_list_i = 0;
    }else{
        list_i = 0;
        alt_list_i = 1;
    }
}
//--------------------------------------------- EMPTY LIST
void SoilStringList::empty_list()
{
        if(list[list_i] != NULL)
        {
            delete[] list[list_i];
        }
        mem_bound.reset();
        alloc_len = 0;
        length = 0;
}
//--------------------------------------------- INSERT
void SoilStringList::insert(const SoilString& str, int pos)
{
    if(mem_bound.calculate(length + 1))
    {
        list[alt_list_i] = NULL;
        list[alt_list_i] = new SoilString[mem_bound.ALLOC_LEN];
        if(list[alt_list_i] != NULL)
        {
            int i = 0;
            while(i < length)
            {
                list[alt_list_i][i] = list[list_i][i];
                i++;
            }
            list[alt_list_i][i] = str;

            if(list[list_i] != NULL)
            {
                delete[] list[list_i];
            }
            alloc_len = mem_bound.ALLOC_LEN;
            length += 1;
            alternate_list_i();

        }else{
            empty_list();
        }
    }else{
        list[list_i][length] = str;
        length += 1;
    }
}
//--------------------------------------------- APPEND
void SoilStringList::append(const SoilString& str)
{
    insert(str, length);
}
//--------------------------------------------- REMOVE
void SoilStringList::remove(int pos)
{

}
//--------------------------------------------- POP
SoilString SoilStringList::pop()
{
    SoilString str;
    return str;
}
//--------------------------------------------- AT
SoilString SoilStringList::at(int pos)
{
    validate_rel_index(length, &pos);
    SoilString str;
    if(pos < length)
    {
        SoilString str = list[list_i][pos];
    }
    return str;
}