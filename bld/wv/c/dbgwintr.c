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
#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "wndregx.h"
#include "strutil.h"
#include "dbgscan.h"


extern char             *GetCmdEntry(const char *,int ,char *);
extern unsigned         ReqExpr();
extern unsigned         OptExpr();
extern void             WndUserAdd(char *,unsigned int );
extern void             WndDlgTxt(const char *);
extern void             SymCompInit( bool code, bool data, bool d2_only, bool dup_ok, mod_handle );
extern void             SymCompFini();
extern char             *GetCmdName( int );

extern const char       WndNameTab[];

static void BadCmd( a_window *wnd )
{
    wnd=wnd;
    Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_WINDOW ) );
}


void MenuCopy( char *dst, char *from, char *to )
{
    char        ampchar;
    bool        ampdumped;

    ampchar = 0;
    ampdumped = FALSE;
    while( *from ) {
        if( *from == '&' ) {
            ++from;
            ampchar = *from;
        }
        if( *from == '\t' ) {
            ++from;
            if( ampchar && !ampdumped ) {
                ampdumped = TRUE;
                *to++ = ' ';
                *to++ = '(';
                *to++ = ampchar;
                *to++ = ')';
            }
            while( to - dst < 30 ) *to++ = ' ';
        }
        *to++ = *from++;
    }
    if( ampchar && !ampdumped ) {
        *to++ = ' ';
        *to++ = '(';
        *to++ = ampchar;
        *to++ = ')';
    }
    *to++ = '\0';
}


void MenuDump( int indent, int num_popups, gui_menu_struct *child )
{
    char        *p;
    int         i;

    while( --num_popups >= 0 ) {
        p = TxtBuff;
        i = indent;
        while( --i >= 0 ) *p++ = ' ';
        if( child->style & GUI_SEPARATOR ) {
            StrCopy( "---------", p );
        } else {
            MenuCopy( TxtBuff, child->label, p );
        }
        WndDlgTxt( TxtBuff );
        if( child->hinttext && child->hinttext[0] ) {
            p = TxtBuff;
            i = indent;
            while( --i >= 0 ) *p++ = ' ';
            p = StrCopy( "- ", p );
            p = StrCopy( child->hinttext, p );
            WndDlgTxt( TxtBuff );
        }
        if( child->num_child_menus != 0 ) {
            MenuDump( indent+4, child->num_child_menus, child->child );
        }
        ++child;
    }
}

extern gui_menu_struct WndMainMenu[];
extern int WndNumMenus;
extern wnd_info *WndInfoTab[];


void XDumpMenus()
{
    int         class;
    char        *p;

    ReqEOC();
    for( class = 0; class < WND_CURRENT; ++class ) {
        p = StrCopy( "The ", TxtBuff );
        p = GetCmdEntry( WndNameTab, class+1, p );
        p = StrCopy( " Window", p );
        WndDlgTxt( TxtBuff );
        MenuDump( 4, WndInfoTab[ class ]->num_popups, WndInfoTab[ class ]->popupmenu );
    }
    WndDlgTxt( "The main menu" );
    MenuDump( 4, WndNumMenus, WndMainMenu );
}

static void XTimeSymComp()
{
    int         i,num;

    num = ReqExpr();
    ReqEOC();
    for( i = 0; i < num; ++i ) {
        SymCompInit( TRUE, TRUE, FALSE, FALSE, NO_MOD );
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
    &BadCmd,
    &XDumpMenus,
    &XTimeSymComp,
};

void ProcInternal()
{
    InternalJmpTab[ ScanCmd( InternalNameTab ) ]();
}

#endif
