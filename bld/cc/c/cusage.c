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


#include "cvars.h"

#if _OS == _QNX
#include <stdlib.h>

extern char **_argv;

void CCusage()
{
    print_usage( _argv );
}

#else

#include <conio.h>
#include <unistd.h>


 #ifdef __OSI__
   extern       char    *_Copyright;
 #endif


static char const *NextUsage( char const *p ) {
    while( *p ) {
        ++p;
    }
    return( p + 1 );
}

void CCusage()
{
    char const  * p;
    unsigned    count;

    count = 0;
    #ifdef __OSI__
        if( _Copyright != NULL ) {
            ConsMsg( _Copyright );
            count = 1;
        }
    #endif
    p = UsageText();
    while( *p != '\0' ) {
        if( ++count > 20 ) {
            Wait_for_return();
            count = 0;
        }
        ConsMsg( p );
        p= NextUsage( p );
    }
}


local void Wait_for_return()
{
    if( ConTTY() ) {
        char const *press;
        press =  CGetMsgStr( PHRASE_PRESS_RETURN );
        ConsMsg( press );
        getch();
    }
}

#endif
