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


extern unsigned     GetValue( void );
extern unsigned     Lookup( const char *, const char *, unsigned );


extern unsigned     NumLines;
extern unsigned     NumColumns;

#if 0
static const char SysOptNameTab[] = {
    "\0"
    ""
};

enum { OPT_LINES = 1 };
#endif


bool OptDelim( char ch )
{
    return(  ch == '/' || ch == '-' );
}

void SetNumLines( int num )
{
    if( num < 10 ) num = 10;
    if( num > 99 ) num = 99;
    NumLines = num;
}

void SetNumColumns( int num )
{
    if( num < 25 ) num = 25;
    if( num > 255 ) num = 255;
    NumColumns = num;
}

/*
 * ProcSysOption -- process system option
 */

bool ProcSysOption( const char *start, unsigned len, int pass )
{
    pass=pass;
#if 0
    switch( Lookup( SysOptNameTab, start, len ) ) {
    default:
        return( FALSE );
    }
    return( TRUE );
#else
    start=start;len=len;
    return( FALSE );
#endif
}


/*
 * ProcSysOptInit -- initial system options
 */

void ProcSysOptInit( void )
{
}
