/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GUI Drawing character mapping definitions
*
****************************************************************************/


/*  Character                  Class           */
/* group of active/inactive state character pairs */
pick( BLANK )               // GUI_FRAME_TOP
pick( HOR_FRAME )           // GUI_INACT_FRAME_TOP
pick( BLANK )               // GUI_FRAME_UL_CORNER
pick( UL_CORNER )           // GUI_INACT_FRAME_UL_CORNER
pick( BLANK )               // GUI_FRAME_LEFT
pick( VERT_FRAME )          // GUI_INACT_FRAME_LEFT
pick( BLANK )               // GUI_FRAME_LL_CORNER
pick( LL_CORNER )           // GUI_INACT_FRAME_LL_CORNER
pick( BLANK )               // GUI_FRAME_BOTTOM
pick( HOR_FRAME )           // GUI_INACT_FRAME_BOTTOM
pick( BLANK )               // GUI_FRAME_LR_CORNER
pick( LR_CORNER )           // GUI_INACT_FRAME_LR_CORNER
pick( BLANK )               // GUI_FRAME_RIGHT
pick( VERT_FRAME )          // GUI_INACT_FRAME_RIGHT
pick( BLANK )               // GUI_FRAME_UR_CORNER
pick( UR_CORNER )           // GUI_INACT_FRAME_UR_CORNER
pick( VERT_LINE )           // GUI_LR_VERT_BAR
pick( RIGHT_TITLE_MARK )    // GUI_INACT_LR_VERT_BAR
pick( HOR_LINE )            // GUI_LR_HORZ_BAR
pick( C2 )                  // GUI_INACT_LR_HORZ_BAR
pick( BLANK )               // GUI_LEFT_TITLE_MARK
pick( LEFT_TITLE_MARK )     // GUI_INACT_LEFT_TITLE_MARK
pick( BLANK )               // GUI_RIGHT_TITLE_MARK
pick( RIGHT_TITLE_MARK )    // GUI_INACT_RIGHT_TITLE_MARK
pick( VERT_LINE )           // GUI_LEFT_GADGET_MARK
pick( LEFT_TITLE_MARK )     // GUI_INACT_LEFT_GADGET_MARK
pick( VERT_LINE )           // GUI_RIGHT_GADGET_MARK
pick( RIGHT_TITLE_MARK )    // GUI_INACT_RIGHT_GADGET_MARK
pick( BLANK )               // GUI_TITLE_SPACE
pick( BLANK )               // GUI_INACT_TITLE_SPACE
pick( CLOSER )              // GUI_CLOSER
pick( CLOSER )              // GUI_INACT_CLOSER
pick( MAXIMIZE )            // GUI_MAXIMIZE_GADGET
pick( MAXIMIZE )            // GUI_INACT_MAXIMIZE_GADGET
pick( MINIMIZE )            // GUI_MINIMIZE_GADGET
pick( MINIMIZE )            // GUI_INACT_MINIMIZE_GADGET
pick( RESIZE )              // GUI_RESIZE_GADGET
pick( RESIZE )              // GUI_INACT_RESIZE_GADGET
pick( BLANK )               // GUI_HOR_SCROLL
pick( HOR_FRAME_DBL )       // GUI_INACT_HOR_SCROLL
pick( BLANK )               // GUI_VERT_SCROLL
pick( VERT_FRAME_DBL )      // GUI_INACT_VERT_SCROLL
pick( LEFT_POINT )          // GUI_LEFT_SCROLL_ARROW
pick( LEFT_POINT )          // GUI_INACT_LEFT_SCROLL_ARROW
pick( RIGHT_POINT )         // GUI_RIGHT_SCROLL_ARROW
pick( RIGHT_POINT )         // GUI_INACT_RIGHT_SCROLL_ARROW
pick( UP_POINT )            // GUI_UP_SCROLL_ARROW
pick( UP_POINT )            // GUI_INACT_UP_SCROLL_ARROW
pick( DOWN_POINT )          // GUI_DOWN_SCROLL_ARROW
pick( DOWN_POINT )          // GUI_INACT_DOWN_SCROLL_ARROW
pick( SLIDER )              // GUI_SCROLL_SLIDER
pick( BLOCK )               // GUI_INACT_SCROLL_SLIDER
/* group of dialog characters (without state) */
pick( BLOCK_SLIDER )        // GUI_DIAL_VERT_SCROLL
pick( UP_POINT )            // GUI_DIAL_UP_SCROLL_ARROW
pick( DOWN_POINT )          // GUI_DIAL_DOWN_SCROLL_ARROW
pick( BLOCK )               // GUI_DIAL_SCROLL_SLIDER
/* group of auxiliary characters (without state) */
pick( BLOCK )               // GUI_RECT_AREA
pick( TOP_HALF )            // GUI_BAR_AREA
pick( VERT_FRAME )          // GUI_LINE_VERT
pick( HOR_FRAME )           // GUI_LINE_HOR
pick( RIGHT_HALF )          // GUI_BOX_LEFT
pick( LEFT_HALF )           // GUI_BOX_RIGHT
