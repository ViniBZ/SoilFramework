#include "soillayout.h"

SoilLayout::SoilLayout(LAYOUT_TYPE t, ALIGNMENT a) : SoilUIElm(LAYOUT)
{
    layout_type = validate_layout_type(t);
    alignment = validate_alignment(a);
    init();

}
SoilLayout::SoilLayout() : SoilUIElm(LAYOUT)
{
    layout_type = validate_layout_type(VERTICAL);
    alignment = validate_alignment(CENTER);
    init();
}
//--------------------------------------------- INIT
void SoilLayout::init()
{
    scrollable = true;

    child = NULL;
    last = NULL;

    ABSOLUTE_VIEW_MIN_SIZE.w = 30;
    ABSOLUTE_VIEW_MIN_SIZE.h = 30;
    view_min_size.w = ABSOLUTE_VIEW_MIN_SIZE.w;
    view_min_size.h = ABSOLUTE_VIEW_MIN_SIZE.h;

    view_size.w = ABSOLUTE_VIEW_MIN_SIZE.w;
    view_size.h = ABSOLUTE_VIEW_MIN_SIZE.h;

    set_auto_calc_size(view_size.w,view_size.h);

    elm_n=0;

    //AUTO_SIZE.w elements if layout_type is HORIZONTAL
    //AUTO_SIZE.h elements if layout_type is VERTICAL
    auto_elm_n = 0;

    set_auto_size(true,true);

    is_main_layout = false;
    is_modal_layout = false;
    updated_by_window = false;

    IN_PAD = 10;
    OUT_PAD = 0;

}
//--------------------------------------------- VALIDATE LAYOUT TYPE
LAYOUT_TYPE SoilLayout::validate_layout_type(LAYOUT_TYPE t)
{

   if(t != VERTICAL && t != HORIZONTAL)
   {
       t = VERTICAL;
   }

   return t;
}

//--------------------------------------------- CHECK NO LOOPED NESTING
ALIGNMENT SoilLayout::validate_alignment(ALIGNMENT a)
{

    if(layout_type == VERTICAL)
    {
        if(a != LEFT && a != CENTER && a != RIGHT)
        {
            a = LEFT;
        }
    }
    if(layout_type == HORIZONTAL)
    {
        if(a != TOP && a != CENTER && a != BOTTOM)
        {
            a = TOP;
        }
    }

    return a;
}
//--------------------------------------------------- SET LAYOUT TYPE
void SoilLayout::set_layout_type(LAYOUT_TYPE t)
{
    layout_type = validate_layout_type(t);
    set_elm_ui_flag(3);
}
//--------------------------------------------------- SET ALIGNMENT
void SoilLayout::set_alignment(ALIGNMENT a)
{
    alignment = validate_alignment(a);
    set_elm_ui_flag(3);
}
//--------------------------------------------------- RECURSIVE PRINT EXT
void SoilLayout::recursive_print_ext(SoilPixmap* px, SOIL_RECT dest_rect)
{
    
}
//--------------------------------------------------- ADD ELEMENT
bool SoilLayout::add_elm(SoilUIElm* e, int pos)
{
    if(e == NULL){return false;}
    if(e->added){return false;}
    if(e->id == id){return false;}
    if(e->ui_type == LAYOUT)
    {
        if(!check_no_looped_nesting(e)){return false;}
    }
    if(elm_n >= 1000){return false;}


    if(child == NULL)
    {
        child = e;
        last = e;

        e->prev = NULL;
        e->next = NULL;

    }else{

        validate_rel_cursor(elm_n, &pos);

        int p = 0;
        SoilUIElm* next_elm = child;
        SoilUIElm* prev_elm = NULL;
        while(next_elm != NULL && p < pos)
        {
            prev_elm = next_elm;
            next_elm = next_elm->next;
            p++;
        }
        if(prev_elm == NULL)
        {
            child = e;
        }else{
            prev_elm->next = e;
        }
        if(next_elm == NULL)
        {
            last = e;
        }else{
            next_elm->prev = e;
        }
        e->prev = prev_elm;
        e->next = next_elm;

        //last->next = e;
        //e->prev = last;
        //last = e;
        //e->next = NULL;
    }

    e->added = true;
    e->parent = (SoilUIElm*)this;
    elm_n++;

    if(engine_running)
    {
        if(e->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*)e;
            layout->recursive_set_engine_properties(dest_pixmap, arrows_pixmap, 
            pixel_density, engine_control, font, external_control);

            layout->recursive_validate_for_execution(ABSOLUTE_VIEW_MAX_SIZE.w, ABSOLUTE_VIEW_MAX_SIZE.h, 
            ABSOLUTE_VIEW_MIN_SIZE.w, ABSOLUTE_VIEW_MIN_SIZE.h);
        }else{
            e->set_dest_pixmap(dest_pixmap);
            e->set_arrows_pixmap(arrows_pixmap);
            e->set_pixel_density(pixel_density);
            e->set_font(font);
            e->set_engine_control(engine_control);
            e->set_external_control(external_control);

            e->validate_for_execution();
        }

        //printf("\nSoilLayout::ADDING 3");

        set_elm_ui_flag(3);
    }
    return true;
}
//--------------------------------------------------- ADD ELEMENT
bool SoilLayout::add_elm(SoilUIElm* e)
{
    return add_elm(e, -1);
}
//--------------------------------------------------- ADD ELEMENT BEFORE ELEMENT IN LAYOUT
bool SoilLayout::add_elm_before(SoilUIElm* ins_e, SoilUIElm* ref_e)
{
    bool found_ref_e = false;
    SoilUIElm* elm = child;
    int pos = 0;
    while(elm != NULL)
    {
        if(elm == ref_e)
        {
            found_ref_e = true;
            break;
        }
        elm = elm->next;
        pos++;
    }
    if(found_ref_e)
    {
        return add_elm(ins_e, pos);
    }
    return false;
}
//--------------------------------------------------- ADD ELEMENT AFTER ELEMENT IN LAYOUT
bool SoilLayout::add_elm_after(SoilUIElm* ins_e, SoilUIElm* ref_e)
{
    bool found_ref_e = false;
    SoilUIElm* elm = child;
    int pos = 0;
    while(elm != NULL)
    {
        if(elm == ref_e)
        {
            pos++;
            found_ref_e = true;
            break;
        }
        elm = elm->next;
        pos++;
    }
    if(found_ref_e)
    {
        return add_elm(ins_e, pos);        
    }
    return false;
}
//--------------------------------------------------- REMOVE ELEMENT
bool SoilLayout::remove_elm(SoilUIElm* e)
{
    if(e == NULL){return false;}
    if(!e->added){return false;}
    if(e->id == id){return false;}
    
    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        if(elm == e)
        {
            SoilUIElm* prev_elm = elm->prev;
            SoilUIElm* next_elm = elm->next;

            //MARKER:it's probably best to use in_layout ids to
            //locate and manipulate the elm

            if(prev_elm != NULL)
            {
                prev_elm->next = next_elm;
            }
            if(next_elm != NULL)
            {
                next_elm->prev = prev_elm;
            }
            if(elm == child)
            {
                child = next_elm;
            }
            if(elm == last)
            {
                last = prev_elm;
            }

            elm->added = false;
            set_elm_ui_flag(3);
            return true;
        }

        elm = elm->next;
    }
    
    return false;
}

