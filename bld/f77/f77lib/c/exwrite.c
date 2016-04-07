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
* Description:  Run-time WRITE statement processor
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "rundat.h"
#include "rtenv.h"
#include "units.h"
#include "exwrite.h"
#include "rtutls.h"
#include "iomain.h"
#include "rdwr.h"


static  void    ExWrite( void ) {
//=========================

    IOCB->flags |= IOF_OUTPT;
    IOPrologue();
    if( IOCB->flags & IOF_NOFMT ) {
        UnFmtOut();
    } else {
        if( IOCB->set_flags & SET_FMTPTR ) {
            FmtRoutine();
        } else {
            FreeOut();
        }
    }
    if( IOCB->set_flags & SET_INTERNAL ) {
        DiscoFile( IOCB->fileinfo );
    } else if( IOCB->flags & IOF_OUTPT ) {
        IOCB->fileinfo->flags |= FTN_FSEXIST;
        IOCB->fileinfo->flags &= ~FTN_LOGICAL_RECORD;
    }
}


int     DoWrite( void ) {
//=================

    IOCB->iostmt = IO_WRITE;
    if( ( IOCB->set_flags & SET_UNIT ) == 0 ) {
        IOCB->unitid = STANDARD_OUTPUT;
        IOCB->set_flags |= SET_UNIT;
    }
    return( IOMain( &ExWrite ) );
}
