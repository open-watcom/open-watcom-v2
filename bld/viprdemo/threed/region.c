/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 Description:
 ============
    Pick 'n poke region stuff.


*/

#ifdef PLAT_OS2
#define INCL_PM
#include <os2.h>
#else
#include <windows.h>
#endif

#ifndef PROD
#define  TRACK_MEM
#endif

#include "mem.h"

#include "wpi.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "region.h"
#include "cgrids.h"


#define MAG_OVERFLOW                    100000
#define BLOCK_SIZE                      100
#define MARK_SIZE                       3
#define VECTOR_FUZZ                     3
#define VECTOR_COEF                     100.0
#define LINE_ZONE_SIZE                  3
#define PI                              3.14159265
#define COLL_BLOCKS                     10
#define LIST_BLOCKS                     10
#define RGN_ELLIPSE_BWIDTH              3

#define _get_offset( top, ptr ) ((unsigned short) \
                                ((char *)(ptr) - (char *) top))
#define _ptr_add( ptr, amnt ) ((void *)((char *)(ptr) + (amnt)) )


enum {
    RGN_STATE_OFF,
    RGN_STATE_ON,
    RGN_STATE_DISABLED
};

enum {
    RGN_REGULAR,
    RGN_GROUP,
    RGN_SET,
    RGN_MAN_GROUP,
    RGN_MAN_SET
};

static rgn_def far                 *Region_list;
static unsigned short               Region_size;
static unsigned short               Region_used_size;
static int                          Curr_state;

static int                          Set_state;
static rgn_set_coll                 Set_coll;       // collection of sets
static int                          Set_curr;

static int                          Major_id;
static int                          Minor_id;
static int                          Data_row;
static int                          Data_col;

#define sqr( xxx )      ((long)(xxx) * (long)(xxx))
#define sgn( xxx )      (((xxx) == 0) ? 0 : (xxx) / abs( xxx ))

static BOOL is_pt_in_rect(
/************************/
    WPI_RECT            *rect,
    WPI_POINT           pt
) {
    int                 left, right, top, bottom;

    _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
    return( pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom  );
}

void rgn_off(
/***********/

) {
    Curr_state = RGN_STATE_OFF;
}

void rgn_on(
/**********/

) {
    Curr_state = RGN_STATE_ON;
}

int rgn_disable(
/**************/

) {
    int         old_state;

    old_state = Curr_state;
    if( Curr_state != RGN_STATE_OFF ) {
        Curr_state = RGN_STATE_DISABLED;
    }

    return( old_state );
}

void rgn_enable(
/**************/

    int                 state
) {
    if( Curr_state != RGN_STATE_OFF ) {
        Curr_state = state;
    }
}

static int get_new_rgn_set(
/*************************/
    void
) {
    rgn_set_def **  old_sets;
    int             set_num;

    if( Set_coll.coll == NULL ) {
        _new( Set_coll.coll, COLL_BLOCKS );
        Set_coll.num_sets = COLL_BLOCKS;
        Set_coll.num_used = 0;
        for( set_num = 0; set_num < COLL_BLOCKS; set_num++ ) {
            Set_coll.coll[ set_num ] = NULL;
        }
    }

    if( Set_coll.num_used >= Set_coll.num_sets ) {
        old_sets = Set_coll.coll;
        _renew( Set_coll.coll, Set_coll.num_sets + COLL_BLOCKS );
        if( Set_coll.coll == NULL ) {
            Set_coll.coll = old_sets;
            return( -1 );
        } else {
            for( set_num = Set_coll.num_sets; set_num < Set_coll.num_sets +
                            COLL_BLOCKS; set_num++ ) {
                Set_coll.coll[ set_num ] = NULL;
            }
            Set_coll.num_sets += COLL_BLOCKS;
        }
    }

    Set_coll.num_used++;

    return( Set_coll.num_used - 1 );
}

static rgn_set_def * get_rgn_set_ptr(
/***********************************/
    int             set_num
) {
    rgn_set_def **  coll;

    if( set_num < Set_coll.num_used ) {
        coll = Set_coll.coll;
        if( coll[ set_num ] == NULL ) {
            coll[ set_num ] = _galloc( sizeof(rgn_set_header_def) +
                        LIST_BLOCKS * sizeof(rgn_set_list_def) );
            if( coll[ set_num ] != NULL ) {
                coll[ set_num ]->info.num_items = LIST_BLOCKS;
                coll[ set_num ]->info.num_used = 0;
            }
        }
        return( coll[ set_num ] );
    } else {
        return( NULL );
    }
}

static void set_rgn_set_ptr(
/**************************/
    int                 set_num,
    rgn_set_def *       curr
) {
    if( set_num < Set_coll.num_used ) {
        Set_coll.coll[ set_num ] = curr;
    }
}

static void add_rgn_def(
/**********************/
    int                 set_num,
    rgn_set_list_def *  new_def
) {
    rgn_set_def *       curr;

    curr = get_rgn_set_ptr( set_num );
    if( curr != NULL ) {
        if( curr->info.num_used >= curr->info.num_items ) {
            _grenew( curr, curr->info.num_items + LIST_BLOCKS );
            if( curr == NULL ) {
                return;
            }
            set_rgn_set_ptr( set_num, curr );
            curr->info.num_items += LIST_BLOCKS;
        }
        curr->list[ curr->info.num_used ] = *new_def;
        curr->info.num_used++;
    }
}

void rgn_begin_group(
/******************/
) {
    rgn_set_def far        *curr;

    if ((Curr_state == RGN_STATE_ON) && (Set_state == RGN_REGULAR)) {
        Set_curr = get_new_rgn_set();
        if( Set_curr == -1 ) {
            return;
        }

        Set_state = RGN_GROUP;

        curr = get_rgn_set_ptr( Set_curr );
        curr->info.exact = FALSE;
    }
}

static void rgn_group_add(
/************************/
) {
    rgn_set_list_def    curr;

    curr.offset = Region_used_size;
    add_rgn_def( Set_curr, &curr );
}

void rgn_end_group(
/*****************/
) {
    if ((Curr_state == RGN_STATE_ON) && (Set_state == RGN_GROUP)) {
        Set_state = RGN_REGULAR;
    }
}

void rgn_begin_set(
/*****************/
) {
    rgn_set_def far        *curr;

    if ((Curr_state == RGN_STATE_ON) && (Set_state == RGN_REGULAR)) {
        Set_curr = get_new_rgn_set();
        if( Set_curr == -1 ) {
            return;
        }

        Set_state = RGN_SET;

        curr = get_rgn_set_ptr( Set_curr );
        curr->info.exact = TRUE;
    }
}

void rgn_set_add(
/***************/
    WPI_POINT               *pt
) {
    rgn_set_list_def        curr;

    if ((Curr_state == RGN_STATE_ON) &&
                (Set_state == RGN_SET || Set_state == RGN_MAN_SET)) {
        curr.pt = *pt;
        add_rgn_def( Set_curr, &curr );
    }
}