//--------------------------------------------- CHECK NO LOOPED NESTING
bool SoilLayout::check_no_looped_nesting(SoilUIElm* elm)
{

    //MARKER:take this limit off, maybe?
    int limit=2000;
    SoilUIElm* cur_par = parent;
    while(limit > 0 && cur_par != NULL)
    {
        if(cur_par == elm){return false;}

        cur_par = cur_par->parent;
        limit--;
    }
    return true;
}
//--------------------------------------------- SET AS MAIN LAYOUT
void SoilLayout::set_as_main_layout()
{
    is_main_layout = true;
    OUT_PAD = IN_PAD;
    on_window = true;


    win_cut_bound.start_x = 0;
    win_cut_bound.start_y = 0;
    win_cut_bound.end_x = view_size.w;
    win_cut_bound.end_y = view_size.h;

    win_cut_rect = bound_to_rect(win_cut_bound);

    win_pos_bound.start_x = 0;
    win_pos_bound.start_y = 0;
    win_pos_bound.end_x = view_size.w;
    win_pos_bound.end_y = view_size.h;

    win_pos_rect = bound_to_rect(win_pos_bound);

    rel_pos_bound = win_pos_bound;
    rel_pos_rect = bound_to_rect(rel_pos_bound);

    set_elm_ui_flag(3);

}
//--------------------------------------------- SET AS MAIN LAYOUT
void SoilLayout::set_as_modal_layout()
{
    is_modal_layout = true;
    OUT_PAD = IN_PAD;
    on_window = true;


    win_cut_bound.start_x = 0;
    win_cut_bound.start_y = 0;
    win_cut_bound.end_x = view_size.w;
    win_cut_bound.end_y = view_size.h;

    win_cut_rect = bound_to_rect(win_cut_bound);

    win_pos_bound.start_x = 0;
    win_pos_bound.start_y = 0;
    win_pos_bound.end_x = view_size.w;
    win_pos_bound.end_y = view_size.h;

    win_pos_rect = bound_to_rect(win_pos_bound);

    rel_pos_bound = win_pos_bound;
    rel_pos_rect = bound_to_rect(rel_pos_bound);

    set_elm_ui_flag(3);
}
//--------------------------------------------- UPDATE ABSOLUTE MAX SIZES
//this is to be called only once right after the ABSOLUTE MAXIMUM
//size has been set on SoilEngine based on the display size in
//SoilEngine::init_window()
void SoilLayout::recursive_validate_for_execution(int MAX_W,int MAX_H, int MIN_W, int MIN_H)
{


    if(is_main_layout || is_modal_layout)
    {
        parent = NULL;
        ABSOLUTE_VIEW_MIN_SIZE.w = MIN_W;
        ABSOLUTE_VIEW_MIN_SIZE.h = MIN_H;
        ABSOLUTE_VIEW_MAX_SIZE.w = MAX_W;
        ABSOLUTE_VIEW_MAX_SIZE.h = MAX_H;

        set_min_size(view_min_size.w,view_min_size.h);
        set_max_size(view_max_size.w,view_max_size.h);

        //MARKER: if min and max are equal in main_layout
        //then it's USER_RESIZABLE false
    }

    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        if(elm->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*) elm;
            layout->recursive_validate_for_execution(MAX_W,MAX_H,MIN_W,MIN_H);
        }
        if(elm->ui_type == WIDGET)
        {
            elm->validate_for_execution();
        }

        elm = elm->next;
    }
    validate_for_execution();
    auto_elm_count();


}
//--------------------------------------------- SET SIZE
SIZE SoilLayout::set_size(int w, int h)
{
    SIZE new_size = SoilUIElm::set_size(w,h);

    if(is_main_layout || is_modal_layout)
    {

        //update_from_size();
        rel_pos_rect.w = view_size.w;
        rel_pos_rect.h = view_size.h;
        rel_pos_bound = rect_to_bound(rel_pos_rect);
        win_cut_rect.w = view_size.w;
        win_cut_rect.h = view_size.h;
        win_cut_bound = rect_to_bound(win_cut_rect);
        win_pos_rect.w = view_size.w;
        win_pos_rect.h = view_size.h;
        win_pos_bound = rect_to_bound(win_pos_rect);
        abs_pos_rect.w = view_size.w;
        abs_pos_rect.h = view_size.h;
        abs_pos_bound = rect_to_bound(abs_pos_rect);
    }
    return new_size;
}
//--------------------------------------------- GET LAYOUT TYPE
LAYOUT_TYPE SoilLayout::get_layout_type()
{
    return layout_type;
}
//--------------------------------------------- GET ALIGNMENT
ALIGNMENT SoilLayout::get_alignment()
{
    return alignment;
}
//--------------------------------------------- RECURSIVE SET ENGINE PROPERTIES
//this is to be called just once, right in SoilEngine::execute
void SoilLayout::recursive_set_engine_properties(SoilPixmap* d, SoilPixmap *a, int pix_d, SoilEngineControl *eng_c, SoilFont* f, SoilControl* ext_c)
{
    set_engine_properties(d,a,pix_d,eng_c,f,ext_c);
    /*
    set_dest_pixmap(d);
    set_arrows_pixmap(a);
    set_pixel_density(pix_d);
    set_font(f);
    set_engine_control(eng_c);
    set_external_control(ext_c);
*/
    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        if(elm->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*) elm;
            layout->recursive_set_engine_properties(d,a,pix_d,eng_c,f,ext_c);
        }
        if(elm->ui_type == WIDGET)
        {
            elm->set_engine_properties(d,a,pix_d,eng_c,f,ext_c);
            /*
            elm->set_dest_pixmap(d);
            elm->set_arrows_pixmap(a);
            elm->set_pixel_density(pix_d);
            elm->set_font(f);
            elm->set_engine_control(eng_c);
            elm->set_external_control(ext_c);
            */
        }
        elm = elm->next;
    }

}


