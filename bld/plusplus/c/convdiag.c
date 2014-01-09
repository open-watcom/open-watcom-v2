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

static TYPE cnv_type_src;       // conversion type: source
static TYPE cnv_type_tgt;       // conversion type: target


void ConversionInfDisable(      // DISABLE DISPLAY OF CONVERSION INFORMATION
    void )
{
    cnv_type_src = NULL;
}


static void infMsgType(         // DISPLAY INFORMATION FOR A CONVERSION TYPE
    MSG_NUM msg_num,            // - message number
    TYPE type )                 // - TYPE in error
{
    TYPE cl_type;               // - type, when class or ref to class

    InfMsgPtr( msg_num, type );
    cl_type = ClassTypeForType( type );
    if( cl_type != NULL && !TypeDefined( cl_type ) ) {
        InfMsgPtr( INF_CLASS_NOT_DEFINED, cl_type );
    } else {
        TYPE ptr_type = PointerTypeEquivalent( type );
        if( NULL != ptr_type ) {
            type_flag not_used;
            cl_type = StructType( TypePointedAt( type, &not_used ) );
            if( NULL != cl_type && !TypeDefined( cl_type ) ) {
                InfMsgPtr( INF_CLASS_NOT_DEFINED, cl_type );
            }
        }
    }
}


void ConversionDiagnoseInfTgt(  // DIAGNOSE TARGET CONVERSION
    void )
{
    if( cnv_type_tgt != NULL ) {
        if( NULL == GenericType( cnv_type_tgt ) ) {
            infMsgType( INF_TGT_CNV_TYPE, cnv_type_tgt );
        }
    }
    ConversionInfDisable();
}


void ConversionDiagnoseInf(     // DIAGNOSE SOURCE/TARGET CONVERSION
    void )
{
    if( cnv_type_src != NULL ) {
        infMsgType( INF_SRC_CNV_TYPE, cnv_type_src );
        ConversionDiagnoseInfTgt();
    }
}


static unsigned conversionErr(  // ISSUE CONVERSION ERROR
    PTREE expr,                 // - current expression
    MSG_NUM msg_no )            // - diagnostic
{
    PTreeErrorExpr( expr, msg_no );
    ConversionDiagnoseInf();
    return CNV_ERR;
}


unsigned ConversionDiagnose(    // DIAGNOSE RETURN FROM A CONVERSION
    unsigned retn,              // - return value: CNV_...
    PTREE expr,                 // - current expression
    CNV_DIAG *diagnosis )       // - diagnosis information
{
    switch( retn ) {
      case CNV_OK_TRUNC :
        NodeWarnPtrTrunc( expr );
        retn = CNV_OK;
        break;
      case CNV_OK_TRUNC_CAST :
        NodeWarnPtrTruncCast( expr );
        retn = CNV_OK;
        break;
      case CNV_ERR :
        PTreeErrorNode( expr );
        ConversionDiagnoseInf();
        retn = CNV_ERR;
        break;
      case CNV_IMPOSSIBLE :
        retn = conversionErr( expr, diagnosis->msg_impossible );
        break;
      case CNV_AMBIGUOUS :
        retn = conversionErr( expr, diagnosis->msg_ambiguous );
        break;
      case CNV_PRIVATE :
        retn = conversionErr( expr, diagnosis->msg_private );
        break;
      case CNV_PROTECTED :
        retn = conversionErr( expr, diagnosis->msg_protected );
        break;
      case CNV_VIRT_DER :
        retn = conversionErr( expr, diagnosis->msg_virt_der );
        break;
      case CNV_TRUNC_THIS :
        ConversionInfDisable();
        retn = conversionErr( expr, ERR_THIS_OBJ_MEM_MODEL );
        break;
      case CNV_OK :
        break;
      DbgDefault( "ConversionDiagnose: unexpected 'retn' value" );
    }
    return retn;
}


void ConversionTypesSet(        // RE-SET CONVERSION TYPES
    TYPE src,                   // - new source
    TYPE tgt )                  // - new target
{
    cnv_type_src = src;
    cnv_type_tgt = tgt;
}


void ConversionDiagLR           // DIAGNOSE LEFT, RIGHT OPERANDS
    ( TYPE left                 // - left type
    , TYPE right )              // - right type
{
    if( NULL != left ) {
        InfMsgPtr( INF_LEFT_OPERAND_TYPE, left );
    }
    if( NULL != right ) {
        InfMsgPtr( INF_RIGHT_OPERAND_TYPE, right );
    }
}
