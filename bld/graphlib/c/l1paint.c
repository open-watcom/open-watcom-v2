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
* Description:  Flood fill implementation.
*
****************************************************************************/


#include "gdefn.h"
#if !defined( _DEFAULT_WINDOWS )
#include <malloc.h>
#include "stkavail.h"

#define TUNING          6


/*  Use PASCAL pragma to define our convention for
    calling the ScanLeft and ScanRight routines.    */

#if defined ( __386__ )
    #pragma aux scan_call "*" parm caller [es edi] [eax] [ebx] [ecx] [edx] [esi] value [bx];
#else
    #pragma aux scan_call "*" parm caller [es di] [ax] [bx] [cx] [dx] [si] value [bx];
#endif

typedef short (near scan_func)( char far *, int, int, int, int, int );
#pragma aux (scan_call) scan_func;

struct frame {
    short         direction;
    short         left;
    short         right;
    short         ypos;
};


static char     CheckIfOut( short, short, short );
static short    PaintLeft( short, short, short, short );
static short    PaintRight( short, short, short, short );
static short    ScanLeft( short, short, short, short );
static short    ScanRight( short, short, short, short );
static short    AddEntry( short, short, short, short, unsigned, unsigned *, struct frame * );
static char     NotValidFrame( unsigned, unsigned *, struct frame * );
static char     StackCompare( struct frame *, unsigned * );


// Visit code is used to get around a bug in the floodfill algorithm.
// If filling with a non-solid pattern, it is not always possible to
// tell if an area has already been filled, leading to infinite loops.
// For WPAINT, (which uses worldm.lib or worldl.lib) the visit routines
// keep track of what points we have visited.

#if defined( __MEDIUM__ ) || defined( __LARGE__ )
extern int      _flood_is_visited( short, short );
extern void     _flood_visit( short, short, short );
#else
#define _flood_visit( x, y, len )
#endif

#endif


#if defined( _DEFAULT_WINDOWS )
static short _L0Paint( short stop_color, short x, short y )
/*=========================================================
  This function does the filling. */
{
    WORD                fill_style;
    WPI_PRES            dc;
    HBITMAP             bm;
    HBRUSH              brush;
    HBRUSH              old_brush;
    HRGN                temprgn;
    WPI_COLOUR          color;
    short               rc;
    WPI_RECT            clip_rect, temp_rect;
    WPI_INST            inst;
    short               t;
    short               clipy1, clipy2;
  #if defined( __OS2__ )
    WPI_RECT            wrect;
    short               height;
  #endif

    // Map our way of filling to Window API's
    if( stop_color == -1 ) {
        fill_style = FLOODFILLSURFACE;
        stop_color = _getpixel( x, y );
    } else {
        fill_style = FLOODFILLBORDER;
    }

    //Setup before filling
    dc = _Mem_dc;

    // Do the clippings
    temprgn = _ClipRgn;
    clipy1 = _wpi_cvth_y( _CurrState->clip_def.ymin, _GetPresHeight() );
    clipy2 = _wpi_cvth_y( _CurrState->clip_def.ymax + 1, _GetPresHeight() );
    _wpi_setintwrectvalues( &clip_rect,
                           _CurrState->clip_def.xmin,
                           clipy1,
                           _CurrState->clip_def.xmax + 1,
                           clipy2 );
    temp_rect = clip_rect;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_getclipbox( dc, &clip_rect);
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// Setup
    color = _Col2RGB( _CurrColor );

// Check for fillmask
    if( _HaveMask == 0 ) {
        brush = _wpi_createsolidbrush( color );
    } else {
        /* if a mask is defined, convert it to bitmap */
        bm = _Mask2Bitmap( dc, &_FillMask );
        brush = _wpi_createpatternbrush( bm );
    }

    old_brush = _wpi_selectbrush( dc, brush );

// Do the floodfill
    y = _wpi_cvth_y( y, _GetPresHeight() );
    rc = _wpi_extfloodfill( dc, x, y, _Col2RGB( stop_color ), fill_style );


// Cleanup
    if( _HaveMask != 0 ) {
        _wpi_deletebitmap( bm );
    }

    _wpi_getoldbrush( dc, old_brush );
    _wpi_deletebrush( brush );

    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// Update the entire Clipping area
  #if defined( __OS2__ )
    GetClientRect( _CurrWin, &wrect );
    height = _wpi_getheightrect( wrect );
    t = _GetPresHeight() - height - _BitBlt_Coord.ycoord;
  #else
    t = _BitBlt_Coord.ycoord;
  #endif
    inst = _GetInst();
    _wpi_offsetrect( inst, &temp_rect,
                     -_BitBlt_Coord.xcoord,
                     -t );
    _wpi_invalidaterect( _CurrWin, &temp_rect, 0 );
    return ( rc );
}
#endif