void rgn_end_set(
/***************/
) {
    if ((Curr_state == RGN_STATE_ON) && (Set_state == RGN_SET)) {
        Set_state = RGN_REGULAR;
    }
}

extern int rgn_man_group_new(
/***************************/
    void
) {
    int                 group_num;
    rgn_set_def far     *curr;

    if( Curr_state == RGN_STATE_ON ) {
        group_num = get_new_rgn_set();
        curr = get_rgn_set_ptr( group_num );
        curr->info.exact = FALSE;
    } else {
        group_num = -1;
    }

    return( group_num );
}

extern void rgn_man_group_begin(
/******************************/
    int     group_num
) {
    if( (Curr_state == RGN_STATE_ON) && (Set_state == RGN_REGULAR) ) {
        if( 0 <= group_num && group_num < Set_coll.num_used ) {
            Set_state = RGN_MAN_GROUP;
            Set_curr = group_num;
        }
    }
}

extern void rgn_man_group_end(
/****************************/
    void
) {
    if( (Curr_state == RGN_STATE_ON) && (Set_state == RGN_MAN_GROUP) ) {
        Set_state = RGN_REGULAR;
    }
}

extern int rgn_man_set_new(
/*************************/
    void
) {
    int                 group_num;
    rgn_set_def far     *curr;

    if( Curr_state == RGN_STATE_ON ) {
        group_num = get_new_rgn_set();
        curr = get_rgn_set_ptr( group_num );
        curr->info.exact = TRUE;
    } else {
        group_num = -1;
    }

    return( group_num );
}

extern void rgn_man_set_begin(
/****************************/
    int     set_num
) {
    if( (Curr_state == RGN_STATE_ON) && (Set_state == RGN_REGULAR) ) {
        if( 0 <= set_num && set_num < Set_coll.num_used ) {
            Set_state = RGN_MAN_SET;
            Set_curr = set_num;
        }
    }
}

extern void rgn_man_set_end(
/**************************/
    void
) {
    if( (Curr_state == RGN_STATE_ON) && (Set_state == RGN_MAN_SET) ) {
        Set_state = RGN_REGULAR;
    }
}

void rgn_begin(
/*************/

) {
    if( Curr_state != RGN_STATE_OFF ) {
        Region_size = 0;
        Region_used_size = 0;
        Region_list = NULL;

        Set_state = RGN_REGULAR;
        Set_coll.num_sets = 0;
        Set_coll.num_used = 0;
        Set_coll.coll = NULL;
        Set_curr = 0;
    }
}

static long get_set_coll_size(
/****************************/
    void
) {
    long            size;
    rgn_set_def **  curr_set;

    /* room for the rgn_set_index */
    size = sizeof(rgn_set_coll_header) + Set_coll.num_used * sizeof(short);

    for( curr_set = Set_coll.coll; curr_set < Set_coll.coll +
            Set_coll.num_used; curr_set++ ) {
        size += sizeof(rgn_set_header_def) +
                    (**curr_set).info.num_used * sizeof(rgn_set_list_def);
    }

    return( size );
}

static char * copy_set_coll(
/**************************/
    char *          ptr
) {
    rgn_set_index * index;
    rgn_set_def *   curr_set;
    int             set_num;
    int             set_size;

    index = (rgn_set_index *) ptr;
    index->info.num_sets = Set_coll.num_used;
    ptr += sizeof(rgn_set_coll_header) + Set_coll.num_used * sizeof(short);

    for( set_num = 0; set_num < Set_coll.num_used; set_num++ ) {
        index->offset[ set_num ] = _get_offset( index, ptr );
        curr_set = Set_coll.coll[ set_num ];
        set_size = sizeof(rgn_set_header_def) +
                    curr_set->info.num_used * sizeof(rgn_set_list_def);
        memcpy( ptr, curr_set, set_size );
        ptr += set_size;
    }

    return( ptr );
}

static void free_set_coll(
/************************/
    void
) {
    rgn_set_def **      curr_set;

    if( Set_coll.coll != NULL ) {
        for( curr_set = Set_coll.coll; curr_set < Set_coll.coll +
                Set_coll.num_used; curr_set++ ) {
            if( *curr_set != NULL ) {
                _gfree( *curr_set );
            }
        }
        _free( Set_coll.coll );
    }

    Set_coll.num_sets = 0;
    Set_coll.num_used = 0;
    Set_coll.coll = NULL;
}


HANDLE rgn_end(
/*************/
    void
) {
    HANDLE          hld;
    HANDLE          rgn_hld;
    rgn_tag_def *   tag;
    char far *      ptr;
    char far *      src;
    long            size;

    if( Curr_state != RGN_STATE_OFF && Region_list != NULL ) {
        /* allocate the contigous block of memory */
        size = sizeof(rgn_tag_def) + get_set_coll_size() + Region_used_size;
        hld = GlobalAlloc( GMEM_MOVEABLE, size );

        /* copy the pieces */
        if( hld != NULL ) {
            ptr = GlobalLock( hld );
            tag = (rgn_tag_def *) ptr;
            ptr += sizeof(rgn_tag_def);
            tag->set_coll_offset = _get_offset( tag, ptr );
            ptr = (char far *) copy_set_coll( (char *) ptr );
            tag->rgn_offset = _get_offset( tag, ptr );
            tag->rgn_size = Region_used_size;
            src = _ptr_add( Region_list, Region_size - Region_used_size );
            _fmemcpy( ptr, src, Region_used_size );
            GlobalUnlock( hld );
        }

        /* free up unneed memory */
        rgn_hld = (HANDLE) _wpi_getglobalhdl( (DWORD) Region_list );
        GlobalUnlock( rgn_hld );
        GlobalFree( rgn_hld );
        Region_list = NULL;
        free_set_coll();
    } else {
        hld = NULL;
    }

    return( hld );
}

static rgn_def far * get_rgn_list_ptr(
/************************************/
    int                 size
) {
    HANDLE                  hld;
    rgn_def far            *ret;
    rgn_def far            *src;
    rgn_def far            *dst;
    rgn_def far            *ptr;

    if (Region_list == NULL) {
        Region_size = BLOCK_SIZE;
        hld = GlobalAlloc( GMEM_MOVEABLE, (Region_size + sizeof(short)) );
        Region_list = (rgn_def far *)GlobalLock( hld );
    }

    if (Region_size < Region_used_size + size) {
        src = (rgn_def far *)((long)Region_list + Region_size -
                                                        Region_used_size);
        if (size > BLOCK_SIZE) {
            Region_size += size;
        } else {
            Region_size += BLOCK_SIZE;
        }
        hld = GlobalAlloc( GMEM_MOVEABLE, (Region_size + sizeof(short)) );
        ptr = (rgn_def far *)GlobalLock( hld );
        dst = (rgn_def far *)((long)ptr + Region_size - Region_used_size);
        _fmemmove( dst, src, Region_used_size );
        GlobalUnlock( _wpi_getglobalhdl( Region_list ) );
        GlobalFree( _wpi_getglobalhdl( Region_list ) );
        Region_list = ptr;
    }

    Region_used_size += size;
    ret = (rgn_def far *)((long)Region_list + Region_size - Region_used_size);

    return( ret );
}

