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
* Description:  Default ui lib attribute values.
*
****************************************************************************/


#include <string.h>
#include "uidef.h"
#include "uiattrs.h"

#if defined( __WATCOMC__ ) && defined( _M_IX86 )
   #define PROGRAM_VGA
#endif

#if defined( PROGRAM_VGA )
#pragma aux SetColourRegister =         \
    "   MOV     AX, 1010h   "           \
    "   INT     10h         "           \
        parm caller [bx] [dh] [ch] [cl] \
        modify      [ax];

#pragma aux SetBlinkAttr =              \
    "   MOV     AX, 1003h   "           \
    "   INT     10h         "           \
        parm caller [bl]                \
        modify      [ax];

static  char    BlinkAttr       = 1;
void    SetBlinkAttr( char );
void    SetColourRegister( unsigned, char, char, char );

a_colour    VGAcolours[16] = {
//      RED   GREEN    BLUE
    {   0,      0,      0   },      //  Black
    {   32,     32,     32  },      //  Gray
    {   47,     47,     47  },      //  Silver
    {   0,      0,      32  },      //  Navy
    {   0,      0,      63  },      //  Blue
    {   0,      63,     63  },      //  Aqua
    {   32,     0,      0   },      //  Maroon
    {   63,     0,      0   },      //  Red
    {   32,     0,      32  },      //  Purple
    {   63,     0,      63  },      //  Fuchsia
    {   0,      32,     32  },      //  Teal
    {   0,      32,     0   },      //  Green
    {   0,      63,     0   },      //  Lime
    {   32,     32,     0   },      //  Olive
    {   63,     63,     0   },      //  Yellow
    {   63,     63,     63  }       //  White
};

#define VGA_MENU    VGA_SILVER
#define VGA_CMENU   VGA_BLUE

ATTR ui_vga_colour_attrs[] =
{       _bg( VGA_MENU  ) |  _fg( VGA_BLACK )   // MENU
,       _bg( VGA_MENU  ) |  _fg( VGA_BLACK )   // ACTIVE
,       _bg( VGA_CMENU ) |  _fg( VGA_AQUA  )   // CURR_ACTIVE
,       _bg( VGA_MENU  ) |  _fg( VGA_GRAY  )   // INACTIVE
,       _bg( VGA_CMENU ) |  _fg( VGA_GRAY  )   // CURR_INACTIVE
,       _bg( VGA_BLACK ) |  _fg( VGA_WHITE )   // SHADOW
,       _bg( VGA_MENU  ) |  _fg( VGA_BLUE  )   // DEFAULT_HOTSPOT
,       _bg( VGA_WHITE ) |  _fg( VGA_BLACK )   // NORMAL
,       _bg( VGA_WHITE ) |  _fg( VGA_GRAY  )   // BRIGHT
,       _bg( VGA_SILVER) |  _fg( VGA_BLACK )   // EDIT
,       _bg( VGA_BLACK ) |  _fg( VGA_WHITE )   // REVERSE
,       _bg( VGA_WHITE ) |  _fg( VGA_BLACK )   // EDIT2
,       _bg( VGA_SILVER) |  _fg( VGA_BLACK )   // HOTSPOT
,       _bg( VGA_BLACK ) |  _fg( VGA_WHITE )   // HELP
,       _bg( VGA_MENU  ) |  _fg( VGA_BLUE  )   // HOT
,       _bg( VGA_CMENU ) |  _fg( VGA_WHITE )   // HOT_CURR
,       _bg( VGA_MENU  ) |  _fg( VGA_BLUE  )   // HOT_QUIET
,       _bg( VGA_GRAY  ) |  _fg( VGA_WHITE )   // CURR_EDIT
,       _bg( VGA_FUCHSIA)|  _fg( VGA_BLACK )   // CURR_MARK_EDIT
,       _bg( VGA_TEAL  ) |  _fg( VGA_BLACK )   // MARK_NORMAL
,       _bg( VGA_OLIVE ) |  _fg( VGA_BLACK )   // MARK_EDIT
,       _bg( VGA_GRAY  ) |  _fg( VGA_BLUE  )   // CURR_HOTSPOT_KEY
,       _bg( VGA_BLACK ) |  _fg( VGA_AQUA  )   // EDIT_DIAL
,       0                                      // SELECT_DIAL
,       0                                      // CURR_EDIT_DIAL
,       _bg( VGA_NAVY )  |  _fg( VGA_WHITE )   // CURR_SELECT_DIAL
,       _bg( VGA_WHITE ) |  _fg( VGA_SILVER)   // FRAME
,       _bg( VGA_SILVER) |  _fg( VGA_BLACK )   // SCROLL_ICON
,       _bg( VGA_SILVER) |  _fg( VGA_BLACK )   // SCROLL_BAR
,       _bg( VGA_WHITE)  |  _fg( VGA_SILVER)   // DIAL_FRAME
,       _bg( VGA_BLACK ) |  _fg( VGA_WHITE )   // BROWSE
,       _bg( VGA_GRAY  ) |  _fg( VGA_WHITE )   // CURR_HOTSPOT
,       _bg( VGA_BLACK ) |  _fg( VGA_RED   )   // ERROR
,       _bg( VGA_GREEN ) |  _fg( VGA_WHITE )   // HINT
,       _bg( VGA_BLACK ) |  _fg( VGA_RED   )   // WARNING
,       _bg( VGA_WHITE ) |  _fg( VGA_WHITE )   // OFF_HOTSPOT
,       _bg( VGA_WHITE ) |  _fg( VGA_BLUE  )   // RADIO_HOTSPOT
};
#endif

