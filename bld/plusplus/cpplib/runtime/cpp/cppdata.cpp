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



#define GBL _WPRTLINK
#include "cpplib.h"
#include "rtexcept.h"

#if defined(__MT__) && defined(__386__)
#include "rtinit.h"
AXI( CPPLIB(multi_thread_init), INIT_PRIORITY_THREAD )
#endif

#ifndef NDEBUG

    extern "C" {

        // THESE CAUSE THE DEBUGGING ROUTINES TO BE FORCED INTO MODULES

        extern void CPPLIB( DbgRtDumpAutoDtor )( void );
        extern void CPPLIB( DbgRtDumpModuleDtor )( void );

        #pragma extref ( CPPLIB( DbgRtDumpAutoDtor ) );
        #pragma extref ( CPPLIB( DbgRtDumpModuleDtor ) );

    };

#endif

#if defined( M_PC_INTEL )
extern "C" {

    // Note: these have _WRTLINK because they are used only to check
    //       consistency. Linker must resolve references to them.

    #pragma aux no_extra_underscores "*";

    #if defined( FS_REGISTRATION_NT )
        _WPRTLINK int __pragma("no_extra_underscores") __compiled_under_NT;
    #elif defined( FS_REGISTRATION_OS2 )
        _WPRTLINK int __pragma("no_extra_underscores") __compiled_under_OS2;
    #else
        _WPRTLINK int __pragma("no_extra_underscores") __compiled_under_generic;
    #endif

};
#elif defined( M_ALPHA )
extern "C" {

        _WPRTLINK int __compiled_under_generic;

};
#endif
