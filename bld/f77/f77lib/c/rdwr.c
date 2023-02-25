/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  READ/WRITE common
*
****************************************************************************/


#include "ftnstd.h"
#include <string.h>
#include "rundat.h"
#include "errcod.h"
#include "rtsysutl.h"
#include "rtutls.h"
#include "ioerr.h"
#include "rdwr.h"
#include "sdfile.h"


void    DoOpen( void ) {
//================

// Do the actual open of a file.

    ftnfile     *fcb;
    byte        action;

    fcb = IOCB->fileinfo;
    action = fcb->action;
    OpenAction( fcb );
    fcb->flags &= ~FTN_FSEXIST;
    if( fcb->fileptr != NULL ) {
        fcb->flags |= FTN_FSEXIST;
    }
    ChkIOErr( fcb );
}

void    _AllocBuffer( ftnfile *fcb ) {
//====================================

    if( fcb->buffer == NULL ) {
        if( IsCarriage() ) {
            fcb->bufflen += strlen( NormalCtrlSeq );
        }
        fcb->buffer = RChkAlloc( fcb->bufflen + 1 );
    }
}


void    IOPrologue( void ) {
//====================

    ftnfile     *fcb;
    byte        form;
    byte        accm;

    IOCB->typ = PT_NOT_STARTED;
    if( IOCB->flags & BAD_REC ) {
        IOErr( IO_IREC );
        // never return
    }
    if( IOCB->set_flags & SET_INTERNAL ) {
        F_Connect();
    } else {
        ChkUnitId();
        FindFtnFile();
        if( IOCB->fileinfo == NULL ) {
            IOCB->set_flags &= ~SET_FILENAME;
            ConnectFile();
        }
    }
    fcb = IOCB->fileinfo;
    if( fcb->action == ACT_DEFAULT ) {
        fcb->action = ACTION_RW;
    }
    ChkIOOperation( fcb );
    if( fcb->cctrl == CC_DEFAULT ) {
        fcb->cctrl = CC_NO;
    }
    accm = fcb->accmode;
    if( accm == ACCM_DEFAULT ) {
        if( IOCB->set_flags & SET_RECORDNUM ) {
            accm = ACCM_DIRECT;
        } else {
            accm = ACCM_SEQUENTIAL;
        }
        fcb->accmode = accm;
    } else {
        if( IOCB->set_flags & SET_RECORDNUM ) {
            if( ( accm == ACCM_SEQUENTIAL ) || ( accm == ACCM_APPEND ) ) {
                IOErr( IO_REC2_ACCM );
                // never return
            }
        } else {
            if( accm == ACCM_DIRECT ) {
                IOErr( IO_REC1_ACCM );
                // never return
            }
        }
    }
    if( accm == ACCM_DIRECT ) {
        fcb->recnum = IOCB->recordnum;  // set up recordnumber
    }
    form = fcb->formatted;
    if( form == 0 ) {                   // set up format if it was
        if( IOCB->flags & IOF_NOFMT ) { // not previously set
            form = UNFORMATTED_IO;
        } else {
            form = FORMATTED_IO;
        }
        fcb->formatted = form;
    } else {
        if( IOCB->flags & IOF_NOFMT ) {
            if( form == FORMATTED_IO ) {
                IOErr( IO_AF1 );
                // never return
            }
        } else {
            if( form == UNFORMATTED_IO ) {
                IOErr( IO_AF2 );
                // never return
            }
        }
    }
    if( fcb->internal != NULL ) {
        fcb->bufflen = fcb->internal->len;
        fcb->buffer = RChkAlloc( fcb->bufflen );
    } else {
        if( ( accm <= ACCM_SEQUENTIAL )
          && ( fcb->eofrecnum != 0 )
          && ( fcb->recnum >= fcb->eofrecnum )
            /*
             * Consider: READ( 1, *, END=10 )
             *           ...
             *     10    WRITE( 1, * ) 'write after EOF'
             * if an EOF condition was encounterd, we don't want IO_PAST_EOF
             * on the write
             */
          && (IOCB->flags & IOF_OUTPT) == 0 ) {
            if( fcb->recnum == fcb->eofrecnum ) {
                fcb->recnum++;
                SysEOF();
                // never return
            }
            IOErr( IO_PAST_EOF );
            // never return
        }
        _AllocBuffer( fcb );
        if( fcb->fileptr == NULL ) {
            DoOpen();
        }
        if( fcb->accmode == ACCM_DIRECT ) {
            SeekFile( fcb );
            ChkIOErr( fcb );
        }
        fcb->col = 0;
    }
    if( (IOCB->flags & IOF_OUTPT) && ( fcb->col == 0 ) ) {
        memset( fcb->buffer, ' ', fcb->bufflen );
    }
}


void    UpdateRecNum( ftnfile *fcb ) {
//====================================

    if( _NoRecordOrganization( fcb ) )
        return;
    if( _LogicalRecordOrganization( fcb ) ) {
        if( (fcb->flags & FTN_LOGICAL_RECORD) == 0 ) {
            // increment record count if this is first
            // physical record of logical record
            ++fcb->recnum;
        }
        // next record will be part of a logical record
        fcb->flags |= FTN_LOGICAL_RECORD;
    } else {
        ++fcb->recnum;
    }
}
