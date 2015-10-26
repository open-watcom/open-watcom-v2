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
// FNXTRECL     : Get next record length
//

#include "ftnstd.h"
#include "rundat.h"
#include "fio.h"

extern  void            IOOk(b_file *);
extern  unsigned_32     FGetVarRecLen(b_file *);

intstar4        __fortran FNEXTRECL( intstar4 *unit ) {
//===================================================

    ftnfile     *fcb;
    signed_32   size;


    fcb = Files;
    while( fcb && ( *unit != fcb->unitid ) ) {
        fcb = fcb->link;
    }

    // Lots of error checks
    if( ( fcb == NULL ) || ( fcb->fileptr == NULL ) ||
        !_LogicalRecordOrganization( fcb ) ||
        ( IOCB && (IOCB->flags & IOF_ACTIVE) && (IOCB->fileinfo == fcb) ) ) {
        return( -1 );
    }

    size = FGetVarRecLen( fcb->fileptr );
    IOOk( fcb->fileptr );
    return( size );
}
