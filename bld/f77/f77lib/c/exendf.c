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
* Description:  Run-time ENDFILE statement processor
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "rundat.h"
#include "rtenv.h"
#include "rtsysutl.h"
#include "rtutls.h"
#include "iomain.h"
#include "exutil.h"


static  void    ExEndFile( void ) {
//===========================

    ftnfile     *fcb;

    ChkUnitId();
    FindFtnFile();
    ChkConnected();
    fcb = IOCB->fileinfo;
    ChkIOOperation( fcb );
    ChkSequential( IO_AEND );
    ClrBuff();
    if( !( fcb->flags & FTN_FSEXIST ) ) {
        SysCreateFile( fcb );
    }
    // Some files like TERMINAL, SERIAL have no EOF.
    if( !NoEOF( fcb ) ) {
        EndFilef( fcb );        // set end of file
        if( !_NoRecordOrganization( fcb ) ) {
            fcb->eofrecnum = fcb->recnum;
            fcb->recnum++;          // recnum is set 1 past endfile record
        }
        ChkIOErr( fcb );
        fcb->flags |= FTN_EOF;
    }
}


int     IOEndf( void ) {
//================

    IOCB->iostmt = IO_EFILE;
    return( IOMain( &ExEndFile ) );
}