//--------------------------------------------- AUTO ELEMENTS COUNT
void SoilLayout::auto_elm_count()
{

    //This counts the AUTO_SIZE.h elements for VERTICAL layout
    //and AUTO_SIZE.w elements for HORIZONTAL layout


    auto_elm_n = 0;
    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        if(layout_type == VERTICAL && elm->AUTO_SIZE.h){auto_elm_n++;}
        if(layout_type == HORIZONTAL && elm->AUTO_SIZE.w){auto_elm_n++;}
        elm = elm->next;
    }

}

//--------------------------------------------- UPDATE CONTENT SIZE
void SoilLayout::update_content_size()
{
    if(elm_n == 0)
    {
        set_content_size(0,0);
        return;
    }

    SIZE tmp_content_size;
    tmp_content_size.w = 2 * OUT_PAD;
    tmp_content_size.h = 2 * OUT_PAD;
    SoilUIElm* elm;
    int perp = 0;
    //CALCULATING CONTENT SIZE( VERTICAL )
    if(layout_type == VERTICAL)
    {
        //the following line shouldn't be executed if elm_n is 0
        tmp_content_size.h += (elm_n - 1) * IN_PAD;
        elm = child;
        while(elm != NULL)
        {

            tmp_content_size.h += elm->view_size.h;
            if(elm->view_size.w > perp){perp = elm->view_size.w;}

            elm = elm->next;
        }
        tmp_content_size.w += perp;
    }

    //CALCULATING CONTENT SIZE( HORIZONTAL )
    if(layout_type == HORIZONTAL)
    {
        //the following line shouldn't be executed if elm_n is 0
        tmp_content_size.w += (elm_n - 1) * IN_PAD;
        elm = child;
        while(elm != NULL)
        {
            tmp_content_size.w += elm->view_size.w;
            if(elm->view_size.h > perp){perp = elm->view_size.h;}

            elm = elm->next;
        }
        tmp_content_size.h += perp;
    }
    //set_content_size() calls calc_scrollbar_properties()
    set_content_size(tmp_content_size.w, tmp_content_size.h);
}
//--------------------------------------------- RECURSIVE LOAD AUTO SIZES
//it has the loop for calling load_auto_sizes() in its elements
void SoilLayout::recursive_load_auto_sizes(int free_space_w, int free_space_h,bool do_all)
{
    if(!do_all && elm_ui_flag > 2){do_all = true;}

    if(do_all)
    {
        load_auto_sizes(free_space_w,free_space_h);
        if(is_main_layout || is_modal_layout)
        {
            //MARKER: Why main_layout calls update_from_size()?
            update_from_size();
        }
    }

    SoilUIElm* elm = child;
    SoilLayout* layout;
    while(elm != NULL)
    {
        if(!elm->is_visible())
        {
            elm = elm->next;
            continue;
        }

        if(elm->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*) elm;
            layout->recursive_load_auto_sizes(elm->view_size.w, elm->view_size.h,do_all);
        }
        //printf("\nELM ID:%d  Soil_444", elm->id);
        //printf("\nelm view_size.w:%d",elm->view_size.w);
        //printf("\nelm view_size.h:%d",elm->view_size.h);

        elm = elm->next;
    }

    if(engine_control_is_set)
    {
        if(is_main_layout && engine_control->anchor_elm != NULL && engine_control->hover_elm != NULL)
        {
            layout = (SoilLayout*) engine_control->hover_elm;
            layout->recursive_load_auto_sizes(layout->view_size.w, layout->view_size.h, true);
        }
    }
}
//--------------------------------------------- LOAD AUTO SIZES
//it changes the AUTO elms sizes according to the available space
//it also update the content_size values
void SoilLayout::load_auto_sizes(int free_space_w, int free_space_h)
{

    //SET SoilUIElm::AUTO_SIZE==true elements sizes BASED ON THE LAYOUT SIZE!

    int auto_elm_i = auto_elm_n;
    int e = 0;

    int free_piece = 0;
    int free_rest = 0;
    int free_inc = 0;

    int comp_size = 0;

    //perp is w for vertical layout and h for horizontal
    //it is for the calculating content_size loop
    int perp = 0;

    free_space_w -= 2 * OUT_PAD;//CONTENT INCREMENT
    free_space_h -= 2 * OUT_PAD;//CONTENT INCREMENT

    //content_size.w = 2 * OUT_PAD;
    //content_size.h = 2 * OUT_PAD;


    SoilUIElm* elm;
    int base_w;
    int base_h;
    /////////////////////// VERTICAL
    if(layout_type == VERTICAL)
    {
        elm = child;
        while(elm != NULL)
        {
            
            if(!elm->is_visible())
            {
                elm = elm->next;
                continue;
            }

            base_w = (-1);
            base_h = elm->view_size.h;
            if(e > 0)
            {
                free_space_h -= IN_PAD;//CONTENT INCREMENT
            }
            if(elm->AUTO_SIZE.h)
            {
                base_h = elm->view_min_size.h;
            }
            if(elm->AUTO_SIZE.w)
            {
                base_w = free_space_w;
            }
            free_space_h -= base_h;//CONTENT INCREMENT
            elm->set_auto_calc_size(base_w,base_h);

            elm = elm->next;
            e++;
        }

        while(auto_elm_i > 0 && free_space_h > 0)
        {

            free_piece = free_space_h / auto_elm_i;
            free_rest = free_space_h % auto_elm_i;
            free_inc = 0;
            elm = child;

            while(elm != NULL)
            {
                
                if(!elm->is_visible())
                {
                    elm = elm->next;
                    continue;
                }

                if(elm->AUTO_SIZE.h)
                {
                    comp_size = elm->view_max_size.h - elm->auto_calc_size.h;
                    if(comp_size > 0)
                    {
                        if(free_rest > 0)
                        {
                            free_inc = free_piece + 1;
                            free_rest--;
                        }else{
                            free_inc = free_piece;
                        }

                        if(free_inc < comp_size)
                        {
                            elm->set_auto_calc_size(-1,elm->auto_calc_size.h + free_inc);
                            free_space_h -= free_inc;//CONTENT INCREMENT
                        }else{
                            elm->set_auto_calc_size(-1,elm->view_max_size.h);
                            free_space_h -= comp_size;//CONTENT INCREMENT
                            auto_elm_i--;

                            if(auto_elm_i == 0){break;}
                        }
                        if(free_space_h <= 0){break;}

                    }else{
                        //non variant AUTO elm
                        auto_elm_i--;
                    }
                }
                elm = elm->next;
            }
        }//while distributing free space loop

    }// end if vertical

    //HORIZONTAL
    if(layout_type == HORIZONTAL)
    {
        elm = child;
        while(elm != NULL)
        {
            
            if(!elm->is_visible())
            {
                elm = elm->next;
                continue;
            }

            base_h = (-1);
            base_w = elm->view_size.w;
            if(e > 0)
            {
                free_space_w -= IN_PAD;
            }
            if(elm->AUTO_SIZE.w)
            {

                base_w = elm->view_min_size.w;
            }
            if(elm->AUTO_SIZE.h)
            {
                base_h = free_space_h;
            }
            free_space_w -= base_w;
            elm->set_auto_calc_size(base_w,base_h);

            elm = elm->next;
            e++;
        }


        while(auto_elm_i > 0 && free_space_w> 0)
        {
            free_piece = free_space_w / auto_elm_i;
            free_rest = free_space_w % auto_elm_i;
            free_inc = 0;
            elm = child;

            while(elm != NULL)
            {
                
                if(!elm->is_visible())
                {
                    elm = elm->next;
                    continue;
                }
                if(elm->AUTO_SIZE.w)
                {
                    comp_size = elm->view_max_size.w - elm->auto_calc_size.w;
                    if(comp_size > 0)
                    {
                        if(free_rest > 0)
                        {
                            free_inc = free_piece + 1;
                            free_rest--;
                        }else{
                            free_inc = free_piece;
                        }
                        if(free_inc < comp_size)
                        {
                            elm->set_auto_calc_size(elm->auto_calc_size.w + free_inc,-1);
                            free_space_w -= free_inc;
                        }else{
                            elm->set_auto_calc_size(elm->view_max_size.w,-1);
                            free_space_w -= comp_size;
                            auto_elm_i--;

                            if(auto_elm_i == 0){break;}
                        }
                        if(free_space_w <= 0){break;}
                    }else{
                        //non variant AUTO elm
                        auto_elm_i--;
                    }
                }
                elm = elm->next;
            }
        }
    }// end if horizontal

    //UPDATING THE PROPERTIES DERIVED FROM VIEW_SIZE AND CALLING
    //SET SIZE FOR THE INHERITING CLASSES SUCH AS WIDGETS
    elm = child;
    while(elm != NULL)
    {
        //ui flags are being set in elm->set_size()
        //but are unset on SoilWidget::print() and SoilEngine::reload_ui
        elm->set_size(elm->auto_calc_size.w,elm->auto_calc_size.h);
        //elm->update_from_size();
        elm = elm->next;
    }

    update_content_size();

}

