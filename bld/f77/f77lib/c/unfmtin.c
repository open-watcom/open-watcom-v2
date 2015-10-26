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
// UNFMTIN      : Run-time unformatted input
//

#include "ftnstd.h"
#include "rundat.h"
#include "errcod.h"
#include "pgmacc.h"
#include "iotype.h"

#include <string.h>

extern  void                    IOErr(int,...);
extern  void                    NextRec(void);
extern  bool                    IsFixed(void);
extern  bool                    CheckLogicalRecord(ftnfile *);

/* Forward declarations */
static  void    StreamUnFmtIn( void );
static  void    RecordUnFmtIn( void );
static  void    IUnString( void );
static  void    IUnArray( void );


extern  const byte __FAR        SizeVars[];


void    UnFmtIn( void ) {
//=================

    if( _NoRecordOrganization( IOCB->fileinfo ) ) {
        StreamUnFmtIn();
    } else {
        RecordUnFmtIn();
    }
}


void    NextUnFmtRec( void ) {
//======================

    if( _LogicalRecordOrganization( IOCB->fileinfo ) ) {
        if( CheckLogicalRecord( IOCB->fileinfo ) ) {
            NextRec();
            return;
        }
    }
    IOErr( IO_UNFMT_RECL );
}


static  void    UnFmtItem( void *s ) {
//====================================

    switch( IOCB->typ ) {
    case PT_LOG_1:
        *(logstar1 PGM *)(IORslt.pgm_ptr) = *(logstar1 *)s;
        break;
    case PT_LOG_4:
        *(logstar4 PGM *)(IORslt.pgm_ptr) = *(logstar4 *)s;
        break;
    case PT_INT_1:
        *(intstar1 PGM *)(IORslt.pgm_ptr) = *(intstar1 *)s;
        break;
    case PT_INT_2:
        *(intstar2 PGM *)(IORslt.pgm_ptr) = *(intstar2 *)s;
        break;
    case PT_INT_4:
        *(intstar4 PGM *)(IORslt.pgm_ptr) = *(intstar4 *)s;
        break;
    case PT_REAL_4:
        *(single PGM *)(IORslt.pgm_ptr) = *(single *)s;
        break;
    case PT_REAL_8:
        *(double PGM *)(IORslt.pgm_ptr) = *(double *)s;
        break;
    case PT_REAL_16:
        *(extended PGM *)(IORslt.pgm_ptr) = *(extended *)s;
        break;
    case PT_CPLX_8:
        ((scomplex PGM *)(IORslt.pgm_ptr))->imagpart =
                        ((scomplex *)s)->imagpart;
        ((scomplex PGM *)(IORslt.pgm_ptr))->realpart =
                        ((scomplex *)s)->realpart;
        break;
    case PT_CPLX_16:
        ((dcomplex PGM *)(IORslt.pgm_ptr))->imagpart =
                        ((dcomplex *)s)->imagpart;
        ((dcomplex PGM *)(IORslt.pgm_ptr))->realpart =
                        ((dcomplex *)s)->realpart;
        break;
    case PT_CPLX_32:
        ((xcomplex PGM *)(IORslt.pgm_ptr))->imagpart =
                        ((xcomplex *)s)->imagpart;
        ((xcomplex PGM *)(IORslt.pgm_ptr))->realpart =
                        ((xcomplex *)s)->realpart;
        break;
    }
}


static  void    RecordUnFmtIn( void ) {
//===============================

    ftnfile     *fcb;
    int         len;

    fcb = IOCB->fileinfo;
    NextRec();
    for(;;) {
        IOCB->typ = IOTypeRtn();
        if( IOCB->typ == PT_NOTYPE ) break;
        if( fcb->col == fcb->len ) {
            NextUnFmtRec();
        }
        if( IOCB->typ == PT_CHAR ) {
            IUnString();
        } else if( IOCB->typ == PT_ARRAY ) {
            IUnArray();
        } else {
            len = SizeVars[ IOCB->typ ];
            if( fcb->col + len > fcb->len ) {
                IOErr( IO_UNFMT_RECL );
            }
            UnFmtItem( fcb->buffer + fcb->col );
            fcb->col += len;
        }
    }
}


static  void    IUnStream( char HPGM *dst, unsigned long len ) {
//==============================================================

    uint        amt;
    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    for(;;) {
        amt = fcb->bufflen;
        if( amt > len ) {
            amt = len;
        }
        fcb->len = amt;
        NextRec();
        pgm_memput( dst, fcb->buffer, fcb->len );
        len -= amt;
        if( len == 0 ) break;
        dst += amt;
    }
}


static  void    StreamUnFmtIn( void ) {
//===============================

    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    for(;;) {
        IOCB->typ = IOTypeRtn();
        if( IOCB->typ == PT_NOTYPE ) break;
        if( IOCB->typ == PT_CHAR ) {
            IUnStream( IORslt.string.strptr, IORslt.string.len );
        } else if( IOCB->typ == PT_ARRAY ) {

            uint        elmt_size;

            if( IORslt.arr_desc.typ == PT_CHAR ) {
                elmt_size = IORslt.arr_desc.elmt_size;
            } else {
                elmt_size = SizeVars[ IORslt.arr_desc.typ ];
            }
            IUnStream( IORslt.arr_desc.data,
                       IORslt.arr_desc.num_elmts * elmt_size );
        } else {
            fcb->len = SizeVars[ IOCB->typ ];
            if( fcb->len > fcb->bufflen ) {
                IOErr( IO_UNFMT_RECL );
            }
            NextRec();
            UnFmtItem( fcb->buffer );
        }
    }
}


static  void    IUnBytes( char HPGM *dst, unsigned long len ) {
//=============================================================

    uint        amt;
    ftnfile     *fcb;
    char        *src;

    fcb = IOCB->fileinfo;
    if( IsFixed() && ( fcb->col + len > fcb->len ) ) {
        IOErr( IO_UNFMT_RECL );
    }
    src = fcb->buffer + fcb->col;
    for(;;) {
        amt = fcb->len - fcb->col;
        if( amt > len ) {
            amt = len;
        }
        pgm_memput( dst, src, amt );
        fcb->col += amt;
        len -= amt;
        if( len == 0 ) break;
        dst += amt;
        src = fcb->buffer;
        NextUnFmtRec();
    }
}


static  void    IUnArray( void ) {
//==========================

    uint        elmt_size;

    if( IORslt.arr_desc.typ == PT_CHAR ) {
        elmt_size = IORslt.arr_desc.elmt_size;
    } else {
        elmt_size = SizeVars[ IORslt.arr_desc.typ ];
    }
    IUnBytes( IORslt.arr_desc.data, IORslt.arr_desc.num_elmts * elmt_size );
}


static  void    IUnString( void ) {
//===========================

    IUnBytes( IORslt.string.strptr, IORslt.string.len );
}
