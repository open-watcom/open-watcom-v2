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


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include "liballoc.h"
#include "rtdata.h"
#include "exitwmsg.h"
#if defined(__SW_BM)
#include "thread.h"
#endif

#ifndef __PENPOINT__
    #ifndef __SW_BM
        static MAX_CHAR_TYPE    cvt_buffer[20];
    #endif
#endif

_WCRTLINK CHAR_TYPE *__CVTBuffer()
{
#if defined(__PENPOINT__)
    if( _RWD_cvtbuf == NULL ) {
        _RWD_cvtbuf = lib_malloc( 20 * sizeof( wchar_t ) );
        if( _RWD_cvtbuf == NULL ) {
            __fatal_runtime_error(
                "Not enough memory to allocate cvt buffer\r\n", 1 );
        }
    }
#endif
    return( (CHAR_TYPE *)_RWD_cvtbuf );
}
