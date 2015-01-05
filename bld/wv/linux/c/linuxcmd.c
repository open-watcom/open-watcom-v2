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


#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgio.h"
#include "dbgname.h"
#include "dbgmem.h"


extern unsigned         GetValue( void );
extern unsigned         Lookup( const char *, const char *, unsigned );
extern void             WantEquals(void);
extern void             GetRawItem( char * );
extern char             *GetFileName( int pass );


extern char         *DbgTerminal;
extern int          DbgLines;
extern int          DbgColumns;

char                    XConfig[2048];

static const char SysOptNameTab[] = {
    "Console\0"
    "XConfig\0"
    ""
};

enum { OPT_CONSOLE = 1, OPT_XCONFIG };


void SetNumLines( int num )
{
    if( num < 10 || num > 999 ) num = 0;
    DbgLines = num;
}

void SetNumColumns( int num )
{
    if( num < 10 || num > 999 ) num = 0;
    DbgColumns = num;
}

bool OptDelim( char ch )
{
    return( ch == '-' );
}


/*
 * ProcSysOption -- process system option
 */

bool ProcSysOption( const char *start, unsigned len, int pass )
{
    char        *p;

    pass=pass;
    switch( Lookup( SysOptNameTab, start, len ) ) {
    case OPT_CONSOLE:
        _Free( DbgTerminal );
        DbgTerminal = GetFileName( pass );
        break;
    case OPT_XCONFIG:
        WantEquals();
        p = &XConfig[ strlen( XConfig ) ];
        *p++ = ' ';
        GetRawItem( p );
        if( pass == 1 ) XConfig[0] = NULLCHAR;
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}


/*
 * ProcSysOptInit -- initial system options
 */

void ProcSysOptInit( void )
{
    _SwitchOn( SW_TRUE_ARGV );
}
