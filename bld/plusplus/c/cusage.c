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


#include "plusplus.h"
#include "wio.h"
#include "cbanner.h"
#include "cusage.h"


#define NUM_ROWS        24

#define ConsoleMessage(text)    MsgDisplayLine( text )

static const char EUsage[] = {
    #include "cmdlnusg.gh"
    "\0"
};

static char const *nextUsage( char const *p )
{
    while( *p != '\0' ) {
        ++p;
    }
    DbgAssert( *p == '\0' );
    return( p + 1 );
}

static bool Wait_for_return( char const *page_text )
/**************************************************/
// return true if we should stop printing
{
    int   c;

    ConsoleMessage( page_text );
//    fflush( stdout );
    c = getchar();
    return( c == 'q' || c == 'Q' );
}

void CCusage( void )
/******************/
{
    char const  *usage_text;
    char const  *page_text;
    int         count;

    count = CBanner();
    if( CompFlags.ide_console_output && count ) {
        ConsoleMessage( "" );
        ++count;
    }
    usage_text = IntlUsageText();
    if( usage_text == NULL ) {
        usage_text = EUsage;
    }
    page_text = usage_text;
    while( *(usage_text = nextUsage( usage_text )) != '\0' ) {
        if( CompFlags.ide_console_output ) {
            if( count == NUM_ROWS - 2 ) {
                if( Wait_for_return( page_text ) )
                    break;
                count = 0;
            }
            ++count;
        }
        ConsoleMessage( usage_text );
    }
}