//--------------------------------------------- RECURSIVE SET POSITIONS
void SoilLayout::recursive_set_positions(bool do_all)
{
    if(!do_all && elm_ui_flag > 2){do_all = true;}

    if(do_all){set_positions();}

    SoilUIElm* elm = child;
    SoilLayout* layout;
    while(elm != NULL)
    {
        
        if(!elm->is_visible())
        {
            elm = elm->next;
            continue;
        }

        if(elm->ui_type == LAYOUT)
        {
            layout = (SoilLayout*) elm;
            layout->recursive_set_positions(do_all);
        }
        elm = elm->next;
    }

    if(engine_control_is_set)
    {
        if(is_main_layout && engine_control->anchor_elm != NULL && engine_control->hover_elm != NULL)
        {
            layout = (SoilLayout*) engine_control->hover_elm;
            layout->recursive_set_positions(true);
        }
    }


}
//--------------------------------------------- SET POSITIONS
void SoilLayout::set_positions()
{
    SoilUIElm* elm = child;
    SoilLayout* layout;

    LAYOUT_TYPE T = layout_type;
    ALIGNMENT A = alignment;

    POINT next_pos;
    next_pos.x = OUT_PAD;
    next_pos.y = OUT_PAD;

    //perpendicular content size
    int perp;
    if(T == VERTICAL)
    {
        perp = content_view_rect.w;

    }
    if(T == HORIZONTAL)
    {
        perp = content_view_rect.h;

    }
    int half_perp = perp / 2;

    while(elm != NULL)
    {
        
        if(!elm->is_visible())
        {
            elm = elm->next;
            continue;
        }

        //orientation of THIS layout
        if(T == VERTICAL)
        {
            if(A==CENTER){next_pos.x = half_perp - elm->view_size.w/2;}
            if(A==LEFT){next_pos.x = OUT_PAD;}
            if(A==RIGHT){next_pos.x = perp - (elm->view_size.w + OUT_PAD);}

            if(next_pos.x < OUT_PAD){next_pos.x = OUT_PAD;}
        }
        if(T == HORIZONTAL)
        {
            if(A==CENTER){next_pos.y = half_perp - elm->view_size.h/2;}
            if(A==TOP){next_pos.y = OUT_PAD;}
            if(A==BOTTOM){next_pos.y = perp - (elm->view_size.h + OUT_PAD);}

            if(next_pos.y < OUT_PAD){next_pos.y = OUT_PAD;}
        }

        elm->rel_pos_rect.x = next_pos.x;
        elm->rel_pos_rect.y = next_pos.y;

        elm->rel_pos_rect.w = elm->view_size.w;
        elm->rel_pos_rect.h = elm->view_size.h;


        elm->rel_pos_bound = rect_to_bound(elm->rel_pos_rect);

        if(T==VERTICAL){next_pos.y += IN_PAD + elm->view_size.h;}
        if(T==HORIZONTAL){next_pos.x += IN_PAD + elm->view_size.w;}

        elm = elm->next;
    }

}

