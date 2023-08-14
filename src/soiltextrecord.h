#ifndef SOILTEXTRECORD_H
#define SOILTEXTRECORD_H

#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include "soilstring.h"
#include "soilmembound.h"
#include "load_save_files.h"

#define UNDO_I 0
#define REDO_I 1
#define DEFAULT_BLOCK_SIZE_LIMIT 100
#define DEFAULT_MAX_FILE_USAGE 2000

class SoilTextRecord : public SoilObject
{

public:
    static int rec_next_id;
    static int node_order_id;
    unsigned int rec_id;

    SoilString* target_str;

    //The record will be divided in 4 blocks for mem
    // ---------------- stack properties
    bool save_excess;
    SoilString excess_filename[2];
    char* excess_fn[2];
    char* excess_alt_fn[2];

    //max memory(in ram stack) usage is defined by mem[2]
    int max_file_usage;//counted in nodes
    int block_size_limit;

    int node_n[2];//total (in_mem_n + in_file_n)
    int in_mem_node_n[2];//counted in nodes
    int in_file_node_n[2];//counted in number of nodes
    int in_file_block_n[2];//counted in number of blocks
    int file_size[2];//counted in bytes

    //There are 4 blocks for memory text records
    int BI[2][4];//value is absolute block index(0 - 3) of the arrays([2][block index])
    int CB[2];//current block, virtual index (BI index)

    SoilString rec_str[2][4];
    int block_node_n[2][4];
    int block_size[2][4];
    STR_REC_NODE* first[2][4];//
    STR_REC_NODE* last[2][4];

    //the value for each node's size added to block_size[][] is STR_REC_NODE_DATA_LEN
    //which is the size it takes when recorded to file, but sizeof(STR_REC_NODE) is bigger

    // ----------------    

    //typing_update_rec controls if consecutive one char actions will be 
    //lumped as one action for the convenience of the user typing
    bool typing_update_rec;

    //file format:
    //undo_node_n, redo_node_n, undo_rec_str_len, redo_rec_str_len, undo_nodes..., redo_nodes..., undo_rec_str(UTF8), redo_rec_str(UTF8)
    SoilTextRecord();
    ~SoilTextRecord();

    void init();
        
    STR_REC_NODE make_removal_rec_node(int str_pos, int len);
    STR_REC_NODE make_insertion_rec_node(const SoilString& str, int str_pos, int rec_pos);
    STR_REC_NODE make_transformation_rec_node(const SoilString& str, int str_pos, int rem_len, int rec_pos);
    STR_REC_NODE make_ch_set_rec_node(SOIL_CHAR sch, int str_pos);
    STR_REC_NODE make_case_change_rec_node(int str_pos, int n, int action);//case change in a range
    STR_REC_NODE make_multi_trans_rec_node(int action, int ins_len, int rem_len, int rec_pos);//case change in a range

    void copy_invert_text_field_vars(STR_REC_NODE * node, STR_REC_NODE origin);  


    // --------------- stack methods
    void shift_bi_left(int stack_i);
    void shift_bi_right(int stack_i);
    void reset_bi(int stack_i);
    void inc_cb(int stack_i);//increase
    void dec_cb(int stack_i);//decrease
    STR_REC_NODE* get_last_node(int stack_i, int* last_node_abs_bi);
    void empty_block(int stack_i, int abs_bi, bool saved_to_file);
    void rec_str_insert(int stack_i, int abs_bi, const SoilString& str, int pos);
    void rec_str_insert(int stack_i, int abs_bi, SoilIncString* str, int pos);
    void rec_str_insert(int stack_i, int abs_bi, SOIL_CHAR ch, int pos);
    void rec_str_remove(int stack_i, int abs_bi, int pos, int n);
    //void rec_str_remove_ch(int stack_i, int abs_bi, int pos);
    void push_node(int stack_i, STR_REC_NODE node);
    STR_REC_NODE pop_node(int stack_i);
    void increment_limit_check(int stack_i);
    void decrement_limit_check(int stack_i);

    bool write_node_to_file(STR_REC_NODE node, FILE* f);
    bool save_block_to_file(int stack_i, int abs_bi, FILE* f);
    bool save_block_to_excess_file(int stack_i, int abs_bi);

    bool read_node_from_data(STR_REC_NODE* node, unsigned char data[STR_REC_NODE_DATA_LEN]);
    void load_block_from_file(int stack_i, int abs_bi, FILE* f, int* loaded_node_n);
    void pop_block_from_excess_file(int stack_i, int abs_bi);
    
    void empty_stack(int stack_i);
    void remove_excess_file(int stack_i);    
    void start_str_op();
    void end_str_op();
    /// public

    bool set_excess_storage(std::string storage_dir, int max_usage);
    bool set_excess_storage(std::string storage_dir);
    void unset_excess_storage();  
    bool save_stack_to_file(int stack_i, FILE* f);
    bool load_stack_from_file(int stack_i, FILE* f);

    // ------------ end stack methods


    //EXTERNAL
public:
    void set_target_str(SoilString* str);
    void unset_target_str();
    virtual int soilobject_callback(int arg);
    STR_REC_NODE undo();
    STR_REC_NODE redo();

    void update_last_action_text_added(STR_REC_NODE* last_undo_node);
    void update_last_action_text_removed(SOIL_CHAR ch, STR_REC_NODE* last_undo_node, int last_undo_abs_bi);

    void set_last_undo_rec_text_field_vars(bool selected_before, int cursor_1_before, int cursor_2_before, bool selected_after, int cursor_1_after, int cursor_2_after);
    void set_last_redo_rec_text_field_vars(bool selected_before, int cursor_1_before, int cursor_2_before, bool selected_after, int cursor_1_after, int cursor_2_after);
    void print_data(FILE* fd);
    void print_state_file(FILE* f);
    void node_to_soil_str(STR_REC_NODE node, SoilString* str);
};


#endif // SOILTEXTRECORD_H