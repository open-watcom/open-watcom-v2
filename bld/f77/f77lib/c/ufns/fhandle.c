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
// FHANDLE      : get system file handle
//

#include "ftnstd.h"
#include "rundat.h"
#include "ftextfun.h"
#include "fio.h"
#include "posio.h"
#include "units.h"
#include "ftnapi.h"
#include "posopen.h"
#include "poserr.h"
#include "rtsysutl.h"

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>


intstar2        __fortran SYSHANDLE( intstar4 *unit ) {
//===================================================

    ftnfile     *fcb;

    fcb = Files;
    for(;;) {
        if( fcb == NULL ) {
            if( *unit == PRE_STANDARD_INPUT ) {
                return( fileno( stdin ) );
            } else if( *unit == PRE_STANDARD_OUTPUT ) {
                return( fileno( stdout ) );
            } else {
                return( -1 );
            }
        }
        if( *unit == fcb->unitid ) {
            if( fcb->fileptr == NULL ) {
                 return( -1 );
            }
            return( ((a_file *)(fcb->fileptr))->handle );
        }
        fcb = fcb->link;
    }
}


intstar4        __fortran SETSYSHANDLE( intstar4 *unit, intstar2 *handle ) {
//========================================================================

    ftnfile     *fcb;
    struct stat stat_buff;

    fcb = Files;
    for(;;) {
        if( fcb == NULL ) return( -1 );
        if( *unit == fcb->unitid ) {
            if( fstat( *handle, &stat_buff ) == -1 ) {
                 return( -1 );
            }
            if( fcb->fileptr != NULL ) {
                Closef( fcb->fileptr );
                if( Errorf( NULL ) != IO_OK ) {
                    return( -1 );
                }
            }
            fcb->fileptr = _AllocFile( *handle, _FileAttrs( fcb ), 0 );
            if( fcb->fileptr == NULL ) {
                return( -1 );
            } else {
                _AllocBuffer( fcb );
                return( 0 );
            }
        }
        fcb = fcb->link;
    }
}
