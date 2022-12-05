#include "soilwidget.h"
SoilWidget::SoilWidget(WIDGET_TYPE wt) : SoilUIElm(WIDGET)
{

    widget_type = wt;
    //view_size.w = 40;
    //view_size.h = 40;
    default_scrollbar_system = true;

    pixmap.set_dynamic_bound(true, 30, 30);
}
//--------------------------------------------- DRAW INNER BORDER
void SoilWidget::draw_inner_border(Uint32 color, int padding)
{
    if(!pixmap.loaded){return;}
    int full_padding = 2*padding;
    SOIL_RECT border_rect;
    border_rect.x = padding;
    border_rect.y = padding;
    border_rect.w = view_size.w - (full_padding);
    border_rect.h = view_size.h - (full_padding);

    //MARKER:make sure there is no problem the spx_draw_rect rect be bigger than the pixmap size
    pixmap.spx_draw_outline_rect(border_rect, 1, color);
    set_elm_ui_flag(1);
}
//--------------------------------------------- RECURSIVE PRINT
void SoilWidget::recursive_print(bool do_all)
{
    print();
}
//--------------------------------------------- PRINT
void SoilWidget::print()
{
    elm_ui_flag = 0;

    if(!on_window){return;}
    if(!dest_pixmap_is_set){return;}
    if(!pixmap.loaded){return;}
    //if(!on_window){return;}
    SOIL_RECT copy_rect = win_cut_rect;
    //This is for the widgets that have a pixmap to paste to dest_pixmap

    if(default_scrollbar_system)
    {
        //it means content is displayed in the pixmap and content_view_rect tells the offset of the view
        copy_rect = add_to_rect_pos(copy_rect,content_view_rect.x,content_view_rect.y);
    }
    //MARKER: I need to subtract when the view goes outside of the content
    //meaning CONTENT_VIEW_RECT.X/Y is NEGATIVE
    //and when it gets to 0 I subtract the w/h and the X/Y in win_pos_rect
    dest_pixmap->spx_paste_pixmap(pixmap,copy_rect,win_pos_rect);
    if(engine_control_is_set)
    {
        if(engine_control->focus_elm_id == id)
        {
            dest_pixmap->spx_draw_patch_rect(win_pos_rect, 0xFFFFFFFF);
        }else{
            SOIL_RECT erase_rect = win_pos_rect;
            erase_rect.w += 2;
            erase_rect.h += 1;
            //dest_pixmap->spx_draw_patch_rect(win_pos_rect, 0xFFFF0000);
        }
        //dest_pixmap->spx_draw_patch_rect(win_pos_rect, 0xFFC0C000);
    }
    

    //AROUND ELM BORDER
    
    SOIL_RECT cont_view;
    cont_view.x = win_pos_rect.x - 2;
    cont_view.y = win_pos_rect.y - 2;
    cont_view.w = win_pos_rect.w + 4;
    cont_view.h = win_pos_rect.h + 4;
    //dest_pixmap->spx_draw_patch_rect(cont_view, 0xFFF00000);
    



    //The scrollable system basic works setting a position offset
    //for content_view_rect relative to content_size
    //if default_scrollbar_system is true, content_view_rect is the rect taken from pixmap to paste to dest_pixmap
    if(scrollable)
    {
        print_scrollbars();
    }
    elm_ui_flag = 0;

    //draw_patch_rect(dest_pixmap->color_px_pt,dest_pixmap->size,win_pos_rect,0xFFFF0000);
}