BOOL rgn_is_on(
/*************/

) {
    return( Curr_state == RGN_STATE_ON );
}

void rgn_set_ids(
/***************/

    int                 major_id,
    int                 minor_id,
    int                 data_row,
    int                 data_col
) {
    Major_id = major_id;
    Minor_id = minor_id;
    Data_row = data_row;
    Data_col = data_col;
}

static void get_rect(
/*******************/
    WPI_POINT far       *pts,
    int                 num_pts,
    WPI_RECT far        *rect
) {
    int                 count;
    int                 left, right, top, bottom;

    _wpi_setintrectvalues( (WPI_RECT *) rect,
                        pts[0].x, pts[0].y, pts[0].x, pts[0].y );

    for (count = 1; count < num_pts; count++) {
        _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
        if (pts[count].x < left) {
            _wpi_setintrectvalues( (WPI_RECT *) rect,
                        pts[count].x, top, right, bottom );
            _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
        }
        if (pts[count].x > right) {
            _wpi_setintrectvalues( (WPI_RECT *) rect,
                        left, top, pts[count].x, bottom );
            _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
        }
        if (pts[count].y < top) {
            _wpi_setintrectvalues( (WPI_RECT *) rect,
                        left, pts[count].y, right, bottom );
            _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
        }
        if (pts[count].y > bottom) {
            _wpi_setintrectvalues( (WPI_RECT *) rect,
                        left, top, right, pts[count].y );
        }
    }
}

void rgn_rectangle(
/*****************/

    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2
) {
    rgn_def far         *curr;
    WPI_POINT           pts[2];
    unsigned short      size;

    if( Curr_state == RGN_STATE_ON ) {
        size =  sizeof(rgn_info_def) + sizeof(rgn_rect_def);
        curr = get_rgn_list_ptr( size );
        if (curr != NULL) {
            curr->info.size = size;
            curr->info.type = CGR_RGN_RECT;
            curr->info.major_id = Major_id;
            curr->info.minor_id = Minor_id;
            curr->info.data_row = Data_row;
            curr->info.data_col = Data_col;
            pts[0].x = x1;
            pts[0].y = y1;
            pts[1].x = x2;
            pts[1].y = y2;
            get_rect( pts, 2, &(curr->data.rect.rect) );
            if (Set_state != RGN_REGULAR) {
                curr->info.set_num = Set_curr;
                if (Set_state == RGN_GROUP || Set_state == RGN_MAN_GROUP) {
                    rgn_group_add();
                }
            } else {
                curr->info.set_num = -1;
            }
        }
    }
}

void rgn_ellipse_set(
/*******************/
    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2,
    rgn_type            type_ellipse
) {
    rgn_def far         *curr;
    WPI_POINT           pts[2];
    unsigned short      size;

    if( Curr_state == RGN_STATE_ON ) {
        size =  sizeof(rgn_info_def) + sizeof(rgn_ellipse_def);
        curr = get_rgn_list_ptr( size );
        if (curr != NULL) {
            curr->info.size = size;
            curr->info.type = type_ellipse;
            curr->info.major_id = Major_id;
            curr->info.minor_id = Minor_id;
            curr->info.data_row = Data_row;
            curr->info.data_col = Data_col;
            pts[0].x = x1;
            pts[0].y = y1;
            pts[1].x = x2;
            pts[1].y = y2;
            get_rect( pts, 2, &(curr->data.ellipse.rect) );
            if (Set_state != RGN_REGULAR) {
                curr->info.set_num = Set_curr;
                if (Set_state == RGN_GROUP || Set_state == RGN_MAN_GROUP) {
                    rgn_group_add();
                }
            } else {
                curr->info.set_num = -1;
            }
        }
    }
}

void rgn_ellipse(
/***************/
    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2
) {
    rgn_ellipse_set( x1, y1, x2, y2, CGR_RGN_ELLIPSE );
}

void rgn_ellipse_border(
/**********************/
    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2
) {
    rgn_ellipse_set( x1, y1, x2, y2, CGR_RGN_ELLIPSE_BORDER );
}


void rgn_line(
/************/

    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2
) {
    rgn_def far        *curr;
    unsigned short      size;

    if( Curr_state == RGN_STATE_ON ) {
        size =  sizeof(rgn_info_def) + sizeof(rgn_line_def);
        curr = get_rgn_list_ptr( size );
        if (curr != NULL) {
            curr->info.size = size;
            curr->info.type = CGR_RGN_LINE;
            curr->info.major_id = Major_id;
            curr->info.minor_id = Minor_id;
            curr->info.data_row = Data_row;
            curr->info.data_col = Data_col;
            curr->data.line.p1.x = x1;
            curr->data.line.p1.y = y1;
            curr->data.line.p2.x = x2;
            curr->data.line.p2.y = y2;
            if (Set_state != RGN_REGULAR) {
                curr->info.set_num = Set_curr;
                if (Set_state == RGN_GROUP || Set_state == RGN_MAN_GROUP) {
                    rgn_group_add();
                }
            } else {
                curr->info.set_num = -1;
            }
        }
    }
}

void rgn_polygon(
/***************/

    WPI_POINT far       *pts,
    int                 num_pts
) {
    int                 count;
    rgn_def far         *curr;
    unsigned short      size;

    if( Curr_state == RGN_STATE_ON ) {
        size = sizeof(rgn_info_def) + sizeof(int) + (sizeof(WPI_POINT) * num_pts);
        curr = get_rgn_list_ptr( size );
        if (curr != NULL) {
            curr->info.size = size;
            curr->info.type = CGR_RGN_POLY;
            curr->info.major_id = Major_id;
            curr->info.minor_id = Minor_id;
            curr->info.data_row = Data_row;
            curr->info.data_col = Data_col;
            curr->data.poly.num_pts = num_pts;
            for (count = 0; count < num_pts; count++) {
                curr->data.poly.pts[count].x = pts[count].x;
                curr->data.poly.pts[count].y = pts[count].y;
            }
            if (Set_state != RGN_REGULAR) {
                curr->info.set_num = Set_curr;
                if (Set_state == RGN_GROUP || Set_state == RGN_MAN_GROUP) {
                    rgn_group_add();
                }
            } else {
                curr->info.set_num = -1;
            }
        }
    }
}

