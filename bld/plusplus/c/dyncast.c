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


#include "plusplus.h"
#include "cgfront.h"
#include "errdefns.h"
#include "convctl.h"
#include "rtfuncod.h"
#include "analtyid.h"
#include "rtti.h"

PTREE DynamicCastVoid           // DYNAMIC CAST CODE: VOID *
    ( CONVCTL* ctl )            // - conversion information
{
    PTREE expr;                 // - cast expression
    PTREE from;                 // - type being case from
    TOKEN_LOCN locn;            // - cast location
    CLASSINFO *info;            // - source (or base class of source) class info

    expr = ctl->expr;
    expr = PTreeExtractLocn( expr, &locn );
    expr = NodePruneLeftTop( expr );
    expr->locn = locn;
    from = NodeTypeid( ctl->src.pted );
    info = GetWithinOffsetOfVFPtr( ctl->src.pted, &expr );
    expr = NodeArguments( from
                        , NodeOffset( info->vf_offset )
                        , expr
                        , NULL );
    expr = RunTimeCall( expr, ctl->tgt.orig , RTF_DYN_CAST_VOID );
    return expr;
}

PTREE DynamicCast               // DYNAMIC CAST CODE
    ( CONVCTL* ctl )            // - conversion information
{
    PTREE expr;                 // - cast expression
    TOKEN_LOCN locn;            // - cast location
    PTREE from;                 // - type being cast from
    PTREE to;                   // - type being cast to
    CLASSINFO *info;            // - source (or base class of source) class info

    if( ctl->tgt.pted->id == TYP_VOID ) {
        return( DynamicCastVoid( ctl ) );
    } else {
        expr = ctl->expr;
        expr = PTreeExtractLocn( expr, &locn );
        expr = NodePruneLeftTop( expr );
        expr->locn = locn;
        from = NodeTypeid( ctl->src.pted );
        to = NodeTypeid( ctl->tgt.pted );
        info = GetWithinOffsetOfVFPtr( ctl->src.pted, &expr );
        expr = NodeArguments( to
                            , from
                            , NodeOffset( info->vf_offset )
                            , expr
                            , NULL );
        DbgAssert( ctl->src.reference == ctl->tgt.reference );
        if( ctl->src.reference && ctl->tgt.reference ) {
            // ref => ref
            expr = RunTimeCall( expr, ctl->tgt.orig, RTF_DYN_CAST_REF );
        } else {
            // ptr => ptr
            DbgAssert( !ctl->src.reference && !ctl->tgt.reference );
            expr = RunTimeCall( expr, ctl->tgt.orig , RTF_DYN_CAST_PTR );
        }
    }
    return expr;
}
