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


#ifndef wsystem_class
#define wsystem_class

#include "system.hpp"

#include "wobject.hpp"
#include "wstates.hpp"

typedef HINSTANCE  WModuleHandle;
typedef FARPROC WProcAddr;

WCLASS WSystemService {
    public:
        static int WEXPORT sysExec( const char *, WWindowState, WWindowType );
        static int WEXPORT sysExecBackground( const char * );
        static void WEXPORT sysYield( void );
        static void WEXPORT sysSleep( unsigned long );
        static WModuleHandle WEXPORT loadLibrary( const char * );
        static WProcAddr WEXPORT getProcAddr( WModuleHandle, const char * );
        static void WEXPORT freeLibrary( WModuleHandle );
};

#endif
