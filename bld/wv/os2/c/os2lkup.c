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


#include "dbgdefn.h"
#include <stdlib.h>
#define INCL_DOSMISC
#include "os2.h"
#include "envlkup.h"


size_t EnvLkup( const char *name, char *buff, size_t buff_len )
{
#ifdef _M_I86
    const char  __far *env;
#else
    const char  *env;
#endif
    size_t      len;
    bool        output;
    char        c;

#ifdef _M_I86
    if( DosScanEnv( (char *)name, (char __far * __far *)&env ) != 0 )
#else
    // use getenv() so that autoenv has an effect (we can't
    // reliably modify the "master" process environment on OS/2)
    env = getenv( name );
    if( env == NULL )
#endif
        return( 0 );

    output = false;
    if( buff_len != 0 && buff != NULL ) {
        --buff_len;
        output = true;
    }
    for( len = 0; (c = *env++) != NULLCHAR; ++len ) {
        if( output ) {
            if( len >= buff_len ) {
                break;
            }
            *buff++ = c;
        }
    }
    if( output ) {
        buff[len] = NULLCHAR;
    }
    return( len );
}