void rgn_pie(
/***********/

    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2,
    int                 x3,
    int                 y3,
    int                 x4,
    int                 y4
) {
    rgn_def far        *curr;
    unsigned short      size;

    if( Curr_state == RGN_STATE_ON ) {
        size = sizeof(rgn_info_def) + sizeof(rgn_pie_def);
        curr = get_rgn_list_ptr( size );
        if (curr != NULL) {
            curr->info.size = size;
            curr->info.type = CGR_RGN_PIE;
            curr->info.major_id = Major_id;
            curr->info.minor_id = Minor_id;
            curr->info.data_row = Data_row;
            curr->info.data_col = Data_col;
            curr->data.pie.pie_pts[0].x = x1;
            curr->data.pie.pie_pts[0].y = y1;
            curr->data.pie.pie_pts[1].x = x2;
            curr->data.pie.pie_pts[1].y = y2;
            curr->data.pie.pie_pts[2].x = x3;
            curr->data.pie.pie_pts[2].y = y3;
            curr->data.pie.pie_pts[3].x = x4;
            curr->data.pie.pie_pts[3].y = y4;
            if (Set_state != RGN_REGULAR) {
                curr->info.set_num = Set_curr;
                if (Set_state == RGN_GROUP || Set_state == RGN_MAN_GROUP) {
                    rgn_group_add();
                }
            } else {
                curr->info.set_num = -1;
            }
        }
    }
}

void rgn_line_boxes(
/******************/

    int                 x1,
    int                 y1,
    int                 x2,
    int                 y2
) {
    rgn_def far        *curr;
    unsigned short      size;

    if( Curr_state == RGN_STATE_ON ) {
        size =  sizeof(rgn_info_def) + sizeof(rgn_line_def);
        curr = get_rgn_list_ptr( size );
        if (curr != NULL) {
            curr->info.size = size;
            curr->info.type = CGR_RGN_LINE_BOXES;
            curr->info.major_id = Major_id;
            curr->info.minor_id = Minor_id;
            curr->info.data_row = Data_row;
            curr->info.data_col = Data_col;
            curr->data.line_boxes.p1.x = x1;
            curr->data.line_boxes.p1.y = y1;
            curr->data.line_boxes.p2.x = x2;
            curr->data.line_boxes.p2.y = y2;
            if (Set_state != RGN_REGULAR) {
                curr->info.set_num = Set_curr;
                if (Set_state == RGN_GROUP || Set_state == RGN_MAN_GROUP) {
                    rgn_group_add();
                }
            } else {
                curr->info.set_num = -1;
            }
        }
    }
}

static BOOL check_in_ellipse(
/***************************/
    WPI_RECT far    *rect,
    int             x,
    int             y
) {
    WPI_POINT       cen;
    long            a_sqr;
    long            b_sqr;
    int             left, right, top, bottom;

    _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
    cen.x = (right + left) / 2;
    cen.y = (bottom + top) / 2;

    a_sqr = sqr( (right - left) / 2 );
    b_sqr = sqr( (bottom - top) / 2 );

    return( (b_sqr * (a_sqr - sqr( x - cen.x ))) >
                                    a_sqr * sqr( y - cen.y ) );
}

static BOOL check_in_line(
/************************/
    rgn_line_def far       *line,
    int                     x,
    int                     y
) {
    WPI_POINT               pt;
    WPI_POINT               rot_pt;
    int                     rad;

    pt.x = line->p2.x - line->p1.x;
    pt.y = line->p2.y - line->p1.y;
    x -= line->p1.x;
    y -= line->p1.y;

    rad = sqrt( sqr( pt.x ) + sqr( pt.y ) );
    if (rad != 0) {
        rot_pt.x = ((long)x * pt.x + (long)y * pt.y) / rad;
        rot_pt.y = ((long)y * pt.x - (long)x * pt.y) / rad;
        if( (rot_pt.x <= rad) && (rot_pt.x >= 0) &&
                                        (abs( rot_pt.y ) <= LINE_ZONE_SIZE) ) {
            return( TRUE );
        }
    } else if( x == 0 && y == 0 ) {
        return( TRUE );
    }

    return( FALSE );
}

static int next_idx(
/******************/

    int                 idx,
    int                 size
) {
    ++idx;
    if( idx == size ) {
        idx = 0;
    }

    return( idx );
}

static BOOL check_in_polygon(
/***************************/

    int                 num_pts,
    WPI_POINT far      *pts,
    int                 in_x,
    int                 in_y
) {
    int                 i;
    WPI_POINT far       *pt;
    int                 end;
    BOOL                above;
    int                 count;
    int                 int_x;
    int                 dy;
    BOOL                one_on;
    WPI_POINT           curr_pt;
    WPI_POINT           prev_pt;
    BOOL                on_right;
    BOOL                on_left;
    BOOL                last_on_right;

    on_right = FALSE;
    on_left = FALSE;
    for( i = 0, pt = pts;; ) {
        if( pt->y != in_y ) {
            break;
        }
        if( pt->x >= in_x ) {
            on_right = TRUE;
        }
        if( pt->x <= in_x ) {
            on_left = TRUE;
        }

        ++i;
        if( i == num_pts ) {
            /* all poly points lie on the line */
            return( on_right && on_left );
        }
        ++pt;
    }
    above = pt->y < in_y;
    prev_pt = *pt;
    i = next_idx( i, num_pts );
    end = i;
    one_on = FALSE;

    count = 0;
    for( ;; ) {
        curr_pt = pts[i];
        if( curr_pt.y == in_y ) {
            /* this point lies along test line */
            on_right = curr_pt.x >= in_x;
            if( one_on ) {
                /* this is the 2+ point along the test line. See if the test
                   point lies on an edge */
                if( last_on_right != on_right ) {
                    /* the test point is nessled between the current and
                       previous point. Test point on edge counts as
                       inside */
                    return( TRUE );
                }
            }
            one_on = TRUE;
            last_on_right = on_right;
        } else {
            dy = curr_pt.y - prev_pt.y;
            if( dy != 0 ) {
                if( ( ( curr_pt.y <= in_y ) && ( prev_pt.y >= in_y ) ) ||
                        ( ( curr_pt.y >= in_y ) && ( prev_pt.y <= in_y ) ) ) {
                    int_x = prev_pt.x + ((long)(in_y - prev_pt.y) *
                                            (curr_pt.x - prev_pt.x) ) / dy;
                    if( int_x >= in_x ) {
                        /* The line intersects with the test line */
                        if( !one_on || above != ( curr_pt.y < in_y ) ) {
                            /* either the two points which make this line
                               both do not lie on the test line, or the
                               previous one did: in latter case, intersection
                               counts if last non-test-line point and this
                               one lie on different sides */
                            ++count;
                        }
                    }
                }
            }
            one_on = FALSE;
            above = curr_pt.y < in_y;
        }
        i = next_idx( i, num_pts );
        if( i == end ) {
            break;
        }
        prev_pt = curr_pt;
    }

    return( count & 1 );
}

static BOOL check_above_line(
/***************************/
    WPI_POINT far          *pt1,
    WPI_POINT far          *pt2,
    int                     x,
    int                     y
) {
    return( (WPI_VERT_MULT * ((pt1->x - pt2->x) * (y - pt2->y) -
                                    (pt1->y - pt2->y) * (x - pt2->x))) < 0 );
}

