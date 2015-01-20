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
* Description:  Compiler usage information output routines.
*
****************************************************************************/


#include "cvars.h"

static char const *nextUsage( char const *p )
{
    while( *p != '\0' ) {
        ++p;
    }
    return( p + 1 );
}


#if defined( __UNIX__ )

void CCusage( void )
{
    char const  *p;

    p = UsageText();
    while( *(p = nextUsage( p )) != '\0' ) {
        ConsMsg( p );
    }
}

#else

#include <conio.h>


#ifdef __OSI__
    extern       char    *_Copyright;
#endif


static void Wait_for_return( char const *press )
{
    if( ConTTY() ) {
        ConsMsg( press );
        getch();
    }
}


void CCusage( void )
{
    char const  *page_text;
    char const  *p;
    unsigned    count;

    count = 2;
#ifdef __OSI__
    if( _Copyright != NULL ) {
        ConsMsg( _Copyright );
        count = 1;
    }
#endif
    p = UsageText();
    page_text = p;
    while( *(p = nextUsage( p )) != '\0' ) {
        if( ++count > 21 ) {
            Wait_for_return( page_text );
            count = 0;
        }
        ConsMsg( p );
    }
}

#endif