#define C_MENU      C_WHITE
#define C_CMENU     C_BLACK
#define C_BACK      C_BLUE

#ifdef __UNIX__
    #define C_BRIGHT_WHITE_ON_WHITE C_BROWN
#else
    #define C_BRIGHT_WHITE_ON_WHITE (BRIGHT | C_WHITE)
#endif

ATTR ui_colour_attrs[] =
{       _bg( C_MENU  ) |  _fg(          C_BLUE  )   // MENU
,       _bg( C_MENU  ) |  _fg(          C_BLUE  )   // ACTIVE
,       _bg( C_CMENU ) |  _fg(          C_WHITE )   // CURR_ACTIVE
,       _bg( C_MENU  ) |  _fg( BRIGHT | C_BLACK )   // INACTIVE
,       _bg( C_CMENU ) |  _fg(          C_WHITE )   // CURR_INACTIVE
,       _bg( C_BACK  ) |  _fg(          C_BLACK )   // SHADOW
,       _bg( C_WHITE ) |  _fg( C_BRIGHT_WHITE_ON_WHITE )    // DEFAULT_HOTSPOT
,       _bg( C_BACK  ) |  _fg(          C_WHITE )   // NORMAL
,       _bg( C_BACK  ) |  _fg( BRIGHT | C_WHITE )   // BRIGHT
,       _bg( C_WHITE ) |  _fg(          C_BLACK )   // EDIT
,       _bg( C_CYAN  ) |  _fg(          C_BLACK )   // REVERSE
,       _bg( C_WHITE ) |  _fg(          C_BLACK )   // EDIT2
,       _bg( C_WHITE ) |  _fg(          C_BLUE  )   // HOTSPOT
,       _bg( C_BLACK ) |  _fg( BRIGHT | C_WHITE )   // HELP
,       _bg( C_MENU  ) |  _fg( C_BRIGHT_WHITE_ON_WHITE )    // HOT
,       _bg( C_CMENU ) |  _fg( BRIGHT | C_WHITE )   // HOT_CURR
,       _bg( C_MENU  ) |  _fg( C_BRIGHT_WHITE_ON_WHITE )    // HOT_QUIET
,       _bg( C_BLACK ) |  _fg( BRIGHT | C_WHITE )   // CURR_EDIT
,       _bg( C_MAGENTA)|  _fg(          C_BLACK )   // CURR_MARK_EDIT
,       _bg( C_CYAN  ) |  _fg(          C_BLACK )   // MARK_NORMAL
,       _bg( C_BROWN ) |  _fg(          C_BLACK )   // MARK_EDIT
,       _bg( C_CYAN  ) |  _fg( BRIGHT | C_WHITE )   // CURR_HOTSPOT_KEY
,       _bg( C_BLACK ) |  _fg(          C_CYAN  )   // EDIT_DIAL
,       0                                           // SELECT_DIAL
,       0                                           // CURR_EDIT_DIAL
,       _bg( C_BLUE  ) |  _fg(          C_WHITE )   // CURR_SELECT_DIAL
,       _bg( C_BACK  ) |  _fg(          C_WHITE )   // FRAME
,       _bg( C_WHITE ) |  _fg(          C_BLACK )   // SCROLL_ICON
,       _bg( C_BROWN ) |  _fg( BRIGHT | C_WHITE )   // SCROLL_BAR
,       _bg( C_BACK  ) |  _fg(          C_CYAN  )   // DIAL_FRAME
,       _bg( C_BLACK ) |  _fg(          C_WHITE )   // BROWSE
,       _bg( C_CYAN  ) |  _fg(          C_BLUE  )   // CURR_HOTSPOT
,       _bg( C_BACK  ) |  _fg( BRIGHT | C_RED   )   // ERROR
,       _bg( C_CYAN  ) |  _fg( BRIGHT | C_WHITE )   // HINT
,       _bg( C_BACK  ) |  _fg( BRIGHT | C_RED   )   // WARNING
,       _bg( C_WHITE ) |  _fg( BRIGHT | C_BLUE  )   // OFF_HOTSPOT
,       _bg( C_BACK  ) |  _fg( BRIGHT | C_WHITE )   // RADIO_HOTSPOT
};

