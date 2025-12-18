/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "format.h"
#include "errcod.h"
#include "rundat.h"
#include "fmtdef.h"
#include "fmtdat.h"
#include "rterr.h"
#include "rfmtemit.h"


static char     PGM *Fmt_revert;    // position to revert to if required

void    R_FEmInit( void )
//=======================
{
    Fmt_revert = FmtBuff;
}

static void CheckHole( uint size )
//================================
{
    if( IOCB->fmtlen < size ) {
        RTErr( FM_TOO_LARGE );
    }
    IOCB->fmtlen -= size;
}

void    R_FEmCode( int code )
//===========================
{
    CheckHole( sizeof( byte ) );
    if( ( code & REV_CODE ) == REV_CODE ) {
        code &= ~REV_CODE;
        Fmt_revert = IOCB->u.ptr;
    }
    if( IOCB->flags & IOF_EXTEND_FORMAT ) {
        code |= EXTEND_FORMAT;
    }
    *IOCB->u.ptr++ = code;
}

void    R_FEmChar( char PGM *cur_char_ptr )
//=========================================
{
    CheckHole( 1 );
    *IOCB->u.ptr++ = *cur_char_ptr;
}

void    R_FEmNum( int num )
//=========================
{
    CheckHole( sizeof( int ) );
    *(int PGM *)IOCB->u.ptr = num;
    IOCB->u.ptr += sizeof( int );
}

void    R_FEmEnd( void )
//======================
{
    R_FEmCode( END_FORMAT );
    R_FEmNum( IOCB->u.ptr - Fmt_revert );
}

void    R_FEmByte( int signed_num )
//=================================
{
    uint num = signed_num;      // needed to match signature

    CheckHole( sizeof( byte ) );
    if( num > 256 ) {
        RTErr( FM_SPEC_256 );
    } else {
        *IOCB->u.ptr++ = num;
    }
}
