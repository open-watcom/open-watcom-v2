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
* Description:  Generate netware version file.
*
****************************************************************************/

#include <stdio.h>
#include <banner.h>

int     bannerver = _BANVER;
char *  bannerstr = banner2a();

int main ( int argc, char *argv[] )
{
    int majorver, minorver, revision;

    if( argc <= 1 || freopen( argv[1], "w", stdout ) != stdout ) {
        fprintf( stderr, "Can't open output file\n" );
        return 1;
    }

    majorver = bannerver / 1000;
    minorver = ( bannerver / 10 ) - ( majorver * 100 );
    revision = bannerver - ( ( bannerver / 10 ) * 10 );
#ifdef _BETAVER
    revision = 'b' - 'a' + 1;   /* 2 = b for beta */
#endif

    printf( "OP VERSION = %u.%u.%u\n", majorver, minorver, revision);
    printf( "OP COPYRIGHT '%s'\n", bannerstr);
    return( 0 );
}
