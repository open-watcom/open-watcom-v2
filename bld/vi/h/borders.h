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
* Description:  Window border characters data pick file
*
****************************************************************************/


/*       enum             UnixNG        UnixG            DosNG    DosG  */
vi_pick( WB_TOPLEFT,      UI_ULCORNER,  UI_EQUIVALENT,  '\xDA',  '\xF0' )
vi_pick( WB_TOPRIGHT,     UI_URCORNER,  UI_URCORNER,    '\xBF',  '\xBF' )
vi_pick( WB_BOTTOMLEFT,   UI_LLCORNER,  UI_LLCORNER,    '\xC0',  '\xC0' )
vi_pick( WB_BOTTOMRIGHT,  UI_LRCORNER,  UI_UDARROW,     '\xD9',  '\x12' )
vi_pick( WB_LEFTSIDE,     UI_VLINE,     UI_VLINE,       '\xB3',  '\xB3' )
vi_pick( WB_TOPBOTTOM,    UI_HLINE,     UI_HLINE,       '\xC4',  '\xC4' )
vi_pick( WB_RIGHTT,       UI_RTEE,      UI_RTEE,        '\xB4',  '\xB4' )
vi_pick( WB_LEFTT,        UI_LTEE,      UI_LTEE,        '\xC3',  '\xC3' )
vi_pick( WB_UPTRIANGLE,   UI_UPOINT,    UI_UPOINT,      '\x1E',  '\x1E' )
vi_pick( WB_DOWNTRIANGLE, UI_DPOINT,    UI_DPOINT,      '\x1F',  '\x1F' )
vi_pick( WB_RIGHTSIDE,    UI_VLINE,     UI_CKBOARD,     '\xB3',  '\xB0' )
vi_pick( WB_THUMB,        UI_BLOCK,     UI_BLOCK,       '\xDB',  '\xDB' )
