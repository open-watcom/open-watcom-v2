/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
// FHANDLE      : get system file handle
//

#include "ftnstd.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rundat.h"
#include "units.h"
#include "ftnapi.h"
#include "posopen.h"
#include "poserr.h"
#include "posutil.h"
#include "rtsysutl.h"
#include "rdwr.h"


intstar2        __fortran SYSHANDLE( intstar4 *unit ) {
//===================================================

    ftnfile     *fcb;

    for( fcb = Files; fcb != NULL; fcb = fcb->link ) {
        if( *unit == fcb->unitid ) {
            if( fcb->fileptr == NULL ) {
                 return( -1 );
            }
            return( FGetFileHandle( fcb->fileptr ) );
        }
    }
    if( *unit == PRE_STANDARD_INPUT ) {
        return( fileno( stdin ) );
    } else if( *unit == PRE_STANDARD_OUTPUT ) {
        return( fileno( stdout ) );
    } else {
        return( -1 );
    }
}


intstar4        __fortran SETSYSHANDLE( intstar4 *unit, intstar2 *handle ) {
//========================================================================

    ftnfile     *fcb;
    struct stat stat_buff;

    for( fcb = Files; fcb != NULL; fcb = fcb->link ) {
        if( *unit == fcb->unitid ) {
            if( fstat( *handle, &stat_buff ) == -1 )
                break;
            if( fcb->fileptr != NULL ) {
                Closef( fcb->fileptr );
                if( !IOOk( NULL ) ) {
                    break;
                }
            }
            fcb->fileptr = _AllocFile( *handle, _FileAttrs( fcb ), 0 );
            if( fcb->fileptr != NULL ) {
                _AllocBuffer( fcb );
                return( 0 );
            }
            break;
        }
    }
    return( -1 );
}
