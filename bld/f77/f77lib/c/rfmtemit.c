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
* Description:  Process FORMAT specificaitons
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "format.h"
#include "errcod.h"
#include "rundat.h"
#include "fmtdef.h"
#include "fmtdat.h"

void    R_FEmInit( void ) {
//===================

}

void    CheckHole( uint size ) {
//==============================

    if( IOCB->fmtlen < size ) {
        RTErr( FM_TOO_LARGE );
    }
    IOCB->fmtlen -= size;
}

void    R_FEmCode( int code ) {
//=============================

    CheckHole( sizeof( byte ) );
    if( ( code & REV_CODE ) == REV_CODE ) {
        code &= ~REV_CODE;
        Fmt_revert.rt = (char PGM *)(IOCB->fmtptr);
    }
    if( IOCB->flags & IOF_EXTEND_FORMAT ) {
        code |= EXTEND_FORMAT;
    }
    *(byte PGM *)IOCB->fmtptr = code;
    IOCB->fmtptr = (fmt_desc PGM *)((byte PGM *)IOCB->fmtptr + 1);
}

void    R_FEmChar( char PGM *cur_char_ptr ) {
//===========================================

    CheckHole( sizeof( char ) );
    *(char PGM *)IOCB->fmtptr = *cur_char_ptr;
    IOCB->fmtptr = (fmt_desc PGM *)((char PGM *)IOCB->fmtptr + 1);
}

void    R_FEmNum( int num ) {
//===========================

    CheckHole( sizeof( int ) );
    *(int PGM *)IOCB->fmtptr = num;
    IOCB->fmtptr = (fmt_desc PGM *)((int PGM *)IOCB->fmtptr + 1);
}

void    R_FEmEnd( void ) {
//==================

    R_FEmCode( END_FORMAT );
    R_FEmNum( (char PGM *)(IOCB->fmtptr) - Fmt_revert.rt );
}

void    R_FEmByte( int signed_num ) {
//=============================

    uint num = signed_num;	// needed to match signature

    CheckHole( sizeof( byte ) );
    if( num > 256 ) {
        RTErr( FM_SPEC_256 );
    } else {
        *(byte PGM *)IOCB->fmtptr = num;
        IOCB->fmtptr = (fmt_desc PGM *)((byte PGM *)IOCB->fmtptr + 1);
    }
}