static BOOL check_inside_pie(
/***************************/
    WPI_POINT far           *pie,
    int                     x,
    int                     y
) {
    WPI_POINT               cen;

    cen.x = (pie[0].x + pie[1].x) / 2;
    cen.y = (pie[0].y + pie[1].y) / 2;

    if (check_above_line( &pie[2], &cen, pie[3].x, pie[3].y )) {
        if (check_above_line( &pie[2], &cen, x, y ) &&
                                !check_above_line( &pie[3], &cen, x, y )) {
            return( TRUE );
        }
    } else {
        if (check_above_line( &pie[2], &cen, x, y ) ||
                                !check_above_line( &pie[3], &cen, x, y )) {
            return( TRUE );
        }
    }
    return( FALSE );
}

void far *rgn_find(
/*****************/
/* find the first region section which this point is in. Return NULL if none */

    void far            *rgn_ptr,
    int                 x,
    int                 y
) {
    rgn_tag_def far    *rgn_list;
    rgn_def far         *curr;
    WPI_RECT            tmp_rect;
    WPI_POINT           point;
    WPI_RECT            rect;
    rgn_def far        *rgn_top;
    int                 left, right, top, bottom;

    rgn_list = rgn_ptr;

    if( rgn_list != NULL ) {
        rgn_top = _ptr_add( rgn_list, rgn_list->rgn_offset );
        for (curr = rgn_top; curr < _ptr_add( rgn_top, rgn_list->rgn_size );
                            curr = _ptr_add( curr, curr->info.size ) ) {
            switch (curr->info.type) {
            case CGR_RGN_RECT:
                point.x = x;
                point.y = y;
                if (is_pt_in_rect( (WPI_RECT *) &(curr->data.rect.rect), point)) {
                    return( curr );
                }
                break;

            case CGR_RGN_ELLIPSE_BORDER:
                tmp_rect = curr->data.ellipse.rect;
                _wpi_getintrectvalues( tmp_rect, &left, &top, &right, &bottom );
                left -= RGN_ELLIPSE_BWIDTH;
                right += RGN_ELLIPSE_BWIDTH;
                top -= RGN_ELLIPSE_BWIDTH;
                bottom += RGN_ELLIPSE_BWIDTH;
                _wpi_setintrectvalues( &tmp_rect, left, top, right, bottom );
                if( check_in_ellipse( &(tmp_rect), x, y ) ) {
                    tmp_rect = curr->data.ellipse.rect;
                    _wpi_getintrectvalues( tmp_rect, &left, &top, &right, &bottom );
                    left += RGN_ELLIPSE_BWIDTH;
                    right -= RGN_ELLIPSE_BWIDTH;
                    top += RGN_ELLIPSE_BWIDTH;
                    bottom -= RGN_ELLIPSE_BWIDTH;
                    _wpi_setintrectvalues( &tmp_rect, left, top, right, bottom );
                    if( !check_in_ellipse( &(tmp_rect), x, y ) ) {
                        return( curr );
                    }
                }
                break;

            case CGR_RGN_ELLIPSE:
                if (check_in_ellipse( &(curr->data.ellipse.rect), x, y )) {
                    return( curr );
                }
                break;

            case CGR_RGN_LINE:
                if (check_in_line( &(curr->data.line), x, y )) {
                    return( curr );
                }
                break;

            case CGR_RGN_LINE_BOXES:
                if( check_in_line( &(curr->data.line_boxes), x, y ) ) {
                    return( curr );
                }
                point.x = x;
                point.y = y;

                left = curr->data.line_boxes.p1.x - MARK_SIZE;
                right = curr->data.line_boxes.p1.x + MARK_SIZE;
                top = curr->data.line_boxes.p1.y - MARK_SIZE;
                bottom = curr->data.line_boxes.p1.y + MARK_SIZE;
                _wpi_setintrectvalues( &rect, left, top, right, bottom );
                if( is_pt_in_rect( &rect, point) ) {
                    return( curr );
                }

                left = curr->data.line_boxes.p2.x - MARK_SIZE;
                right = curr->data.line_boxes.p2.x + MARK_SIZE;
                top = curr->data.line_boxes.p2.y - MARK_SIZE;
                bottom = curr->data.line_boxes.p2.y + MARK_SIZE;
                _wpi_setintrectvalues( &rect, left, top, right, bottom );
                if( is_pt_in_rect( &rect, point) ) {
                    return( curr );
                }
                break;

            case CGR_RGN_POLY:
                if( check_in_polygon( curr->data.poly.num_pts,
                                                curr->data.poly.pts, x, y ) ) {
                    return( curr );
                }
                break;

            case CGR_RGN_PIE:
                top = curr->data.pie.pie_pts[0].y;
                bottom = curr->data.pie.pie_pts[1].y;
                left = curr->data.pie.pie_pts[0].x;
                right = curr->data.pie.pie_pts[1].x;
                _wpi_setintrectvalues( &rect, left, top, right, bottom );

                if (check_in_ellipse( &rect, x, y )) {
                    if (check_inside_pie( &(curr->data.pie.pie_pts), x, y )) {
                        return( curr );
                    }
                }
                break;
            }
        }
    }
    return( NULL );
}

static void remove_mark(
/**********************/
    WPI_PRES            dc,
    rgn_marker_def      *mark
) {
    WPI_PRES            mem_dc;
    WPI_HANDLE          old_bmp;
    short               width;
    WPI_INST            inst;
    HDC                 t_dc;

    if (mark != NULL) {
        width = 2 * MARK_SIZE + 1;
#ifdef PLAT_OS2
        /* Don't ask, it just needs to be here - trust me */
        width += 1;
#endif
        _wpi_setanchorblock( dc, inst );
        mem_dc = _wpi_createcompatiblepres( dc, inst, &t_dc );
        old_bmp = _wpi_selectobject( mem_dc, mark->bmp );
        _wpi_bitblt( dc, mark->pt.x - MARK_SIZE, mark->pt.y - MARK_SIZE,
                width, width, mem_dc, 0, 0, SRCCOPY );
        _wpi_selectobject( mem_dc, old_bmp );
        _wpi_deletecompatiblepres( mem_dc, t_dc );
    }
}

static void mark_point(
/*********************/
    WPI_PRES            dc,
    int                 x,
    int                 y,
    rgn_marker_def      *slot
) {
    WPI_PRES            mem_dc;
    WPI_HANDLE          old_bmp;
    HBITMAP             bitmap;
    short               width;
    WPI_INST            inst;
    HDC                 t_dc;

    if (slot != NULL) {
        width = 2 * MARK_SIZE + 1;
#ifdef PLAT_OS2
        /* Don't ask, it just needs to be here - trust me */
        width += 1;
#endif
        _wpi_setanchorblock( dc, inst );
        mem_dc = _wpi_createcompatiblepres( dc, inst, &t_dc );
        bitmap = _wpi_createcompatiblebitmap( dc, width, width );
        old_bmp = _wpi_selectobject( mem_dc, bitmap );
        _wpi_bitblt( mem_dc, 0, 0, width+1, width+1, dc, x - MARK_SIZE, y - MARK_SIZE, SRCCOPY );
        _wpi_bitblt( mem_dc, 0, 0, width, width, dc, x - MARK_SIZE, y - MARK_SIZE, SRCCOPY );
        _wpi_selectobject( mem_dc, old_bmp );
        _wpi_deletecompatiblepres( mem_dc, t_dc );
        slot->pt.x = x;
        slot->pt.y = y;
        slot->bmp = bitmap;
    }

    _wpi_rectangle( dc, x - MARK_SIZE, y - MARK_SIZE,
                                        x + MARK_SIZE, y + MARK_SIZE );
}

