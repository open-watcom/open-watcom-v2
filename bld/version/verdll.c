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

/*
 Description:
 ============
 This is the dll side of the version verification. This module must be linked
 into the dll and must NOT be put in the import library (although references to
 its exported functions are ok).

*/

#include <windows.h>
#include "version.h"

#define WINIEXP __export far PASCAL     // use with in-obj export rtns

#define _glue( a, b ) a ## b
#define _xglue( a, b ) _glue( a, b )
#define _str(a) # a
#define _xstr(a) _str(a)

#define _func_name( func ) _xglue( DLL_PREFIX, func )

#pragma off (unreferenced);

static char VersionStr[] = "Version Verification: "
                            _xstr( DLL_FILE_NAME )
                            " actual version "
                            _xstr( DLL_VERSION );

unsigned WINIEXP _func_name( version_dll_num ) (
/**********************************************/
    void
) {
    return( DLL_VERSION );
}
