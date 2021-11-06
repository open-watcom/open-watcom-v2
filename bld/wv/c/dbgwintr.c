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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "wndregx.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgmain.h"
#include "dbgshow.h"
#include "dbgparse.h"
#include "dbgwdlg.h"
#include "namelist.h"
#include "symcomp.h"
#include "wndmenu.h"
#include "dbgwintr.h"


#ifndef NDEBUG
extern void         WndUserAdd(char *,unsigned int );
#endif

static void BadCmd( void )
{
    Error( ERR_LOC, LIT_ENG( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_WINDOW ) );
}

#ifndef NDEBUG
static void MenuCopy( char *dst, const char *from, char *to )
{
    char        ampchar;
    bool        ampdumped;

    ampchar = 0;
    ampdumped = false;
    while( *from != NULLCHAR ) {
        if( *from == '&' ) {
            ++from;
            ampchar = *from;
        }
        if( *from == '\t' ) {
            ++from;
            if( ampchar && !ampdumped ) {
                ampdumped = true;
                *to++ = ' ';
                *to++ = '(';
                *to++ = ampchar;
                *to++ = ')';
            }
            while( to - dst < 30 ) {
                *to++ = ' ';
            }
        }
        *to++ = *from++;
    }
    if( ampchar && !ampdumped ) {
        *to++ = ' ';
        *to++ = '(';
        *to++ = ampchar;
        *to++ = ')';
    }
    *to++ = NULLCHAR;
}


static void MenuDump( int indent, const gui_menu_items *menus )
{
    char        *p;
    int         i;
    int         j;

    for( i = 0; i < menus->num_items; ++i ) {
        p = TxtBuff;
        j = indent;
        while( j-- > 0 )
            *p++ = ' ';
        if( menus->menu[i].style & GUI_STYLE_MENU_SEPARATOR ) {
            StrCopy( "---------", p );
        } else {
            MenuCopy( TxtBuff, menus->menu[i].label, p );
        }
        WndDlgTxt( TxtBuff );
        if( menus->menu[i].hinttext != NULL && menus->menu[i].hinttext[0] != NULLCHAR ) {
            p = TxtBuff;
            j = indent;
            while( j-- > 0 )
                *p++ = ' ';
            p = StrCopy( "- ", p );
            p = StrCopy( menus->menu[i].hinttext, p );
            WndDlgTxt( TxtBuff );
        }
        MenuDump( indent + 4, &menus->menu[i].child );
    }
}

static void XDumpMenus( void )
{
    wnd_class_wv    wndclass;
    char            *p;

    ReqEOC();
    for( wndclass = 0; wndclass < NUM_WNDCLS; wndclass++ ) {
        p = StrCopy( "The ", TxtBuff );
        p = GetCmdEntry( WndNameTab, wndclass, p );
        p = StrCopy( " Window", p );
        WndDlgTxt( TxtBuff );
        MenuDump( 4, &WndInfoTab[wndclass]->popup );
    }
    WndDlgTxt( "The main menu" );
    MenuDump( 4, &WndMainMenu );
}

static void XTimeSymComp( void )
{
    int         i, num;

    num = ReqExpr();
    ReqEOC();
    for( i = 0; i < num; ++i ) {
        SymCompInit( true, true, false, false, NO_MOD );
        SymCompFini();
    }
}

#define INTERNAL_DEFS \
    pick( "Dumpmenu", XDumpMenus   ) \
    pick( "Symcomp",  XTimeSymComp )

static const char InternalNameTab[] =
{
    #define pick(t,p)   t "\0"
    INTERNAL_DEFS
    #undef pick
};

static void (*InternalJmpTab[])() =
{
    #define pick(t,p)   p,
    INTERNAL_DEFS
    #undef pick
};
#endif

void ProcInternal( void )
{
#ifndef NDEBUG
    int     cmd;

    cmd = ScanCmd( InternalNameTab );
    if( cmd < 0 ) {
        BadCmd();
    } else {
        InternalJmpTab[cmd]();
    }
#else
    BadCmd();
#endif
}

