#ifndef SOILINCSTRING_H
#define SOILINCSTRING_H

#include "string_functions.h"
#include "soilinfrastring.h"
#include "soilobject.h"

class SoilIncString : public SoilObject
{
public:
    INC_STR_NODE *first_node;
    INC_STR_NODE *last_node;
    bool loaded;
    int length;//total char number, not alloc'ed but used
    int byte_n;
    int node_n;

    //mark_node_pt, mark_node_i and mark_in_i keep a position for quick external access
    INC_STR_NODE* mark_node_pt;
    int mark_node_i;
    int mark_in_i;
    bool mark_at_end;

    int ALLOC_N;

    SoilIncString();
    SoilIncString(int alloc_n);
    ~SoilIncString();
    void init();
    void release();

    void alloc_first_node();
    void alloc_next_node();
    void append(SOIL_CHAR sch);//
    void append(const SoilInfraString& str, int pos, int n);//
    INC_STR_NODE* node_pos_from_index(int ind, int* node_i_arg, int* in_i_arg);
    SOIL_CHAR* soil_char_addr(int ind);

    bool set_ch_at(int ind, SOIL_CHAR sch);//
    bool at(int ind, SOIL_CHAR* sch);

    void reset_mark();//
    bool set_mark(int ind);//
    bool next_mark(SOIL_CHAR* sch);//
    void print_data();
};

#endif // SOILINCSTRING_H
