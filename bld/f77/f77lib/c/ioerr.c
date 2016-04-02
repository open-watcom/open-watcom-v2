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
* Description:  I/O error processing
*
****************************************************************************/


#include "ftnstd.h"
#include "xfflags.h"
#include "errcod.h"
#include "rundat.h"
#include "cioconst.h"
#include "blderr.h"
#include "rtspawn.h"
#include "rterr.h"
#include "rtsysutl.h"


static  void    SysIOErr( int errcode, ... ) {
//============================================

    va_list     args;

    va_start( args, errcode );
    RTErrHandler( errcode, args );
    va_end( args );
}


void    IOErr( int errcode, ... ) {
//=================================

// Handle a run time i/o error.

    char        errbuff[ERR_BUFF_SIZE+1];
    va_list     args;

    va_start( args, errcode );
        if( ( IOCB->set_flags & (SET_IOSPTR|SET_ERRSTMT) ) == 0 ) {
            if( errcode == IO_FILE_PROBLEM ) {
                GetIOErrMsg( IOCB->fileinfo, errbuff );
                SysIOErr( errcode, errbuff );
            } else {
                RTErrHandler( errcode, args );
            }
        } else {
            errcode = ErrCodOrg( errcode );
            if( IOCB->fileinfo != NULL ) {
                IOCB->fileinfo->error = errcode;
            }
            IOCB->status = errcode;
        }
        RTSuicide();
    va_end( args );
}