static HANDLE mark_rect(
/**********************/
    WPI_PRES        dc,
    WPI_RECT far    *rect,
    HANDLE          bmps_hld,
    short           start
) {
    rgn_mark_def   *bmps;
    int             left, right, top, bottom;

    _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        bmps->num_bmps = 4;
        mark_point( dc, left, top, &(bmps->mark[start]) );
        mark_point( dc, left, bottom, &(bmps->mark[start + 1]) );
        mark_point( dc, right, top, &(bmps->mark[start + 2]) );
        mark_point( dc, right, bottom, &(bmps->mark[start + 3]) );
        LocalUnlock( bmps_hld );
    } else {
        mark_point( dc, left, top, NULL );
        mark_point( dc, left, bottom, NULL );
        mark_point( dc, right, top, NULL );
        mark_point( dc, right, bottom, NULL );
    }

    return( bmps_hld );
}

static HANDLE mark_ellipse(
/*************************/
    WPI_PRES        dc,
    WPI_RECT far    *rect,
    HANDLE          bmps_hld,
    short           start
) {
    rgn_mark_def   *bmps;
    int             x;
    int             y;
    int             left, right, top, bottom;

    _wpi_getintrectvalues( *rect, &left, &top, &right, &bottom );
    x = (left + right) / 2;
    y = (top + bottom) / 2;

    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        bmps->num_bmps = 4;
        mark_point( dc, x, top, &(bmps->mark[start]) );
        mark_point( dc, x, bottom, &(bmps->mark[start + 1]) );
        mark_point( dc, left, y, &(bmps->mark[start + 2]) );
        mark_point( dc, right, y, &(bmps->mark[start + 3]) );
        LocalUnlock( bmps_hld );
    } else {
        mark_point( dc, x, top, NULL );
        mark_point( dc, x, bottom, NULL );
        mark_point( dc, left, y, NULL );
        mark_point( dc, right, y, NULL );
    }

    return( bmps_hld );
}

static HANDLE mark_line(
/**********************/
    WPI_PRES            dc,
    WPI_POINT           p1,
    WPI_POINT           p2,
    HANDLE              bmps_hld,
    short               start
) {
    rgn_mark_def   *bmps;

    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        bmps->num_bmps = 2;
        mark_point( dc, p1.x, p1.y, &(bmps->mark[start]) );
        mark_point( dc, p2.x, p2.y, &(bmps->mark[start + 1]) );
        LocalUnlock( bmps_hld );
    } else {
        mark_point( dc, p1.x, p1.y, NULL );
        mark_point( dc, p2.x, p2.y, NULL );
    }

    return( bmps_hld );
}

static HANDLE mark_poly(
/**********************/
    WPI_PRES            dc,
    int                 num_pts,
    WPI_POINT far       *pts,
    HANDLE              bmps_hld,
    short               start
) {
    rgn_mark_def       *bmps;
    short               count;

    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        bmps->num_bmps = num_pts;
        for (count = 0; count < num_pts; count++ ) {
            mark_point( dc, pts[count].x, pts[count].y,
                                      &(bmps->mark[count + start]) );
        }
        LocalUnlock( bmps_hld );
    } else {
        for (count = 0; count < num_pts; count++ ) {
            mark_point( dc, pts[count].x, pts[count].y, NULL );
        }
    }

    return( bmps_hld );
}

static void get_pie_pt(
/*********************/
    WPI_POINT far      *point,
    long                a_sqr,
    long                b_sqr,
    WPI_POINT          *cen,
    WPI_POINT          *mark
) {
    WPI_POINT           vec;
    float               val;
    float               ratio;

    if ((point->x != cen->x) || (point->y != cen->y)) {
        vec.x = point->x - cen->x;
        vec.y = point->y - cen->y;

        /* This is done to prevent overflows in the calculation */
        if( sqr( vec.x ) + sqr( vec.y ) >= MAG_OVERFLOW ) {
            vec.x /= 4;
            vec.y /= 4;
        }

        val = (float)(a_sqr * b_sqr) /
                (float)((b_sqr * sqr( vec.x )) + (a_sqr * sqr( vec.y )));
        ratio = sqrt( val );
        mark->x = (ratio * vec.x) + cen->x;
        mark->y = (ratio * vec.y) + cen->y;
    }
}

static void get_mid_pt(
/*********************/
    WPI_POINT far      *pts,
    WPI_POINT far      *pie,
    WPI_POINT          *mid_pt
) {
    float           theta;
    short           x;
    short           y;

    mid_pt->x = (pts[1].x + pts[2].x) / 2;
    mid_pt->y = (pts[1].y + pts[2].y) / 2;

    if (sqr( mid_pt->x - pts[0].x ) + sqr( mid_pt->y - pts[0].y )
                                                    <= sqr( VECTOR_FUZZ )) {
        /* Vector is too small (50% slices) */
        if (pts[1].x != pts[2].x) {
            if (pts[1].y != pts[2].y) {
                /* arbitrary angle case */
                theta = atan( (float) ((float) (pts[2].y - pts[1].y) /
                                            (float) (pts[2].x - pts[1].x)) );

                if (theta > 0) {
                    if (pts[2].y < pts[1].y) {
                        theta = - theta - PI / 2.0;
                    } else {
                        theta = PI / 2.0 - theta;
                    }
                } else {
                    if (pts[2].y > pts[1].y) {
                        theta = - theta - PI / 2.0;
                    } else {
                        theta = PI / 2.0 - theta;
                    }
                }

                x = (short)(VECTOR_COEF * cos( theta ));
                y = (short)(VECTOR_COEF * sin( theta ));

            } else {
                /* vertical */
                x = 0;
                y = VECTOR_COEF * sgn( pts[2].x - pts[1].x );
            }
        } else {
            /* horizontal */
            x = VECTOR_COEF * sgn( pts[2].y - pts[1].y );
            y = 0;
        }
#ifdef PLAT_OS2
        mid_pt->x = pts[0].x + x;
        mid_pt->y = pts[0].y - y;
#else
        mid_pt->x = pts[0].x - x;
        mid_pt->y = pts[0].y + y;
#endif

    } else if (!check_inside_pie( pie, mid_pt->x, mid_pt->y )) {
        /* Pie is a reflex angle pie */
        mid_pt->x = (-1 * (mid_pt->x - pts[0].x)) + pts[0].x;
        mid_pt->y = (-1 * (mid_pt->y - pts[0].y)) + pts[0].y;
    }
}

