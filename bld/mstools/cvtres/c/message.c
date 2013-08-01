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
#include <stdio.h>
#include "banner.h"
#include "error.h"
#include "message.h"
#include "system.h"


static char usageMsg[] = {
    #include "usagemsg.gh"
    "\0"
};

static int              quietMode = 0;


/*
 * Print the copyright banner.
 */
void BannerMessage( void )
/************************/
{
    static int          alreadyPrinted;
    static char *       helpMsg = {
        banner1w( "C/C++ CVTRES Clone for " CPU_NAME " ", _CVTRES_CLONE_VERSION_ ) "\n"
        banner2 "\n"
        banner2a( "1995" ) "\n"
        banner3 "\n"
        banner3a "\n"
    };

    if( !alreadyPrinted && !quietMode ) {
        printf( helpMsg );
        alreadyPrinted = 1;
    }
}


/*
 * Read a key from the console.
 */
static int get_key( void )
/************************/
{
    int                 ch;

    ch = getch();
    if( ch == 0 ) {                     /* handle extended keys */
        ch = getch();
    }
    return( ch );
}

static char const *NextUsage( char const *p )
{
    while( *p ) {
        ++p;
    }
    return( p + 1 );
}

#define LINE_COUNT      15
/*
 * Print a help message.
 */
void PrintHelpMessage( void )
/***************************/
{
    int                 num;
    int                 ch;
    char const          *p;
    char const          *page_text;

    BannerMessage();
    p = page_text = usageMsg;
    num = 0;
    while( *(p = NextUsage( p )) != '\0' ) {
        if( num == LINE_COUNT ) {
            printf( page_text );
            fflush( stdout );
            ch = get_key();
            printf( "\n" );
            if( ch == 'q' )  
                break;
            num = -1;
        }
        printf( "%s\n", p );
        ++num;
    }
}


/*
 * Print a message listing unsupported options which were used (using the
 * opts string) and a complete list of unsupported options.
 */
void UnsupportedOptsMessage( const char *opts )
/*********************************************/
{
    Warning( "Ignoring unsupported option(s): %s", opts );
}


/*
 * Disable printing of the copyright banner.
 */
void QuietModeMessage( void )
/***************************/
{
    quietMode = 1;
}
