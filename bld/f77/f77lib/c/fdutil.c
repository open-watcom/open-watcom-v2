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
* Description:  "FILEDEF" utilities
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftnio.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void    *LocFile( char *name ) {
//==============================

// Search the environment for the given name.

    return( getenv( name ) );
}


void    *LocUnit( int unit ) {
//============================

// Search FILEDEF chain for file name associated with unit &.
// Return 0 if none found, non-zero otherwise.

    char        numbuff[MAX_INT_SIZE+1];

    ltoa( unit, numbuff, 10 );
    return( LocFile( numbuff ) );
}


void    ExtractInfo( char *handle, ftnfile *fcb ) {
//=================================================

// Set the file name in the fcb.

    fcb->filename = RChkAlloc( strlen( handle ) + sizeof( char ) );
    strcpy( fcb->filename, handle );
}
