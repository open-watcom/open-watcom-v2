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
#include "fnovload.h"

#if 0
FNOV_RESULT UdcFind(            // FIND USER DEF CONVERSION TO ARGUMENT
    TYPE cl_type,               // - type for class
    type_flag this_qualifier,   // - T cv-qual *this; (cv-qual part)
    TYPE *tgt_type,             // - addr( target type )
    SYMBOL *psym )              // - addr( best conversion SYMBOL )
{
    SCOPE scope;
    SYMBOL sym;
    FNOV_RESULT ovret;

    sym = NULL;
    cl_type = ClassTypeForType( cl_type );
    if( cl_type != NULL ) {
        scope = cl_type->u.c.scope;
        sym = ScopeBestConversion( scope, this_qualifier, tgt_type );
    }
    if( sym == NULL ) {
        ovret = FNOV_NO_MATCH;
    } else if( *tgt_type == NULL ) {
        ovret = FNOV_AMBIGUOUS;
    } else {
        ovret = FNOV_NONAMBIGUOUS;
    }
    if( psym != NULL ) {
        *psym = sym;
    }
    return( ovret );
}
#endif


PTREE UdcCall                   // CALL UDC FUNCTION
    ( PTREE src                 // - source expression
    , TYPE udcf_type            // - type for udcf
    , CALL_DIAG* diagnosis )    // - call diagnosis
{
    PTREE node;                 // - node under construction
    SEARCH_RESULT* result;      // - result for scope diagnosis
    TYPE class_type;            // - class for UDCF

    class_type = ClassTypeForType( NodeType( src ) );
    result = ScopeFindScopedMemberConversion
                    ( class_type->u.c.scope
                    , NULL
                    , udcf_type
                    , TF1_NULL );
    node = NodeMakeCallee( result->sym );
    node->u.symcg.result = result;
    node->cgop = CO_NAME_CONVERT;
    node = NodeDottedFunction( src, node );
    node = PTreeCopySrcLocation( node, src );
    node = NodeBinary( CO_CALL_NOOVLD, node, NULL );
    node = PTreeCopySrcLocation( node, src );
    node = AnalyseCall( node, diagnosis );
    return node;
}


TYPE UdcFindType                // FIND TARGET TYPE FOR UDCF
    ( TYPE src                  // - source class or reference to it
    , TYPE tgt )                // - target type
{
    TYPE result;                // - result type
    FNOV_COARSE_RANK rank;      // - UDC RANKING
    boolean is_ctor;            // - TRUE ==> ctor udc, FALSE ==> udcf udc
    FNOV_LIST* fnov_list;       // - matches list
    FNOV_DIAG diag;             // - lookup diagnosis

    if( NULL == ClassTypeForType( src ) ) {
        result = NULL;
    } else {
        rank = UdcLocate( FNOV_UDC_COPY
                        , src
                        , tgt
                        , NULL
                        , &is_ctor
                        , &fnov_list
                        , &diag );
        if( rank == OV_RANK_UD_CONV ) {
            result = SymFuncReturnType( fnov_list->sym );
        } else {
            result = NULL;
        }
        FnovListFree( &fnov_list );
        FnovFreeDiag( &diag );
    }
    return result;
}
