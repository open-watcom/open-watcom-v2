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
#include <banner.h>
#undef  _BANEXTRA
#define _BANEXTRA " "


char CompilerID[] = { "__WATCOMC__=" BANSTR( _BANVER ) };

#define _COMPILER " Optimizing Compiler "

#if _CPU == 386
 #define _386 "32"
#else
 #define _386 "16"
#endif


void CBanner()
{
    if( CompFlags.banner_printed ) return;              /* 13-mar-90 */
    if( CompFlags.quiet_mode )     return;
#if _MACHINE == _ALPHA
    BannerMsg( banner1w( "C Alpha AXP Optimizing Compiler ",
                                                _WCC_VERSION_ ) );
#elif _MACHINE == _PPC
    BannerMsg( banner1w( "C Power PC Optimizing Compiler ",
                                                _WCC_VERSION_ ) );
#else
    BannerMsg( banner1w( "C" _386 _COMPILER, _WCC_VERSION_ ) );
#endif
    BannerMsg( banner2( "1984" ) );
    BannerMsg( banner3 );
#if 0
    ChkExpiryDate();
#endif
    CompFlags.banner_printed = 1;                       /* 13-mar-90 */
}

#if 0
void ChkExpiryDate()
{
    if( expired() ) {
        BannerMsg( "Beta Test period has expired" );
        exit( 1 );
    }
}

int expired()
{
    struct tm *t;
    auto time_t tod;

    /* get current date */
    tod = time( &tod );
    t = localtime( &tod );
    if( t->tm_year != 88 ) return( 1 );
    if( t->tm_mon >= 4 )   return( 1 );     /* if past April, too bad */
    return( 0 );
}
#endif
