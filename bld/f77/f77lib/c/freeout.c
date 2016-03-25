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
* Description:  Free-format output
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "rundat.h"
#include "iotype.h"
#include "undefrtn.h"

#include <string.h>

static  void    OutReal( void ) {
//=========================

    FmtRealRtn( IOCB->buffer, &IORslt.single );
    F_SendData( IOCB->buffer, REAL_IO_WINDOW );
}


static  void    OutDble( void ) {
//=========================

    FmtDoubleRtn( IOCB->buffer, &IORslt.dble );
    F_SendData( IOCB->buffer, DOUBLE_IO_WINDOW );
}


static  void    OutXtnd( void ) {
//=========================

    FmtExtendedRtn( IOCB->buffer, &IORslt.extended );
    F_SendData( IOCB->buffer, EXTENDED_IO_WINDOW );
}


static  void    OutCplx( void ) {
//=========================

    char        *buff;

    buff = IOCB->buffer;
    *buff = '(';
    FmtRealRtn( buff + sizeof( char ), &IORslt.scomplex.realpart );
    buff += strlen( buff );
    *buff = ',';
    FmtRealRtn( buff + sizeof( char ), &IORslt.scomplex.imagpart );
    buff += strlen( buff );
    *buff = ')';
    buff[ 1 ] = NULLCHAR;
    F_SendData( IOCB->buffer, COMPLEX_IO_WINDOW );
}


static  void    OutDbcx( void ) {
//=========================

    char        *buff;

    buff = IOCB->buffer;
    *buff = '(';
    FmtDoubleRtn( buff + sizeof( char ), &IORslt.dcomplex.realpart );
    buff += strlen( buff );
    *buff = ',';
    FmtDoubleRtn( buff + sizeof( char ), &IORslt.dcomplex.imagpart );
    buff += strlen( buff );
    *buff = ')';
    buff[ 1 ] = NULLCHAR;
    F_SendData( IOCB->buffer, DCOMPLEX_IO_WINDOW );
}


static  void    OutXtcx( void ) {
//=========================

    char        *buff;

    buff = IOCB->buffer;
    *buff = '(';
    FmtExtendedRtn( buff + sizeof( char ), &IORslt.xcomplex.realpart );
    buff += strlen( buff );
    *buff = ',';
    FmtExtendedRtn( buff + sizeof( char ), &IORslt.xcomplex.imagpart );
    buff += strlen( buff );
    *buff = ')';
    buff[ 1 ] = NULLCHAR;
    F_SendData( IOCB->buffer, XCOMPLEX_IO_WINDOW );
}


static  void    OutString( void ) {
//===========================

    if( IOCB->flags & NML_DIRECTED ) {
        Drop( '\'' );
    }
    SendStrRtn( IORslt.string.strptr, IORslt.string.len );
    if( IOCB->flags & NML_DIRECTED ) {
        Drop( '\'' );
    }
}


void    (* __FAR OutRtn[])( void ) = {        // this is not const anymore
        NULL,
        &OutLogCG,                      // these CG rtns might be modified
        &OutLogCG,
        &OutIntCG,
        &OutIntCG,
        &OutIntCG,
        &OutReal,
        &OutDble,
        &OutXtnd,
        &OutCplx,
        &OutDbcx,
        &OutXtcx,
        &OutString
};


void    FreeOut( void ) {
//=================

    PTYPE       typ;

    CheckCCtrl();
    for(;;) {
        typ = IOTypeRtn();
        IOCB->typ = typ;
        if( typ == PT_NOTYPE ) break;
        if( typ == PT_ARRAY ) {
            IOCB->arr_desc = IORslt.arr_desc;
            typ = IOCB->arr_desc.typ;
            IOCB->typ = typ;
            if( typ == PT_CHAR ) {
                IORslt.string.len = IOCB->arr_desc.elmt_size;
            } else {
                IOCB->arr_desc.elmt_size = SizeVars[ typ ];
            }
            for(;;) {
                if( typ == PT_CHAR ) {
                    IORslt.string.strptr = IOCB->arr_desc.data;
                } else {
                    IOItemResult( IOCB->arr_desc.data, typ );
                }
                OutRtn[ typ ]();
                --IOCB->arr_desc.num_elmts;
                if( IOCB->arr_desc.num_elmts == 0 ) break;
                IOCB->arr_desc.data += IOCB->arr_desc.elmt_size;
            }
        } else {
            OutRtn[ typ ]();
        }
    }
    SendEOR();
}
