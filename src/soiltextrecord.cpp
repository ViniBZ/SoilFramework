#include "soiltextrecord.h"

int SoilTextRecord::rec_next_id = 1;
int SoilTextRecord::node_order_id = 1;
SoilTextRecord::SoilTextRecord()
{
    init();
}

SoilTextRecord::~SoilTextRecord()
{
    int i = 0;
    while(i < 2)
    {
        empty_stack(i);
        i++;
    }
}

//---------------------------------------------- INIT
void SoilTextRecord::init()
{
    target_str = NULL;
    rec_id = rec_next_id;
    rec_next_id++;

    block_size_limit = DEFAULT_BLOCK_SIZE_LIMIT;
    max_file_usage = DEFAULT_MAX_FILE_USAGE;
    save_excess = false;
    int i = 0;
    int b = 0;
    while(i < 2)
    {
        b = 0;
        while(b < 4)
        {
            BI[i][b] = b;
            block_size[i][b] = 0;
            block_node_n[i][b] = 0;
            rec_str[i][b] = "";
            first[i][b] = NULL;
            last[i][b] = NULL;
            b++;
        }
        CB[i] = 0;

        excess_fn[i] = NULL;
        excess_alt_fn[i] = NULL;
        node_n[i] = 0;
        in_mem_node_n[i] = 0;
        in_file_node_n[i] = 0;
        in_file_block_n[i] = 0;
        file_size[i] = 0;

        i++;
    }
    typing_update_rec = true;
}

