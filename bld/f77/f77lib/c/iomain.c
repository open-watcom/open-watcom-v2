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
* Description:  I/O statement entry point
*
****************************************************************************/


#include "ftnstd.h"
#include "frtdata.h"
#include "fthread.h"
#include "xfflags.h"
#include "rundat.h"
#include "iotype.h"
#include "errcod.h"
#include "thread.h"
#include "rtenv.h"
#include "rtspawn.h"
#include "rterr.h"
#include "rtsysutl.h"
#include "rtutls.h"
#include "iomain.h"


void     __ReleaseIOSys( void )
//=============================
{
    IOCB->flags &= ~IOF_ACTIVE;
    _ReleaseFIO();
    IOTypeRtn = &IOType;
}


int     IOMain( void (*io_rtn)( void ) ) {
//==================================

    int         io_stmt;
    int         io_stat;

    // turn of IOF_SETIOCB so that we can detect "i/o already active"  - by
    // the time we call IOMain(), no one should be checking IOF_SETIOCB
    IOCB->flags &= ~IOF_SETIOCB;
    io_stmt = IOCB->iostmt;
    if( ( RTSpawn( io_rtn ) != 0 ) && ( IOCB->fileinfo != NULL ) &&
        ( ( io_stmt == IO_READ ) || ( io_stmt == IO_WRITE ) ) ) {
        IOCB->fileinfo->col = 0; // so next statement starts new record
        if( ( io_stmt == IO_READ ) &&
                            _LogicalRecordOrganization( IOCB->fileinfo ) ) {
            SkipLogicalRecord( IOCB->fileinfo );
        }
        IOCB->fileinfo->flags &= ~FTN_LOGICAL_RECORD; // in case we got EOF
        if( IOCB->set_flags & SET_INTERNAL ) {
            DiscoFile( IOCB->fileinfo );
        }
    }
    if( _RWD_XcptFlags & XF_IO_INTERRUPTED ) {
        RTErr( KO_INTERRUPT );
    }
    if( _RWD_XcptFlags & XF_FATAL_ERROR ) {
        __ReleaseIOSys(); // so other threads can continue
        RTSuicide();
    }
    io_stat = IOCB->status;
    if( IOCB->set_flags & SET_IOSPTR ) { // set up IOSTAT
        *IOCB->iosptr = io_stat;
    }
    if( io_stat == 0 ) {
        while( IOCB->typ != PT_NOTYPE ) {     // flush the io list
            IOCB->typ = IOTypeRtn();
        }
    }
    if( io_stmt == IO_READ ) {
        // Consider: READ( 1, *, END=10 )
        //              ...
        //      10   WRITE( 1, * ) 'write after EOF'
        // the record number got incremented, so if an EOF condition
        // was encounterd we must adjust the record number so that
        // we don't get IO_PAST_EOF on the write
        if( ( IOCB->set_flags & SET_INTERNAL ) == 0 ) {
            // Consider:    READ(5,*,IOSTAT=IOS) I
            //              READ(5,*) I
            // If the first read gets EOF, then we must clear eof before
            // doing the next read so that we don't get EOF again.
            // Note: This is to be done only for files that don't have an
            // EOF (like TERMINAL).
            ClearEOF();
        }
    }
#if defined( __SW_BM ) && !defined( _SA_LIBRARY )
    // we cannot release the i/o system for READ/WRITE statements since
    // co-routines are not done yet
    if( (io_stmt != IO_READ) && (io_stmt != IO_WRITE) )
#endif
        __ReleaseIOSys();
    _RWD_XcptFlags &= ~XF_IO_INTERRUPTABLE;
    return( io_stat );
}
