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
* Description:  array i/o support routines
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "rundat.h"
#include "iotype.h"

static  void    NextArrayItem(void) {
//===============================

    if( IOCB->flags & IOF_OUTPT ) {
        if( IOCB->typ == PT_CHAR ) {
            IORslt.string.strptr = IOCB->arr_desc.data;
            IORslt.string.len = IOCB->arr_desc.elmt_size;
        } else {
            IOItemResult( IOCB->arr_desc.data, IOCB->typ );
        }
    } else {
        if( IOCB->typ == PT_CHAR ) {
            IORslt.string.strptr = IOCB->arr_desc.data;
            IORslt.string.len = IOCB->arr_desc.elmt_size;
        } else {
            IORslt.pgm_ptr = IOCB->arr_desc.data;
        }
    }
    IOCB->arr_desc.data += IOCB->arr_desc.elmt_size;
    IOCB->arr_desc.num_elmts--;
    if( IOCB->arr_desc.num_elmts == 0 ) {
        IOCB->flags &= ~IOF_ARRAY_IO;
    }
}

void    ArrayIOType(void) {
//=====================

    if( IOCB->flags & IOF_ARRAY_IO ) {
        NextArrayItem();
    } else {
        IOCB->typ = IOTypeRtn();
        if( IOCB->typ == PT_ARRAY ) {
            IOCB->typ = IORslt.arr_desc.typ;
            IOCB->flags |= IOF_ARRAY_IO;
            IOCB->arr_desc = IORslt.arr_desc;
            if( IOCB->typ != PT_CHAR ) {
                IOCB->arr_desc.elmt_size = SizeVars[ IOCB->typ ];
            }
            NextArrayItem();
        }
    }
}
