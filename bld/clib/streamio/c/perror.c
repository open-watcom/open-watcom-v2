/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of perror() - print error message.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rterrno.h"
#include "thread.h"


_WCRTLINK void __F_NAME(perror,_wperror)( const CHAR_TYPE *s )
{
    __null_check( s, 0 );
    if( s != NULL && *s != NULLCHAR ) {
        __F_NAME(fputs,fputws)( s, stderr );
        __F_NAME(fputs,fputws)( STRING( ": " ), stderr );
    }
    __F_NAME(fputs,fputws)( __F_NAME(strerror,_wcserror)( _RWD_errno ), stderr );
    __F_NAME(fputc,fputwc)( STRING( '\n' ), stderr );
}
