/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include <banner.h>
#include "fingprnt.h"
#include "cbanner.h"


#if _CPU == 8086
#define _TARGET_    "x86 16-bit"
#elif _CPU == 386
#define _TARGET_    "x86 32-bit"
#elif _CPU == _AXP
#define _TARGET_    "Alpha AXP"
#elif _CPU == _MIPS
#define _TARGET_    "MIPS"
#elif _CPU == _PPC
#define _TARGET_    "PowerPC"
#else
#error Unknown System
#endif

#define ConsoleMessage(text)    MsgDisplayLine( text )

void CBanner( void )
{
    if( !CompFlags.banner_printed ) {
        CompFlags.banner_printed = true;
        if( !CompFlags.quiet_mode ) {
            ConsoleMessage( banner1t( "C++ " _TARGET_ " Optimizing Compiler" ) );
            ConsoleMessage( banner1v( _WPP_VERSION_ ) );
            ConsoleMessage( banner2 );
            ConsoleMessage( banner2a( 1989 ) );
            ConsoleMessage( banner3 );
            ConsoleMessage( banner3a );
            if( Token[0] != '$' ) {             /* if finger print present */
                ConsoleMessage( Token );        /* - print it */
            }
        }
    }
}
