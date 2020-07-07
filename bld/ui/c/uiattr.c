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
* Description:  Default ui lib attribute values.
*
****************************************************************************/


#include <string.h>
#include "uidef.h"
#include "uiattrs.h"

#if defined( __DOS__ ) && defined( _M_IX86 )
   #define PROGRAM_VGA
#endif

#if defined( PROGRAM_VGA )

#include "int10.h"

static bool     BlinkAttr = true;

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

ATTR ui_vga_colour_attrs[] = {
    #define pick(id,vga,col,bw,mono,local) vga,
    #include "_uiattrs.h"
    #undef pick
};
#endif

#define C_MENU      C_WHITE
#define C_CMENU     C_BLACK
#define C_BACK      C_BLUE

ATTR ui_colour_attrs[] = {
    #define pick(id,vga,col,bw,mono,local) col,
    #include "_uiattrs.h"
    #undef pick
};

ATTR ui_bw_attrs[] = {
    #define pick(id,vga,col,bw,mono,local) bw,
    #include "_uiattrs.h"
    #undef pick
};

ATTR ui_mono_attrs[] = {
    #define pick(id,vga,col,bw,mono,local) mono,
    #include "_uiattrs.h"
    #undef pick
};

static ATTR local_attrs[] = {
    #define pick(id,vga,col,bw,mono,local) local,
    #include "_uiattrs.h"
    #undef pick
};

bool UIAPI uiattrs( void )
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
    return( true );
}


#if defined( PROGRAM_VGA )

static void setvgacolours( void )
{
    int             i;
    a_colour        col;

    for( i = 0; i < 16; i++ ) {
        col = VGAcolours[i];
        _BIOSVideoSetColorRegister( i, col.red, col.green, col.blue );
    }
    uisetblinkattr( false );
}

bool UIAPI uivgaattrs( void )
{
    if( UIData->colour == M_VGA || UIData->colour == M_EGA ) {
        setvgacolours();
        memcpy( UIData->attrs, ui_vga_colour_attrs, sizeof( local_attrs ) );
        return( true );
    }
    return( false );
}

void UIAPI uisetblinkattr( bool on )
{
    if( on ) {
        if( !BlinkAttr ) {
            _BIOSVideoSetBlinkAttr( true );
        }
        BlinkAttr = true;
    } else {
        if( BlinkAttr ) {
            _BIOSVideoSetBlinkAttr( false );
            BlinkAttr = false;
        }
    }
}

bool UIAPI uigetblinkattr( void )
{
    return( BlinkAttr );
}

#else

bool UIAPI uivgaattrs( void )
{
    return( false );
}

void UIAPI uisetblinkattr( bool on )
{
    /* unused parameters */ (void)on;
}

bool UIAPI uigetblinkattr( void )
{
    return( false );
}

#endif

ATTR UIAPI uisetattr( UIATTR uiattr, ATTR new_attr )
{
    ATTR    old_attr;

    old_attr = UIData->attrs[uiattr];
    UIData->attrs[uiattr] = new_attr;
    return( old_attr );
}
