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


#ifndef NDEBUG

#if defined( NEVERdEFINED )
//20181107@1523-WOI: COMMENTED OUT TO GET CLEAN COMPILE WHEN DEBUG=1.  NONE OF THIS STUFF WILL COMPILE, AND IT IS CLEARLY NOT NEEDED FOR PRODUCTION SINCE IT IS ONLY DEFINED WHEN DEBUG=1

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


extern void         WndUserAdd(char *,unsigned int );

static void BadCmd( void )
{
    Error( ERR_LOC, LIT_ENG( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_WINDOW ) );
}


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


static void MenuDump( int indent, int popup_num_items, gui_menu_struct *child )
{
    char        *p;
    int         i;

    while( --popup_num_items >= 0 ) {
        p = TxtBuff;
        i = indent;
        while( i-- > 0 )
            *p++ = ' ';
        if( child->style & GUI_STYLE_MENU_SEPARATOR ) {
            StrCopy( "---------", p );
        } else {
            MenuCopy( TxtBuff, child->label, p );
        }
        WndDlgTxt( TxtBuff );
        if( child->hinttext != NULL && child->hinttext[0] != NULLCHAR ) {
            p = TxtBuff;
            for( i = indent; i > 0; --i )
                *p++ = ' ';
            p = StrCopy( "- ", p );
            p = StrCopy( child->hinttext, p );
            WndDlgTxt( TxtBuff );
        }
        if( child->child_num_items > 0 ) {
            MenuDump( indent + 4, child->child_num_items, child->child );
        }
        ++child;
    }
}

OVL_EXTERN void XDumpMenus( void )
{
    wnd_class_wv    wndclass;
    char            *p;

    ReqEOC();
    for( wndclass = 0; wndclass < NUM_WNDCLS; wndclass++ ) {
        p = StrCopy( "The ", TxtBuff );
        p = GetCmdEntry( WndNameTab, wndclass, p );
        p = StrCopy( " Window", p );
        WndDlgTxt( TxtBuff );
        MenuDump( 4, WndInfoTab[wndclass]->popup_num_items, WndInfoTab[wndclass]->popupmenu );
    }
    WndDlgTxt( "The main menu" );
    MenuDump( 4, WndNumMenus, WndMainMenu );
}

OVL_EXTERN void XTimeSymComp( void )
{
    int         i, num;

    num = ReqExpr();
    ReqEOC();
    for( i = 0; i < num; ++i ) {
        SymCompInit( true, true, false, false, NO_MOD );
        SymCompFini();
    }
}

static const char InternalNameTab[] =
{
    "Dumpmenu\0"
    "Symcomp\0"
};

static void (*InternalJmpTab[])() =
{
    &XDumpMenus,
    &XTimeSymComp,
};

void ProcInternal( void )
{
    int     cmd;

    cmd = ScanCmd( InternalNameTab );
    if( cmd < 0 ) {
        BadCmd();
    } else {
        InternalJmpTab[cmd]();
    }
}

#endif

#endif
