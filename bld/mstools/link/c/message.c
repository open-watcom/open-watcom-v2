/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include <string.h>
#include "bool.h"
#include "banner.h"
#include "error.h"
#include "message.h"
#include "system.h"


static const char usageMsg[] = {
    #include "usagemsg.gh"
};

static bool             quietMode = false;


/*
 * Print the copyright banner.
 */
void BannerMessage( void )
/************************/
{
    static bool         alreadyPrinted = false;
    static char *       helpMsg = {
        banner1w( "C/C++ LINK Clone for " CPU_NAME " ", _LINK_CLONE_VERSION_ ) "\n"
        banner2 "\n"
        banner2a( 1995 ) "\n"
        banner3 "\n"
        banner3a "\n"
        "Compatible with LINK Version " _MS_LINK_VERSION_ "\n"
    };

    if( !alreadyPrinted && !quietMode ) {
        printf( helpMsg );
        alreadyPrinted = true;
    }
}


/*
 * Print a help message.
 */
void PrintHelpMessage( void )
/***************************/
{
    char const  *p;

    BannerMessage();
    for( p = usageMsg; *p != '\0'; ) {
        puts( p );
        while( *p++ != '\0' ) ;
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
    quietMode = true;
}
