/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Compiler usage information output routines.
*
****************************************************************************/


#include "cvars.h"


#define NUM_ROWS    24

static char const *nextUsage( char const *p )
{
    while( *p != '\0' ) {
        ++p;
    }
    return( p + 1 );
}

static bool Wait_for_return( char const *press )
{
    int     c;

    ConsMsg( press );
    c = getchar();
    return( c == 'q' || c == 'Q' );
}

void CCusage( void )
{
    char const  *page_text;
    char const  *p;
    int         count;

    count = CBanner();
    if( GlobalCompFlags.ide_console_output && count ) {
        ConsMsg( "" );
        ++count;
    }
    p = UsageText();
    page_text = p;
    while( *(p = nextUsage( p )) != '\0' ) {
        if( GlobalCompFlags.ide_console_output ) {
            if( count == NUM_ROWS - 2 ) {
                if( Wait_for_return( page_text ) )
                    break;
                count = 0;
            }
            ++count;
        }
        ConsMsg( p );
    }
}
