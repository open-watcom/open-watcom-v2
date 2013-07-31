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
* Description:  C++ compiler banner.
*
****************************************************************************/


#include "plusplus.h"

#include <stdio.h>
#include <banner.h>

#include "fingprnt.h"

#define __puts( x )     MsgDisplayBanner( x )

#if _CPU == 386
 #define __TARCH "32"
#elif _CPU == 8086
 #define __TARCH "16"
#elif _CPU == _AXP
 #define __TARCH " AXP"
#endif

#define _C      "C++"

#define _COMPILER " Optimizing Compiler"

void CBanner( void )
{
    if( CompFlags.banner_printed ) return;
    if( CompFlags.quiet_mode )     return;
#if _CPU == 8086
  #if defined( _BETAVER )
    __puts( banner1w1( "C++ x86 16-bit Optimizing Compiler" ) );
    __puts( banner1w2( _WPP_VERSION_ ) );
  #else
    __puts( banner1w( "C++ x86 16-bit Optimizing Compiler", _WPP_VERSION_ ) );
  #endif
#elif _CPU == 386
  #if defined( _BETAVER )
    __puts( banner1w1( "C++ x86 32-bit Optimizing Compiler" ) );
    __puts( banner1w2( _WPP_VERSION_ ) );
  #else
    __puts( banner1w( "C++ x86 32-bit Optimizing Compiler", _WPP_VERSION_ ) );
  #endif
#elif _CPU == _AXP
  #if defined( _BETAVER )
    __puts( banner1w1( "C++ Alpha AXP Optimizing Compiler" ) );
    __puts( banner1w2( _WPP_VERSION_ ) );
  #else
    __puts( banner1w( "C++ Alpha AXP Optimizing Compiler", _WPP_VERSION_ ) );
  #endif
#endif
    __puts( banner2( "1989" ) );
    __puts( banner3 );
    __puts( banner3a );
    if( Token[0] != '$' ) {             /* if finger print present */
        __puts( Token );                /* - print it */
    }
    CompFlags.banner_printed = 1;                       /* 13-mar-90 */
}
