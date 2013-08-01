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
* Description:  C compiler mainline.
*
****************************************************************************/


#include "cvars.h"
#include <banner.h>

#if _CPU == 8086
#define _TARGET_    "x86 16-bit"
#elif _CPU == 386
#define _TARGET_    "x86 32-bit"
#elif _CPU == _AXP
#define _TARGET_    "Alpha AXP"
#elif _CPU == _PPC
#define _TARGET_    "PowerPC"
#elif _CPU == _MIPS
#define _TARGET_    "MIPS"
#else
#error Unknown System
#endif

char CompilerID[] = { "__WATCOMC__=" BANSTR( _BANVER ) };

void CBanner( void )
{
    if( CompFlags.banner_printed ) return;
    if( CompFlags.quiet_mode )     return;
#if defined( _BETAVER )
    BannerMsg( banner1w1( "C " _TARGET_ " Optimizing Compiler" ) );
    BannerMsg( banner1w2( _WCC_VERSION_ ) );
#else
    BannerMsg( banner1w1( "C " _TARGET_ " Optimizing Compiler", _WCC_VERSION_ ) );
#endif
    BannerMsg( banner2 );
    BannerMsg( banner2a( "1984" ) );
    BannerMsg( banner3 );
    BannerMsg( banner3a );
    CompFlags.banner_printed = 1;
}
