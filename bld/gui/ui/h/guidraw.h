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


#ifndef draw_pick
    extern char DrawingChars[];
    extern void GUIInitDrawingChars( bool dbcs );
    #define DRAW( x ) DrawingChars[ DRAW_##x ]
    #define DRAW_ENUM
    /* SEE NOTE BELOW IF YOU CHANGE THE NEXT LINE */
    #define draw_pick( a,b,c,d ) DRAW_##a,
    enum {
#endif

//                              IBM     DOS/V   NEC
draw_pick( BLANK,               '\x20', '\x20', '\x20' )
draw_pick( VERT_FRAME_DBL,      '\xba', '\x05', '\x04' )
draw_pick( HOR_FRAME_DBL,       '\xcd', '\x06', '\x02' )
draw_pick( VERT_FRAME,          '\xb3', '\x05', '\x04' )
draw_pick( HOR_FRAME,           '\xc4', '\x06', '\x02' )
draw_pick( LEFT_TITLE_MARK,     '\xb4', '\x17', '\x13' )
draw_pick( RIGHT_TITLE_MARK,    '\xc3', '\x19', '\x11' )
draw_pick( LEFT_TITLE_MARK_DBL, '\xb5', '\x17', '\x13' )
draw_pick( RIGHT_TITLE_MARK_DBL,'\xc6', '\x19', '\x11' )
draw_pick( VERT_LINE,           '\xb3', '\x05', '\x04' )
draw_pick( HOR_LINE,            '\xc4', '\x06', '\x04' )
draw_pick( LEFT_HALF,           '\xdd', '\x20', '\x20' )
draw_pick( RIGHT_HALF,          '\xde', '\x20', '\x20' )
draw_pick( TOP_HALF,            '\xdf', '\x0e', '\x20' )
draw_pick( BOTTOM_HALF,         '\xdc', '\x20', '\x20' )
draw_pick( RESIZE,              '\x12', '\x12', '\x2a' )
draw_pick( UL_CORNER,           '\xda', '\x01', '\x07' )
draw_pick( UR_CORNER,           '\xbf', '\x02', '\x09' )
draw_pick( LL_CORNER,           '\xc0', '\x03', '\x0c' )
draw_pick( LR_CORNER,           '\xd9', '\x04', '\x0f' )
draw_pick( UL_RESIZE_CORNER,    '\xc9', '\x01', '\x07' )
draw_pick( UR_RESIZE_CORNER,    '\xbb', '\x02', '\x09' )
draw_pick( LL_RESIZE_CORNER,    '\xc8', '\x03', '\x0c' )
draw_pick( LR_RESIZE_CORNER,    '\xbc', '\x04', '\x0f' )
draw_pick( MAXIMIZE,            '\x1e', '\x1c', '\x1e' )
draw_pick( MINIMIZE,            '\x1f', '\x07', '\x1f' )
draw_pick( CLOSER,              '\xfe', '\x0e', '\x3d' )
draw_pick( VERT_SCROLL,         '\xba', '\x05', '\x04' )
draw_pick( HORZ_SCROLL,         '\xcd', '\x06', '\x02' )
draw_pick( BLOCK,               '\xdb', '\x14', '\x23' )
draw_pick( SLIDER,              '\xb1', '\x1a', '\x23' )
draw_pick( BLOCK_SLIDER,        '\xb0', '\x1a', '\x04' )
draw_pick( LEFT_ARROW,          '\x1b', '\x1f', '\x1d' )
draw_pick( RIGHT_ARROW,         '\x1a', '\x1e', '\x1c' )
draw_pick( UP_ARROW,            '\x18', '\x1c', '\x1e' )
draw_pick( DOWN_ARROW,          '\x19', '\x07', '\x1f' )
draw_pick( LEFT_POINT,          '\x11', '\x1f', '\x1d' )
draw_pick( RIGHT_POINT,         '\x10', '\x1e', '\x1c' )
draw_pick( UP_POINT,            '\x1e', '\x1c', '\x1e' )
draw_pick( DOWN_POINT,          '\x1f', '\x07', '\x1f' )
draw_pick( C2,                  '\xc2', '\xc2', '\xc2' )

/*
 * NOTE: This is done because some platforms (such as AIX)
 * does not like having an ',' (comman) after the last
 * enumerated entry
 */
#ifdef DRAW_ENUM
        #undef draw_pick
        #define draw_pick( a,b,c,d ) DRAW_##a
#endif

draw_pick( LAST,                0,      0,      0 )

#ifdef DRAW_ENUM
        #undef draw_pick
        #define draw_pick( a,b,c,d ) DRAW_##a,
#endif


#ifdef DRAW_ENUM
    };
    #undef DRAW_ENUM
#endif

#undef draw_pick
