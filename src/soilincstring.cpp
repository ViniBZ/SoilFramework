#include "soilincstring.h"

SoilIncString::SoilIncString()
{
    ALLOC_N = 10;
    init();
}

SoilIncString::SoilIncString(int alloc_n)
{
    ALLOC_N = alloc_n;
    init();
}

SoilIncString::~SoilIncString()
{
    release();
}
// ----------------------------------------------- INIT
void SoilIncString::init()
{
    first_node = NULL;
    last_node = NULL;
    loaded = false;
    length = 0;//char number
    byte_n = 0;
    node_n = 0;

    reset_mark();
}
// ----------------------------------------------- RELEASE
void SoilIncString::release()
{
    if(loaded)
    {
        INC_STR_NODE* cur = first_node;
        INC_STR_NODE* next;
        int i = 0;
        while(i < node_n)
        {
            delete[] cur->data;
            next = cur->next;
            delete cur;
            cur = next;
            i++;
        }
        first_node = NULL;
        last_node = NULL;
        reset_mark();
        loaded = false;
        length = 0;
        byte_n = 0;
        node_n = 0;
    }
}
// ----------------------------------------------- ALLOC FIRST NODE
void SoilIncString::alloc_first_node()
{
    //MARKER:validate allocs
    first_node = new INC_STR_NODE;
    first_node->data = new SOIL_CHAR[ALLOC_N];
    first_node->len = 0;
    last_node = first_node;
    mark_node_pt = first_node;
    node_n = 1;
    loaded = true;
}
// ----------------------------------------------- ALLCO NEXT NODE
void SoilIncString::alloc_next_node()
{
    INC_STR_NODE* new_node;
    new_node = new INC_STR_NODE;
    new_node->data = new SOIL_CHAR[ALLOC_N];
    new_node->len = 0;
    last_node->next = new_node;
    last_node = new_node;
    node_n++;
}
// ----------------------------------------------- APPEND
void SoilIncString::append(SOIL_CHAR sch)
{
    //MARKER: make error verification in this class
    if(last_node == NULL)
    {
        alloc_first_node();
    }else{
        if(last_node->len == ALLOC_N)
        {
            alloc_next_node();
        }
    }
    last_node->data[last_node->len] = sch;
    last_node->len++;
    length++;
    byte_n += sch.N;
}
// ----------------------------------------------- APPEND
void SoilIncString::append(const SoilInfraString& str, int pos, int n)
{
    get_absolute_range_pos(str.alloc_len, &pos, &n);

    int i = pos;
    int e = pos + n;
    while(i < e)
    {
        if(last_node == NULL)
        {
            alloc_first_node();
        }else{
            if(last_node->len == ALLOC_N)
            {
                alloc_next_node();
            }
        }
        last_node->data[last_node->len] = str.str[i];
        last_node->len++;
        length++;
        byte_n += str.str[i].N;
        i++;
    }

}
// ----------------------------------------------- NODE POS FROM INDEX
INC_STR_NODE* SoilIncString::node_pos_from_index(int ind, int* node_i_arg, int* in_i_arg)
{
    if(node_n == 0){return NULL;}
    if(ind >= length){return NULL;}

    int i = 0;
    int node_i = ind / ALLOC_N;
    int in_i = ind % ALLOC_N;
    if(node_i >= node_n){return NULL;}
    INC_STR_NODE* cur = first_node;

    if(node_i == node_n-1)
    {
        cur = last_node;
    }else{
        while(i < node_i)
        {
            cur = cur->next;
            i++;
        }
    }
    if(in_i >= cur->len){return NULL;}
    
    *node_i_arg = node_i;
    *in_i_arg = in_i;

    return cur;
}
// ----------------------------------------------- SOIL CHAR ADDRESS
SOIL_CHAR* SoilIncString::soil_char_addr(int ind)
{
    int node_i;
    int in_i;
    INC_STR_NODE* node_pt = node_pos_from_index(ind, &node_i, &in_i);
    if(node_pt == NULL){return NULL;}

    return &node_pt->data[in_i];
}
// ----------------------------------------------- SET_CH
bool SoilIncString::set_ch_at(int ind, SOIL_CHAR sch)
{
    SOIL_CHAR* sch_pt = soil_char_addr(ind);
    if(sch_pt == NULL)
    {
        return false;
    }
    char dif = sch.N - sch_pt->N;
    *sch_pt = sch;
    byte_n += dif;

    return true;
}

// ----------------------------------------------- AT
bool SoilIncString::at(int ind, SOIL_CHAR* sch)
{
    SOIL_CHAR* sch_pt = soil_char_addr(ind);
    if(sch_pt == NULL)
    {
        return false;
    }
    *sch = *sch_pt;
    return true;
}
// ----------------------------------------------- RESET mark
void SoilIncString::reset_mark()
{
    mark_node_pt = first_node;
    mark_node_i = 0;
    mark_in_i = 0;
    mark_at_end = false;
}
// ----------------------------------------------- SET mark
bool SoilIncString::set_mark(int ind)
{
    int node_i;
    int in_i;
    INC_STR_NODE* node_pt = node_pos_from_index(ind, &node_i, &in_i);
    if(node_pt == NULL){return false;}

    mark_node_pt = node_pt;
    mark_node_i = node_i;
    mark_in_i = in_i;
    mark_at_end = false;
    return true;
}
// ----------------------------------------------- NEXT mark
//this is meant as a way to quickly go through the content
bool SoilIncString::next_mark(SOIL_CHAR* sch)
{
    if(node_n == 0){return false;}
    if(mark_at_end){return false;}

    *sch = mark_node_pt->data[mark_in_i];
    mark_in_i++;
    if(mark_in_i == mark_node_pt->len)
    {
        if(mark_node_i == (node_n - 1))
        {
            mark_at_end = true;
        }else{
            mark_node_pt = mark_node_pt->next;
            mark_node_i++;
            mark_in_i = 0;
        }
    }
    return true;
}

// ----------------------------------------------- PRINT DATA
void SoilIncString::print_data()
{
    printf("\nSoilIncString ------ print data");
    int i = 0;
    printf("\nlength:%d",length);
    printf("\nnode_n:%d",node_n);
    printf("\nbyte_n:%d", byte_n);
    printf("\n");
    INC_STR_NODE* cur = first_node;
    while(i < node_n)
    {
        printf("\nNODE %d, len:%d\n",i,cur->len);
        int c = 0;
        while(c < cur->len)
        {
            printf("\nN:%d",cur->data[c].N);
            printf(". B[0]:%d",cur->data[c].B[0]);
            printf(". ch val:%d",cur->data[c].B[0]);
            c++;
        }
        cur = cur->next;
        i++;
    }
    printf("\nSoilIncString ------  END print data");
    printf("\n.");
}