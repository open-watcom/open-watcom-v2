/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  pick file with UI attribute id data definition
*
****************************************************************************/


/*    id                vga                                 colour                              bw                              mono                        local                       */
pick( MENU,             _attr( VGA_MENU,    VGA_BLACK ),  _attr( C_MENU,    C_BLUE ),           _attr( BG_WHITE, FG_BLACK ),  _attr( WHITE, BLACK ),        _attr( BLACK, BR_WHITE )     )
pick( ACTIVE,           _attr( VGA_MENU,    VGA_BLACK ),  _attr( C_MENU,    C_BLUE ),           _attr( BG_WHITE, FG_BLACK ),  _attr( WHITE, BLACK ),        _attr( BLACK, BR_WHITE )     )
pick( CURR_ACTIVE,      _attr( VGA_CMENU,   VGA_AQUA ),   _attr( C_CMENU,   C_WHITE ),          _attr( BG_GREY5, FG_WHITE ),  _attr( BLACK, BR_WHITE ),     _attr( WHITE, BLACK )        )
pick( INACTIVE,         _attr( VGA_MENU,    VGA_GRAY ),   _attr( C_MENU,    C_BR_BLACK ),       _attr( BG_WHITE, FG_GREY10 ), _attr( WHITE, BR_WHITE ),     _attr( BLACK, WHITE )        )
pick( CURR_INACTIVE,    _attr( VGA_CMENU,   VGA_GRAY ),   _attr( C_CMENU,   C_WHITE ),          _attr( BG_GREY5, FG_GREY10 ), _attr( BLACK, WHITE ),        _attr( WHITE, BLACK )        )
pick( SHADOW,           _attr( VGA_BLACK,   VGA_WHITE ),  _attr( C_BACK,    C_BLACK ),          _attr( BG_BLACK, FG_WHITE ),  _attr( BLACK, BR_WHITE ),     _attr( BLACK, BR_WHITE )     )
pick( DEFAULT_HOTSPOT,  _attr( VGA_MENU,    VGA_BLUE ),   _attr( C_WHITE,   C_BR_WHITE_ON_WH ), _attr( BG_WHITE, FG_GREY14 ), _attr( WHITE, BR_WHITE ),     _attr( BLACK, BR_UNDERLINE ) )
pick( NORMAL,           _attr( VGA_WHITE,   VGA_BLACK ),  _attr( C_BACK,    C_WHITE ),          _attr( BG_BLACK, FG_GREY10 ), _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( BRIGHT,           _attr( VGA_WHITE,   VGA_GRAY ),   _attr( C_BACK,    C_BR_WHITE ),       _attr( BG_BLACK, FG_WHITE ),  _attr( BLACK, BR_WHITE ),     _attr( BLACK, BR_WHITE )     )
pick( EDIT,             _attr( VGA_SILVER,  VGA_BLACK ),  _attr( C_WHITE,   C_BLACK ),          _attr( BG_GREY5, FG_GREY10 ), _attr( BLACK, UNDERLINE ),    _attr( BLACK, UNDERLINE )    )
pick( REVERSE,          _attr( VGA_BLACK,   VGA_WHITE ),  _attr( C_CYAN,    C_BLACK ),          _attr( BG_WHITE, FG_BLACK ),  _attr( WHITE, BLACK ),        _attr( WHITE, BLACK )        )
pick( EDIT2,            _attr( VGA_WHITE,   VGA_BLACK ),  _attr( C_WHITE,   C_BLACK ),          _attr( BG_GREY5, FG_GREY13 ), _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( HOTSPOT,          _attr( VGA_SILVER,  VGA_BLACK ),  _attr( C_WHITE,   C_BLUE ),           _attr( BG_WHITE, FG_BLACK ),  _attr( WHITE, BLACK ),        _attr( WHITE, BLACK )        )
pick( HELP,             _attr( VGA_BLACK,   VGA_WHITE ),  _attr( C_BLACK,   C_BR_WHITE ),       _attr( BG_BLACK, FG_WHITE ),  _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( HOT,              _attr( VGA_MENU,    VGA_BLUE ),   _attr( C_MENU,    C_BR_WHITE_ON_WH ), _attr( BG_WHITE, FG_GREY14 ), _attr( WHITE, BR_WHITE ),     _attr( BLACK, BR_UNDERLINE ) )
pick( HOT_CURR,         _attr( VGA_CMENU,   VGA_WHITE ),  _attr( C_CMENU,   C_BR_WHITE ),       _attr( BG_GREY5, FG_GREY14 ), _attr( BLACK, BR_WHITE ),     _attr( WHITE, UNDERLINE )    )
pick( HOT_QUIET,        _attr( VGA_MENU,    VGA_BLUE ),   _attr( C_MENU,    C_BR_WHITE_ON_WH ), _attr( BG_WHITE, FG_GREY14 ), _attr( WHITE, BR_WHITE ),     _attr( BLACK, UNDERLINE )    )
pick( CURR_EDIT,        _attr( VGA_GRAY,    VGA_WHITE ),  _attr( C_BLACK,   C_BR_WHITE ),       _attr( BG_GREY3, FG_GREY14 ), _attr( BLACK, BR_UNDERLINE ), _attr( BLACK, BR_UNDERLINE ) )
pick( CURR_MARK_EDIT,   _attr( VGA_FUCHSIA, VGA_BLACK ),  _attr( C_MAGENTA, C_BLACK ),          0,                            0,                            0                            )
pick( MARK_NORMAL,      _attr( VGA_TEAL,    VGA_BLACK ),  _attr( C_CYAN,    C_BLACK ),          _attr( BG_WHITE, FG_BLACK ),  _attr( WHITE, BLACK ),        _attr( WHITE, BLACK )        )
pick( MARK_EDIT,        _attr( VGA_OLIVE,   VGA_BLACK ),  _attr( C_BROWN,   C_BLACK ),          0,                            0,                            0                            )
pick( CURR_HOTSPOT_KEY, _attr( VGA_GRAY,    VGA_BLUE ),   _attr( C_CYAN,    C_BR_WHITE ),       _attr( BG_WHITE, FG_GREY14 ), _attr( WHITE, BR_WHITE ),     _attr( BLACK, BR_UNDERLINE ) )
pick( EDIT_DIAL,        _attr( VGA_BLACK,   VGA_AQUA ),   _attr( C_BLACK,   C_CYAN ),           _attr( BG_BLACK, FG_GREY14 ), _attr( BLACK, BR_WHITE ),     _attr( BLACK, BR_WHITE )     )
pick( SELECT_DIAL,      0,                                0,                                    0,                            0,                            0                            )
pick( CURR_EDIT_DIAL,   0,                                0,                                    0,                            0,                            0                            )
pick( CURR_SELECT_DIAL, _attr( VGA_NAVY,    VGA_WHITE ),  _attr( C_BLUE,    C_WHITE ),          0,                            0,                            0                            )
pick( FRAME,            _attr( VGA_WHITE,   VGA_SILVER ), _attr( C_BACK,    C_WHITE ),          _attr( BG_BLACK, FG_GREY13 ), _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( SCROLL_ICON,      _attr( VGA_SILVER,  VGA_BLACK ),  _attr( C_WHITE,   C_BLACK ),          _attr( BG_GREY5, FG_GREY14 ), _attr( WHITE, BLACK ),        _attr( WHITE, BLACK )        )
pick( SCROLL_BAR,       _attr( VGA_SILVER,  VGA_BLACK ),  _attr( C_BROWN,   C_BR_WHITE ),       _attr( BG_BLACK, FG_GREY12 ), _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( DIAL_FRAME,       _attr( VGA_WHITE,   VGA_SILVER ), _attr( C_BACK,    C_CYAN ),           _attr( BG_BLACK, FG_GREY14 ), _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( BROWSE,           _attr( VGA_BLACK,   VGA_WHITE ),  _attr( C_BLACK,   C_WHITE ),          _attr( BG_BLACK, FG_GREY12 ), _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( CURR_HOTSPOT,     _attr( VGA_BLACK,   VGA_WHITE ),  _attr( C_CYAN,    C_BLUE ),           _attr( BG_WHITE, FG_GREY14 ), _attr( WHITE, BR_WHITE ),     _attr( BLACK, BR_UNDERLINE ) )
pick( ERROR,            _attr( VGA_BLACK,   VGA_RED ),    _attr( C_BACK,    C_BR_RED ),         _attr( BG_BLACK, FG_WHITE ),  _attr( BLACK, BR_WHITE ),     _attr( BLACK, BR_WHITE )     )
pick( HINT,             _attr( VGA_GREEN,   VGA_WHITE ),  _attr( C_CYAN,    C_BR_WHITE ),       _attr( BG_WHITE, FG_BLACK ),  _attr( WHITE, BLACK ),        _attr( WHITE, BLACK )        )
pick( WARNING,          _attr( VGA_BLACK,   VGA_RED ),    _attr( C_BACK,    C_BR_RED ),         _attr( BG_BLACK, FG_WHITE ),  _attr( BLACK, BR_WHITE ),     _attr( BLACK, BR_WHITE )     )
pick( OFF_HOTSPOT,      _attr( VGA_WHITE,   VGA_WHITE ),  _attr( C_WHITE,   C_BR_BLUE ),        _attr( BG_BLACK, FG_WHITE ),  _attr( BLACK, WHITE ),        _attr( BLACK, WHITE )        )
pick( RADIO_HOTSPOT,    _attr( VGA_WHITE,   VGA_BLUE ),   _attr( C_BACK,    C_BR_WHITE ),       _attr( BG_BLACK, FG_GREY14 ), _attr( BLACK, BR_UNDERLINE ), _attr( BLACK, BR_UNDERLINE ) )

