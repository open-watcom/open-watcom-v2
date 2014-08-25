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
* Description:  Drawing character definitions
*
****************************************************************************/


//                         IBM     DOS/V   NEC,    internal
pick( BLANK,               '\x20', '\x20', '\x20', '\x20' )
pick( VERT_FRAME_DBL,      '\xba', '\x05', '\x04', UI_DVLINE )
pick( HOR_FRAME_DBL,       '\xcd', '\x06', '\x02', UI_DHLINE )
pick( VERT_FRAME,          '\xb3', '\x05', '\x04', UI_VLINE )
pick( HOR_FRAME,           '\xc4', '\x06', '\x02', UI_HLINE )
pick( LEFT_TITLE_MARK,     '\xb4', '\x17', '\x13', UI_RTEE )
pick( RIGHT_TITLE_MARK,    '\xc3', '\x19', '\x11', UI_LTEE )
pick( VERT_LINE,           '\xb3', '\x05', '\x04', UI_VLINE )
pick( HOR_LINE,            '\xc4', '\x06', '\x04', UI_HLINE )
pick( LEFT_HALF,           '\xdd', '\x20', '\x20', UI_LBLOCK )
pick( RIGHT_HALF,          '\xde', '\x20', '\x20', UI_RBLOCK )
pick( TOP_HALF,            '\xdf', '\x0e', '\x20', UI_UBLOCK )
pick( RESIZE,              '\x12', '\x12', '\x2a', UI_UDARROW )
pick( UL_CORNER,           '\xda', '\x01', '\x07', UI_ULCORNER )
pick( UR_CORNER,           '\xbf', '\x02', '\x09', UI_URCORNER )
pick( LL_CORNER,           '\xc0', '\x03', '\x0c', UI_LLCORNER )
pick( LR_CORNER,           '\xd9', '\x04', '\x0f', UI_LRCORNER )
pick( MAXIMIZE,            '\x1e', '\x1c', '\x1e', UI_UPOINT )
pick( MINIMIZE,            '\x1f', '\x07', '\x1f', UI_DPOINT )
pick( CLOSER,              '\xfe', '\x0e', '\x3d', UI_SQUARE )
pick( VERT_SCROLL,         '\xba', '\x05', '\x04', UI_DVLINE )
pick( HORZ_SCROLL,         '\xcd', '\x06', '\x02', UI_DHLINE )
pick( BLOCK,               '\xdb', '\x14', '\x23', UI_BLOCK )
pick( SLIDER,              '\xb1', '\x1a', '\x23', UI_BOARD )
pick( BLOCK_SLIDER,        '\xb0', '\x1a', '\x04', UI_CKBOARD )
pick( LEFT_POINT,          '\x11', '\x1f', '\x1d', UI_LPOINT )
pick( RIGHT_POINT,         '\x10', '\x1e', '\x1c', UI_RPOINT )
pick( UP_POINT,            '\x1e', '\x1c', '\x1e', UI_UPOINT )
pick( DOWN_POINT,          '\x1f', '\x07', '\x1f', UI_DPOINT )
pick( C2,                  '\xc2', '\xc2', '\xc2', UI_TTEE )