//---------------------------------------------- MAKE REMOVAL REC NODE
//when exec'ed WILL REMOVE text
STR_REC_NODE SoilTextRecord::make_removal_rec_node(int str_pos, int rem_len)
{
    STR_REC_NODE node;
    node.type = 0;
    node.rec_pos = 0;
    node.str_pos = str_pos;
    node.ins_len = 0;
    node.rem_len = rem_len;
    node.selected_before = false;
    node.cursor_1_before = (-1);
    node.cursor_1_after = (-1);
    node.selected_after = false;
    node.cursor_2_before = (-1);
    node.cursor_2_after = (-1);
    node.prev = NULL;
    node.next = NULL;
    return node;
}
//---------------------------------------------- MAKE INSERTION REC NODE
//when exec'ed WILL INSERT text
//MARKER: change the str parameter, it doesn't need the str here, just the length
//same for make_transformation_rec_node
STR_REC_NODE SoilTextRecord::make_insertion_rec_node(const SoilString& str, int str_pos, int rec_pos)
{
    STR_REC_NODE node;
    node.type = 10;
    node.rec_pos = rec_pos;//can be from undo_rec or redo_rec
    node.str_pos = str_pos;
    node.ins_len = str.get_length();
    node.rem_len = 0;
    node.selected_before = false;
    node.cursor_1_before = (-1);
    node.cursor_1_after = (-1);
    node.selected_after = false;
    node.cursor_2_before = (-1);
    node.cursor_2_after = (-1);
    node.prev = NULL;
    node.next = NULL;
    return node;
}
//---------------------------------------------- MAKE TRANSFORMATION NODE
STR_REC_NODE SoilTextRecord::make_transformation_rec_node(const SoilString& str, int str_pos, int rem_len, int rec_pos)
{
    STR_REC_NODE node;
    node.type = 20;
    node.rec_pos = rec_pos;//can be from undo_rec or redo_rec
    node.str_pos = str_pos;
    node.ins_len = str.get_length();
    node.rem_len = rem_len;
    node.selected_before = false;
    node.cursor_1_before = (-1);
    node.cursor_1_after = (-1);
    node.selected_after = false;
    node.cursor_2_before = (-1);
    node.cursor_2_after = (-1);
    node.prev = NULL;
    node.next = NULL;
    return node;
}
//---------------------------------------------- MAKE CHAR SET NODE
STR_REC_NODE SoilTextRecord::make_ch_set_rec_node(SOIL_CHAR sch, int str_pos)
{
    //no need to rec_pos because it's just one char the linked list for the nodes, undo_rec_first/redo_rec_first
    //should correspond to the data in the undo_rec/redo_rec, so this will just pop one char from the rec string
    STR_REC_NODE node;
    node.type = 30;
    node.rec_pos = 0;//can be from undo_rec or redo_rec
    node.str_pos = str_pos;
    node.ins_len = 0;
    node.rem_len = 0;
    node.selected_before = false;
    node.cursor_1_before = (-1);
    node.cursor_1_after = (-1);
    node.selected_after = false;
    node.cursor_2_before = (-1);
    node.cursor_2_after = (-1);
    node.prev = NULL;
    node.next = NULL;
    return node;
}
//---------------------------------------------- MAKE CHANGE CASE NODE
STR_REC_NODE SoilTextRecord::make_case_change_rec_node(int str_pos, int n, int action)
{
    //action:
    //40: invert case
    //41: to upper case
    //42: to lower case
    STR_REC_NODE node;
    node.type = action;
    node.rec_pos = 0;
    node.str_pos = str_pos;
    node.ins_len = n;//doesn't mean chars will be inserted, it's just the range where the chars will be changed
    node.rem_len = 0;
    node.selected_before = false;
    node.cursor_1_before = (-1);
    node.cursor_1_after = (-1);
    node.selected_after = false;
    node.cursor_2_before = (-1);
    node.cursor_2_after = (-1);
    node.prev = NULL;
    node.next = NULL;
    return node;
}
//---------------------------------------------- MAKE MULTI TRANFORMATION NODE
STR_REC_NODE SoilTextRecord::make_multi_trans_rec_node(int action, int ins_len, int rem_len, int rec_pos)
{
    //action:
    //70: match_case == true AND match_accent == true
    //71: match_case == false OR match_accent == false
    STR_REC_NODE node;
    node.type = action;
    node.rec_pos = rec_pos;
    //these will be coded for each occurrence in undo_rec / redo_rec
    node.str_pos = 0;
    node.ins_len = ins_len;
    node.rem_len = rem_len;
    node.selected_before = false;
    node.cursor_1_before = (-1);
    node.cursor_1_after = (-1);
    node.selected_after = false;
    node.cursor_2_before = (-1);
    node.cursor_2_after = (-1);
    node.prev = NULL;
    node.next = NULL;
    return node;
}
//MARKER:maybe change arg order
//---------------------------------------------- COPY AND INVERT TEXT FIELD VARS
void SoilTextRecord::copy_invert_text_field_vars(STR_REC_NODE * node, STR_REC_NODE origin)
{

    node->selected_after = origin.selected_before;
    node->cursor_1_after = origin.cursor_1_before;
    node->cursor_2_after = origin.cursor_2_before;

    node->selected_before = origin.selected_after;
    node->cursor_1_before = origin.cursor_1_after;
    node->cursor_2_before = origin.cursor_2_after;
}
//---------------------------------------------- STACK METHODS
//---------------------------------------------- INC CB
void SoilTextRecord::shift_bi_left(int stack_i)
{
    int tmp[4] = {BI[stack_i][0], BI[stack_i][1], BI[stack_i][2], BI[stack_i][3]};
    BI[stack_i][0] = tmp[1];
    BI[stack_i][1] = tmp[2];
    BI[stack_i][2] = tmp[3];
    BI[stack_i][3] = tmp[0];
}
//---------------------------------------------- INC CB
void SoilTextRecord::shift_bi_right(int stack_i)
{
    int tmp[4] = {BI[stack_i][0], BI[stack_i][1], BI[stack_i][2], BI[stack_i][3]};
    BI[stack_i][0] = tmp[3];
    BI[stack_i][1] = tmp[0];
    BI[stack_i][2] = tmp[1];
    BI[stack_i][3] = tmp[2];
}
//---------------------------------------------- RESET BI
void SoilTextRecord::reset_bi(int stack_i)
{
    int i = 0;
    while(i < 4)
    {
        BI[stack_i][i] = i;
        i++;
    }
}
//---------------------------------------------- INC (INCREASE) CB
void SoilTextRecord::inc_cb(int stack_i)
{

    if(CB[stack_i] < 3)
    {
        CB[stack_i] += 1;
    }
}
//---------------------------------------------- DEC (DECREASE) CB
void SoilTextRecord::dec_cb(int stack_i)
{

    if(CB[stack_i] > 0)
    {
        CB[stack_i] -= 1;
    }
}
//---------------------------------------------- GET LAST NODE
STR_REC_NODE* SoilTextRecord::get_last_node(int stack_i, int* last_node_abs_bi)
{
    STR_REC_NODE* last_node;
    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];
    
    if(block_node_n[stack_i][abs_bi] > 0)
    {
        *last_node_abs_bi = abs_bi;
        return last[stack_i][abs_bi];
    }
    if(vir_bi > 0)
    {
        int prev_abs_bi = BI[stack_i][vir_bi - 1];
        *last_node_abs_bi = prev_abs_bi;
        return last[stack_i][prev_abs_bi];
    }
    return NULL;

}
//---------------------------------------------- REMOVE BLOCK
void SoilTextRecord::empty_block(int stack_i, int abs_bi, bool saved_to_file)
{
    rec_str[stack_i][abs_bi].empty_itself();

    STR_REC_NODE* cur_node = first[stack_i][abs_bi];
    STR_REC_NODE* next_node;
    while(cur_node != NULL)
    {
        next_node = cur_node->next;
        delete cur_node;
        cur_node = next_node;
    }
    first[stack_i][abs_bi] = NULL;
    last[stack_i][abs_bi] = NULL;
    if(!saved_to_file)
    {
        node_n[stack_i] -= block_node_n[stack_i][abs_bi];
    }
    in_mem_node_n[stack_i] -= block_node_n[stack_i][abs_bi];
    block_node_n[stack_i][abs_bi] = 0;
    block_size[stack_i][abs_bi] = 0;
}
//---------------------------------------------- REC STR INSERT
void SoilTextRecord::rec_str_insert(int stack_i, int abs_bi, const SoilString& str, int pos)
{
    int size = str.get_byte_n();
    rec_str[stack_i][abs_bi].insert(str, pos);
    block_size[stack_i][abs_bi] += size;    
}
//---------------------------------------------- REC STR INSERT
void SoilTextRecord::rec_str_insert(int stack_i, int abs_bi, SoilIncString* str, int pos)
{
    int size = str->byte_n;
    rec_str[stack_i][abs_bi].insert(str, pos);
    block_size[stack_i][abs_bi] += size;    
}
//---------------------------------------------- REC STR INSERT
void SoilTextRecord::rec_str_insert(int stack_i, int abs_bi, SOIL_CHAR ch, int pos)
{
    rec_str[stack_i][abs_bi].insert_ch_in_range(ch, pos);
    block_size[stack_i][abs_bi] += ch.N;
}
//---------------------------------------------- REC STR REMOVE
void SoilTextRecord::rec_str_remove(int stack_i, int abs_bi, int pos, int n)
{
    int old_byte_n = rec_str[stack_i][abs_bi].get_byte_n();
    rec_str[stack_i][abs_bi].remove(pos, n);
    //MARKER: get how much was actually removed from rec_str from SoilString::remove() return value?
    //in bytes
    //if(n < 0){n *= (-1);}
    int cur_byte_n = rec_str[stack_i][abs_bi].get_byte_n();
    block_size[stack_i][abs_bi] -= (old_byte_n - cur_byte_n);
    //printf("\npos : %d. n: %d", pos, n);
    //printf("\nblock_size(4) stack_i:%d. abs_bi:%d.  : %d",stack_i, abs_bi, block_size[stack_i][abs_bi]);
}
//---------------------------------------------- PUSH NODE TO STACK
void SoilTextRecord::push_node(int stack_i, STR_REC_NODE node)
{
    STR_REC_NODE * new_node = new STR_REC_NODE;
    *new_node = node;
    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];

    //MARKER: I guess that's not the actual block size inc
    // by not counting the actual STR_REC_NODE size
    block_size[stack_i][abs_bi] += STR_REC_NODE_DATA_LEN;
    //printf("\nblock_size(5) stack_i:%d. abs_bi:%d. : %d",stack_i, abs_bi,block_size[stack_i][abs_bi]);

    //PUSH UNDO NODE
    new_node->prev = last[stack_i][abs_bi];
    new_node->next = NULL;
    if(block_node_n[stack_i][abs_bi] == 0)
    {
        first[stack_i][abs_bi] = new_node;
    }else{        
        last[stack_i][abs_bi]->next = new_node;
    }
    last[stack_i][abs_bi] = new_node;
    block_node_n[stack_i][abs_bi] += 1;

    node_n[stack_i] += 1;
    in_mem_node_n[stack_i] += 1;

    increment_limit_check(stack_i);

}
//---------------------------------------------- POP NODE FROM STACK
STR_REC_NODE SoilTextRecord::pop_node(int stack_i)
{
    bool retrieved_excess = false;

    STR_REC_NODE node;
    if(node_n[stack_i] == 0)
    {
        node.type = (-1);
    
        return node;
    }

    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];

    //
    node_n[stack_i] -= 1;
    in_mem_node_n[stack_i] -= 1;
    block_node_n[stack_i][abs_bi] -= 1;
    block_size[stack_i][abs_bi] -= STR_REC_NODE_DATA_LEN;//
    //printf("\nblock_size(7) stack_i:%d. abs_bi:%d. : %d",stack_i, abs_bi,block_size[stack_i][abs_bi]);

    node = *last[stack_i][abs_bi];
    
    delete last[stack_i][abs_bi];
    last[stack_i][abs_bi] = node.prev;
    if(last[stack_i][abs_bi] != NULL)
    {
        last[stack_i][abs_bi]->next = NULL;
    }
    if(block_node_n[stack_i][abs_bi] == 0)
    {
        first[stack_i][abs_bi] = NULL;
    }
    

    //MARKER: check type to remove rec str section
    //printf("\n");
    node.prev = NULL;
    node.next = NULL;

    return node;
}
//---------------------------------------------- INCREMENT LIMIT CHECK
void SoilTextRecord::increment_limit_check(int stack_i)
{
    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];
    if(block_size[stack_i][abs_bi] > block_size_limit)
    {
        if(vir_bi == 3)
        {
            bool save_excess_res = false;

            if(save_excess)
            {
                //save excess

                save_excess_res = save_block_to_excess_file(stack_i, BI[stack_i][0]);
            }
            empty_block(stack_i, BI[stack_i][0], save_excess_res);
            shift_bi_left(stack_i);

        }else{
            inc_cb(stack_i);
        }
    }
}
//---------------------------------------------- DECREMENT LIMIT CHECK
void SoilTextRecord::decrement_limit_check(int stack_i)
{

    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];

    //printf("\nblock_size(9) stack_i:%d. abs_bi:%d. : %d",stack_i, abs_bi,block_size[stack_i][abs_bi]);
    if(block_size[stack_i][abs_bi] == 0)
    {
        if(vir_bi <= 2 && save_excess && in_file_block_n[stack_i] > 0)
        {
            //open excess file
            shift_bi_right(stack_i);
            pop_block_from_excess_file(stack_i, BI[stack_i][0]);
        }else{
            dec_cb(stack_i);
        }
    }
}
//---------------------------------------------- SAVE NODE TO FILE
bool SoilTextRecord::write_node_to_file(STR_REC_NODE node, FILE* f)
{

    SoilString node_str;
    //printf("\n\nWRITTEN TO FILE:");
    node_to_soil_str(node, &node_str);
    //node_str.printf_str(1,2);

    if(f == NULL){
        return false;
    }
    unsigned int val = 0;
    //printf("\nwriting . . . . . . ..  ");
    val = node.order_id;
    fputc(val, f);

    val = node.type;
    fputc(val, f);

    val = (node.rec_pos & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.rec_pos & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.rec_pos & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.rec_pos & 0x000000FF;
    fputc(val, f);

    val = (node.str_pos & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.str_pos & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.str_pos & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.str_pos & 0x000000FF;
    fputc(val, f);

    val = (node.ins_len & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.ins_len & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.ins_len & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.ins_len & 0x000000FF;
    fputc(val, f);

    val = (node.rem_len & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.rem_len & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.rem_len & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.rem_len & 0x000000FF;
    fputc(val, f);

    if(node.selected_before)
    {
        val = 1;
        fputc(val, f);
    }else{
        val = 0;
        fputc(val, f);
    }

    val = (node.cursor_1_before & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.cursor_1_before & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.cursor_1_before & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.cursor_1_before & 0x000000FF;
    fputc(val, f);

    val = (node.cursor_2_before & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.cursor_2_before & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.cursor_2_before & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.cursor_2_before & 0x000000FF;
    fputc(val, f);

    if(node.selected_before)
    {
        val = 1;
        fputc(val, f);
    }else{
        val = 0;
        fputc(val, f);
    }

    val = (node.cursor_1_after & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.cursor_1_after & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.cursor_1_after & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.cursor_1_after & 0x000000FF;
    fputc(val, f);

    val = (node.cursor_2_after & 0xFF000000)>>24;
    fputc(val, f);
    val = (node.cursor_2_after & 0x00FF0000)>>16;
    fputc(val, f);
    val = (node.cursor_2_after & 0x0000FF00)>>8;
    fputc(val, f);
    val = node.cursor_2_after & 0x000000FF;
    fputc(val, f);

    return true;
}

//---------------------------------------------- SAVE BLOCK TO FILE
bool SoilTextRecord::save_block_to_file(int stack_i, int abs_bi, FILE* f)
{
    if(f == NULL){
        return false;
    }
    
    int size_sum = 0;

    long int l = ftell(f);

    write_int_to_file(f, block_size[stack_i][abs_bi] + 16);
    //block node n
    write_int_to_file(f, block_node_n[stack_i][abs_bi]);
    //rec str position in this
    write_int_to_file(f, 16 + (STR_REC_NODE_DATA_LEN * block_node_n[stack_i][abs_bi]));
    //rec str length
    int str_len = rec_str[stack_i][abs_bi].get_byte_n();
    write_int_to_file(f, str_len);

    file_size[stack_i] += 16;
    size_sum += 16;

    SoilString tmp_str;

    STR_REC_NODE* cur_node = first[stack_i][abs_bi];
    while(cur_node != NULL)
    {
        write_node_to_file(*cur_node, f);
        cur_node = cur_node->next;
        in_file_node_n[stack_i] += 1;
        file_size[stack_i] += STR_REC_NODE_DATA_LEN;
        size_sum += STR_REC_NODE_DATA_LEN;

    }
    rec_str[stack_i][abs_bi].save_to_file(f);
    in_file_block_n[stack_i] += 1;
    file_size[stack_i] += rec_str[stack_i][abs_bi].get_byte_n();
    size_sum += rec_str[stack_i][abs_bi].get_byte_n();

return true;
}
//---------------------------------------------- SAVE BLOCK TO EXCESS FILE
bool SoilTextRecord::save_block_to_excess_file(int stack_i, int abs_bi)
{

    if(!save_excess)
    {
        return false;
    }
    if(excess_fn[stack_i] == NULL)
    {
        return false;
    }
    //print_data();
    FILE *f = NULL;
    FILE *f_old = NULL;

    bool get_from_old = false;
    bool valid_f = true;
    int to_lose_size = 0;
    //printf("\nin file node n: %d",in_file_node_n[stack_i]);
    if(in_file_node_n[stack_i] == 0)
    {
        //case 0: just write to file
        f = fopen(excess_fn[stack_i], "w");
        //printf("\nCASE 0 : WRITING TO FILE...");
    }else{

        if(file_size[stack_i] > max_file_usage)
        {
            to_lose_size = file_size[stack_i] - max_file_usage;
            //case 2: copy part of the old file to new one, than the node
            f_old = fopen(excess_fn[stack_i], "r");
            f = fopen(excess_alt_fn[stack_i], "w");
            get_from_old = true;
            if(f_old == NULL){valid_f = false;}
            //printf("\nCASE 2 : WRITING TO FILE PLUS SOME OLD CONTENT...");
        }else{
            //case 1: just append to file
            //printf("\nCASE 1 : APPENDING TO FILE...");
            f = fopen(excess_fn[stack_i], "a");
        }
    }
    if(f == NULL){valid_f = false;}
    if(!valid_f)
    {
        if(f_old != NULL){fclose(f_old);}
        if(f != NULL){fclose(f);}
        
        return false;
    }

    //
    if(get_from_old)
    {
        int lost_block_size;
        int lost_block_node_n;
        int lost_block_str_pos;
        int lost_block_str_len;

        int next_block_header = 0;

        int total_lost_size = 0;
        int bi = 0;
        while(total_lost_size < to_lose_size && in_file_block_n[stack_i] > 0)
        {
            get_int_from_file(f_old, &lost_block_size);
            get_int_from_file(f_old, &lost_block_node_n);
            get_int_from_file(f_old, &lost_block_str_pos);
            get_int_from_file(f_old, &lost_block_str_len);            

            node_n[stack_i] -= lost_block_node_n;
            in_file_node_n[stack_i] -= lost_block_node_n;
            in_file_block_n[stack_i] -= 1;
            file_size[stack_i] -= lost_block_size;

            total_lost_size += lost_block_size;

            next_block_header += lost_block_size;
            fseek(f_old, next_block_header, SEEK_SET);

            bi++;
        }
        int ch;
        while((ch = fgetc(f_old)) != EOF)
        {
            fputc(ch, f);
        }
        fclose(f_old);
    }

    int nodes_saved = block_node_n[stack_i][abs_bi];
    bool save_block_res = save_block_to_file(stack_i, abs_bi, f);

    fclose(f);

    if(save_block_res)
    {
        /*
        block_size[stack_i][abs_bi] = 0;
        block_node_n[stack_i][abs_bi] = 0;
        in_mem_node_n[stack_i][abs_bi] -= nodes_saved;
        */
    }else{
        return false;
    }
    
    if(get_from_old)
    {
        remove(excess_fn[stack_i]);
        rename(excess_alt_fn[stack_i], excess_fn[stack_i]);
    }

    //save
    //block_size (4B)
    //rec_str_pos (4B)
    //str_len
    //nodes
    //rec_str

return true;
}
//---------------------------------------------- READ NODE FROM FILE
bool SoilTextRecord::read_node_from_data(STR_REC_NODE* node, unsigned char data[STR_REC_NODE_DATA_LEN])
{

    if(node == NULL)
    {
        return false;
    }

    //MARKER: set id_offset to 0 or remove it entirely once STR_REC_NODE no longer
    //has order_id
    int id_offset = 1;
    node->order_id = data[0];

    node->type = data[0+id_offset];
    //printf("\nSoilTextRecord::read_node_from_data()");
    //printf("\ndata[0] : %d",data[0]);
    //printf("\ntype from data : %d",node->type);

    node->rec_pos = data[1+id_offset]<<24;
    node->rec_pos += data[2+id_offset]<<16;
    node->rec_pos += data[3+id_offset]<<8;
    node->rec_pos += data[4+id_offset];

    //printf("\nrec pos from data : %d",node->rec_pos);


    node->str_pos = data[5+id_offset]<<24;
    node->str_pos += data[6+id_offset]<<16;
    node->str_pos += data[7+id_offset]<<8;
    node->str_pos += data[8+id_offset];

    node->ins_len = data[9+id_offset]<<24;
    node->ins_len += data[10+id_offset]<<16;
    node->ins_len += data[11+id_offset]<<8;
    node->ins_len += data[12+id_offset];

    node->rem_len = data[13+id_offset]<<24;
    node->rem_len += data[14+id_offset]<<16;
    node->rem_len += data[15+id_offset]<<8;
    node->rem_len += data[16+id_offset];

    if(data[17+id_offset] == 1)
    {
        node->selected_before = true;
    }else{
        node->selected_before = false;
    }

    node->cursor_1_before = data[18+id_offset]<<24;
    node->cursor_1_before += data[19+id_offset]<<16;
    node->cursor_1_before += data[20+id_offset]<<8;
    node->cursor_1_before += data[21+id_offset];
    

    node->cursor_2_before = data[22+id_offset]<<24;
    node->cursor_2_before += data[23+id_offset]<<16;
    node->cursor_2_before += data[24+id_offset]<<8;
    node->cursor_2_before += data[25+id_offset];

    if(data[26+id_offset] == 1)
    {
        node->selected_after = true;
    }else{
        node->selected_after = false;
    }

    
    node->cursor_1_after = data[27+id_offset]<<24;
    node->cursor_1_after += data[28+id_offset]<<16;
    node->cursor_1_after += data[29+id_offset]<<8;
    node->cursor_1_after += data[30+id_offset];


    node->cursor_2_after = data[31+id_offset]<<24;
    node->cursor_2_after += data[32+id_offset]<<16;
    node->cursor_2_after += data[33+id_offset]<<8;
    node->cursor_2_after += data[34+id_offset];

    return true;
}
//---------------------------------------------- LOAD BLOCK FROM FILE
void SoilTextRecord::load_block_from_file(int stack_i, int abs_bi, FILE* f, int* loaded_node_n)
{

    //printf("\nLOADING block from file 1...");
    if(f == NULL)
    {
        return;
    }
    //printf("\nLOADING block from file 2...");
    //printf("\nLOADING block from file 3...");
    *loaded_node_n = 0;
    //int bi_end = in_file_node_n[stack_i] - 1;
    int seek_pos = 0;
    
    int tmp_block_data_size = 0;
    int tmp_block_node_n = 0;
    int str_pos = 0;
    int tmp_str_len = 0;

    SoilString tmp_str;

    long int l = ftell(f);

    get_int_from_file(f, &tmp_block_data_size);
    get_int_from_file(f, &tmp_block_node_n);
    get_int_from_file(f, &str_pos);
    get_int_from_file(f, &tmp_str_len);

    SoilString node_str;

    int ni = 0;
    int i = 0;
    STR_REC_NODE* new_node;
    STR_REC_NODE* cur_node;
    unsigned int ch;
    unsigned char data[STR_REC_NODE_DATA_LEN];
    while((ch = fgetc(f)) != EOF && ni < tmp_block_node_n)
    {
        data[i] = ch;
        i++;
        if(i == STR_REC_NODE_DATA_LEN)
        {
            new_node = new STR_REC_NODE;
            read_node_from_data(new_node, data);
            node_to_soil_str(*new_node, &node_str);
            new_node->prev = NULL;
            new_node->next = NULL;
            if(ni == 0)
            {
                first[stack_i][abs_bi] = new_node;
            }else{
                cur_node->next = new_node;
                new_node->prev = cur_node;
            }

            cur_node = new_node;
            ni++;
            in_mem_node_n[stack_i] += 1;
            block_node_n[stack_i][abs_bi] += 1;
            block_size[stack_i][abs_bi] += STR_REC_NODE_DATA_LEN;
            //printf("\nblock_size(b) stack_i:%d. abs_bi:%d. : %d",stack_i, abs_bi,block_size[stack_i][abs_bi]);
            in_file_node_n[stack_i] -= 1;
            *loaded_node_n += 1;
            i = 0;
        }
        if(ni == tmp_block_node_n){break;}
    }
    
    //printf("\nloaded_nodes_n: %d", loaded_nodes_n);
    if(ni > 0)
    {
        last[stack_i][abs_bi] = cur_node;
    }
    //MARKER:perhaps empty rec_str[stack_i][abs_bi] first
    rec_str[stack_i][abs_bi].prepend_from_file(f, 0, tmp_str_len);
    block_size[stack_i][abs_bi] += rec_str[stack_i][abs_bi].get_byte_n();
    //printf("\nblock_size(c) stack_i:%d. abs_bi:%d. : %d",stack_i, abs_bi,block_size[stack_i][abs_bi]);
    
}
//---------------------------------------------- LOAD BLOCK FROM EXCESS FILE
void SoilTextRecord::pop_block_from_excess_file(int stack_i, int abs_bi)
{ 
    if(!save_excess)
    {
        return;
    }
    FILE * f_old = NULL;
    f_old = fopen(excess_fn[stack_i], "r");
    if(f_old == NULL)
    {
        return;
    }

    //printf("\nPOPING block... stack_i: %d. abs_bi: %d", stack_i, abs_bi);

    bool left_in_file = false;
    if(in_file_block_n[stack_i] > 1)
    {
        left_in_file = true;
    }


    FILE * f = NULL;
    int bi_end = in_file_block_n[stack_i];
    
    int next_block_limit = 0;
    int tmp_block_data_size = 0;
    int bi = 1;

    long int l = ftell(f_old);
    int i = 0;

    if(left_in_file)
    {
        f = fopen(excess_alt_fn[stack_i], "w");
        if(f == NULL)
        {
            fclose(f_old);
            return;
        }

        bool first_block = true;
        int ch;
        i = 0;
        next_block_limit = 0;
        //MARKER: the loop below lacks file limit control
        while(i < file_size[stack_i])
        {
            if(i == next_block_limit)
            {
                if(bi == in_file_block_n[stack_i]){break;}
                
                get_int_from_file(f_old, &tmp_block_data_size);                
                write_int_to_file(f, tmp_block_data_size);
                
                //printf("\nnext block limit: %d", next_block_limit);
                next_block_limit += tmp_block_data_size;
                bi++;
                i+=4;
            }else{
                fputc(fgetc(f_old), f);

                i++;
            }
            //printf("\nI:%d",i);            
        }

        fclose(f);

    }
    int loaded_node_n = 0;
    load_block_from_file(stack_i, abs_bi, f_old, &loaded_node_n);
    file_size[stack_i] = next_block_limit;
    in_file_block_n[stack_i] -= 1;

    //print_data();
    fclose(f_old);

    if(!left_in_file)
    {
        remove(excess_fn[stack_i]);
    }else{
        rename(excess_alt_fn[stack_i], excess_fn[stack_i]);
    }

}
//---------------------------------------------- EMPTY STACK
void SoilTextRecord::empty_stack(int stack_i)
{

    if(node_n[stack_i] == 0)
    {
        return;
    }
    int b = 0;
    int i = 0;
    while(b < 4)
    {
        empty_block(stack_i, BI[stack_i][b], false);
        b++;
    }

    //MARKER: validate conditions to actually atempt to remove file
    if(save_excess)
    {
        remove_excess_file(stack_i); 
        //remove_excess_file(REDO_I);
    }
    //mem[stack_i].calculate(0);
    node_n[stack_i] = 0;
    in_mem_node_n[stack_i] = 0;
    CB[stack_i] = 0;
    reset_bi(stack_i);
    
    return;
}
//-------------------------------------------------------------------------------------- 
//---------------------------------------------- REMOVE FILE EXCESS
void SoilTextRecord::remove_excess_file(int stack_i)
{

    int nodes_lost = in_file_node_n[stack_i];
    if(std::filesystem::exists(excess_fn[stack_i]) && access(excess_fn[stack_i], R_OK) == 0 && access(excess_fn[stack_i], W_OK) == 0)
    {
        in_file_node_n[stack_i] = 0;
        file_size[stack_i] = 0;
        remove(excess_fn[stack_i]);
    }
    in_file_block_n[stack_i] = 0;
    node_n[stack_i] -= nodes_lost;

}
//---------------------------------------------- START STRING OPERATIONS
void SoilTextRecord::start_str_op()
{
    target_str->signal_receiver[0] = false;
    target_str->rec_action = true;
}
//---------------------------------------------- END STRING OPERATIONS
void SoilTextRecord::end_str_op()
{
    target_str->signal_receiver[0] = true;
    target_str->rec_action = false;
    target_str->str_changed();
}
//---------------------------------------------- SET EXCESS FILES
//0 if both were successful, 1 if undo_file failed, 2 if redo_file failed, 3 if both failed
//max_usage is set in KB
bool SoilTextRecord::set_excess_storage(std::string storage_dir, int max_usage)
{

    //printf("\nSETTING STORAGE START****************");
    char ch = storage_dir.at(storage_dir.length()-1);  
    if(ch != '/')
    {
        storage_dir += '/';
    }
    bool failed = false;
    if(std::filesystem::exists(storage_dir))
    {
        //printf("\ndir exists");
        int i = 0;
        while(i < 2)
        {
            // -------------------------------------------- set file name
            excess_filename[i] = storage_dir;
            if(i == UNDO_I)
            {
                excess_filename[i].append("undo_stack_excess_");
            }else{
                excess_filename[i].append("redo_stack_excess_");
            }
            excess_filename[i].append((int)rec_id, 16, 8, 8);

            excess_fn[i] = excess_filename[i].c_str_alloc();
            excess_filename[i].insert('_',-1);
            excess_alt_fn[i] = excess_filename[i].c_str_alloc();
            excess_filename[i].remove(-2, 1);
            
            // -------------------------------------------- set file validity
            if(!std::filesystem::exists(excess_fn[i]) && !std::filesystem::exists(excess_alt_fn[i]))
            {
                std::ofstream myfile;
                myfile.open(excess_fn[i]);
                if(myfile.is_open())
                {
                    myfile.close();
                    remove(excess_fn[i]);
                }else{
                    failed = true;
                    break;
                }

                myfile.open(excess_alt_fn[i]);
                if(myfile.is_open())
                {
                    myfile.close();
                    remove(excess_alt_fn[i]);
                }else{
                    failed = true;
                    break;
                }

            }else{
                failed = true;
                break;
            }

            i++;
        }

    }else{
        //printf("\ndir doesnt exists");
        

        return false;
    }

    if(failed)
    {
        if(excess_fn[UNDO_I] != NULL){delete[] excess_fn[UNDO_I];}
        if(excess_alt_fn[UNDO_I] != NULL){delete[] excess_alt_fn[UNDO_I];}
        if(excess_fn[REDO_I] != NULL){delete[] excess_fn[REDO_I];}
        if(excess_alt_fn[REDO_I] != NULL){delete[] excess_alt_fn[REDO_I];}
        //printf("\nFAILED TO SET EXCESS");
        return false;
    }

    //printf("\nSETTING STORAGE****************");
    save_excess = true;

    //MARKER: validate argument
    //max_file_usage = max_usage;
    max_file_usage = max_usage;
    
    return true;
}
//---------------------------------------------- SET EXCESS STORAGE
bool SoilTextRecord::set_excess_storage(std::string storage_dir)
{
    return set_excess_storage(storage_dir, DEFAULT_MAX_FILE_USAGE);
}
//---------------------------------------------- UNSET EXCESS FILES
//0 if both were successful, 1 if undo_file failed, 2 if redo_file failed, 3 if both failed
void SoilTextRecord::unset_excess_storage()
{
    if(!save_excess)
    {
        return;
    }
    int i = 0;
    while(i < 2)
    {
        if(in_file_node_n[i] > 0)
        {
            //retrieve_excess_from_file(i, in_file_node_n[i]);
        }
        //remove_excess_file already changes in_file_node_n and node_n
        remove_excess_file(i);
        excess_filename[i] = "";

        if(excess_fn[i] != NULL)
        {
            delete[] excess_fn[i];
            excess_fn[i] = NULL;
        }

        if(excess_alt_fn[i] != NULL)
        {
            delete[] excess_alt_fn[i];
            excess_alt_fn[i] = NULL;
        }

        i++;
    }
    save_excess = false;
}
//---------------------------------------------- SAVE STACK TO FILE
bool SoilTextRecord::save_stack_to_file(int stack_i, FILE* f)
{
    //print_data(stdout);
    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];
    
    int block_data_size = 0;
    int block_n = 0;
    int i = 0;
    while(i <= CB[stack_i])
    {
        abs_bi = BI[stack_i][i];
        if(block_size[stack_i][abs_bi] > 0)
        {
            block_data_size += (16 + block_size[stack_i][abs_bi]);
            block_n++;
        }
        i++;
    }
    
    write_int_to_file(f, block_data_size);
    write_int_to_file(f, block_n);
    write_int_to_file(f, file_size[stack_i]);
    write_int_to_file(f, in_file_block_n[stack_i]);
    
    write_int_to_file(f, in_file_node_n[stack_i]);

    long int l = ftell(f);
    

    i = 0;
    while(i < block_n)
    {
        abs_bi = BI[stack_i][i];
        save_block_to_file(stack_i, abs_bi, f);
        i++;
    }

    if(file_size[stack_i] > 0)
    {
        FILE* fe = NULL;
        fe = fopen(excess_fn[stack_i], "r");

        if(fe == NULL){return false;}

        int ch;
        i = 0;
        while(i < file_size[stack_i])
        {
            ch = fgetc(fe);
            if(ch == EOF){return false;}
            fputc(ch, f);
            i++;
        }
        fclose(fe);
    }
    
    return true;

}
//---------------------------------------------- LOAD STACK FROM FILE
bool SoilTextRecord::load_stack_from_file(int stack_i, FILE* f)
{
    empty_stack(stack_i);

    int abs_bi = BI[stack_i][CB[stack_i]];
    int vir_bi = CB[stack_i];

    int block_data_size;
    int block_n;
    int excess_file_size;
    int excess_block_n;
    int excess_node_n;

    get_int_from_file(f, &block_data_size);
    get_int_from_file(f, &block_n);
    get_int_from_file(f, &excess_file_size);
    get_int_from_file(f, &excess_block_n);
    get_int_from_file(f, &excess_node_n);

    int loaded_node_n = 0;
    int i = 0;
    while(i < block_n)
    {
        abs_bi = BI[stack_i][i];
        load_block_from_file(stack_i, abs_bi, f, &loaded_node_n);
        node_n[stack_i] += loaded_node_n;
        in_mem_node_n[stack_i] += loaded_node_n;

        i++;
    }
    
    if(i > 0){CB[stack_i] = i-1;}
    in_mem_node_n[stack_i] += loaded_node_n;

    if(excess_file_size > 0)
    {
        FILE* fe = NULL;
        fe = fopen(excess_fn[stack_i], "w");

        if(fe == NULL){return false;}

        int ch;
        i = 0;
        while(i < excess_file_size)
        {
            ch = fgetc(f);
            if(ch == EOF)
            {
                fclose(fe);
                return false;
            }
            fputc(ch, fe);
            i++;
        }
        file_size[stack_i] = i;
        fclose(fe);
    }
    in_file_node_n[stack_i] = excess_node_n;
    in_file_block_n[stack_i] = excess_block_n;


    node_n[stack_i] += excess_node_n;

    return true;
}
// ------------------------------- END OF STACK METHODS

//---------------------------------------------- SET TARGET STRING
void SoilTextRecord::set_target_str(SoilString* str)
{

    //MARKER: make sure to always reset the recs when setting target_str
    //if direct is true, the SoilString will set the recs everytime it is changed
    //if direct is false, the recs have to be set externally, but 
    if(str == NULL)
    {
        return;
    }
    SoilObject* obj = (SoilObject*)this;
    str->set_soilobject_receiver(1, obj);

    //MARKER: instead of REC_ACTIVE, would I use signal_receiver[] ???
    str->REC_ACTIVE = true;
    str->set_update_cursor(true);
    target_str = str;
}
//---------------------------------------------- SET TARGET STRING
void SoilTextRecord::unset_target_str()
{

    //MARKER: check if its ok to set the string receiver to NULL like this
    if(target_str != NULL)
    {   
        target_str->set_soilobject_receiver(1, NULL);
        target_str->REC_ACTIVE = false;
        target_str->set_update_cursor(false);
        target_str = NULL;
    }
    empty_stack(UNDO_I);
    empty_stack(REDO_I);

}
//---------------------------------------------- SOILOBJECT CALLBACK
//this is called when the target_str is changed
int SoilTextRecord::soilobject_callback(int arg)
{
    if(target_str == NULL)
    {
        return (-1);
    }
    STR_REC_NODE node = target_str->last_rec;
    node.order_id = node_order_id;
    node_order_id++;

    int undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    int undo_vir_bi = CB[UNDO_I];

    if(arg == 1)
    {

        //INSERTION HAPPENED
        int action_group = node.type / 10;
        if(action_group == 0)
        {
            if(node.type == 0)
            {
                //generic insert
                push_node(UNDO_I, node);
                //increment_limit_check(UNDO_I);
                empty_stack(REDO_I);
                
            }else{
                //undo will remove
                //action_text_added(node.str_pos, node.rem_len);
                bool updated = false;
                if(typing_update_rec && node_n[UNDO_I] > 0)
                {
                    //if they were two consecutive char of the same kind(space or non-space)
                    //STR_REC_NODE* last_undo_node = last[UNDO_I][undo_abs_bi];
                    int last_undo_abs_bi;
                    STR_REC_NODE* last_undo_node = get_last_node(UNDO_I, &last_undo_abs_bi);
                    if(last_undo_node != NULL)
                    {
                        if(node.type == (last_undo_node->type) && node.str_pos == (last_undo_node->str_pos + last_undo_node->rem_len))
                        {
                            
                            update_last_action_text_added(last_undo_node);
                            updated = true;
                        }
                    }
                }
                
                if(!updated)
                {

                    push_node(UNDO_I, node);
                    //increment_limit_check(UNDO_I);
                    empty_stack(REDO_I);
                }

            }
            
        }
        //REMOVAL HAPPENED
        if(action_group == 1)
        {
            if(node.type == 10)
            {
                //generic text remove
                SoilString sec_str = target_str->section(node.str_pos, node.ins_len);
                node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
                rec_str_insert(UNDO_I, undo_abs_bi, sec_str, -1);
                push_node(UNDO_I, node);
                //increment_limit_check(UNDO_I);
                empty_stack(REDO_I);
                return 0;
            }
            if(node.type == 16)
            {
                node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
                rec_str_insert(UNDO_I, undo_abs_bi, target_str->multi_trans_inc_str, -1);
                push_node(UNDO_I, node);
                empty_stack(REDO_I);
            }else{
                //one char action
                SOIL_CHAR ch = target_str->at(node.str_pos);
                bool updated = false;
                //MARKER: typing_update_rec should be revised... there should be a way to get last undo node
                //across blocks
                if(typing_update_rec && node_n[UNDO_I] > 0)
                {
                    int last_undo_abs_bi = 0;
                    STR_REC_NODE* last_undo_node = get_last_node(UNDO_I, &last_undo_abs_bi);
                    if(last_undo_node != NULL)
                    {
                        if(node.type == (last_undo_node->type))
                        {
                            int pos_dif = 0;
                            if(node.type < 13)
                            {
                                //ex: removed to the left, ex: backspace
                                pos_dif = 1;
                            }
                            if(node.str_pos == last_undo_node->str_pos - pos_dif)
                            {
                                update_last_action_text_removed(ch, last_undo_node, last_undo_abs_bi);
                                updated = true;
                            }
                        }
                    }
                    //printf("\nACT type:%d. ins_len:%d. rem_len:%d. str_pos:%d.",node.type, node.ins_len,node.rem_len,node.str_pos);
                }
                if(!updated)
                {
                    node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
                    rec_str_insert(UNDO_I, undo_abs_bi, ch, -1);
                    push_node(UNDO_I, node);
                    //increment_limit_check(UNDO_I);
                    empty_stack(REDO_I);
                }
            }
        }
        //TRANSFORMATION (REMOVAL + INSERTION) HAPPENED
        if(action_group == 2)
        {
            //undo would remove then insert
            SoilString sec_str = target_str->section(node.str_pos, node.ins_len);
            node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
            rec_str_insert(UNDO_I, undo_abs_bi, sec_str, -1);
            push_node(UNDO_I, node);
            //increment_limit_check(UNDO_I);
            empty_stack(REDO_I);
        }
        //A CHAR WAS CHANGED(WITH NO INSERTION OR REMOVAL)
        if(action_group == 3)
        {
            //set char
            SOIL_CHAR sch = target_str->at(node.str_pos);
            node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
            //node.rec_pos = (-2);
            //action_text_ch_set(sch, node.str_pos);

            //rec_str[UNDO_I][undo_abs_bi].append(sch);
            rec_str_insert(UNDO_I, undo_abs_bi, sch, -1);
            push_node(UNDO_I, node);
            //increment_limit_check(UNDO_I);  
            empty_stack(REDO_I);
        }
        //CHAR CASE CHANGE(EITHER 1 CHAR OR A SECTION OF THE STR)
        if(action_group == 4)
        {
            //change case
            if(node.type == 40)
            {
                push_node(UNDO_I, node);
                //increment_limit_check(UNDO_I);
                empty_stack(REDO_I);
            }else{
                node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
                rec_str_insert(UNDO_I, undo_abs_bi, target_str->multi_trans_inc_str, -1);
                push_node(UNDO_I, node);
                //increment_limit_check(UNDO_I);
                empty_stack(REDO_I);
            }
        }

        //TRANSFORM IN MULTIPLE PLACES(SEARCH_REPLACE)
        if(action_group == 7)
        {
            if(target_str->multi_trans_inc_str != NULL)
            {                
                node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
                //MARKER:set get_length() for SoilIncString?
                //node.rec_pos = (target_str->multi_trans_inc_str->length + 1)*(-1);
                //rec_str[UNDO_I][undo_abs_bi].append(target_str->multi_trans_inc_str);
                rec_str_insert(UNDO_I, undo_abs_bi, target_str->multi_trans_inc_str, -1);
                push_node(UNDO_I, node);
                //increment_limit_check(UNDO_I);
                empty_stack(REDO_I);
            }
        }

        if(action_group == 8)
        {
            if(target_str->multi_trans_inc_str != NULL)
            {
                node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
                //node.rec_pos = (target_str->multi_trans_inc_str->length + 1)*(-1);
                
                // /rec_str[UNDO_I][undo_abs_bi].append(target_str->multi_trans_inc_str);
                rec_str_insert(UNDO_I, undo_abs_bi, target_str->multi_trans_inc_str, -1);

                push_node(UNDO_I, node);
                //increment_limit_check(UNDO_I);
                empty_stack(REDO_I);
            }
        }

    }
    
    return 0;
}

//---------------------------------------------- UNDO
//this returns what is potentially the absolute cursor position when redoing, or -1 if there is
//no record for undo
STR_REC_NODE SoilTextRecord::undo()
{
    STR_REC_NODE ret_node;
    ret_node.type = (-1);

    
    if(node_n[UNDO_I] == 0)
    {
        return ret_node;
    }

    if(target_str == NULL)
    {
        return ret_node;
    }

    int undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    int undo_vir_bi = CB[UNDO_I];

    int redo_abs_bi = BI[REDO_I][CB[REDO_I]];
    int redo_vir_bi = CB[REDO_I];
    
    /*
    printf("\n\n-------------------------------------------- INFO (undo)");
    printf("\nundo_vir_bi:%d. undo_abs_bi:%d.", undo_vir_bi, undo_abs_bi);
    printf("\nN n[U]:%d. mem N n[U]:%d.", node_n[UNDO_I], in_mem_node_n[UNDO_I]);
    printf("\nCB[U]:%d. BL N n:%d. BL size:%d", CB[UNDO_I], block_node_n[UNDO_I][undo_abs_bi], block_size[UNDO_I][undo_abs_bi]);
    printf("\nfile N n:%d. file BL n:%d. file size:%d.",in_file_node_n[UNDO_I], in_file_block_n[UNDO_I], file_size[UNDO_I]);
    printf("\n.");
    */

    if(block_node_n[UNDO_I][undo_abs_bi] == 0)
    {
        //printf("\nGOT HERE 1");
        decrement_limit_check(UNDO_I);
    }

    undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    undo_vir_bi = CB[UNDO_I];

    redo_abs_bi = BI[REDO_I][CB[REDO_I]];
    redo_vir_bi = CB[REDO_I];


    start_str_op();
    STR_REC_NODE node = pop_node(UNDO_I);//(7)

    //printf("\nUNDO type:%d. ins_len:%d. rem_len:%d. str_pos:%d.",node.type, node.ins_len,node.rem_len,node.str_pos);

    SoilString node_str;
    node_to_soil_str(node, &node_str);
    //node_str.printf_str(2,2);
    ret_node = node;

    int action_group = node.type / 10;
    //UNDO REMOVES
    if(action_group == 0)
    {
        SoilString sec_str = target_str->section(node.str_pos, node.rem_len);
        STR_REC_NODE new_node = make_insertion_rec_node(sec_str, node.str_pos, rec_str[REDO_I][redo_abs_bi].get_length());
        
        copy_invert_text_field_vars(&new_node, node);

        new_node.order_id = node.order_id;
        push_node(REDO_I, new_node);
        //rec_str[REDO_I][redo_abs_bi].append(sec_str);
        rec_str_insert(REDO_I, redo_abs_bi, sec_str, -1);
        //increment_limit_check(REDO_I);
        target_str->remove(node.str_pos, node.rem_len);
        end_str_op();
    }
    //UNDO INSERTS
    if(action_group == 1)
    {   
        if(node.type == 16)
        {
            SoilString ins_str = rec_str[UNDO_I][undo_abs_bi].section(node.rec_pos, node.ins_len);
            int prev_length = target_str->get_length();

            target_str->rec_op_double_insertion(node, ins_str);

            STR_REC_NODE new_node = node;
            new_node.str_pos = SoilString::coded_int_from_2_soil_char(ins_str.at(0), ins_str.at(1));
            new_node.ins_len = prev_length;
            new_node.type = 6;
            copy_invert_text_field_vars(&new_node, node);
            rec_str_remove(UNDO_I, undo_abs_bi, node.rec_pos, node.ins_len);
            push_node(REDO_I, new_node);
            end_str_op();
            
        }else{
            //printf("\n\naction group 1");
            SoilString ins_str = rec_str[UNDO_I][undo_abs_bi].section(node.rec_pos, node.ins_len);
            STR_REC_NODE new_node = make_removal_rec_node(node.str_pos, node.ins_len);
            copy_invert_text_field_vars(&new_node, node);
            new_node.order_id = node.order_id;
            push_node(REDO_I, new_node);
            rec_str_remove(UNDO_I, undo_abs_bi, node.rec_pos, node.ins_len);
            target_str->insert(ins_str, node.str_pos);
            end_str_op();
        }
    }
    //UNDO REMOVES THEN INSERTS
    if(action_group == 2)
    {
        SoilString sec_str = target_str->section(node.str_pos, node.rem_len);
        SoilString ins_str = rec_str[UNDO_I][undo_abs_bi].section(node.rec_pos, node.ins_len);
        STR_REC_NODE new_node = make_transformation_rec_node(sec_str, node.str_pos, node.ins_len, rec_str[REDO_I][redo_abs_bi].get_length());

        copy_invert_text_field_vars(&new_node, node);

        new_node.order_id = node.order_id;
        push_node(REDO_I, new_node);
        rec_str_insert(REDO_I, redo_abs_bi, sec_str, -1);
        rec_str_remove(UNDO_I, undo_abs_bi, node.rec_pos, node.ins_len);

        target_str->replace(ins_str, node.str_pos, node.rem_len);
        end_str_op();
    }
    if(node.type == 30)
    {
        SOIL_CHAR save_sch = target_str->at(node.str_pos);

        int undo_str_len = rec_str[UNDO_I][undo_abs_bi].get_length();
        SOIL_CHAR set_sch = rec_str[UNDO_I][undo_abs_bi].at(undo_str_len - 1);
        //rec_str[UNDO_I][undo_abs_bi].remove_soil_char(rec_str[UNDO_I][undo_abs_bi].get_length() - 1);
        rec_str_remove(UNDO_I, undo_abs_bi, undo_str_len - 1, 1);

        //rec_str[REDO_I][redo_abs_bi].append(save_sch);
        rec_str_insert(REDO_I, redo_abs_bi, save_sch, -1);
        push_node(REDO_I, node);
        //increment_limit_check(REDO_I);

        target_str->set_ch_at(node.str_pos, set_sch);
        end_str_op();
    }
    if(action_group == 4)
    {
        int op = node.type;
        if(op == 40)
        {
            push_node(REDO_I, node);
            //increment_limit_check(REDO_I);
            target_str->section_invert_case(node.str_pos, node.ins_len);
        }

        if(op == 41 || op == 42)
        {
            int total_rec_n = SoilString::coded_int_from_2_soil_char(rec_str[UNDO_I][undo_abs_bi].at(node.rec_pos), rec_str[UNDO_I][undo_abs_bi].at(node.rec_pos + 1));
            int rec_str_len = 2 + (4 * total_rec_n);
            SoilString tmp_str = rec_str[UNDO_I][undo_abs_bi].section(node.rec_pos, rec_str_len);
            target_str->rec_op_section_set_case_undo(node, tmp_str);

            if(op == 41)
            {
                node.type = 42;
            }else{
                node.type = 41;
            }

            push_node(REDO_I, node);
            rec_str_remove(UNDO_I, undo_abs_bi, node.rec_pos, rec_str_len);
            //increment_limit_check(REDO_I);
        }
        end_str_op();

    }
    if(action_group == 7)
    {
        int total_rec_n = SoilString::coded_int_from_2_soil_char(rec_str[UNDO_I][undo_abs_bi].at(node.rec_pos), rec_str[UNDO_I][undo_abs_bi].at(node.rec_pos + 1));

        int rec_str_len = 2;
        int new_type = 70;
        if(node.type == 71)
        {
            rec_str_len += total_rec_n*(node.ins_len+2);
            new_type = 72;
        }else{
            rec_str_len += (2*total_rec_n) + node.ins_len;
        }
        SoilString tmp_str = rec_str[UNDO_I][undo_abs_bi].section(node.rec_pos, rec_str_len);
        SoilString new_rec_str;
        target_str->rec_op_search_replace_str(node, tmp_str, &new_rec_str);
        STR_REC_NODE new_node = make_multi_trans_rec_node(new_type, node.rem_len, node.ins_len, rec_str[REDO_I][redo_abs_bi].get_length());
        copy_invert_text_field_vars(&new_node, node);
        new_node.order_id = node.order_id;
        push_node(REDO_I, new_node);
        //rec_str[UNDO_I][undo_abs_bi].remove(node.rec_pos, rec_str_len);
        rec_str_remove(UNDO_I, undo_abs_bi, node.rec_pos, rec_str_len);

        //rec_str[REDO_I][redo_abs_bi].append(new_rec_str);
        rec_str_insert(REDO_I, redo_abs_bi, new_rec_str, -1);
        //increment_limit_check(REDO_I);

        end_str_op();
    }

    if(action_group == 8)
    {
        if(node.type == 80)
        {
            //MARKER:revise all cursor pos vs character(index) pos incoherence

            int total_rec_n = SoilString::coded_int_from_2_soil_char(rec_str[UNDO_I][undo_abs_bi].at(node.rec_pos), rec_str[UNDO_I][undo_abs_bi].at(node.rec_pos + 1));
            int rec_str_len = 4 + (4*total_rec_n);
            SoilString tmp_str = rec_str[UNDO_I][undo_abs_bi].section(node.rec_pos, rec_str_len);       

            target_str->rec_op_simplify_undo(node, tmp_str);
            //rec_str[UNDO_I][undo_abs_bi].remove(node.rec_pos, rec_str_len);
            rec_str_remove(UNDO_I, undo_abs_bi, node.rec_pos, rec_str_len);

            node.type = 81;
            push_node(REDO_I, node);
        }
        end_str_op();
    }
    return ret_node;
}
//---------------------------------------------- REDO
//this returns what is potentially the absolute cursor position when undoing, or -1 if there is
//no record for redo
STR_REC_NODE SoilTextRecord::redo()
{

    STR_REC_NODE ret_node;
    ret_node.type = (-1);

    if(node_n[REDO_I] == 0)
    {
        return ret_node;
    }
    if(target_str == NULL)
    {
        return ret_node;
    }

    int undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    int undo_vir_bi = CB[UNDO_I];

    int redo_abs_bi = BI[REDO_I][CB[REDO_I]];
    int redo_vir_bi = CB[REDO_I];

    if(block_node_n[REDO_I][redo_abs_bi] == 0)
    {
        decrement_limit_check(REDO_I);
    }

    undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    undo_vir_bi = CB[UNDO_I];

    redo_abs_bi = BI[REDO_I][CB[REDO_I]];
    redo_vir_bi = CB[REDO_I];

    start_str_op();
    STR_REC_NODE node = pop_node(REDO_I);
    ret_node = node;

/*
    printf("\n\n-------------------------------------------- INFO(redo)");
    printf("\nN n[U]:%d. mem N n[U]:%d.", node_n[UNDO_I], in_mem_node_n[UNDO_I]);
    printf("\nCB[U]:%d. BL N n:%d. BL size:%d", CB[UNDO_I], block_node_n[UNDO_I][undo_abs_bi], block_size[UNDO_I][undo_abs_bi]);
    printf("\nfile N n:%d. file BL n:%d. file size:%d.",in_file_node_n[UNDO_I], in_file_block_n[UNDO_I], file_size[UNDO_I]);
    printf("\n.");
*/
    int action_group = node.type / 10;
    if(action_group == 0)
    {
        if(node.type == 6)
        {
            SoilString sec_str;
            SOIL_CHAR sch_1, sch_2;

            SoilString::code_int_in_2_soil_char(node.str_pos, &sch_1, &sch_2);
            sec_str.append(sch_1);
            sec_str.append(sch_2);
            sec_str.append(target_str->section(0, node.str_pos));
            
            int prev_length = target_str->get_length();
            int second_pos = node.str_pos + node.ins_len;
            int second_sec = prev_length - second_pos;

            SoilString::code_int_in_2_soil_char(second_sec, &sch_1, &sch_2);
            sec_str.append(sch_1);
            sec_str.append(sch_2);
            sec_str.append(target_str->section(second_pos, second_sec));

            target_str->section_itself(node.str_pos, node.ins_len);
            STR_REC_NODE new_node = node;
            new_node.type = 16;
            new_node.ins_len = sec_str.get_length();
            copy_invert_text_field_vars(&new_node, node);
            push_node(UNDO_I, new_node);
            rec_str_insert(UNDO_I, undo_abs_bi, sec_str, -1);
            end_str_op();

        }else{
            SoilString sec_str = target_str->section(node.str_pos, node.rem_len);
            STR_REC_NODE new_node = make_insertion_rec_node(sec_str, node.str_pos, rec_str[UNDO_I][undo_abs_bi].get_length());
            copy_invert_text_field_vars(&new_node, node);

            new_node.order_id = node.order_id;
            push_node(UNDO_I, new_node);
            //rec_str[UNDO_I][undo_abs_bi].append(sec_str);
            rec_str_insert(UNDO_I, undo_abs_bi, sec_str, -1);
            //increment_limit_check(UNDO_I);

            target_str->remove(node.str_pos, node.rem_len);
            end_str_op();
        }
    }
    if(action_group == 1)
    {
       
        SoilString ins_str = rec_str[REDO_I][redo_abs_bi].section(node.rec_pos, node.ins_len);
        STR_REC_NODE new_node = make_removal_rec_node(node.str_pos, node.ins_len);
        copy_invert_text_field_vars(&new_node, node);

        new_node.order_id = node.order_id;
        push_node(UNDO_I, new_node);
        //increment_limit_check(UNDO_I);
        //rec_str[REDO_I][redo_abs_bi].remove(node.rec_pos, node.ins_len);
        rec_str_remove(REDO_I, redo_abs_bi, node.rec_pos, node.ins_len);

        target_str->insert(ins_str, node.str_pos);

        end_str_op();       
    }
    if(action_group == 2)
    {
        SoilString sec_str = target_str->section(node.str_pos, node.rem_len);
        SoilString ins_str = rec_str[REDO_I][redo_abs_bi].section(node.rec_pos, node.ins_len);
        STR_REC_NODE new_node = make_transformation_rec_node(sec_str, node.str_pos, node.ins_len, rec_str[UNDO_I][undo_abs_bi].get_length());
        copy_invert_text_field_vars(&new_node, node);
        new_node.order_id = node.order_id;
        push_node(UNDO_I, new_node);
        rec_str_insert(UNDO_I, undo_abs_bi, sec_str, -1);
        rec_str_remove(REDO_I, redo_abs_bi, node.rec_pos, node.ins_len);

        target_str->replace(ins_str, node.str_pos, node.rem_len);
        end_str_op();
    }
    if(action_group == 3)
    {
        int redo_str_len = rec_str[REDO_I][redo_abs_bi].get_length();
        SOIL_CHAR save_sch = target_str->at(node.str_pos);
        SOIL_CHAR set_sch = rec_str[REDO_I][redo_abs_bi].at(redo_str_len - 1);
        rec_str_remove(REDO_I, redo_abs_bi, redo_str_len - 1, 1);
        rec_str_insert(UNDO_I, undo_abs_bi, save_sch, -1);
        //copy_invert_text_field_vars(&node, node);
        push_node(UNDO_I, node);
        target_str->set_ch_at(node.str_pos, set_sch);
        end_str_op();        
    }

    if(action_group == 4)
    {
        int op = node.type;
        if(op == 40)
        {
            push_node(UNDO_I, node);
            target_str->section_invert_case(node.str_pos, node.ins_len);
        }
        if(op == 41 || op == 42)
        {
            bool to_lower = true;
            if(op == 41)
            {
                to_lower = false;
                node.type = 42;
            }else{
                node.type = 41;
            }   
        
            node.rec_pos = rec_str[UNDO_I][undo_abs_bi].get_length();
            push_node(UNDO_I, node);
            SoilIncString rec_inc_str;
            target_str->section_set_case(node.str_pos, node.ins_len, &rec_inc_str, to_lower);
            rec_str_insert(UNDO_I, undo_abs_bi, &rec_inc_str, -1);
        }
        end_str_op();
    }

    if(action_group == 7)
    {
        int total_rec_n = SoilString::coded_int_from_2_soil_char(rec_str[REDO_I][redo_abs_bi].at(node.rec_pos), rec_str[REDO_I][redo_abs_bi].at(node.rec_pos + 1));
        int rec_str_len = 2;
        int new_type = 70;
        if(node.type == 72)
        {
            new_type = 71;
        }
        rec_str_len += (2*total_rec_n) + node.ins_len;
        SoilString tmp_str = rec_str[REDO_I][redo_abs_bi].section(node.rec_pos, rec_str_len);
        SoilString new_rec_str;
        target_str->rec_op_search_replace_str(node, tmp_str, &new_rec_str);
        STR_REC_NODE new_node = make_multi_trans_rec_node(new_type, node.rem_len, node.ins_len, rec_str[UNDO_I][undo_abs_bi].get_length());
        copy_invert_text_field_vars(&new_node, node);
        rec_str_remove(REDO_I, redo_abs_bi, node.rec_pos, rec_str_len);
        rec_str_insert(UNDO_I, undo_abs_bi, new_rec_str, -1);
        new_node.order_id = node.order_id;
        push_node(UNDO_I, new_node);  
        end_str_op();
    }

    if(action_group == 8)
    {
        if(node.type == 81)
        {
            SoilIncString rec_inc_str;
            int ret = target_str->simplify(true, &rec_inc_str);
            //rec_str[UNDO_I][undo_abs_bi].append(&rec_inc_str);
            rec_str_insert(UNDO_I, undo_abs_bi, &rec_inc_str, -1);
            node.type = 80;
            push_node(UNDO_I, node);
        }
        end_str_op();
    }

    return ret_node;
}

//---------------------------------------------- UPDATE LAST ACTION TEXT ADDED
void SoilTextRecord::update_last_action_text_added(STR_REC_NODE* last_undo_node)
{

    //int undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    //int undo_vir_bi = CB[UNDO_I];

    if(node_n[UNDO_I] != 0)
    {
        last_undo_node->rem_len += 1;
        last_undo_node->cursor_1_after += 1;       
    
    }
    empty_stack(REDO_I);
}
//---------------------------------------------- UPDATE LAST ACTION TEXT REMOVED
void SoilTextRecord::update_last_action_text_removed(SOIL_CHAR ch, STR_REC_NODE* last_undo_node, int last_undo_abs_bi)
{
    //int undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    //int undo_vir_bi = CB[UNDO_I];

    if(node_n[UNDO_I != 0])
    {
        if(last_undo_node->type < 13)
        {
            // REMOVE LEFT
            last_undo_node->ins_len += 1;
            last_undo_node->str_pos -= 1;
            last_undo_node->cursor_1_after -= 1;
            int ins_pos = last_undo_node->ins_len * (-1);
            //rec_str[UNDO_I][undo_abs_bi].insert(ch, ins_pos);
            rec_str_insert(UNDO_I, last_undo_abs_bi, ch, ins_pos);
        
        }else{
            // REMOVE RIGHT
            last_undo_node->ins_len += 1;
            //rec_str[UNDO_I][undo_abs_bi].append(ch);
            rec_str_insert(UNDO_I, last_undo_abs_bi, ch, -1);
        }
    }
    empty_stack(REDO_I);
}

//---------------------------------------------- SET LAST UNDO REC TEXT FIELD VARS
void SoilTextRecord::set_last_undo_rec_text_field_vars(bool selected_before, int cursor_1_before, int cursor_2_before, bool selected_after, int cursor_1_after, int cursor_2_after)
{
    int undo_abs_bi = BI[UNDO_I][CB[UNDO_I]];
    if(node_n[UNDO_I] == 0)
    {
        return;
    }
    STR_REC_NODE* last_undo_node = last[UNDO_I][undo_abs_bi];
    last_undo_node->selected_before = selected_before;
    last_undo_node->cursor_1_before = cursor_1_before;
    last_undo_node->cursor_2_before = cursor_1_before;

    last_undo_node->selected_after = selected_after;
    last_undo_node->cursor_1_after = cursor_1_after;
    last_undo_node->cursor_2_after = cursor_2_after;
}
//---------------------------------------------- SET LAST REDO REC TEXT FIELD VARS
void SoilTextRecord::set_last_redo_rec_text_field_vars(bool selected_before, int cursor_1_before, int cursor_2_before, bool selected_after, int cursor_1_after, int cursor_2_after)
{
    int redo_abs_bi = BI[REDO_I][CB[REDO_I]];
    if(node_n[REDO_I] == 0)
    {
        return;
    }
    STR_REC_NODE* last_redo_node = last[REDO_I][redo_abs_bi];
    last_redo_node->selected_before = selected_before;
    last_redo_node->cursor_1_before = cursor_1_before;
    last_redo_node->cursor_2_before = cursor_1_before;

    last_redo_node->selected_after = selected_after;
    last_redo_node->cursor_1_after = cursor_1_after;
    last_redo_node->cursor_2_after = cursor_2_after;

}
//---------------------------------------------- PRINT DATA
void SoilTextRecord::print_data(FILE* fd)
{
if(fd != NULL)
{
 	fprintf(fd,"\nlogger_orig/soiltextrecord.cpp : void SoilTextRecord::print_data()");
}
//if(fd != NULL){fclose(fd);}

    //return;
    int s = 0;
    int abs_bi;
    int i = 0;

    fprintf(fd, "\n - - - - - - - - - - - - - - - - - - - - - PRINT DATA");

    while(s < 2)
    {
        if(s == UNDO_I)
        {
            fprintf(fd, "\n\n**********************UNDO REC");
        }else{
            fprintf(fd, "\n\n**********************REDO REC");
        }
        fprintf(fd, "\nin file node n:%d",in_file_node_n[s]);
        fprintf(fd, ". in file block n:%d",in_file_block_n[s]);
        fprintf(fd, ". file size:%d",file_size[s]);
        fprintf(fd, "\n");
        i = 0;
        while(i < 4)
        {
            abs_bi = BI[s][i];
            fprintf(fd, "\nBLOCK %d. ABS I: %d node n:%d. SIZE: %d. REC_STR_LEN: %d.", i, abs_bi, block_node_n[s][BI[s][i]], block_size[s][BI[s][i]], rec_str[s][BI[s][i]].get_length());
            if(i == CB[s])
            {
                fprintf(fd, " ::::::::CURRENT");
            }

            STR_REC_NODE* cur_node = first[s][BI[s][i]];
            while(cur_node != NULL)
            {
                fprintf(fd, "\n - node type:%d rec_pos:%d order_id:%d",cur_node->type, cur_node->rec_pos, cur_node->order_id);


                cur_node = cur_node->next;
            }
            i++;
        }
        
        s++;
    }
    fprintf(fd, "\n - - - - - - - - - - - - - - - PRINT DATA END");

if(fd != NULL)
{
 	fprintf(fd,"\nlogger_orig/soiltextrecord.cpp : void SoilTextRecord::print_data() : END");

}
}


//---------------------------------------------- PRINT STATE FILE
void SoilTextRecord::print_state_file(FILE* f)
{
    printf("\n\n ------------------------ PRINT STATE FILE");
    if(f == NULL)
    {
        printf("\nINVALID FILE DESCRIPTOR");
        return;
    }
    int content = 0;
    content = fgetc(f);
    printf("\ncontent type:");
    if(content == 0)
    {
        printf("no record");
    }else{
        printf("WITH record");
    }
    int val = 0;
    get_int_from_file(f, &val);
    printf("\nTEXT LEN:%d", val);

    SoilString str;
    str.load_from_file(f,val);
    str.printf_str(2,2);

    printf("\n");

    int s = 0;
    while(s < 2)
    {
        if(s == 0)
        {
            printf("\n******** UNDO stack:\n");
        }else{
            printf("\n******** REDO stack:\n");
        }

        int block_data_size;
        int block_n;
        int excess_file_size;
        int excess_block_n;
        int excess_node_n;
        get_int_from_file(f, &block_data_size);
        get_int_from_file(f, &block_n);
        get_int_from_file(f, &excess_file_size);
        get_int_from_file(f, &excess_block_n);
        get_int_from_file(f, &excess_node_n);
        printf("\nMEM TOTAL BLOCK DATA SIZE:%d", block_data_size);
        printf("\nMEM TOTAL BLOCK N:%d", block_n);
        printf("\nEXCESS FILE SIZE:%d", excess_file_size);
        printf("\nFILE BLOCK N:%d", excess_block_n);
        printf("\nFILE NODE N:%d", excess_node_n);
        
        int i = 0;
        while(i < block_n + excess_block_n && i < 100)
        {
            if(i == block_n)
            {
                printf("\n---------- >>> EXCESS FILE:");
            }
            printf("\n\nBLOCK %d:",i);
            int tmp_block_data_size = 0;
            int tmp_block_node_n = 0;
            int str_pos = 0;
            int tmp_str_len = 0;
            get_int_from_file(f, &tmp_block_data_size);
            get_int_from_file(f, &tmp_block_node_n);
            get_int_from_file(f, &str_pos);
            get_int_from_file(f, &tmp_str_len);

            printf("\n:BLOCK DATA SIZE:%d", tmp_block_data_size);
            printf("\n:BLOCK NODE N:%d", tmp_block_node_n);
            printf("\n:STR POS:%d", str_pos);
            printf("\n:STR LEN:%d", tmp_str_len);

            int j = 16;
            while(j < tmp_block_data_size)
            {
                fgetc(f);
                j++;
            }
            
            i++;
        }
        
        printf("\n");
        s++;
    }


    printf("\n\n ------------------------ PRINT STATE FILE (END)");
}
//---------------------------------------------- NODE TO SOIL STRING
void SoilTextRecord::node_to_soil_str(STR_REC_NODE node, SoilString* str)
{

    *str = "";
    str->append("TYPE: ");
    int type = (int)node.type;
    str->append(type);
    int group = type / 10;
    if(group == 0){str->append(" (INSERTED / REMOVES)");}
    if(group == 1){str->append(" (REMOVED / INSERTS)");}
    if(group == 2){str->append(" (TRANSFORMED / TRANSFORMS)");}
    if(group == 3){str->append(" (SET CHAR / SETS CHAR)");}
    if(group == 4){str->append(" (CHANGED CASE / CHANGES CASE)");}

    str->append("\nREC POS: ");
    str->append(node.rec_pos);
    str->append(".  STR POS: ");
    str->append(node.str_pos);
    str->append(".  INS LEN: ");
    str->append(node.ins_len);
    str->append(".  REM LEN: ");
    str->append(node.rem_len);
    if(node.selected_before)
    {
        str->append("\nSEL BEF: TRUE");
    }else{
        str->append("\nSEL BEF: FALSE");
    }
    str->append(".  CUR_1 BEF: ");
    str->append(node.cursor_1_before);
    str->append(".  CUR_2 BEF: ");
    str->append(node.cursor_2_before);
    if(node.selected_after)
    {
        str->append("\nSEL AFT: TRUE");
    }else{
        str->append("\nSEL AFT: FALSE");
    }
    str->append(".  CUR_1 AFT: ");
    str->append(node.cursor_1_after);
    str->append(".  CUR_2 AFT: ");
    str->append(node.cursor_2_after);
}