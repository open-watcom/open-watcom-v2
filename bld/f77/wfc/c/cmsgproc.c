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
// MSGPROC   : Message processing.
//

#include "ftnstd.h"
#include "cioconst.h"
#include "inout.h"

#include <stdarg.h>

extern  void            (*__BldErrMsg)(uint,char *,va_list);


void    MsgPrtLstNL( uint msg, ... ) {
//====================================

// Display message to the listing file.

    char        msgbuff[LIST_BUFF_SIZE+1];
    va_list     args;

    va_start( args, msg );
    __BldErrMsg( msg, msgbuff, args );
    va_end( args );
    PrtLstNL( msgbuff );
}


void    MsgPrintErr( uint msg, ...  ) {
//=====================================

// Display message to the error and listing file.

    char        msgbuff[LFERR_BUFF_SIZE+1];
    va_list     args;

    va_start( args, msg );
    __BldErrMsg( msg, msgbuff, args );
    va_end( args );
    PrintErr( msgbuff );
}


void    MsgJustErr( uint msg, ...  ) {
//====================================

// Display message to error file.

    char        msgbuff[ERR_BUFF_SIZE+1];
    va_list     args;

    va_start( args, msg );
    __BldErrMsg( msg, msgbuff, args );
    va_end( args );
    JustErr( msgbuff );
}
