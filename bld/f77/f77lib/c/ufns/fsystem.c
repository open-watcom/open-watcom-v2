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


//
// FSYSTEM      : execute a DOS command
//

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "ftnstd.h"
#include <pgmacc.h>


intstar4        fortran FSYSTEM( string PGM *command ) {
//======================================================

    char        *buff;
    int         len;

    len = command->len;
    for(;;) {
        if( len == 0 ) return( -1 );
        if( command->strptr[len - 1] != ' ' ) break;
        --len;
    }
    buff = alloca( len + sizeof( char ) );
    if( buff == NULL ) return( -1 );
    pgm_memget( buff, command->strptr, len );
    buff[len] = NULLCHAR;
    return( system( buff ) );
}
