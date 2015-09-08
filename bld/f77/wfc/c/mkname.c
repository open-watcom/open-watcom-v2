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
// MKNAME       : file name processing
//

#include "ftnstd.h"
#include <stdlib.h>
#include <string.h>

#include "clibext.h"


int     CopyMaxStr( char *str, char *buff, int max_len ) {
//========================================================

    int         len;

    len = strlen( str );
    if( len > max_len ) {
        len = max_len;
    }
    memcpy( buff, str, len );
    buff[ len ] = NULLCHAR;
    return( len );
}

int     MakeName( char *fn, char *extn, char *buff ) {
//====================================================

    int         len;

    len = CopyMaxStr( fn, buff, _MAX_PATH - 1 );
    if( extn != NULL ) {
        buff += len;
        *buff = '.';
        ++buff;
        ++len;
        len += CopyMaxStr( extn, buff, _MAX_PATH - 1 - len );
    }
    return( len );
}


