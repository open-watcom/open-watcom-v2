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
* Description:  Run-time READ statement processor
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "rundat.h"
#include "rtenv.h"
#include "units.h"

static  void    ExRead( void ) {
//========================

    IOCB->flags &= ~IOF_OUTPT;
    IOPrologue();
    if( IOCB->flags & IOF_NOFMT ) {
        UnFmtIn();
    } else {
        if( IOCB->set_flags & SET_FMTPTR ) {
            FmtRoutine();
        } else {
            FreeIn();
        }
    }
    if( IOCB->set_flags & SET_INTERNAL ) {
        DiscoFile( IOCB->fileinfo );
    } else {
        IOCB->fileinfo->flags &= ~FTN_LOGICAL_RECORD;
        if( _LogicalRecordOrganization( IOCB->fileinfo ) ) {
            SkipLogicalRecord( IOCB->fileinfo );
        }
    }
}


int     DoRead( void ) {
//================

    IOCB->iostmt = IO_READ;
    if( ( IOCB->set_flags & SET_UNIT ) == 0 ) {
        IOCB->unitid = STANDARD_INPUT;
        IOCB->set_flags |= SET_UNIT;
    }
    return( IOMain( &ExRead ) );
}