ATTR ui_bw_attrs[] =
{       _bg( BG_WHITE ) | _fg( FG_BLACK  )          // MENU
,       _bg( BG_WHITE ) | _fg( FG_BLACK  )          // ACTIVE
,       _bg( BG_GREY5 ) | _fg( FG_WHITE  )          // CURR_ACTIVE
,       _bg( BG_WHITE ) | _fg( FG_GREY10 )          // INACTIVE
,       _bg( BG_GREY5 ) | _fg( FG_GREY10 )          // CURR_INACTIVE
,       _bg( BG_BLACK ) | _fg( FG_WHITE  )          // DIALOGUE
,       _bg( BG_WHITE ) | _fg( FG_GREY14 )          // DEFAULT_HOTSPOT
,       _bg( BG_BLACK ) | _fg( FG_GREY10 )          // NORMAL
,       _bg( BG_BLACK ) | _fg( FG_WHITE  )          // BRIGHT
,       _bg( BG_GREY5 ) | _fg( FG_GREY10 )          // EDIT
,       _bg( BG_WHITE ) | _fg( FG_BLACK  )          // REVERSE
,       _bg( BG_GREY5 ) | _fg( FG_GREY13 )          // EDIT2
,       _bg( BG_WHITE ) | _fg( FG_BLACK  )          // HOTSPOT
,       _bg( BG_BLACK ) | _fg( FG_WHITE  )          // HELP
,       _bg( BG_WHITE ) | _fg( FG_GREY14 )          // HOT
,       _bg( BG_GREY5 ) | _fg( FG_GREY14 )          // HOT_CURR
,       _bg( BG_WHITE ) | _fg( FG_GREY14 )          // HOT_QUIET
,       _bg( BG_GREY3 ) | _fg( FG_GREY14 )          // CURR_EDIT
,       0                                           // CURR_MARK_EDIT
,       _bg( BG_WHITE ) | _fg( FG_BLACK  )          // MARK_NORMAL
,       0                                           // MARK_EDIT
,       _bg( BG_WHITE ) | _fg( FG_GREY14 )          // CURR_HOTSPOT_KEY
,       _bg( BG_BLACK ) | _fg( FG_GREY14 )          // EDIT_DIAL
,       0                                           // SELECT_DIAL
,       0                                           // CURR_EDIT_DIAL
,       0                                           // CURR_SELECT_DIAL
,       _bg( BG_BLACK ) | _fg( FG_GREY13 )          // FRAME
,       _bg( BG_GREY5 ) | _fg( FG_GREY14 )          // SCROLL_ICON
,       _bg( BG_BLACK ) | _fg( FG_GREY12 )          // SCROLL_BAR
,       _bg( BG_BLACK ) | _fg( FG_GREY14 )          // DIAL_FRAME
,       _bg( BG_BLACK ) | _fg( FG_GREY12 )          // BROWSE
,       _bg( BG_WHITE ) | _fg( FG_GREY14 )          // CURR_HOTSPOT
,       _bg( BG_BLACK ) | _fg( FG_WHITE  )          // ERROR
,       _bg( BG_WHITE ) | _fg( FG_BLACK  )          // HINT
,       _bg( BG_BLACK ) | _fg( FG_WHITE  )          // WARNING
,       _bg( BG_BLACK ) | _fg( FG_WHITE  )          // OFF_HOTSPOT
,       _bg( BG_BLACK ) | _fg( FG_GREY14 )          // RADIO_HOTSPOT
};