static HANDLE mark_pie(
/*********************/
    WPI_PRES            dc,
    WPI_POINT far       *pie,
    HANDLE              bmps_hld,
    short               start
) {
    short               count;
    rgn_mark_def       *bmps;
    WPI_POINT           mid_pt;
    WPI_POINT           pts[4];
    long                a_sqr;
    long                b_sqr;

    pts[0].x = (pie[0].x + pie[1].x) / 2;
    pts[0].y = (pie[0].y + pie[1].y) / 2;
    a_sqr = sqr( (pie[1].x - pie[0].x) / 2 );
    b_sqr = sqr( (pie[1].y - pie[0].y) / 2 );
    get_pie_pt( &pie[2], a_sqr, b_sqr, &pts[0], &pts[1] );
    get_pie_pt( &pie[3], a_sqr, b_sqr, &pts[0], &pts[2] );
    get_mid_pt( pts, pie, &mid_pt );
    get_pie_pt( &mid_pt, a_sqr, b_sqr, &pts[0], &pts[3] );

    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        bmps->num_bmps = 4;
        for (count = 0; count < 4; count++) {
            mark_point( dc, pts[count].x, pts[count].y,
                                            &(bmps->mark[count + start]) );
        }
        LocalUnlock( bmps_hld );
    } else {
        for (count = 0; count < 4; count++) {
            mark_point( dc, pts[count].x, pts[count].y, NULL );
        }
    }

    return( bmps_hld );
}

static HANDLE mark_set(
/*********************/
    rgn_set_def far    *set,
    WPI_PRES            dc,
    BOOL                bitmaps
) {
    HANDLE              bmps_hld;
    rgn_mark_def *      bmps;
    rgn_set_list_def *  list;
    int                 curr_pt;

    bmps_hld = NULL;
    if (bitmaps) {
        bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                    + (set->info.num_used - 1) * sizeof(rgn_marker_def) );
    }

    list = (rgn_set_list_def *) set->list;
    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        bmps->num_bmps = set->info.num_used;
        for( curr_pt = 0; curr_pt < set->info.num_used; curr_pt++ ) {
            mark_point( dc, list[ curr_pt ].pt.x, list[ curr_pt ].pt.y,
                        &bmps->mark[ curr_pt ] );
        }
        LocalUnlock( bmps_hld );
    } else {
        for( curr_pt = 0; curr_pt < set->info.num_used; curr_pt++ ) {
            mark_point( dc, list[ curr_pt ].pt.x, list[ curr_pt ].pt.y,
                        NULL );
        }
    }

    return( bmps_hld );
}

static HANDLE mark_group(
/***********************/
    rgn_tag_def far    *list,
    rgn_set_def far    *set,
    WPI_PRES            dc,
    BOOL                bitmaps
) {
    rgn_def far        *rgn;
    short               count;
    HANDLE              bmps_hld;
    short               size;
    short               num_bmps;
    rgn_mark_def       *bmps;

    bmps_hld = NULL;
    size = 0;
    num_bmps = 0;
    if (bitmaps) {
        bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                                                - sizeof(rgn_marker_def) );
        size = sizeof(rgn_mark_def) - sizeof(rgn_marker_def);
    }
    for (count = 0; count < set->info.num_used; count++) {
        rgn = (void *) ((char *)list + list->rgn_offset + list->rgn_size
                                    - set->list[ count ].offset );
        switch (rgn->info.type) {
        case CGR_RGN_RECT:
            if (bitmaps) {
                bmps_hld = LocalReAlloc( bmps_hld, 4 * sizeof(rgn_marker_def)
                                                    + size, LMEM_MOVEABLE );
                size += (4 * sizeof(rgn_marker_def));
            }
            mark_rect( dc, &(rgn->data.rect.rect), bmps_hld, num_bmps );
            num_bmps += 4;
            break;

        case CGR_RGN_ELLIPSE_BORDER:
            if (bitmaps) {
                bmps_hld = LocalReAlloc( bmps_hld, 4 * sizeof(rgn_marker_def)
                                                    + size, LMEM_MOVEABLE );
                size += (4 * sizeof(rgn_marker_def));
            }
            mark_ellipse( dc, &(rgn->data.ellipse.rect), bmps_hld, num_bmps );
            num_bmps += 4;
            break;

        case CGR_RGN_ELLIPSE:
            if (bitmaps) {
                bmps_hld = LocalReAlloc( bmps_hld, 4 * sizeof(rgn_marker_def)
                                                    + size, LMEM_MOVEABLE );
                size += (4 * sizeof(rgn_marker_def));
            }
            mark_ellipse( dc, &(rgn->data.ellipse.rect), bmps_hld, num_bmps );
            num_bmps += 4;
            break;

        case CGR_RGN_LINE:
            if (bitmaps) {
                bmps_hld = LocalReAlloc( bmps_hld, 2 * sizeof(rgn_marker_def) \
                                                    + size, LMEM_MOVEABLE );
                size += (2 * sizeof(rgn_marker_def));
            }
            mark_line( dc, rgn->data.line.p1, rgn->data.line.p2, bmps_hld,
                                                                num_bmps );
            num_bmps += 2;
            break;

        case CGR_RGN_POLY:
            if (bitmaps) {
                bmps_hld = LocalReAlloc( bmps_hld, rgn->data.poly.num_pts *
                        sizeof(rgn_marker_def) + size, LMEM_MOVEABLE );
                size += (rgn->data.poly.num_pts * sizeof(rgn_marker_def));
            }
            mark_poly( dc, rgn->data.poly.num_pts, &(rgn->data.poly.pts),
                                                        bmps_hld, num_bmps );
            num_bmps += rgn->data.poly.num_pts;
            break;

        case CGR_RGN_PIE:
            if (bitmaps) {
                bmps_hld = LocalReAlloc( bmps_hld, 4 * sizeof(rgn_marker_def)
                                                    + size, LMEM_MOVEABLE );
                size += (4 * sizeof(rgn_marker_def));
            }
            mark_pie( dc, &(rgn->data.pie.pie_pts), bmps_hld, num_bmps );
            num_bmps += 4;
            break;
        }
    }

    if (bmps_hld != NULL) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        if (bmps != NULL) {
            bmps->num_bmps = num_bmps;
        }
        LocalUnlock( bmps_hld );
    }

    return( bmps_hld );
}

void rgn_mark_free(
/*****************/
/* delete the mark bitmaps and array. */

    HANDLE              bitmaps
) {
    short               count;
    rgn_mark_def       *bmps;

    bmps = (rgn_mark_def *)LocalLock( bitmaps );
    for( count = 0; count < bmps->num_bmps; count++ ) {
        if( bmps->mark[count].bmp != 0 ) {
            _wpi_deletebitmap( bmps->mark[count].bmp );
            bmps->mark[count].bmp = NULL;
        }
    }
    LocalUnlock( bitmaps );
    LocalFree( bitmaps );
}

