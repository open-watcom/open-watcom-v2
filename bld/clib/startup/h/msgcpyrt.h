/*****************************************************************************
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
* Description:  Copyright message included from startup files.
*
*****************************************************************************/


#define _DOSTR( p )         #p
#define _MACROSTR( p )      _DOSTR( p )
#if defined( __WASAXP__ ) || defined( __WASPPC__ ) || defined( __WASMIPS__ )
    #define pick(x)         .ascii x
#else
    #define pick(x)         x
#endif

#if defined( __WASAXP__ ) || defined( __AXP__ )
    pick( "Open Watcom C/C++ AXP Run-Time system. " )
#elif defined( __WASPPC__ ) || defined( __PPC__ )
    pick( "Open Watcom C/C++ PPC Run-Time system. " )
#elif defined( __WASMIPS__ ) || defined( __MIPS__ )
    pick( "Open Watcom C/C++ MIPS Run-Time system. " )
#elif defined( _M_I86 )
    pick( "Open Watcom C/C++16 Run-Time system. " )
#elif defined( _M_IX86 )
    pick( "Open Watcom C/C++32 Run-Time system. " )
#endif

pick( "Copyright (c) Open Watcom Contributors 2002-" )
pick( _MACROSTR( _CYEAR ) )
pick( ". " )
pick( "Portions Copyright (C) Sybase, Inc. 1988-2002." )
#if defined( __WASAXP__ ) || defined( __WASPPC__ ) || defined( __WASMIPS__ )
    .ascii 0
#endif

#undef pick
#undef _MACROSTR
#undef _DOSTR
