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


#include "cpplib.h"
#include <string.h>
#include "rtexcept.h"


RT_TYPE_SIG CPPLIB( ts_refed )( // POINT PAST REFERENCE TYPE-SIG, IF REQ'D
    RT_TYPE_SIG sig )           // - the signature
{
    if( sig->hdr.type == THROBJ_REFERENCE ) {
        sig = (RT_TYPE_SIG)( (char*)sig - offsetof( TS_HDR, hdr_ref ) );
        if( sig->base.indirect ) {
            sig = sig->indirected.sig;
        }
    }
    return sig;
}


RT_TYPE_SIG CPPLIB( ts_pnted )( // POINT PAST POINTER TYPE-SIG
    RT_TYPE_SIG sig )           // - the signature
{
    sig = (RT_TYPE_SIG)( (char*)sig - offsetof( TS_HDR, hdr_ptr ) );
    if( sig->base.indirect ) {
        sig = sig->indirected.sig;
    }
    return sig;
}


rboolean CPPLIB( ts_equiv )(    // TEST IF TYPE SIG.S ARE EQUIVALENT
    RT_TYPE_SIG tgt,            // - target type signature
    RT_TYPE_SIG src,            // - source type signature
    rboolean zero_thrown )      // - true ==> zero was thrown
{
    rboolean retn;              // - true ==> conversion possible

    if( tgt == NULL ) {
        retn = true;
    } else {
        tgt = CPPLIB( ts_refed )( tgt );
        retn = false;
        if( zero_thrown ) {
            switch( tgt->hdr.type ) {
              case THROBJ_PTR_SCALAR :
              case THROBJ_VOID_STAR :
              case THROBJ_PTR_CLASS :
                retn = true;
                break;
            }
        }
        if( ! retn ) {
            src = CPPLIB( ts_refed )( src );
            if( src == tgt ) {
                retn = true;
            } else if( tgt->hdr.type != src->hdr.type ) {
                retn = false;
            } else switch( tgt->hdr.type ) {
              case THROBJ_VOID_STAR :
              case THROBJ_ANYTHING :
                retn = true;
                break;
              case THROBJ_PTR_SCALAR :
                if( tgt->base.indirect != src->base.indirect ) {
                    retn = false;
                    break;
                }
                // drops thru
              case THROBJ_PTR_CLASS :
                retn = CPPLIB( ts_equiv )( CPPLIB( ts_pnted )( tgt )
                                         , CPPLIB( ts_pnted )( src )
                                         , zero_thrown );
                break;
              case THROBJ_CLASS :
              case THROBJ_CLASS_VIRT :
                if( tgt->clss.size == src->clss.size ) {
                    if( 0 == strcmp( tgt->clss.name, src->clss.name ) ) {
                        retn = true;
                    } else {
                        retn = false;
                    }
                } else {
                    retn = false;
                }
                break;
              case THROBJ_PTR_FUN :
              case THROBJ_SCALAR :
                if( tgt->scalar.size == src->scalar.size ) {
                    if( 0 == strcmp( tgt->scalar.name, src->scalar.name ) ) {
                        retn = true;
                    } else {
                        retn = false;
                    }
                } else {
                    retn = false;
                }
                break;
              default :
                GOOF( "TsEquiv -- bad THROBJ_..." );
            }
        }
    }
    return retn;
}