//--------------------------------------------- RECURSIVE SET ELEMENTS WINDOW AREA
void SoilLayout::recursive_set_elm_window_area(bool do_all)
{
    if(!do_all && elm_ui_flag > 1){do_all = true;}

    if(is_main_layout && engine_control->hover_elm != NULL){do_all = true;}

    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        
        if(!elm->is_visible())
        {
            elm->on_window = false;
            elm = elm->next;
            continue;
        }
        if(do_all){set_elm_window_area(elm);}

        if(elm->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*) elm;
            layout->recursive_set_elm_window_area(do_all);
        }

        elm = elm->next;
    }


}
//--------------------------------------------- SET ELEMENTS WINDOW AREA
void SoilLayout::set_elm_window_area(SoilUIElm* elm)
{
    //printf("\nSoilLayout::SETTING AREA **************** ID: %d ",id );
    if(is_main_layout)
    {
        //printf("\nSoilLayout:: MAIN LAYOUT");        
    }
    

    //This method calculates the area the SOIL_RECT that represents
    //the area its visible part occupies on the WINDOW (win_pos_rect)
    //and the SOIL_RECT that such area occupies RELATIVE TO ITS OWN
    //view_rect(win_cut_rect)

    BOUND_RECT WCB = win_cut_bound;
    SOIL_RECT WCR = win_cut_rect;
    BOUND_RECT WPB = win_pos_bound;
    BOUND_RECT IB;//intersection_bounds
    BOUND_RECT AB;//abs pos

    BOUND_RECT CVB = content_view_bound;
    SOIL_RECT CVR = content_view_rect;

    //trim the scrollbar portion of the view area
    if(WCB.end_x > CVR.w){WCB.end_x = CVR.w;}
    if(WCB.end_y > CVR.h){WCB.end_y = CVR.h;}

    BOUND_RECT WCB_plus = add_to_bound_pos(WCB, CVB.start_x,CVB.start_y);
    IB = bound_intersection(WCB_plus, elm->rel_pos_bound);


    //abs_pos
    AB = elm->rel_pos_bound;
    AB = subtract_from_bound_pos(AB, CVB.start_x, CVB.start_y);
    AB = add_to_bound_pos(AB, abs_pos_bound.start_x, abs_pos_bound.start_y);

    elm->abs_pos_bound = AB;
    elm->abs_pos_rect = bound_to_rect(AB);

    if(IB.start_x == IB.end_x || IB.start_y == IB.end_y)
    {
        elm->on_window = false;
        
        reset_bound(&elm->win_cut_bound);
        reset_rect(&elm->win_cut_rect);
        reset_bound(&elm->win_pos_bound);
        reset_rect(&elm->win_pos_rect);
        
    }else{
        elm->on_window = true;

        //win_cut_bound/rect
        elm->win_cut_bound = subtract_from_bound_pos(IB,elm->rel_pos_bound.start_x,elm->rel_pos_bound.start_y);
        elm->win_cut_rect = bound_to_rect(elm->win_cut_bound);
        //win_pos_bound/rect
        IB = subtract_from_bound_pos(IB, CVB.start_x,CVB.start_y);
        IB = add_to_bound_pos(IB, WPB.start_x, WPB.start_y);
        IB = subtract_from_bound_pos(IB, WCB.start_x, WCB.start_y);
        elm->win_pos_bound = IB;
        elm->win_pos_rect = bound_to_rect(elm->win_pos_bound);

    }//else on_window

    int elm_id = elm->get_id();

    if(engine_control_is_set)
    {
        if(engine_control->anchor_elm != NULL && engine_control->anchor_elm == elm && engine_control->hover_elm != NULL)
        {
            SoilUIElm* anchor_elm_pt = engine_control->anchor_elm;
            SoilUIElm* hover_elm_pt = engine_control->hover_elm;
            if(anchor_elm_pt != hover_elm_pt)
            {
                SIZE win_size = engine_control->get_window_size();
                SOIL_RECT win_rect;
                win_rect.x = 0;
                win_rect.y = 0;
                win_rect.w = win_size.w;
                win_rect.h = win_size.h;

                //-------------- WIN POS
                //anchor_rect is the total anchor area relative to the window                    
                hover_elm_pt->abs_pos_rect.x = anchor_elm_pt->abs_pos_rect.x;
                hover_elm_pt->abs_pos_rect.y = anchor_elm_pt->abs_pos_rect.y + anchor_elm_pt->view_size.h;
                hover_elm_pt->abs_pos_rect.w = hover_elm_pt->view_size.w;
                hover_elm_pt->abs_pos_rect.h = hover_elm_pt->view_size.h;

                hover_elm_pt->rel_pos_rect.x = 0;
                hover_elm_pt->rel_pos_rect.y = 0;
                hover_elm_pt->rel_pos_rect.w = hover_elm_pt->view_size.w;
                hover_elm_pt->rel_pos_rect.h = hover_elm_pt->view_size.h;

                hover_elm_pt->win_pos_rect = rect_intersection(hover_elm_pt->abs_pos_rect, win_rect);
                hover_elm_pt->win_cut_rect = hover_elm_pt->win_pos_rect;

                hover_elm_pt->win_cut_rect.x = 0;
                hover_elm_pt->win_cut_rect.y = 0;

                if(hover_elm_pt->abs_pos_rect.x < 0){hover_elm_pt->win_cut_rect.x -= hover_elm_pt->abs_pos_rect.x;}
                if(hover_elm_pt->abs_pos_rect.y < 0){hover_elm_pt->win_cut_rect.y -= hover_elm_pt->abs_pos_rect.y;}

                hover_elm_pt->win_pos_bound = rect_to_bound(hover_elm_pt->win_pos_rect);
                hover_elm_pt->win_cut_bound = rect_to_bound(hover_elm_pt->win_cut_rect);
                hover_elm_pt->abs_pos_bound = rect_to_bound(hover_elm_pt->abs_pos_rect);
                hover_elm_pt->rel_pos_bound = rect_to_bound(hover_elm_pt->rel_pos_rect);

                if(hover_elm_pt->win_pos_bound.start_x == hover_elm_pt->win_pos_bound.end_x || hover_elm_pt->win_pos_bound.start_y == hover_elm_pt->win_pos_bound.end_y)
                {
                    hover_elm_pt->on_window = false;
                }else{
                    hover_elm_pt->on_window = true;
                }

                if(hover_elm_pt->ui_type == LAYOUT)
                {

                    //printf("\nSoilLayout::hover_elm set elm window area");
                    
                    SoilLayout* layout = (SoilLayout*) hover_elm_pt;
                    layout->recursive_set_elm_window_area(true);
                }
            
                hover_elm_pt->print_elm_area();
            }            
        }
    }//engine control is set

    //elm->print_elm_area();

    //printf("\nSoilLayout::SETTING AREA ********  END  ******* ID: %d ",id );
    

}

