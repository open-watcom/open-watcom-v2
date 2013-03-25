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

#include <unistd.h>

#include "cusage.h"
#include "errdefns.h"

static const char EUsage[] = {
    #include "cmdlnusg.gh"
    "\0"
};

static char const *nextUsage( char const *p )
{
    while( *p ) {
        ++p;
    }
    DbgAssert( *p == '\0' );
    return( p + 1 );
}

#ifdef __UNIX__

void CCusage( void )
/******************/
{
    char const *usage_text;

    usage_text = IntlUsageText();
    if( usage_text == NULL ) {
        usage_text = EUsage;
    }
    while( *(usage_text = nextUsage( usage_text )) != '\0' ) {
        MsgDisplayLine( usage_text );
    }
}

#else

#define NUM_ROWS        20

#ifdef __OSI__
#define output(text) puts(text)
extern       char    *_Copyright;
#else
#define output(text) MsgDisplayLine( text )
#endif

typedef struct usage_data {
    unsigned    count;
    unsigned    nrows;
    char const  *page_text;
} usage_data;

static boolean Wait_for_return( char const *page_text )
/*****************************************************/
// return TRUE if we should stop printing
{
    if( CompFlags.ide_console_output ) {
        int   c;

        output( page_text );
        fflush( stdout );
        c = getchar();
        return( c == 'q' || c == 'Q' );
    }
    return FALSE;
}

static boolean willPrintALine( usage_data *info )
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
        usage_text = EUsage;
    }
    info.page_text = usage_text;
    while( *(usage_text = nextUsage( usage_text )) != '\0' ) {
        if( willPrintALine( &info ) )
            break;
        output( usage_text );
    }
}

#endif
