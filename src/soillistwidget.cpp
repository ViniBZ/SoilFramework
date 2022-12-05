#include "soillistwidget.h"

SoilListWidget::SoilListWidget() : SoilLayout(VERTICAL, LEFT)
{
    length = 0;
    list = NULL;
    value = (-1);
    set_auto_size(false,false);
    set_size(arrow_w + (2*content_pad), arrow_w + (2*content_pad));

    IN_PAD = 3;
    OUT_PAD = 3;
}

SoilListWidget::~SoilListWidget()
{
    int i = 0;
    STR_LIST_ITEM* item = list;
    STR_LIST_ITEM* next_item;
    while(item != NULL)
    {
        next_item = item->next;
        delete item;
        item = next_item;
    }
    length = 0;
}
//--------------------------------------------- INTERNAL SET VALUE
void SoilListWidget::internal_set_value(int v)
{
    value = v;

    if(engine_control_is_set && engine_control != NULL && SIGNALS[VALUE_CHANGED].active )
    {
        SIGNALS[VALUE_CHANGED].arguments.value_changed_arg.value = value;
        engine_control->push_SS(&SIGNALS[VALUE_CHANGED]);
    }
    set_elm_ui_flag(1);
}
//--------------------------------------------- SET VALUE
void SoilListWidget::set_value(int v)
{
    if(!validate_abs_index(length, &v)){return;}
    internal_set_value(v);

    if(soilobject_receiver[0] != NULL)
    {
        soilobject_receiver[0]->soilobject_callback(0);
    }
}
//--------------------------------------------- GET VALUE
int SoilListWidget::get_value()
{
    return value;
}
//--------------------------------------------- GET OPTION N
int SoilListWidget::get_length()
{
    return length;
}
//--------------------------------------------- INSERT
void SoilListWidget::insert(const SoilString& str, int pos)
{
    validate_rel_cursor(length, &pos);

    int i = 0;
    STR_LIST_ITEM* prev_item = NULL;
    STR_LIST_ITEM* next_item = list;
    while(i < pos && next_item != NULL)
    {
        prev_item = next_item;
        next_item = next_item->next;
        i++;
    }

    STR_LIST_ITEM* new_item = new STR_LIST_ITEM;

    if(i == 0)
    {
        list = new_item;
    }
    if(prev_item != NULL)
    {
        prev_item->next = new_item;
    }

    add_elm(&new_item->label);

    new_item->label.set_as_clickable_item(true);
    
    new_item->label.set_text(str);
    new_item->label.set_soilobject_receiver(0, (SoilObject* )this);

    SIZE item_size = new_item->label.get_view_size();
    SIZE new_size = view_size;
    if((item_size.w+(2*OUT_PAD)) > view_size.w){new_size.w = item_size.w+(2*OUT_PAD);}

    if(length == 0)
    {
        new_size.h = item_size.h + (2*OUT_PAD);
    }else{
        new_size.h += item_size.h + IN_PAD;
    }

    new_item->next = next_item;
    length++;

    set_size(new_size.w, new_size.h);
}
//--------------------------------------------- PUSH
void SoilListWidget::push(const SoilString& str)
{
    insert(str, -1);
}
//--------------------------------------------- REMOVE
void SoilListWidget::remove(int ind)
{
    if(length == 0){return;}
    if(!validate_rel_index(length, &ind)){return;}
    int i = 0;
    STR_LIST_ITEM* prev_item = NULL;
    STR_LIST_ITEM* cur_item = list;
    while(i < ind && cur_item != NULL)
    {
        prev_item = cur_item;
        cur_item = cur_item->next;
        i++;
    }

    if(cur_item != NULL)
    {
        if(i == 0)
        {
            list = cur_item->next;
        }else{
            prev_item->next = cur_item->next;
        }
        remove_elm(&cur_item->label);
        delete cur_item;
        length -= 1;
    }

}
//--------------------------------------------- POP
SoilString SoilListWidget::pop()
{
    //MARKER:check the best way to actually procede if there is no item to pop
    //same with at, etc...
    SoilString str;
    if(length == 0){return str;}

    int i = 0;
    STR_LIST_ITEM* prev_item = NULL;
    STR_LIST_ITEM* cur_item = list;
    while(i < (length - 1) && cur_item != NULL)
    {
        prev_item = cur_item;
        cur_item = cur_item->next;
        i++;
    }

    if(cur_item != NULL)
    {
        if(i == 0)
        {
            list = cur_item->next;
        }else{
            prev_item->next = cur_item->next;
        }
        str = *cur_item->label.soil_string;

        remove_elm(&cur_item->label);
        delete cur_item;
        length -= 1;
    }
    return str;
}
//--------------------------------------------- AT
SoilString SoilListWidget::at(int ind)
{
    bool valid_ind = validate_rel_index(length, &ind);
    SoilString str;
    if(!valid_ind){return str;}

    STR_LIST_ITEM* item = list;
    int i = 0;
    while(i < ind && item != NULL)
    {
        item = item->next;
        i++;
    }
    if(item == NULL){return str;}
    str = *item->label.soil_string;
    return str;
}
//--------------------------------------------- SOILOBJECT CALLBACK
int SoilListWidget::soilobject_callback(int arg, int aux_arg)
{   
    
    int item_id = aux_arg;
    STR_LIST_ITEM* item = list;
    int i = 0;
    int elm_value = (-1);
    while(item_id > 0 && item != NULL)
    {
        if(item->label.get_id() == item_id)
        {
            elm_value = i;
            break;
        }
        item = item->next;
        i++;
    }
    set_value(elm_value);

    //MARKER: is this return value correct?
    return 0;
}
//--------------------------------------------- PRINTF LIST
void SoilListWidget::printf_list()
{    
    int count = 0;
    STR_LIST_ITEM* item = list;
    while(item != NULL)
    {
        item->label.soil_string->printf_str(1,0);
        item = item->next;
        count++;
    }
}