void rgn_unmark(
/**************/
/* undraw the markers on the corners of this region */

    void far            *rgn_ptr,
    WPI_PRES            dc,
    HANDLE              bmps_hld
) {
    short               count;
    rgn_mark_def        *bmps;
    rgn_def far         *rgn;

    rgn = rgn_ptr;

    if ((rgn != NULL) && (bmps_hld != NULL)) {
        bmps = (rgn_mark_def *)LocalLock( bmps_hld );
        for (count = (bmps->num_bmps - 1); count >=0; count--) {
            remove_mark( dc, &(bmps->mark[count]) );
        }
        LocalUnlock( bmps_hld );
        rgn_mark_free( bmps_hld );
    }
}

static HANDLE do_group_set_markers(
/*********************************/
    rgn_tag_def far    *list,
    short               num,
    WPI_PRES            dc,
    BOOL                bitmaps
) {
    rgn_set_def far    *set;
    rgn_set_index *     index;

    index = (rgn_set_index *) ((char *)list + list->set_coll_offset);
    if( num < index->info.num_sets ) {
        set = (rgn_set_def *) ((char *)index + index->offset[ num ]);
        if (set->info.exact) {
            return( mark_set( set, dc, bitmaps ) );
        } else {
            return( mark_group( list, set, dc, bitmaps ) );
        }
    } else {
        return( NULL );
    }
}

extern void get_rgn_rect(
/***********************/
    void far            *rgn_ptr,
    WPI_RECT            *rect
) {
    rgn_def far         *rgn;

    rgn = rgn_ptr;
    if( rgn != NULL ) {
        *rect = rgn->data.rect.rect;
    }
}

extern int get_line_point(
/************************/
    void far            *rgn_ptr,
    int                 pos_x,
    int                 pos_y
) {
    rgn_def far         *rgn;

    rgn = rgn_ptr;
    if( rgn != NULL ) {
        if( pos_x >= rgn->data.line.p1.x - MARK_SIZE
                && pos_x <= rgn->data.line.p1.x + MARK_SIZE
                && pos_y >= rgn->data.line.p1.y - MARK_SIZE
                && pos_y <= rgn->data.line.p1.y + MARK_SIZE ) {
            return( 1 );
        } else if( pos_x >= rgn->data.line.p2.x - MARK_SIZE
                && pos_x <= rgn->data.line.p2.x + MARK_SIZE
                && pos_y >= rgn->data.line.p2.y - MARK_SIZE
                && pos_y <= rgn->data.line.p2.y + MARK_SIZE ) {
            return( 2 );
        } else {
            return( 0 );
        }
    }

    return( 0 );
}

extern void get_rgn_line(
/***********************/
    void far            *rgn_ptr,
    WPI_POINT           *p1,
    WPI_POINT           *p2
) {
    rgn_def far         *rgn;

    rgn = rgn_ptr;
    if( rgn != NULL ) {
        *p1 = rgn->data.line_boxes.p1;
        *p2 = rgn->data.line_boxes.p2;
    }
}

HANDLE rgn_mark(
/**************/
/* draw markers on the corners of this region */
    void far            *list_ptr,
    void far            *rgn_ptr,
    WPI_PRES            dc,
    BOOL                bitmaps,
    BOOL                is_moveable
) {
    HANDLE              bmps_hld;
    HBRUSH              brush;
    HBRUSH              old_brush;
    WPI_COLOUR          clr_white;
    WPI_COLOUR          clr_black;
    HPEN                pen;
    HPEN                old_pen;
    rgn_def far         *rgn;
    rgn_tag_def far     *list;

    rgn = rgn_ptr;
    list = list_ptr;

    if( rgn != NULL ) {
        clr_white = _wpi_getrgb( 255, 255, 255 );
        clr_black = _wpi_getrgb( 0, 0, 0 );
        if( is_moveable ) {
            brush = _wpi_createsolidbrush( clr_black ); // black
        } else {
            brush = _wpi_createsolidbrush( clr_white ); // white
        }
        pen = _wpi_createpen( PS_SOLID, 1, clr_black );

        old_brush = _wpi_selectbrush( dc, brush );
        old_pen = _wpi_selectpen( dc, pen );

        bmps_hld = NULL;
        if (rgn->info.set_num == -1) {
            switch (rgn->info.type) {
            case CGR_RGN_RECT:
                if (bitmaps) {
                    bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                                                + 3 * sizeof(rgn_marker_def) );
                }
                mark_rect( dc, &(rgn->data.rect.rect), bmps_hld, 0 );
                break;

            case CGR_RGN_ELLIPSE_BORDER:
                if (bitmaps) {
                    bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                                                + 3 * sizeof(rgn_marker_def) );
                }
                mark_ellipse( dc, &(rgn->data.ellipse.rect), bmps_hld, 0 );
                break;

            case CGR_RGN_ELLIPSE:
                if (bitmaps) {
                    bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                                                + 3 * sizeof(rgn_marker_def) );
                }
                mark_ellipse( dc, &(rgn->data.ellipse.rect), bmps_hld, 0 );
                break;

            case CGR_RGN_LINE:
                if (bitmaps) {
                    bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                                                    + sizeof(rgn_marker_def) );
                }
                mark_line( dc, rgn->data.line.p1, rgn->data.line.p2,
                                                                bmps_hld, 0 );
                break;

            case CGR_RGN_POLY:
                if (bitmaps) {
                    bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def) +
                            (rgn->data.poly.num_pts - 1) * sizeof(rgn_marker_def) );
                }
                mark_poly( dc, rgn->data.poly.num_pts, &(rgn->data.poly.pts),
                                                            bmps_hld, 0 );
                break;

            case CGR_RGN_PIE:
                if (bitmaps) {
                    bmps_hld = LocalAlloc( LMEM_MOVEABLE, sizeof(rgn_mark_def)
                                            + 3 * sizeof(rgn_marker_def) );
                }
                mark_pie( dc, &(rgn->data.pie.pie_pts), bmps_hld, 0 );
                break;
            }
        } else {
            bmps_hld = do_group_set_markers( list, rgn->info.set_num, dc,
                                                                    bitmaps );
        }

        _wpi_getoldbrush( dc, old_brush );
        _wpi_getoldpen( dc, old_pen );
        _wpi_deletebrush( brush );
        _wpi_deletepen( pen );
    }

    return( bmps_hld );
}

int rgn_ids(
/**********/
/* return the region ids for a region. Return major, set minor. */

    void far            *rgn,
    int far             *minor_id,
    int far             *data_row,
    int far             *data_col
) {
    rgn_info_def far    *rgn_info;

    if( rgn != NULL ) {
        rgn_info = rgn;

        *minor_id = rgn_info->minor_id;
        *data_row = rgn_info->data_row;
        *data_col = rgn_info->data_col;
        return( rgn_info->major_id );
    }

    return( 0 );
}
