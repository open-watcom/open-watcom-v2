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
* Description:  Run-time INQUIRE statement processor
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "errcod.h"
#include "rundat.h"
#include "rtenv.h"
#include "rtutls.h"
#include "iomain.h"
#include "ioerr.h"


static  void    ExInquire( void ) {
//===========================

    bool        exist;
    bool        opened;
    bool        connected;
    bool        inq_by_file;

    connected = true;
    if( IOCB->set_flags & SET_FILENAME ) {
        // inquire by file name
        inq_by_file = true;
        // FindFName() will set up a dummy fcb if false is returned
        opened = FindFName();
        if( opened ) {
            exist = ( IOCB->fileinfo->flags & FTN_EXIST ) != 0;
        } else {
            exist = ( IOCB->fileinfo->flags & FTN_FSEXIST ) != 0;
        }
    } else {
        // inquire by unit
        inq_by_file = false;
        if( IOCB->flags & BAD_UNIT ) {
            IOErr( IO_IUNIT );
        }
        FindFtnFile();
        exist = ( IOCB->unitid <= PREC_MAX_UNIT );
        opened = ( IOCB->fileinfo != NULL );
        if( !opened ) {
            connected = false;
        }
    }
    DfltInq();
    if( exist && connected ) {
        InqExList();
    }
    if( opened ) {
        InqOdList();
    } else if( inq_by_file ) { // disconnect dummy fcb created by FindFName()
        DiscoFile( IOCB->fileinfo );
    }
    if( IOCB->set_flags & SET_EXSTPTR ) {
        *IOCB->exstptr = exist;
    }
    if( IOCB->set_flags & SET_OPENPTR ) {
        *IOCB->openptr = opened;
    }
    // if we got this far, no error in inquire so
    // don't let IOMain() overwrite the iostat we set
    IOCB->set_flags &= ~SET_IOSPTR;
}


int     IOInq( void ) {
//===============

    IOCB->iostmt = IO_INQ;
    return( IOMain( &ExInquire ) );
}