//--------------------------------------------- RECURSIVE PRINT
void SoilLayout::recursive_print(bool do_all)
{
    if(!do_all && elm_ui_flag > 0){do_all = true;}

    if(do_all)
    {
        dest_pixmap->spx_draw_solid_rect(win_pos_rect,0xFF000000);
        //dest_pixmap->spx_draw_outline_rect(win_pos_rect, 2, 0xFF554499);
    }

    int i = 0;

    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        
        if(!elm->is_visible())
        {
            elm = elm->next;
            continue;
        }
        if(elm->ui_type == LAYOUT)
        {
            //MARKER:Check if it is really necessary for SoilUIElm to have recursive_print
            //after all apparently hover_elm is supposed to be a SoilLayout
            elm->recursive_print(do_all);
        }else{
            if(do_all || elm->elm_ui_flag > 0)
            {
                elm->recursive_print(true);
            }
        }
        elm->ui_reloaded();

        elm = elm->next;
    }
    ///////////////////////////////// PRINT THIS ELM HERE
    print();

    if(is_main_layout && engine_control_is_set)
    {
        if(engine_control->hover_elm != NULL)
        {
            //engine_control->hover_elm->print_hover_elm_virt();
            engine_control->hover_elm->recursive_print(true);
        }
    }

}
//--------------------------------------------- PRINT
void SoilLayout::print()
{
    if(is_main_layout)
    {
        //recursive_print_elm_area();
    }

    if(scrollable && on_window)
    {

        print_scrollbars();
    }
    ///////////////////////// RESETING THE LAYOUT ELM_UI_FLAG
    elm_ui_flag = 0;


    SOIL_RECT cropped_rect;
    cropped_rect.x = win_pos_rect.x;
    cropped_rect.y = win_pos_rect.y;
    cropped_rect.w = win_pos_rect.w-2;
    cropped_rect.h = win_pos_rect.h-2;

}

