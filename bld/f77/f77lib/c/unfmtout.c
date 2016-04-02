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
* Description:  Run-time unformatted output.
*
****************************************************************************/


#include "ftnstd.h"
#include "rundat.h"
#include "errcod.h"
#include "pgmacc.h"
#include "ftextfun.h"

#include <string.h>
#include "iotype.h"
#include "rtutls.h"
#include "ioerr.h"


extern  void            SendEOR(void);

/* Forward declarations */
static  void    OUnString( void );
static  void    OUnArray( void );


extern  const byte __FAR        SizeVars[];


static  void    IOItemCopy( char PGM *dst, char PGM *src, PTYPE typ ) {
//===================================================================

    switch( typ ) {
    case PT_LOG_1:
        *(logstar1 *)(dst) = *(logstar1 *)src;
        break;
    case PT_LOG_4:
        *(logstar4 *)(dst) = *(logstar4 *)src;
        break;
    case PT_INT_1:
        *(intstar1 *)(dst) = *(intstar1 *)src;
        break;
    case PT_INT_2:
        *(intstar2 *)(dst) = *(intstar2 *)src;
        break;
    case PT_INT_4:
        *(intstar4 *)(dst) = *(intstar4 *)src;
        break;
    case PT_REAL_4:
        *(single *)(dst) = *(single *)src;
        break;
    case PT_REAL_8:
        *(double *)(dst) = *(double *)src;
        break;
    case PT_REAL_16:
        *(extended *)(dst) = *(extended *)src;
        break;
    case PT_CPLX_8:
        ((scomplex *)(dst))->imagpart = ((scomplex *)src)->imagpart;
        ((scomplex *)(dst))->realpart = ((scomplex *)src)->realpart;
        break;
    case PT_CPLX_16:
        ((dcomplex *)(dst))->imagpart = ((dcomplex *)src)->imagpart;
        ((dcomplex *)(dst))->realpart = ((dcomplex *)src)->realpart;
        break;
    case PT_CPLX_32:
        ((xcomplex *)(dst))->imagpart = ((xcomplex *)src)->imagpart;
        ((xcomplex *)(dst))->realpart = ((xcomplex *)src)->realpart;
        break;
    }
}


static  void    OutChkRecPos( ftnfile *fcb, uint len ) {
//======================================================

// Make sure record length has not been exceeded for direct access files.

    if( fcb->col + len > fcb->bufflen ) {
        IOErr( IO_UNFMT_RECL );
    }
}


static  void    OutChkRecBuff( ftnfile *fcb, uint len ) {
//=======================================================

// For sequential access files:
//      1. make sure the user hasn't specified a record size that
//         is too small to contain the i/o item (i.e. a double precision
//         i/o item will not fit in a 4-byte buffer)
//      2. make sure there's room in the buffer for the i/o item

    if( len > fcb->bufflen ) {
        IOErr( IO_UNFMT_RECL );
    }
    if( fcb->col + len > fcb->bufflen ) {
        SendEOR();
    }
}


void    UnFmtOut( void ) {
//==================

    uint        len;
    char        *ptr;
    PTYPE       typ;
    ftnfile     *fcb;
    char        *d;

    fcb = IOCB->fileinfo;
    typ = IOTypeRtn();
    while( typ != PT_NOTYPE ) {
        if( typ == PT_CHAR ) {
            OUnString();
        } else if( typ == PT_ARRAY ) {
            OUnArray();
        } else {
            ptr = (char *)&IORslt;
            len = SizeVars[ typ ];
            if( IsFixed() ) {
                OutChkRecPos( fcb, len );
            } else {
                OutChkRecBuff( fcb, len );
            }
            d = fcb->buffer + fcb->col;
            IOItemCopy( d, ptr, typ );
            fcb->col += len;
        }
        typ = IOTypeRtn();
    }
    IOCB->typ = typ;
    SendEOR();
}


static  void    OUnBytes( char HPGM *src, unsigned long len, PTYPE item_typ ) {
//============================================================================

    char        *dst;
    ftnfile     *fcb;
    uint        amt;

    fcb = IOCB->fileinfo;
    if( IsFixed() ) {
        OutChkRecPos( fcb, len );
    }
    dst = &fcb->buffer[ fcb->col ];
    for(;;) {
        amt = fcb->bufflen - fcb->col;
        // make sure an item does not cross a record boundary
        switch( item_typ ) {
        case PT_LOG_1:
        case PT_INT_1:
        case PT_CHAR:
            break;
        case PT_INT_2:
            amt &= -sizeof( intstar2 );
            break;
        case PT_LOG_4:
            amt &= -sizeof( logstar4 );
            break;
        case PT_INT_4:
            amt &= -sizeof( intstar4 );
            break;
        case PT_REAL_4:
            amt &= -sizeof( single );
            break;
        case PT_REAL_8:
            amt &= -sizeof( double );
            break;
        case PT_REAL_16:
            amt &= -sizeof( extended );
            break;
        case PT_CPLX_8:
            amt &= -sizeof( scomplex );
            break;
        case PT_CPLX_16:
            amt &= -sizeof( dcomplex );
            break;
        case PT_CPLX_32:
            amt &= -sizeof( xcomplex );
            break;
        }
        if( amt > len ) {
            amt = len;
        }
        pgm_memget( dst, src, amt );
        fcb->col += amt;
        len -= amt;
        if( len == 0 ) break;
        src += amt;
        dst = fcb->buffer;
        SendEOR();
    }
}


static  void    OUnArray( void ) {
//==========================

    uint        elmt_size;

    if( IORslt.arr_desc.typ == PT_CHAR ) {
        elmt_size = IORslt.arr_desc.elmt_size;
    } else {
        elmt_size = SizeVars[ IORslt.arr_desc.typ ];
    }
    OUnBytes( IORslt.arr_desc.data, IORslt.arr_desc.num_elmts * elmt_size,
              IORslt.arr_desc.typ );
}


static  void    OUnString( void ) {
//===========================

    OUnBytes( IORslt.string.strptr, IORslt.string.len, PT_CHAR );
}