short _L1Paint( short stop_color, short x, short y )
/*==================================================

    Paint a region on the screen starting at the point (x,y). If the
    stop_color is -1, then painting continues as long as the color of
    the neighbouring pixels is the same as the color of the starting pixel.
    Otherwise, stop_color defines the boundary of the fill region. Returns
    TRUE if paint is successful. Otherwise, returns FALSE and will try
    to paint as much as possible. This function obtains its dynamic
    memory off the stack.   */

{
#if defined( _DEFAULT_WINDOWS )
    short               rc;
#else
    unsigned            max_frames;             /* maximum # of frames      */
    unsigned            stack_count;            /* # of non-active frames   */
    short               left_edge;          /* left pixel of active frame   */
    short               right_edge;         /* right pixel of active frame  */
    short               direction;          /* direction of active frame    */
    short               left_scan;          /* temp. left while scanning    */
    short               right_scan;         /* temp. right while scanning   */
    short               cutoff;             /* stopping pixel in scanning   */
    short               success;                    /* flag for success     */
    char                border_flag;                /* 0 - until; 1 - while */
    struct frame *      stack;                      /* ptr to list of frames*/
#endif

    if( _L1OutCode( x, y ) ) {              /* starting point is outside    */
        _ErrorStatus = _GRINVALIDPARAMETER;
        return( FALSE );                    /* so do nothing                */
    }

#if defined( _DEFAULT_WINDOWS )
    rc = _L0Paint( stop_color, x, y );
    _RefreshWindow();
    return ( rc );
#else
    /*  Calculate the number of frames which can be held on the stack.
        Note : stack[0] always contains the active frame.   */

    max_frames = ( _stackavail() - 0x100 ) / sizeof( struct frame );
    if( max_frames >= 2 ) {                     /* need at least 2 frames   */
        stack = __alloca( _RoundUp( sizeof( struct frame ) * max_frames ) );
    } else {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return( FALSE );                    /* not enough memory so quit    */
    }
    max_frames -= 1;                    /* stack[0] is the active frame     */

    if( stop_color == -1 ) {
        stop_color = _L1GetDot( x, y );     /* get color of starting pixel  */
        if( stop_color == _CurrColor ) {
            if( _HaveMask == 0 ) {                  /* solid fill and pixel */
                _ErrorStatus = _GRINVALIDPARAMETER; /* already in the       */
                return( FALSE );                    /* current color        */
            }
        }
        border_flag = 1;                /* paint inside while same color    */
    } else {
        stop_color &= _CurrState->vc.numcolors - 1;
        if( stop_color == _L1GetDot( x, y ) ) {     /* on border of region */
            _ErrorStatus = _GRINVALIDPARAMETER;
            return( FALSE );
        }
        border_flag = 0;        /* paint inside until border encountered    */
    }
    /* Fill first line starting at (x,y)    */

    _StartDevice();
    left_edge = PaintLeft( x, y, stop_color, border_flag );
    right_edge = PaintRight( x, y, stop_color, border_flag );
    stack[0].ypos = y - 1;                          /* Process the frame    */
    stack[0].right = right_edge;                    /* going down first.    */
    stack[0].left = left_edge;
    stack[0].direction = -1;
    stack_count = 1;                                    /* 1 active frame   */
    stack[1].ypos = y;                                  /* save the frame   */
    stack[1].right = right_edge;                        /* going up         */
    stack[1].left = left_edge;
    stack[1].direction = 1;
    success = 0;                                /* assume success at first  */
    for( ;; ) {                                 /* process all the frames   */
        do {                                /* process frame row per row    */
            direction = stack[0].direction;         /* load active frame    */
            left_edge = stack[0].left;              /* ( frame may change ) */
            right_edge = stack[0].right;
            y = stack[0].ypos;
            if( CheckIfOut( left_edge, right_edge, y ) ) {
                break;                      /* frame is outside viewport    */
            }
            /* Start scanning. Scan left as much as possible.   */
            left_scan = PaintLeft( left_edge, y, stop_color, border_flag );
            right_scan = left_edge;
            left_edge = left_scan;
            /* Check for a hole to the left */
            if( left_scan <= right_scan - 2 ) {
                success += AddEntry( y, right_scan - 2, left_scan, -direction,
                                     max_frames, &stack_count, stack );
            }
            /* Now scan right until we reach the right edge */
            /* and skip holes if any.   */
            cutoff = right_scan;
            if( right_scan < right_edge || right_scan >= left_scan ) {
                do {
                    left_scan = right_scan;
                    right_scan = PaintRight( left_scan, y, stop_color, border_flag );
                    if( right_scan == left_scan ) { /* nothing was painted  */
                        right_scan += 1;                /* skip over hole   */
                    } else {                    /* something was painted    */
                        if( left_scan != cutoff ) {     /* Check for a hole */
                            if( left_edge <= cutoff ) {
                                success += AddEntry( y, cutoff, left_edge,
                                                     direction, max_frames,
                                                     &stack_count, stack );
                            }
                            left_edge = left_scan + 1;  /* adjust left edge */
                        }
                        cutoff = right_scan;        /* adjust stop pixel    */
                    }
                } while( right_scan < right_edge );
            }
            /* Check for a hole to the right    */
            if( right_scan >= right_edge + 2 ) {
                success += AddEntry( y, right_scan, right_edge + 2, -direction,
                                     max_frames, &stack_count, stack );
            }
            stack[0].direction = direction;         /* load new endpoints   */
            stack[0].left = left_edge;              /* after scan           */
            stack[0].right = cutoff;
            stack[0].ypos = y;
            if( left_edge > cutoff )        /* right most point on boundary */
                break;
        } while( StackCompare( stack, &stack_count ) );
        /* We are done with the previous frame so we have to load   */
        /* another one.     */
        do {
            if( stack_count == 0 ) {        /* no more frames to process    */
                _ResetDevice();
                return( !success );             /* flip flag - 0 was true   */
            }
            /* Make the last frame the active frame.    */
            memcpy( &stack[0], &stack[ stack_count ], sizeof( struct frame ) );
            stack_count -= 1;
        } while( !StackCompare( stack, &stack_count ) );
    }
#endif
}


