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


#include <conio.h>
#include <unistd.h>
#include <stdlib.h>

#include "plusplus.h"
#include "cusage.h"
#include "errdefns.h"

#define NUM_ROWS        20

#if defined(__QNX__)

extern char     **_argv;

void CCusage( void )
/******************/
{
    print_usage( _argv );
}

#else

static const char * const Usage[] = {
#include "cmdlnusg.gh"
NULL };

static const char PressReturn[] = {
"\n    "
};

 #ifdef __OSI__
   extern       char    *_Copyright;
 #endif

#ifdef __OSI__
#define output(text) puts(text)
#else
#define output(text) MsgDisplayLine( text )
#endif


static boolean Wait_for_return( char const *page_text )
/*****************************************************/
// return TRUE if we should stop printing
{
    if( CompFlags.ide_console_output ) {
        int   c;
        char *p;
        auto char buff[256];

        p = stpcpy( buff, PressReturn );
        p = stpcpy( p, page_text );
        output( buff );
        fflush( stdout );
        c = getch();
        return c == 'q' || c == 'Q';
    }
    return FALSE;
}

typedef struct usage_data {
    unsigned    count;
    unsigned    nrows;
    char const  *page_text;
} usage_data;

boolean willPrintALine( usage_data *info )
{
    boolean     retval;

    retval = FALSE;
    if( ++(info->count) > info->nrows ) {
        retval = Wait_for_return( info->page_text );
        info->count = 0;
        info->nrows = NUM_ROWS;
    }
    return retval;
}

char const *nextIntlUsage( char const *p ) {
    while( *p ) {
        ++p;
    }
    DbgAssert( *p == '\0' );
    return( p + 1 );
}

void CCusage( void )
/******************/
{
    char const *usage_text;
    usage_data info;

    info.nrows = NUM_ROWS-2;
    info.count = 0;
    #ifdef __OSI__
        if( _Copyright != NULL ) {
            output( _Copyright );
            info.count = 1;
        }
    #endif
    usage_text = IntlUsageText();
    if( usage_text == NULL ) {
        char const * const *p = Usage;
        info.page_text = *p;
        for( ++p; *p != NULL; ++p ) {
            if( willPrintALine( &info ) ) break;
            output( (char*)*p );
        }
    } else {
        info.page_text = usage_text;
        usage_text = nextIntlUsage( usage_text );
        while( *usage_text ) {
            if( willPrintALine( &info ) ) break;
            output( (char*)usage_text );
            usage_text = nextIntlUsage( usage_text );
        }
    }
}
#endif
