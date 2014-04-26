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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdarg.h>
#include <process.h>
#include "rtdata.h"
#include "_process.h"

_WCRTLINK int __F_NAME((execl),_wexecl)( const CHAR_TYPE *path, const CHAR_TYPE *arg0, ... )
    {
        va_list ap;

        arg0 = arg0;
        va_start( ap, path );
        #if defined(__AXP__) || defined(__MIPS__)
            #ifdef __WIDECHAR__
                return( _wexecve( path, (const CHAR_TYPE**)ap.__base,
                        (const CHAR_TYPE **)_RWD_wenviron ) );
            #else
                return( execve( path, (const CHAR_TYPE**)ap.__base,
                        (const CHAR_TYPE **)_RWD_environ ) );
            #endif
        #else
            #ifdef __WIDECHAR__
                return( _wexecve( path, (const CHAR_TYPE**)ap[0],
                        (const CHAR_TYPE **)_RWD_wenviron ) );
            #else
                #ifdef __RDOS__
                    return( execv( path, (const CHAR_TYPE**)ap[0] ) );
                #else
                    return( execve( path, (const CHAR_TYPE**)ap[0],
                            (const CHAR_TYPE **)_RWD_environ ) );
                #endif
            #endif
        #endif
    }