#if !defined( _DEFAULT_WINDOWS )

static char CheckIfOut( short left_edge, short right_edge, short y )
/*==================================================================

    Returns true if TRUE if point is out of viewport or
    left_edge > right_edge. */

{
    if( left_edge > right_edge ) return( TRUE );
    if( _CurrState->clip.xmin > left_edge ) return( TRUE );
    if( left_edge > _CurrState->clip.xmax ) return( TRUE );
    if( _CurrState->clip.ymin > y ) return( TRUE );
    if( y > _CurrState->clip.ymax ) return( TRUE );
    return( FALSE );
}


static short PaintLeft( short x, short y, short stop_color, short border_flag )
/*=============================================================================

    Paint left of starting pixel (x,y) using current color and fill
    style. Return the new left pixel.   */

{
    short               xleft;
    scan_func           *scan;  /* pointer to scan function */
    gr_device _FARD     *dev_ptr;
    short               len;

#if defined( __MEDIUM__ ) || defined( __LARGE__ )
    if( _flood_is_visited( x, y ) ) {
        return( x );
    }
#endif
    dev_ptr = _CurrState->deviceptr;
    ( *dev_ptr->setup )( x, y, stop_color );
    scan = dev_ptr->scanleft;

    if( x >= _CurrState->clip.xmin ) {
        xleft = ( *scan )( _Screen.mem, _Screen.colour, x, _Screen.mask,
                       border_flag, _CurrState->clip.xmin );
    } else {
        xleft = x + 1;
    }
    len = x - xleft + 1;
    _L1Fill( xleft, y, len );
    _flood_visit( xleft, y, len );
    return( xleft );
}


static short PaintRight( short x, short y, short stop_color, short border_flag )
/*==============================================================================

    Paint right of starting pixel (x,y) using current color and fill
    style. Since we don't want to plot the initial pixel twice, move x
    by one to the right before plotting. Return the new left pixel. */

{
    short               xright;    
    scan_func           *scan;  /* pointer to scan function */
    gr_device _FARD     *dev_ptr;
    short               len;

    x += 1;                                 /* don't plot first pixel twice */
#if defined( __MEDIUM__ ) || defined( __LARGE__ )
    if( _flood_is_visited( x, y ) ) {
        return( x - 1 );
    }
#endif
    dev_ptr = _CurrState->deviceptr;
    ( *dev_ptr->setup )( x, y, stop_color );
    scan = dev_ptr->scanright;

    if( x <= _CurrState->clip.xmax ){
        xright = ( *scan )( _Screen.mem, _Screen.colour, x, _Screen.mask,
                       border_flag, _CurrState->clip.xmax );
    } else {
        xright = x - 1;
    }
    len = xright - x + 1;
    _L1Fill( x, y, len );
    _flood_visit( x, y, len );
    return( xright );
}


static void swap( unsigned k, struct frame *stack )
/*=================================================

    Swap frame k with frame 0. Frame k is now the active frame. */

{
    short           length;
    struct frame    temp;

    length = sizeof( struct frame );
    memcpy( &temp, &stack[0], length );
    memcpy( &stack[0], &stack[k], length );
    memcpy( &stack[k], &temp, length );
}


