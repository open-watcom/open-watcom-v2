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
* Description:  Default Windowing - Initialize and finalize Win32 and OS/2
*
****************************************************************************/


#include "variety.h"
#include "rtinit.h"
#include "defwin.h"

#if defined(_M_IX86)
 #pragma aux __init_default_win "*";
#endif
char __init_default_win;

#ifdef DEFAULT_WINDOWING

_WCRTLINK extern void __InitDefaultWin( void );
_WCRTLINK extern void __FiniDefaultWin( void );

#if defined(__SW_BR)
    // need a thunking layer
    void __tnk__InitDefaultWin( void ) { __InitDefaultWin(); }
    void __tnk__FiniDefaultWin( void ) { __FiniDefaultWin(); }
    AXI( __tnk__InitDefaultWin, INIT_PRIORITY_LIBRARY )
    AYI( __tnk__FiniDefaultWin, INIT_PRIORITY_LIBRARY )
#else
    AXI( __InitDefaultWin, INIT_PRIORITY_LIBRARY )
    AYI( __FiniDefaultWin, INIT_PRIORITY_LIBRARY )
#endif
#endif