//--------------------------------------------- RECURSIVE GET MOUSE EVENT
void SoilLayout::recursive_process_mouse_event(int X, int Y, int in_view_x, int in_view_y, SDL_Event E)
{
    //when mouse_up event is triggered outside the window
    //it gets the coord for the mouse cursor but limited to the window
    //size. In which case the element that receives the mouse_up event
    //outside the window is the main_layout
    SoilUIElm* elm = child;
    bool clicked_in_elm = false;
    bool event_taken = false;

    int elm_x;
    int elm_y;

    if(is_main_layout && engine_control_is_set)
    {
        if(engine_control->hover_elm != NULL)
        {
            SoilLayout* hover_layout = (SoilLayout*)engine_control->hover_elm;
            if(point_in_rect(hover_layout->win_pos_rect, X, Y))
            {
                hover_layout->convert_win_to_view_coord(X,Y,&elm_x,&elm_y);
                hover_layout->recursive_process_mouse_event(X,Y,elm_x,elm_y,E);
                event_taken = true;
                clicked_in_elm = true;
            }
        }
    }


    while(elm != NULL && !event_taken)
    {
        if(elm->on_window)
        {
            elm->convert_win_to_view_coord(X,Y,&elm_x,&elm_y);
            if(point_in_rect(elm->win_pos_rect,X,Y))
            {
                
                clicked_in_elm = true;
                event_taken = true;

                if(elm->ui_type == LAYOUT)
                {
                    SoilLayout* layout = (SoilLayout*) elm;
                    layout->recursive_process_mouse_event(X,Y,elm_x,elm_y,E);
                }
                if(elm->ui_type == WIDGET)
                {
                    elm->process_mouse_event(X,Y,elm_x,elm_y,E);
                }
            }
        }
        elm = elm->next;
    }



    //------------------------------- CLICK ON SCROLLBAR
    if(!clicked_in_elm)
    {
        SoilUIElm::process_mouse_event(X,Y,in_view_x,in_view_y,E);
    }else{
        //any mouse event that must be processed not only for the final
        //elm it lands on but also the layouts it is in

    }
}
//--------------------------------------------- RECURSIVE PRINT ELM AREA
void SoilLayout::recursive_print_elm_area()
{
    printf("\nSOILLAYOUT::CALC ELM AREA ------------------------------LAYOUT ID: %d\n",id);
    print_elm_area();
    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        if(elm->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*) elm;
            layout->recursive_print_elm_area();
        }else{
            elm->print_elm_area();
        }
        elm = elm->next;
    }
    printf("\n-------------------------\n");
}

//--------------------------------------------- RECURSIVE PRINT LOG
void SoilLayout::recursive_print_log(bool first_to_print)
{


    printf("\n");
    printf("\n%s>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",log_nest.c_str());
    printf("\n%sLAYOUT:: RECURSIVE PRINTLOG ID:%d  ",log_nest.c_str(),id);
    printf("\n%s>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",log_nest.c_str());

    if(is_main_layout)
    {
        printf("\n%s*****************************",log_nest.c_str());
        printf("\n%s******* MAIN_LAYOUT *********",log_nest.c_str());
        printf("\n%s*****************************",log_nest.c_str());


    }

    if(first_to_print){print_log();}



    SoilUIElm* elm = child;
    while(elm != NULL)
    {
        elm->print_log();
        if(elm->ui_type == LAYOUT)
        {
            SoilLayout* layout = (SoilLayout*) elm;
            layout->recursive_print_log(false);
        }


        elm = elm->next;
    }
    printf("\n");
    printf("\n%s------------------------------------------",log_nest.c_str());
    printf("\n%s(END) LAYOUT:: RECURSIVE PRINTLOG ID:%d ",log_nest.c_str(),id);
    printf("\n%s------------------------------------------",log_nest.c_str());

}