ATTR ui_mono_attrs[] =
{       _bg( WHITE )    | _fg( BLACK )              // MENU
,       _bg( WHITE )    | _fg( BLACK )              // ACTIVE
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // CURR_ACTIVE
,       _bg( WHITE )    | _fg( BRIGHT | WHITE )     // INACTIVE
,       _bg( BLACK )    | _fg( WHITE )              // CURR_INACTIVE
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // DIALOGUE
,       _bg( WHITE )    | _fg( BRIGHT | WHITE )     // DEFAULT_HOTSPOT
,       _bg( BLACK )    | _fg( WHITE )              // NORMAL
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // BRIGHT
,       _bg( BLACK )    | _fg( UNDERLINE )          // EDIT
,       _bg( WHITE )    | _fg( BLACK )              // REVERSE
,       _bg( BLACK )    | _fg( WHITE )              // EDIT2
,       _bg( WHITE )    | _fg( BLACK )              // HOTSPOT
,       _bg( BLACK )    | _fg( WHITE )              // HELP
,       _bg( WHITE )    | _fg( BRIGHT | WHITE )     // HOT
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // HOT_CURR
,       _bg( WHITE )    | _fg( BRIGHT | WHITE )     // HOT_QUIET
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // CURR_EDIT
,       0                                           // CURR_MARK_EDIT
,       _bg( WHITE )    | _fg( BLACK )              // MARK_NORMAL
,       0                                           // MARK_EDIT
,       _bg( WHITE )    | _fg( BRIGHT | WHITE )     // CURR_HOTSPOT_KEY
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // EDIT_DIAL
,       0                                           // SELECT_DIAL
,       0                                           // CURR_EDIT_DIAL
,       0                                           // CURR_SELECT_DIAL
,       _bg( BLACK )    | _fg( WHITE )              // FRAME
,       _bg( WHITE )    | _fg( BLACK )              // SCROLL_ICON
,       _bg( BLACK )    | _fg( WHITE )              // SCROLL_BAR
,       _bg( BLACK )    | _fg( WHITE )              // DIAL_FRAME
,       _bg( BLACK )    | _fg( WHITE )              // BROWSE
,       _bg( WHITE )    | _fg( BRIGHT | WHITE )     // CURR_HOTSPOT
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // ERROR
,       _bg( WHITE )    | _fg( BLACK )              // HINT
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // WARNING
,       _bg( BLACK )    | _fg( WHITE )              // OFF_HOTSPOT
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // RADIO_HOTSPOT
};