static short AddEntry( short ypos, short right, short left, short direction,
/*==================*/ unsigned max_frames, unsigned *stack_count,
                       struct frame *stack )

/*  Add an entry to the stack. Quit if out of stack space.  */

{
    if( *stack_count == max_frames ) {          /* stack is full    */
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return( TRUE );
    }
    *stack_count += 1;                          /* append to stack  */
    stack[ *stack_count ].left = left;
    stack[ *stack_count ].right = right;
    stack[ *stack_count ].ypos = ypos;
    stack[ *stack_count ].direction = direction;
    return( FALSE );
}


static char StackCompare( struct frame * stack, unsigned *stack_count )
/*=====================================================================

    Perform intersections, unions with entries already on the stack.
    Return with FALSE if entry completely cancelled or TRUE otherwise.  */

{
    short           startover;
    unsigned        count;
    unsigned        smallest;
    unsigned        curr;
    short           temp;

    if( *stack_count == 0 ) {                   /* nothing on the stack */
        stack[0].ypos += stack[0].direction;    /* go to next line in same  */
        return( TRUE );                         /* direction    */
    }
    do {
        /* Find shortest of the first TUNING non-active frames on the
           stack and the active frame.  */
        startover = FALSE;
        count = *stack_count;
        if( count > TUNING ) {
            count = TUNING;
        }
        smallest = 0;           /* assume smallest frame is the active one  */
        for( curr = 1; curr <= count; curr++ ) {
            if( stack[ curr ].right - stack[ curr ].left <
                stack[ smallest ].right - stack[ smallest ].left ) {
                smallest = curr;
            }
        }
        /* Interchange the smallest frame and the active frame. */
        if( smallest != 0 ) {
            swap( smallest, stack );
        }

        /* Check entries with opposite direction and same y */

        stack[0].ypos += stack[0].direction;        /* go to new line   */
        count = *stack_count;
        for( curr = 1; curr <= count; curr++ ) {
            if( stack[0].ypos == stack[ curr ].ypos &&
                stack[0].direction != stack[ curr ].direction ) {
                if( stack[0].left - 1 <= stack[ curr ].right    /* overlap? */
                    && stack[0].right + 1 >= stack[ curr ].left ) {
                    if( stack[0].right + 1 >= stack[ curr ].right ) {
                        temp = stack[0].left;
                        stack[0].left = stack[ curr ].right + 2;
                        if( temp + 1 < stack[ curr ].left ) {
                            stack[ curr ].right = stack[ curr ].left - 2;
                            stack[ curr ].left = temp - 1;
                            stack[ curr ].direction = stack[0].direction;
                            stack[ curr ].ypos -= stack[0].direction;
                        } else {
                            stack[ curr ].right = temp - 2;
                        }
                    } else {
                        temp = stack[ curr ].left;
                        stack[ curr ].left = stack[0].right + 2;
                        if( temp + 1 < stack[0].left ) {
                            stack[0].right = stack[0].left - 2;
                            stack[0].left = temp;
                            stack[0].direction = stack[ curr ].direction;
                            startover = TRUE;
                            break;
                        }
                        stack[0].right = temp - 2;
                    }
                    if( stack[0].left > stack[0].right ) {
                        return( NotValidFrame( curr, stack_count, stack ) );
                    }
                }
            }
        }
    } while ( startover );

    /* Check entries with same direction and same y */
    count = *stack_count;
    for( curr = 1; curr <= count; curr++ ) {
        if( stack[0].ypos == stack[ curr ].ypos &&
            stack[0].direction == stack[ curr ].direction ) {
            if( stack[0].left - 1 <= stack[ curr ].right &&     /* overlap? */
                stack[0].right + 1 >= stack[ curr ].left ) {
                if( stack[0].right - 1 <= stack[ curr ].right ) {
                    stack[0].right = stack[ curr ].left - 2;
                }
                if( stack[0].left + 1 >= stack[ curr ].left ) {
                    stack[0].left = stack[ curr ].right + 2;
                }
                if( stack[0].left > stack[0].right ) {
                    return( NotValidFrame( curr, stack_count, stack ) );
                }
            }
        }
    }
    return( TRUE );
}


static char NotValidFrame( unsigned curr, unsigned *stack_count,
/*======================*/ struct frame *stack )

/*  If the current frame is no longer valid then replace it by the last one.*/

{
    if( stack[ curr ].left > stack[ curr ].right ) {
        memcpy( &stack[ curr ], &stack[ *stack_count ], sizeof( struct frame ) );
        *stack_count -= 1;
    }
    return( FALSE );                /* signal no direction  */
}

#endif