static ATTR local_attrs[] =
{       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // MENU
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // ACTIVE
,       _bg( WHITE )    | _fg( BLACK )              // CURR_ACTIVE
,       _bg( BLACK )    | _fg( WHITE )              // INACTIVE
,       _bg( WHITE )    | _fg( BLACK )              // CURR_INACTIVE
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // DIALOGUE
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // DEFAULT_HOTSPOT
,       _bg( BLACK )    | _fg( WHITE )              // NORMAL
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // BRIGHT
,       _bg( BLACK )    | _fg( UNDERLINE )          // EDIT
,       _bg( WHITE )    | _fg( BLACK )              // REVERSE
,       _bg( BLACK )    | _fg( WHITE )              // EDIT2
,       _bg( WHITE )    | _fg( BLACK )              // HOTSPOT
,       _bg( BLACK )    | _fg( WHITE )              // HELP
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // HOT
,       _bg( WHITE )    | _fg( UNDERLINE )          // HOT_CURR
,       _bg( BLACK )    | _fg( UNDERLINE )          // HOT_QUIET
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // CURR_EDIT
,       0                                           // CURR_MARK_EDIT
,       _bg( WHITE )    | _fg( BLACK )              // MARK_NORMAL
,       0                                           // MARK_EDIT
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // CURR_HOTSPOT_KEY
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // EDIT_DIAL
,       0                                           // SELECT_DIAL
,       0                                           // CURR_EDIT_DIAL
,       0                                           // CURR_SELECT_DIAL
,       _bg( BLACK )    | _fg( WHITE )              // FRAME
,       _bg( WHITE )    | _fg( BLACK )              // SCROLL_ICON
,       _bg( BLACK )    | _fg( WHITE )              // SCROLL_BAR
,       _bg( BLACK )    | _fg( WHITE )              // DIAL_FRAME
,       _bg( BLACK )    | _fg( WHITE )              // BROWSE
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // CURR_HOTSPOT
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // ERROR
,       _bg( WHITE )    | _fg( BLACK )              // HINT
,       _bg( BLACK )    | _fg( BRIGHT | WHITE )     // WARNING
,       _bg( BLACK )    | _fg( WHITE )              // OFF_HOTSPOT
,       _bg( BLACK )    | _fg( BRIGHT | UNDERLINE ) // RADIO_HOTSPOT
};

bool uiattrs( void )
{
    ATTR    *from;

    if( UIData->colour == M_MONO ) {
        from = ui_mono_attrs;
    } else if( UIData->colour == M_BW ) {
        from = ui_bw_attrs;
    } else {
        from = ui_colour_attrs;
    }
    if( UIData->attrs == NULL ) {
        UIData->attrs = local_attrs;
    }
    memcpy( UIData->attrs, from, sizeof( local_attrs ) );
    return( TRUE );
}


#if defined( PROGRAM_VGA )
static void setvgacolours( void )
{
    int             i;
    a_colour        col;

    for( i = 0; i < 16; i++ ) {
        col = VGAcolours[i];
        SetColourRegister( i, col.red, col.green, col.blue );
    }
    uisetblinkattr( 0 );
}

bool uivgaattrs( void )
{
    if( UIData->colour == M_VGA || UIData->colour == M_EGA ) {
        setvgacolours();
        memcpy( UIData->attrs, ui_vga_colour_attrs, sizeof( local_attrs ) );
        return( TRUE );
    }
    return( FALSE );
}

void uisetblinkattr( int on )
{
    if( on ) {
        if( BlinkAttr != 1 ) {
            SetBlinkAttr( 1 );
        }
        BlinkAttr = 1;
    } else {
        if( BlinkAttr != 0 ) {
            SetBlinkAttr( 0 );
            BlinkAttr = 0;
        }
    }
}

char uigetblinkattr( void )
{
    return( BlinkAttr );
}
#endif
