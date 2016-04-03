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
#include "convctl.h"
#include "template.h"
#include "class.h"
#include "dbgdumpf.h"


static CNV_DIAG diagImpossible  // DIAGNOSIS FOR IMPOSSIBLE CONVERT FAILURE
=   { ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    };
#if 0
static CNV_DIAG diagMembFunCnv= // diagnosis for member-ptr conversion
{   ERR_MEMB_PTR_FUN_IMPOSSIBLE
,   ERR_MEMB_PTR_FUN_AMBIGUOUS
,   ERR_MEMB_PTR_DERIVED
,   ERR_MEMB_PTR_FUN_PRIVATE
,   ERR_MEMB_PTR_FUN_PROTECTED
};
#endif


PTREE ConvCtlDiagnoseTypes      // DIAGNOSE CASTING TYPES
    ( CONVCTL* ctl )            // - conversion control
{
    ConversionTypesSet( ctl->src.orig, ctl->tgt.orig );
    if( NULL == ctl->src.orig ) {
        ConversionDiagnoseInfTgt();
    } else {
        ConversionDiagnoseInf();
    }
    return ctl->expr;
}


PTREE ConvCtlDiagnose           // DIAGNOSE CASTING ERROR
    ( CONVCTL* ctl )            // - conversion control
{
    PTreeErrorExpr( ctl->expr, ctl->msg_no );
    return ConvCtlDiagnoseTypes( ctl );
}


bool ConvCtlWarning             // ISSUE WARNING
    ( CONVCTL* ctl              // - conversion control
    , MSG_NUM msg_no )          // - message number
{
    bool retn;                  // - return: TRUE ==> ERROR was issued
    msg_status_t status;        // - message status

    status = PTreeErrorExpr( ctl->expr, msg_no );
    if( MS_PRINTED & status ) {
        if( MS_WARNING & status ) {
            ConvCtlDiagnoseTypes( ctl );
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static void diagnoseError       // DIAGNOSE ERROR AND SET UP ERROR OPERAND(S)
    ( CONVCTL* ctl              // - conversion control
    , MSG_NUM msg_no )          // - message #
{
    ctl->msg_no = msg_no;
    ConvCtlDiagnose( ctl );
    ctl->has_err_operand = TRUE;
}


bool ConvCtlTypeInit            // INITIALIZE CONVTYPE
    ( CONVCTL* ctl              // - conversion control
    , CONVTYPE* ctype           // - control info.
    , TYPE type )               // - type
{
    type_id id;                 // - id for unmodified type
    bool retn;                  // - TRUE ==> is bit_field, array, function
    TYPE cl_type;               // - class type

    ctype->orig = type;
    ctype->unmod = TypeGetActualFlags( type, &ctype->modflags );
    ctype->pted = NULL;
    ctype->class_type = NULL;
    ctype->ptedflags = 0;
    ctype->reference = FALSE;
    ctype->array = FALSE;
    ctype->bit_field = FALSE;
    ctype->class_operand = FALSE;
    ctype->pc_ptr = PC_PTR_NOT;
    id = ctype->unmod->id;
    retn = FALSE;
    cl_type = NULL;
    if( id == TYP_BITFIELD ) {
        ctype->bit_field = TRUE;
        retn = TRUE;
    } else if( id == TYP_ARRAY ) {
        ctype->array = TRUE;
        retn = TRUE;
    } else {
        ctype->kind = RkdForTypeId( id );
        switch( ctype->kind ) {
          case RKD_POINTER :
            if( ctype->unmod->flag & TF1_REFERENCE ) {
                TYPE refed = ctype->unmod->of;
                ctype->reference = TRUE;
                cl_type = StructType( refed );
            }
            break;
          case RKD_FUNCTION :
            retn = TRUE;
            break;
          case RKD_CLASS :
            cl_type = ctype->unmod;
            break;
          case RKD_ERROR :
            ctl->has_err_operand = TRUE;
            break;
        }
    }
    if( NULL != cl_type ) {
        cl_type = StructType( cl_type );
        if( ClassCorrupted( cl_type ) ) {
            cl_type = NULL;
            ctype->kind = RKD_ERROR;
            ctl->has_err_operand = TRUE;
        } else {
            ctype->class_type = cl_type;
            ctype->class_operand = TRUE;
        }
    }
    return retn;
}


void ConvCtlClassAnalysis       // ANALYSE CLASS TYPE
    ( CONVTYPE* ctype )         // - control info.
{
    if( ctype->reference ) {
        ctype->pted = TypeGetActualFlags( ctype->unmod->of, &ctype->ptedflags );
    } else {
        ctype->ptedflags = ctype->modflags;
        ctype->pted = ctype->unmod;
    }
}


static void errForFunc          // ISSUE ERROR FOR A FUNCTION
    ( PTREE func                // - function node
    , MSG_NUM msg_no            // - message #
    , CONVCTL* ctl )            // - conversion control
{
    PTreeErrorExpr( func, msg_no );
    ctl->has_err_operand = TRUE;
    ctl->tgt.kind = RKD_ERROR;
    PTreeErrorNode( ctl->expr );
}


static TYPE makeRefPtrFunc      // MAKE REFERENCE OR PTR TO FUNCTION
    ( CONVCTL* ctl              // - conversion control
    , TYPE ftype )              // - function type
{
    return ctl->tgt.reference
         ? MakeReferenceTo( ftype )
         : MakePointerTo( ftype );
}


static void setFunType          // SET FUNCTION TYPE
    ( PTREE node                // - node to be set
    , TYPE ftype )              // - function type
{
    node->flags &= ~ PTF_LVALUE;
    NodeSetType( node, ftype, 0 );
}


static void adjustFnAddr        // SET FUNCTION ADDR
    ( CONVCTL* ctl              // - conversion control
    , PTREE addrof              // - '&' node
    , PTREE func )              // - function node
{
    TYPE argument;              // - expression type
    SYMBOL sym;                 // - symbol for function

    sym = func->u.symcg.symbol;
    if( SymIsThisFuncMember( sym ) ) {
        errForFunc( func, ERR_ADDR_NONSTAT_MEMBER_FUNC, ctl );
    } else {
        sym->flag |= SF_ADDR_TAKEN;
        SymMarkRefed( sym );                                // ok?
        argument = makeRefPtrFunc( ctl, func->type );
        setFunType( addrof, argument );
        setFunType( ctl->expr->u.subtree[1], argument );
    }
}

static void adjustFnMembPtr     // SET MEMB-PTR TYPE
    ( CONVCTL* ctl              // - conversion control
    , PTREE func )              // - function node
{
    TYPE argument;              // - expression type
    SYMBOL sym;                 // - symbol
    CNV_RETN retn;              // - conversion return

//  func = NodeActualNonOverloaded( func );
    sym = func->u.symcg.symbol;
    if( SymIsThisFuncMember( sym ) ) {
        sym->flag |= SF_ADDR_TAKEN;
//      SymMarkRefed( sym );
        argument = MakeMemberPointerTo( SymClass( sym ), sym->sym_type );
//      ctl->expr->u.subtree[1]->type = argument;
        retn = MembPtrConvert( &ctl->expr->u.subtree[1]
                             , argument
                             , CNV_EXPR );
        DbgVerify( retn == CNV_OK, "failed memb ptr adjustment" );
    } else {
        errForFunc( func, ERR_MEMB_PTR_ADDR_OF, ctl );
    }
}


static void adjustFnAddrPtr     // ADJUST FOR &FUNCTION --> PTR
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE addrof;               // - '&' node
    PTREE func;                 // - function node ( &class::func )

    addrof = PTreeOp( &ctl->expr->u.subtree[1] );
    switch( NodeAddrOfFun( addrof, &func ) ) {
      case ADDR_FN_ONE_USED :
      case ADDR_FN_ONE :
        func = NodeActualNonOverloaded( func );
        adjustFnAddr( ctl, addrof, func );
        break;
      case ADDR_FN_MANY_USED :
      case ADDR_FN_MANY :
        switch( _CNV_TYPE( ctl->req ) ) {
          case CNV_ASSIGN :
          case CNV_CAST :
          case CNV_INIT :
          case CNV_INIT_COPY :
          case CNV_FUNC_ARG :
          case CNV_FUNC_RET :
            switch( ConvertOvFunNode( ctl->tgt.orig, func ) ) {
              case CNV_OK :
                adjustFnAddr( ctl, addrof, func );
                break;
              case CNV_AMBIGUOUS :
                errForFunc( func, ctl->diag_cast->msg_ambiguous, ctl );
                break;
              case CNV_IMPOSSIBLE :
                errForFunc( func, ctl->diag_cast->msg_impossible, ctl );
                break;
              DbgDefault("ConvertOfFunNode -- unexpected return code");
            }
            break;
          default :
            errForFunc( func, ERR_ADDR_OF_OVERLOADED_FUN, ctl );
            break;
        }
        break;
    }
}

static void adjustFnAddrMembPtr // ADJUST FOR &FUNCTION --> MEMB-PTR
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE addrof;               // - '&' node
    PTREE func;                 // - function node ( &class::func )
    TYPE ftype;                 // - function type
    TYPE mptype;                // - member-ptr type

    addrof = PTreeOp( &ctl->expr->u.subtree[1] );
    switch( NodeAddrOfFun( addrof, &func ) ) {
      case ADDR_FN_ONE_USED :
        if( ! CompFlags.extensions_enabled ) break;
        // drops thru
      case ADDR_FN_ONE :
        func = NodeActualNonOverloaded( func );
        adjustFnMembPtr( ctl, func );
        break;
      case ADDR_FN_MANY_USED :
        if( ! CompFlags.extensions_enabled ) break;
        // drops thru
      case ADDR_FN_MANY :
        switch( _CNV_TYPE( ctl->req ) ) {
          case CNV_ASSIGN :
          case CNV_INIT :
          case CNV_CAST :
          case CNV_FUNC_ARG :
          case CNV_FUNC_RET :
            mptype = ctl->tgt.unmod;
            if( ctl->tgt.reference ) {
                mptype = mptype->of;
            }
            ftype = MemberPtrType( mptype ) -> of;
            switch( ConvertOvFunNode( MakePointerTo( ftype ), func ) ) {
              case CNV_OK :
                adjustFnMembPtr( ctl, func );
                break;
              case CNV_AMBIGUOUS :
                errForFunc( func, ctl->diag_cast->msg_ambiguous, ctl );
                break;
              case CNV_IMPOSSIBLE :
                errForFunc( func, ctl->diag_cast->msg_impossible, ctl );
                break;
              DbgDefault("ConvertOfFuncnode -- unexpected return code");
            }
            break;
          default :
            errForFunc( func, ERR_ADDR_OF_OVERLOADED_FUN, ctl );
            break;
        }
        break;
    }
}


static void checkSrcForError    // CHECK IF SOURCE HAS ERROR
    ( CONVCTL* ctl )            // - control info.
{
    if( PT_ERROR == ctl->expr->op || PT_ERROR == ctl->expr->u.subtree[1]->op ) {
        ctl->has_err_operand = TRUE;
        ctl->tgt.kind = RKD_ERROR;
    }
}


static CONVCTL* convCtlInitData // INITIALIZE CONVCTL DATA
    ( CONVCTL* ctl              // - control info.
    , PTREE expr                // - expression
    , CNV_REQD request          // - type of conversion
    , CNV_DIAG* diag )          // - diagnosis
{
    ctl->req = request;
    ctl->expr = expr;
    ctl->diag_good = &diagImpossible;
    if( diag == NULL ) {
        diag = &diagImpossible;
    }
    ctl->diag_cast = diag;
    ctl->conv_fun = NULL;
    ctl->conv_type = NULL;
    ctl->destination = NULL;
    ctl->mismatch = 0;
    #define CONVCTL_FLAG( flag ) ctl->flag = FALSE;
    CONVCTL_FLAGS
    #undef CONVCTL_FLAG
    ctl->ctd = 0;
    return ctl;
}


void ConvCtlInitTypes           // INITIALIZE CONVCTL, TYPES
    ( CONVCTL* ctl              // - control info.
    , TYPE src_type             // - source type
    , TYPE tgt_type )           // - target type
{
    ctl = convCtlInitData( ctl, NULL, 0, NULL );
    ConvCtlTypeInit( ctl, &ctl->src, src_type );
    ConvCtlTypeInit( ctl, &ctl->tgt, tgt_type );
}

void ConvCtlTypeDecay           // TYPE DECAY CONVCTL TYPES
    ( CONVCTL *ctl              // - convctl info.
    , CONVTYPE *ctype )         // - type to decay
{
    TYPE ref_type;

    if( ctype->reference ) {
        ref_type = TypeReference( ctype->unmod );
        ConvCtlTypeInit( ctl, ctype, PointerTypeForArray( ref_type ) );
    }
}


void ConvCtlInit                // INITIALIZE CONVCTL
    ( CONVCTL* ctl              // - control info.
    , PTREE expr                // - expression
    , CNV_REQD request          // - type of conversion
    , CNV_DIAG* diag )          // - diagnosis
{
    TYPE src;                   // - source type

    ctl = convCtlInitData( ctl, expr, request, diag );
    expr->u.subtree[0]->type =
        BindTemplateClass( expr->u.subtree[0]->type, NULL, TRUE );
    if( ConvCtlTypeInit( ctl, &ctl->tgt, expr->u.subtree[0]->type ) ) {
        ctl->src.orig = NULL;
        DbgVerify( 0 == ctl->tgt.bit_field, "unexpected bit field" );
        if( ctl->tgt.array ) {
            diagnoseError( ctl, ERR_CAST_TO_ARRAY );
        } else {
            diagnoseError( ctl, ERR_CAST_TO_FUNCTION );
        }
    } else {
        TYPE ref_type;
        if( ctl->tgt.kind == RKD_POINTER ) {
            TYPE pted = TypedefModifierRemoveOnly( ctl->tgt.unmod->of );
            type_id id = pted->id;
            if( ctl->tgt.reference ) {
                if( TYP_FUNCTION == id ) {
                    adjustFnAddrPtr( ctl );
                    checkSrcForError( ctl );
                } else if( TYP_MEMBER_POINTER == id ) {
                    adjustFnAddrMembPtr( ctl );
                    checkSrcForError( ctl );
                } else if( NodeIsUnaryOp( ctl->expr->u.subtree[1], CO_BITFLD_CONVERT ) ) {
                    if( TypeIsConst( ctl->tgt.unmod->of ) ) {
                        ctl->expr->u.subtree[1] = NodeRvalue( ctl->expr->u.subtree[1] );
                    } else {
                        ConversionInfDisable();
                        PTreeErrorExpr( ctl->expr->u.subtree[1], ERR_CANT_REFERENCE_A_BIT_FIELD );
                        PTreeErrorNode( ctl->expr );
                        ctl->has_err_operand = TRUE;
                    }
                }
            } else if( TYP_FUNCTION == id ) {
                adjustFnAddrPtr( ctl );
                checkSrcForError( ctl );
            } else if ( TYP_VOID == id ) {
                TYPE pted_src;
                NodeRvalueRight( expr );
                expr->u.subtree[1]->type = BindTemplateClass( expr->u.subtree[1]->type, NULL, TRUE );
                ConvCtlTypeInit( ctl, &ctl->src, expr->u.subtree[1]->type );
                pted_src = TypedefModifierRemoveOnly( ctl->src.unmod->of );
                if( pted_src != NULL && pted_src->id == TYP_FUNCTION ) {
                    adjustFnAddrPtr( ctl );
                    checkSrcForError( ctl );
                }
            }
        } else if( ctl->tgt.kind == RKD_MEMBPTR ) {
            adjustFnAddrMembPtr( ctl );
            checkSrcForError( ctl );
        }
        if( ! ctl->has_err_operand ) for( ; ; ) {
            src = NodeType( expr->u.subtree[1] );
            ConvCtlTypeInit( ctl, &ctl->src, src );
            if( ctl->tgt.class_operand ) break;
            if( ctl->src.class_operand ) break;
            if( ctl->has_err_operand ) break;
            if( ctl->tgt.reference ) {
                if( ctl->src.reference ) break;
                // lvalue <- rvalue (must be const ref)
                ref_type = TypeReference( ctl->tgt.unmod );
                if( TypeIsConst( ref_type ) ) {
                    PTREE exp;
                    exp = NodeAssignTemporary( ref_type, ctl->expr->u.subtree[1] );
                    ctl->expr->u.subtree[1] = exp;
                } else {
                    diagnoseError( ctl, ERR_TEMP_AS_NONCONST_REF );
                    break;
                }
            } else {
                if( ! ctl->src.reference ) break;
                ref_type = TypeReference( ctl->src.unmod );
                if( FunctionDeclarationType( ref_type ) ) {
                    ctl->expr->u.subtree[1]->type = MakePointerTo( ref_type );
                    ctl->expr->u.subtree[1]->flags &= ~PTF_LVALUE;
                } else {
                    expr->u.subtree[1] = NodeRvalue( expr->u.subtree[1] );
                    src = expr->u.subtree[1]->type;
                }
            }
        }
        if( ctl->has_err_operand ) {
            // do nothing
        } else if( ctl->tgt.class_operand ) {
            ConvCtlClassAnalysis( &ctl->tgt );
            if( ctl->src.class_operand ) {
                ConvCtlClassAnalysis( &ctl->src );
                ctl->ctd = TypeCommonDerivation( ctl->src.class_type, ctl->tgt.class_type );
                ctl->rough = CRUFF_CL_TO_CL;
            } else {
                ctl->rough = CRUFF_SC_TO_CL;
            }
        } else {
            if( ctl->src.class_operand ) {
                ConvCtlClassAnalysis( &ctl->src );
                ctl->rough = CRUFF_CL_TO_SC;
            } else {
                ctl->rough = CRUFF_NO_CL;
            }
        }
    }
    if( ctl->has_err_operand ) {
        ctl->rough = CRUFF_NO_CL;
        ctl->src.kind = RKD_ERROR;
        ctl->tgt.kind = RKD_ERROR;
    }
}


void ConvCtlInitCast            // INITIALIZE CONVCTL FOR CAST EXPRESSION
    ( CONVCTL* ctl              // - control info.
    , PTREE expr                // - expression
    , CNV_DIAG* diag )          // - diagnosis
{
    ConvCtlInit( ctl, expr, CNV_CAST, diag );
    if( ( ctl->tgt.modflags & TF1_CV_MASK ) &&
        ( ctl->tgt.unmod->id != TYP_CLASS ) ) {
        type_flag flag;         // - accumulated flags

        flag = TypeExplicitModFlags( ctl->tgt.orig );
        if( flag & TF1_CV_MASK ) {
            CErr2p( WARN_MEANINGLESS_QUALIFIER_IN_CAST, ctl->tgt.orig );
        }
    }
}


// Note: This handles analysis of chains of references, pointers, and
//       member pointers. It is assumed that the unmodified starts are both
//       of the same type above.
//

#define TF1_EXT_ATTR (TF1_BASED | TF1_MEM_MODEL)
#define TF1_EXT_CV   (TF1_CV_MASK | TF1_UNALIGNED)
typedef struct {                // TYPE_FLAG -- various type flags
    TYPE type;                  // - ptr to ... being operated upon
    void* baser;                // - baser, when based object
    type_id id;                 // - id of current type
    type_flag object;           // - for object (default memory model added)
    type_flag cv;               // - CV + UNALIGNED
    type_flag ext;              // - based, memory model
} TYPE_FLAGS;


static void moveAhead           // MOVES TYPE_FLAGS AHEAD ONE LEVEL
    ( TYPE_FLAGS* tf )          // - the entry
{
    TYPE orig;                  // - original type

    orig = tf->type->of;
    tf->type = TypeModExtract( orig
                             , &tf->object
                             , &tf->baser
                             , TC1_NOT_MEM_MODEL | TC1_NOT_ENUM_CHAR );

    // Note: Any cv-qualifiers applied to an array type affect the
    // array element type, not the array type (3.9.3 (2))
    if( tf->id != TYP_ARRAY ) {
        tf->cv = TF1_NULL;
    }
    tf->cv |= TF1_EXT_CV & tf->object;
    tf->id  = tf->type->id;
    tf->ext = TF1_EXT_ATTR & tf->object;
}


bool ConvCtlAnalysePoints       // ANALYSE CONVERSION INFORMATION FOR POINTS
    ( CONVCTL* info )           // - pointer-conversion information
{
    bool retn;                  // - return: TRUE ==> can convert trivially
    bool first_level;           // - TRUE ==> at first level
    bool const_always;          // - TRUE ==> const on all preceding levels
    bool cv_ok;                 // - TRUE ==> no CV mismatch
    bool got_array;             // - TRUE ==> found array
    TYPE_FLAGS src;             // - source typing info
    TYPE_FLAGS tgt;             // - target typing info
    CTD mp_ctd;                 // - host derivation for member-ptr

    info->src.unmod = BoundTemplateClass( info->src.unmod );
    info->tgt.unmod = BoundTemplateClass( info->tgt.unmod );
    src.type = info->src.unmod;
    src.id = src.type->id;
    src.cv = TF1_NULL;
    tgt.type = info->tgt.unmod;
    tgt.id = tgt.type->id;
    tgt.cv = TF1_NULL;
    mp_ctd = CTD_LEFT;
    if( TYP_MEMBER_POINTER == src.type->id ) {
        if( src.type->u.mp.host != tgt.type->u.mp.host ) {
            mp_ctd = TypeCommonDerivation( tgt.type->u.mp.host, src.type->u.mp.host );
            if( CTD_NO == mp_ctd ) {
                info->bad_mptr_class = TRUE;
            }
            info->diff_mptr_class = TRUE;
        }
    }
    first_level = TRUE;
    const_always = TRUE;
    cv_ok = TRUE;
    got_array = FALSE;
    for( ; ; ) {
        moveAhead( &src );
        moveAhead( &tgt );
        if( cv_ok ) {
            if( first_level ) {
                src.ext = 0;
                tgt.ext = 0;
                info->cv_err_0 = cv_ok;
                info->src.pted = src.type;
                info->tgt.pted = tgt.type;
                info->src.ptedflags = src.object;
                info->tgt.ptedflags = tgt.object;
                info->tgt.pc_ptr = TypePcPtr( tgt.object );
                info->src.pc_ptr = TypePcPtr( src.object );
                if( NULL != tgt.type ) {
                    if( tgt.id == TYP_VOID ) {
                        info->to_void = TRUE;
                        if( src.id == TYP_VOID ) {
                            info->from_void = TRUE;
                        }
                        info->dynamic_cast_ok = TRUE;
                    } else if( src.type != NULL ) {
                        if( TYP_CLASS == src.id ) {
                            if( TYP_CLASS == tgt.id ) {
                                info->ctd = TypeCommonDerivation( src.type, tgt.type );
                                switch( info->ctd ) {
                                  case CTD_NO :
                                    break;
                                  case CTD_LEFT :
                                    if( tgt.type == src.type ) break;
                                  case CTD_LEFT_VIRTUAL :
                                    info->to_base = TRUE;
                                    break;
                                  case CTD_LEFT_PRIVATE :
                                    info->to_private = TRUE;
                                    info->to_base = TRUE;
                                    break;
                                  case CTD_LEFT_PROTECTED :
                                    info->to_protected = TRUE;
                                    info->to_base = TRUE;
                                    break;
                                  case CTD_LEFT_AMBIGUOUS :
                                    info->to_ambiguous = TRUE;
                                    info->to_base = TRUE;
                                    break;
                                  case CTD_RIGHT_AMBIGUOUS :
                                    info->to_ambiguous = TRUE;
                                  case CTD_RIGHT :
                                  case CTD_RIGHT_VIRTUAL :
                                  case CTD_RIGHT_PRIVATE :
                                  case CTD_RIGHT_PROTECTED :
                                    info->to_derived = TRUE;
                                    break;
                                }
                                info->dynamic_cast_ok = TRUE;
                            }
                        } else if( src.id != tgt.id
                                && IntegralType( tgt.type )
                                && IntegralType( src.type )
                                && tgt.id != TYP_ENUM
                                && src.id != TYP_ENUM
                                && CgMemorySize( src.type ) == CgMemorySize( tgt.type ) ) {
                            info->ptr_integral_ext = TRUE;
                        } else if( src.id == TYP_VOID ) {
                            if( CgTypeSize( info->tgt.unmod ) <=
                                CgTypeSize( info->src.unmod ) ) {
                                // this is stupid, but p* --> void* iff
                                // sizeof( void*) >= sizeof( p )
                                info->from_void = TRUE;
                            }
                        }
                    }
                }
                first_level = FALSE;
            }
            if( ( src.id == TYP_ARRAY ) && ( tgt.id == TYP_ARRAY ) ) {
                if( src.type->u.a.array_size == tgt.type->u.a.array_size ) {
                    got_array = TRUE;
                    continue;
                } else {
                    retn = FALSE;
                    break;
                }
            }
            if( cv_ok ) {
                type_flag both;
                if( info->to_void ) {
                    src.cv &= ~ TF1_UNALIGNED;
                }
                both = tgt.cv & src.cv;
                cv_ok = ( src.cv == both );// test cv-mismatch
                if( cv_ok ) {
                    if( tgt.cv != src.cv ) {
                        info->used_cv_convert = TRUE;
                        cv_ok = const_always;
                    }
                } else {
                    info->mismatch = src.cv & ~both;
                }
            }
            if( const_always && (tgt.cv & TF1_CONST) == 0 ) {
                const_always = FALSE;
            }
        }
        if( src.type == tgt.type ) {
            if( src.ext == tgt.ext ) {
                if( TYP_FUNCTION != src.id
                 || ( (TF1_MEM_MODEL & src.object) == (TF1_MEM_MODEL & tgt.object) ) ) {
                    retn = TRUE;
                    break;
                }
            }
            info->reint_cast_ok = cv_ok;
            retn = FALSE;
            break;
        } else if( src.id != tgt.id ) {
            if( info->to_void || info->from_void ) {
                retn = TRUE;
                break;
            }
            if( TYP_FUNCTION != src.id && TYP_FUNCTION != tgt.id ) {
                info->reint_cast_ok = cv_ok;
            }
            retn = FALSE;
            break;
        } else if( src.ext != tgt.ext ) {
            info->reint_cast_ok = cv_ok;
            retn = FALSE;
            break;
        }
        if( TYP_FUNCTION == src.id || TYP_ENUM == src.id ) {
            info->reint_cast_ok = cv_ok;
            retn = ( src.ext == tgt.ext )
                && TypeCompareExclude( src.type
                                     , tgt.type
                                     , TC1_FUN_LINKAGE | TC1_NOT_ENUM_CHAR );
            break;
        } else if( TYP_MEMBER_POINTER == src.id ) {
            if( src.type->u.mp.host != tgt.type->u.mp.host ) {
                info->reint_cast_ok = cv_ok;
                info->static_cast_ok = cv_ok;
                retn = FALSE;
                break;
            }
        } else if( TYP_CLASS == src.id ) {
            info->reint_cast_ok = cv_ok;
            if( info->to_base || info->to_derived ) {
                retn = TRUE;
            } else {
                retn = FALSE;
            }
            break;
        } else if( TYP_ARRAY == src.id ) {
            if( src.type->u.a.array_size == tgt.type->u.a.array_size ) {
                got_array = TRUE;
                continue;
            } else {
                retn = FALSE;
                break;
            }
        } else if( TYP_POINTER != src.id ) {
            retn = TRUE;
            break;
        }
        if( got_array ) {
            // that's it, no more adding consts allowed - this is all
            // to do with special rules for arrays, see 3.9.3 (2)
            const_always = FALSE;
        }
    }
    if( retn ) {
        if( cv_ok ) {
            info->implicit_cast_ok = TRUE;
            info->static_cast_ok = TRUE;
            info->const_cast_ok = TRUE;
            info->reint_cast_ok = TRUE;
        } else {
            retn = FALSE;
            info->const_cast_ok = TRUE;
            info->cv_mismatch = TRUE;
        }
        switch( mp_ctd ) {
          case CTD_LEFT_PROTECTED :
          case CTD_LEFT_PRIVATE :
          case CTD_LEFT_VIRTUAL :
            info->const_cast_ok = FALSE;
            break;
          case CTD_LEFT :
            if( info->to_derived || ( info->from_void && !info->to_void ) ) {
                info->implicit_cast_ok = FALSE;
                info->const_cast_ok = FALSE;
            } else if( info->to_base
                    || ( info->to_void && !info->from_void )
                    || info->diff_mptr_class ) {
                info->const_cast_ok = FALSE;
            }
            break;
          case CTD_RIGHT :
          case CTD_RIGHT_PROTECTED :
          case CTD_RIGHT_PRIVATE :
          case CTD_RIGHT_VIRTUAL :
            info->implicit_cast_ok = FALSE;
            info->const_cast_ok = FALSE;
            info->bad_mptr_class = TRUE;
            break;
          default :
            info->implicit_cast_ok = FALSE;
            info->static_cast_ok = FALSE;
            info->const_cast_ok = FALSE;
            info->bad_mptr_class = TRUE;
            break;
        }
    }
    info->explicit_cast_ok = TRUE;
    if( retn && !info->to_derived && !info->to_base ) {
        info->converts = TRUE;
    }
    return retn;
}


static unsigned checkPtrTrunc(  // CHECK POINTER TRUNCATION
    TYPE proto,                 // - prototype
    TYPE argument,              // - argument
    unsigned conversion )       // - type of conversion
{
    unsigned retn;              // - return: CNV_OK, CNV_OK_TRUNC
                                //         , CNV_OK_TRUNC_CAST

    if( conversion == CNV_CAST ) {
        retn = NodeCheckPtrCastTrunc( proto, argument );
    } else {
        retn = NodeCheckPtrTrunc( proto, argument );
    }
    if( ( retn != CNV_OK )
      &&( ( conversion == CNV_FUNC_THIS ) || ( conversion == CNV_FUNC_CD_THIS ) ) ) {
        retn = CNV_TRUNC_THIS;
    }
    return retn;
}


static void checkClassValue(    // CHECK THAT FUNC. ARG.S, RETURN ARE NOT CLASS
    TYPE ftype,                 // - function type
    PTREE expr,                 // - expression, in case of error
    MSG_NUM msg )               // - message to be used
{
    unsigned count;             // - number of args
    arg_list *alist;            // - function arguments

    if( NULL != StructType( ftype->of ) ) {
        PTreeWarnExpr( expr, msg );
    } else {
        alist = ftype->u.f.args;
        for( count = alist->num_args; ; ) {
            if( count == 0 ) break;
            -- count;
            if( StructType( alist->type_list[ count ] ) ) {
                PTreeWarnExpr( expr, msg );
                break;
            }
        }
    }
}


// types are the same, except (possibly) for linkages
//
static void checkFuncLinkages(  // WARN IF INCOMPATIBLE FUNC. LINKAGES
    TYPE pted_arg,              // - type pointed at by argument
    TYPE pted_pro,              // - type pointed at by prototype
    PTREE expr )                // - expression, in case of warning
{
    pted_arg = FunctionDeclarationType( pted_arg );
    if( NULL != pted_arg ) {
        pted_pro = FunctionDeclarationType( pted_pro );
        if( NULL != pted_pro ) {
            if( pted_pro->u.f.pragma != pted_arg->u.f.pragma
              && !PragmasTypeEquivalent( pted_pro->u.f.pragma, pted_arg->u.f.pragma ) ) {
                PTreeWarnExpr( expr, WARN_CNV_FUNC_PRAGMA );
            }
            if( pted_arg->flag & TF1_PLUSPLUS ) {
                if( (pted_pro->flag & TF1_PLUSPLUS) == 0 ) {
                    checkClassValue( pted_pro
                                   , expr
                                   , WARN_CNV_PRO_CLASS_VALUE );
                }
            } else {
                if( pted_pro->flag & TF1_PLUSPLUS ) {
                    checkClassValue( pted_arg
                                   , expr
                                   , WARN_CNV_ARG_CLASS_VALUE );
                }
            }
        }
    }
}


static PTREE nodeBasedSelfExpr( // FIND EXPR TO BE USED FOR BASED __SELF
    PTREE expr )                // - expression
{
    TYPE node_type;             // - node type
    TYPE pted;                  // - type pointed at
    TYPE umod;                  // - unmodified type
    TYPE tgt;                   // - target type (with __far )
    SYMBOL baser;               // - baser symbol
    type_flag flags;            // - flags for umod

    DbgVerify( (PTreeOpFlags( expr ) & PTO_RVALUE), "nodeBasedSelfExpr -- not CO_FETCH" );
    node_type = NodeType( expr->u.subtree[0] );
    pted = TypePointedAtModified( node_type );
    umod = TypeModExtract( pted, &flags, &baser, TC1_NOT_ENUM_CHAR );
    tgt = TypeRebuildPcPtr( umod, flags, TF1_FAR );
    expr->u.subtree[0] = NodeConvert( tgt, expr->u.subtree[0] );
    expr->u.subtree[0]->flags &= ~ PTF_LVALUE;
    return NodeDupExpr( &expr->u.subtree[0] );
}


static PTREE findBasedStrSym(   // FIND REFERENCE SYMBOL FOR TF1_BASED_STRING
    TYPE expr_type )            // - type of pointer expression
{
    return NodeBasedStr( BasedPtrType( expr_type ) );
}


static PTREE convertFromPcPtr(  // CONVERT SPECIAL TO REGULAR PC PTR
    PTREE expr,                 // - expression
    PC_PTR ptr_class,           // - pointer classification
    PTREE bself )               // - __based_self expression
{
    type_flag flags;            // - flags for item pointed at
    void *baser;                // - based symbol/string
    TYPE type;                  // - unmodified type of element
    TYPE ptr_type;              // - original pointer type
    TYPE tgt_type;              // - target pointer type

    if( ( ptr_class != PC_PTR_REGULAR ) && ( ptr_class != PC_PTR_NOT ) ) {
        ptr_type = NodeType( expr );
        tgt_type = TypeConvertFromPcPtr( ptr_type );
        type = TypeModExtract( TypePointedAtModified( ptr_type )
                             , &flags
                             , &baser
                             , TC1_NOT_ENUM_CHAR );
        switch( ptr_class ) {
          case PC_PTR_FAR16 :
            expr = NodeUnary( CO_FAR16_TO_POINTER, expr );
            expr = NodeSetType( expr, ptr_type, PTF_PTR_NONZERO );
            break;
          case PC_PTR_BASED_VOID :
            expr = NodeConvertFlags( tgt_type, expr, PTF_PTR_NONZERO );
            break;
          case PC_PTR_BASED_STRING :
          { PTREE bsym;         // - basing symbol
            bsym = findBasedStrSym( expr->type );
            expr = NodeBinary( CO_SEG_OP, expr, bsym );
            expr = NodeSetType( expr, tgt_type, PTF_PTR_NONZERO );
          } break;
          case PC_PTR_BASED_SELF :
            expr = NodeBinary( CO_SEG_OP, expr, bself );
            expr = NodeSetType( expr, tgt_type, PTF_PTR_NONZERO );
            break;
          case PC_PTR_BASED_FETCH :
            expr = NodeBinary( CO_SEG_OP
                             , expr
                             , NodeRvalue( MakeNodeSymbol( baser ) ) );
            expr = NodeSetType( expr, tgt_type, PTF_PTR_NONZERO );
            break;
          case PC_PTR_BASED_ADD :
            expr = NodeBinary( CO_PLUS
                             , expr
                             , NodeRvalue( MakeNodeSymbol( baser ) ) );
            expr = NodeSetType( expr, tgt_type, PTF_PTR_NONZERO );
            break;
          default :
            expr->flags |= PTF_PTR_NONZERO;
            break;
        }
    }
    return expr;
}


static PTREE convertToPcPtr(    // CONVERT REGULAR TO SPECIAL PC PTR
    PTREE expr,                 // - expression
    TYPE ptr_type,              // - pointer type of result expression
    PC_PTR ptr_class )          // - pointer classification
{

    switch( ptr_class ) {
      case PC_PTR_REGULAR :
      case PC_PTR_NOT :
        break;
      case PC_PTR_FAR16 :
        expr = NodeUnary( CO_POINTER_TO_FAR16, expr );
        expr = NodeSetType( expr, ptr_type, PTF_PTR_NONZERO );
        break;
      case PC_PTR_BASED_SELF :
      case PC_PTR_BASED_VOID :
      case PC_PTR_BASED_FETCH :
      case PC_PTR_BASED_STRING :
        expr = NodeConvertFlags( ptr_type, expr, PTF_PTR_NONZERO );
        break;
      case PC_PTR_BASED_ADD :
      { PTREE temp;             // - temp
        SYMBOL baser;           // - based symbol
        baser = BasedPtrType( ptr_type )->u.m.base;
        temp = MakeNodeSymbol( baser );
        temp = NodeRvalue( temp );
        expr = NodeBinary( CO_MINUS, expr, temp );
        expr = NodeSetType( expr, ptr_type, PTF_PTR_NONZERO );
      } break;
    }
    return expr;
}


static SCOPE pcPtrScope(        // GET SCOPE OF POINTED-AT ITEM
    TYPE ptype )                // - pointer type
{
    return TypeScope( TypePointedAtModified( ptype ) );
}


static uint_8 pcPtrChk[] =          // used to check for pc ptr errors
//  \       r
//   \  to  e  s
//    \     g  t        f     f
//     \    u  r  s  v  e     a
//      \   l  i  e  o  t  a  r  n
//       \  a  n  l  i  c  d  1  o
// from   \ r  g  f  d  h  d  6  t
//
{ BIT_VECT( 0, 0, 0, 0, 0, 0, 0, 0 )   // - regular
, BIT_VECT( 0, 0, 1, 0, 0, 0, 0, 0 )   // - string
, BIT_VECT( 0, 0, 1, 0, 0, 0, 0, 0 )   // - self
, BIT_VECT( 1, 0, 1, 1, 0, 0, 1, 0 )   // - void
, BIT_VECT( 0, 0, 1, 0, 0, 0, 0, 0 )   // - fetch
, BIT_VECT( 0, 0, 1, 0, 0, 0, 0, 0 )   // - add
, BIT_VECT( 0, 0, 0, 0, 0, 0, 0, 0 )   // - far16
, BIT_VECT( 0, 0, 0, 0, 0, 0, 0, 0 )   // - not
};


unsigned PcPtrValidate(         // VALIDATE PC-FORMAT PTRS
    TYPE typ_tgt,               // - target type
    TYPE typ_src,               // - source type
    PC_PTR pcp_tgt,             // - target classification
    PC_PTR pcp_src,             // - source classification
    PTREE expr,                 // - expression for error
    unsigned conversion )       // - type of conversion
{
    unsigned retn;              // - return: CNV_...

    if( ( 0x80 >> pcp_tgt ) & pcPtrChk[ pcp_src ] ) {
        switch( pcp_src ) {
          DbgDefault( "pcPtrValidate -- bad case" );
          case PC_PTR_BASED_VOID :
            switch( pcp_tgt ) {
              case PC_PTR_REGULAR :
                if( conversion == CNV_CAST ) {
                    retn = CNV_OK;
                } else {
                    PTreeErrorExpr( expr, ERR_SEGOP_OPERANDS );
                    retn = CNV_ERR;
                }
                break;
              case PC_PTR_FAR16 :
                PTreeErrorExpr( expr, ERR_SEGOP_OPERANDS );
                retn = CNV_ERR;
                break;
              case PC_PTR_BASED_VOID :
              { derived_status status;
                SCOPE scope_tgt;
                SCOPE scope_src;
                scope_tgt = pcPtrScope( typ_tgt );
                if( scope_tgt == NULL ) {
                    retn = CNV_OK;
                } else {
                    scope_src = pcPtrScope( typ_src );
                    if( scope_src == NULL ) {
                        retn = CNV_OK;
                    } else {
                        status = ScopeDerived( scope_src, scope_tgt );
                        if( status == DERIVED_YES_BUT_VIRTUAL ) {
                            PTreeErrorExpr( expr, ERR_VIRTUAL_FOR_BASED_VOID );
                            retn = CNV_ERR;
                        } else {
                            retn = CNV_OK;
                        }
                    }
                }
              } break;
              default :
                if( conversion == CNV_CAST ) {
                    retn = CNV_OK;
                } else {
                    retn = CNV_IMPOSSIBLE;
                }
                break;
            }
            break;
          case PC_PTR_BASED_SELF :
          case PC_PTR_BASED_STRING :
          case PC_PTR_BASED_FETCH :
          case PC_PTR_BASED_ADD :
            if( pcp_src == pcp_tgt ) {
                retn = CNV_OK;
            } else if( conversion == CNV_CAST ) {
                retn = CNV_OK;
            } else {
                retn = CNV_IMPOSSIBLE;
            }
            break;
        }
    } else {
        retn = CNV_OK;
    }
    return retn;
}


static unsigned classPtrConversion( // CONVERT CLASS PTR (UP OR DOWN)
    PTREE *a_expr,              // - addr[ expression ]
    TYPE proto,                 // - target type
    unsigned conversion )       // - type of conversion
{
    TYPE expr_type;             // - type for expression
    bool retn;                  // - return: CNV_...
    type_flag pro_flags;        // - flags: target
    type_flag src_flags;        // - flags: source
    void *baser;                // - based entity
    unsigned ptr_conv;          // - type of pointer conversion
    TYPE type;                  // - used to reduce source type

    proto = TypePointedAt( proto, &pro_flags );
    expr_type = NodeType( *a_expr );
    type = TypePointedAtModified( expr_type );
    type = TypeModExtract( type, &src_flags, &baser, TC1_NOT_ENUM_CHAR );
    proto = MakeBasedModifierOf( proto, src_flags, baser );
    proto = MakePointerTo( proto );
    switch( _CNV_TYPE( conversion ) ) {
      case CNV_CAST :
        ptr_conv = CNVPTR_CAST | CNVPTR_VIRT_TO_DERIVED;
        break;
      case CNV_INIT :
      case CNV_FUNC_ARG :
      case CNV_FUNC_RET :
        ptr_conv = CNVPTR_DERIVED_ONLY | CNVPTR_CONST_VOLATILE;
        break;
      default :
        ptr_conv = CNVPTR_DERIVED_ONLY;
        break;
    }
    retn = NodeConvertPtr( ptr_conv
                         , a_expr
                         , expr_type
                         , proto );
    return retn;
}


static uint_8 pcPtrCnv[] =          // used to determine type of conversion
//  \       r
//   \  to  e  s
//    \     g  t        f     f
//     \    u  r  s  v  e     a
//      \   l  i  e  o  t  a  r  n
//       \  a  n  l  i  c  d  1  o
// from   \ r  g  f  d  h  d  6  t
//
{           1, 2, 2, 2, 2, 8, 3, 10 // - regular
,           4, 1, 1, 1, 1, 5, 5, 10 // - string
,           4, 1, 1, 1, 1, 5, 5, 10 // - self
,           2, 1, 1, 1, 1, 5,99, 10 // - void
,           4, 1, 1, 1, 1, 5, 5, 10 // - fetch
,           4, 9, 9, 9, 9, 7, 5, 10 // - add
,           4, 6, 6, 6, 6, 5, 5, 10 // - far16
,          10,10,10,10,10,10,10, 10 // - not
};


static unsigned pcPtrConvertSrcTgt(// PTR CONVERT SOURCE TO TARGET
    PTREE *a_expr,              // - addr[ expression ]
    TYPE tgt_type,              // - target pointer type
    PC_PTR type_tgt,            // - target classification
    PC_PTR type_src,            // - source classification
    bool cl_conv,               // - TRUE ==> do class conversion
    unsigned conversion )       // - type of conversion
{
    PTREE expr;                 // - expression
    TYPE expr_type;             // - type for expression
    uint_8 cnv_fun;             // - conversion function
    type_flag flags_src;        // - flags for source expression ptr. item
    type_flag flags_tgt;        // - flags for target type ptr. item
    void *baser;                // - based entity
    unsigned retn;              // - return: CNV_...
    PTREE bself;                // - expression for based self
    bool is_ref;                // - TRUE ==> is reference

    cnv_fun = pcPtrCnv[ type_src*8 + type_tgt ];
    if( cnv_fun == 10 ) {
        retn = CNV_OK;
    } else {
        if( ( type_src == PC_PTR_BASED_SELF ) && ( ( cnv_fun == 4 ) || ( cnv_fun == 5 ) ) ) {
            bself = nodeBasedSelfExpr( *a_expr );
        } else {
            bself = NULL;
        }
#if 0
        expr_type = NodeType( *a_expr );
        if( cl_conv ) {
            retn = classPtrConversion( a_expr, tgt_type, conversion );
            expr = *a_expr;
        } else {
            expr = *a_expr;
            retn = checkPtrTrunc( tgt_type, expr_type, conversion );
            switch( retn ) {
              case CNV_OK :
                retn = CNV_OK_CV;
                break;
              case CNV_OK_TRUNC :
                retn = CNV_OK_TRUNC_CV;
                break;
              case CNV_OK_TRUNC_CAST :
                retn = CNV_OK_TRUNC_CAST_CV;
                break;
            }
        }
#else
        if( cl_conv ) {
            retn = classPtrConversion( a_expr, tgt_type, conversion );
            if( retn == CNV_OK_CV ) {
                retn = CNV_OK;
            }
        } else {
            retn = CNV_OK;
        }
        expr = *a_expr;
        if( CNV_OK == retn ) {
            expr_type = NodeType( expr );
            retn = checkPtrTrunc( tgt_type, expr_type, conversion );
            switch( retn ) {
              case CNV_OK :
                retn = CNV_OK_CV;
                break;
              case CNV_OK_TRUNC :
                retn = CNV_OK_TRUNC_CV;
                break;
              case CNV_OK_TRUNC_CAST :
                retn = CNV_OK_TRUNC_CAST_CV;
                break;
            }
        }
#endif
        switch( retn ) {
          case CNV_ERR:
          case CNV_VIRT_DER:
          case CNV_IMPOSSIBLE:
          case CNV_AMBIGUOUS:
          case CNV_PRIVATE:
          case CNV_PROTECTED:
            break;
          default:
            tgt_type = TypedefModifierRemove( tgt_type );
            if( tgt_type->flag & TF1_REFERENCE ) {
                is_ref = TRUE;
            } else {
                is_ref = FALSE;
            }
            tgt_type = TypePointedAtModified( tgt_type );
            TypeModExtract( tgt_type, &flags_tgt, &baser, TC1_NOT_MEM_MODEL );
            tgt_type = TypePointedAt( expr_type, &flags_src );
            flags_tgt = ( flags_tgt & TF1_MEM_MODEL ) | ( flags_src & ~TF1_MEM_MODEL );
            tgt_type = MakeBasedModifierOf( tgt_type, flags_tgt, baser );
            if( is_ref ) {
                tgt_type = MakeReferenceTo( tgt_type );
            } else {
                tgt_type = MakePointerTo( tgt_type );
            }
            switch( cnv_fun ) {
              DbgDefault( "pcPtrConvert -- impossible conversion" );
#if 0
              case 1:       // convert to pointer type, if not class conv
                if( cl_conv ) break;
#else
              case 1:       // convert to pointer type using code-gen
                if( tgt_type == expr_type ) break;
#endif
                // fall thru
              case 2:       // truncate to __based pointer, except TF1_ADD
                expr = NodeConvert( tgt_type, expr );
                break;
              case 3:       // convert to FAR16
              case 8:       // convert regular to TF1_ADD
                expr = convertToPcPtr( expr, tgt_type, type_tgt );
                break;
              case 4:       // convert from pc ptr to regular
                expr = convertFromPcPtr( expr, type_src, bself );
                break;
              case 5:       // convert from __based to FAR16, TF1_ADD
              case 7:       // convert TF1_ADD to TF1_ADD (optimize?)
                expr = convertFromPcPtr( expr, type_src, bself );
                expr = convertToPcPtr( expr, tgt_type, type_tgt );
                break;
              case 6:       // convert from FAR16 to __based
              case 9:       // convert TF1_ADD to __based
                expr = convertFromPcPtr( expr, type_src, bself );
                expr = NodeConvert( tgt_type, expr );
                break;
            }
        }
        *a_expr = expr;
    }
    return retn;
}


CNV_RETN CastPtrToPtr           // IMPLICIT/EXPLICIT CAST PTR -> PTR
    ( CONVCTL* ctl )            // - conversion control
{
    CNV_RETN retn;              // - conversion return
    PTREE expr;                 // - resultant expression
    PTREE dtor;                 // - dtoring operand

    expr = *NodeReturnSrc( &ctl->expr, &dtor );
    if( expr->op == PT_SYMBOL ) {
        expr->u.symcg.symbol->flag |= SF_ADDR_TAKEN;
    }
    expr = NodeSetType( ctl->expr, ctl->tgt.orig, PTF_LV_CHECKED );
    if( ctl->converts ) {
        checkFuncLinkages( ctl->src.pted, ctl->tgt.pted, expr );
        retn = pcPtrConvertSrcTgt( &expr->u.subtree[1]
                                 , ctl->tgt.orig
                                 , ctl->tgt.pc_ptr
                                 , ctl->src.pc_ptr
                                 , FALSE
                                 , ctl->req );
    } else if( ctl->to_void ) {
        retn = pcPtrConvertSrcTgt( &expr->u.subtree[1]
                                 , ctl->tgt.orig
                                 , ctl->tgt.pc_ptr
                                 , ctl->src.pc_ptr
                                 , FALSE
                                 , ctl->req );
    } else if( ctl->to_base ) {
        retn = pcPtrConvertSrcTgt( &expr->u.subtree[1]
                                 , ctl->tgt.orig
                                 , ctl->tgt.pc_ptr
                                 , ctl->src.pc_ptr
                                 , TRUE
                                 , ctl->req );
    } else if( ctl->to_derived ) {
        retn = pcPtrConvertSrcTgt( &expr->u.subtree[1]
                                 , ctl->tgt.orig
                                 , ctl->tgt.pc_ptr
                                 , ctl->src.pc_ptr
                                 , TRUE
                                 , ctl->req );
    } else {
        retn = CNV_IMPOSSIBLE;
    }
    switch( retn ) {
      case CNV_IMPOSSIBLE :
        retn = NodeCheckPtrCastTrunc( ctl->tgt.orig, ctl->src.orig );
        break;
      case CNV_OK_CV :
        retn = CNV_OK;
        break;
      case CNV_OK_TRUNC_CV :
        retn = CNV_OK_TRUNC;
        break;
      case CNV_OK_TRUNC_CAST_CV :
        retn = CNV_OK_TRUNC_CAST;
        break;
    }
    return retn;
}


// DEBUG SUPPORT


#ifndef NDEBUG

#include "dbg.h"

#define dfnRKD(a) # a
static char const * const rkdstr[] =
{ dfnRKDs
};
#undef dfnRKD

void DbgConvType                // DUMP CONVTYPE INFORMATION
    ( CONVTYPE* type )          // - structure
{
    printf( "  Original type : " );
    DumpFullType( type->orig );
    printf( "  Unmodified type : " );
    DumpFullType( type->unmod );
    printf( "  Pointed type : " );
    DumpFullType( type->pted );
    printf( "  Class type : " );
    DumpFullType( type->class_type );
    printf( "  kind = %s\n"
            "  modflags(%X) ptedflags(%x) reference(%d)\n"
            "  array(%d) bit_field(%d) class_operand(%d)\n"
          , rkdstr[ type->kind ]
          , type->modflags
          , type->ptedflags
          , type->reference
          , type->array
          , type->bit_field
          , type->class_operand );
}


#define DfnCRUFF(a) # a
static char const * const ruffstr[] =
{   DfnCRUFFs
};
#undef DfnCRUFF



void DbgConvCtl                 // DUMP CONVCTL INFORMATION
    ( CONVCTL* info )           // - structure
{
    VBUF vbuf;

    printf( "CONVCTL[%p] rough[%s]\n\n"
          , info
          , ruffstr[ info->rough ] );
    printf( "Source Type information\n" );
    DbgConvType( &info->src );
    printf( "\nTarget Type information\n" );
    DbgConvType( &info->tgt );
    printf( "\n" );
    DbgPrintPTREE( info->expr );
    printf( "\n" );
    if( NULL == info->destination ) {
        printf( "No destination node" );
    } else {
        DbgPrintPTREE( info->destination );
    }
    DbgPrintPTREE( info->expr );
    printf( "\ndiag_good(%p) diag_cast(%p) msg_no(%x)"
            " req(%x) rough(%x) ctd(%x)"
            "\nconv_fun[%p] %s"
            "\n"
          , info->diag_good
          , info->diag_cast
          , info->msg_no
          , info->req
          , info->rough
          , info->ctd
          , info->conv_fun, DbgSymNameFull( info->conv_fun, &vbuf )
          );
    if( NULL != info->conv_type ) {
        printf( "conversion type:" );
        DumpFullType( info->conv_type );
    }
    printf( "\nFlags:\n" );
    #define CONVCTL_FLAG( f ) printf( "%d : " #f "\n", info->f );
    CONVCTL_FLAGS
    #undef CONVCTL_FLAG
    VbufFree( &vbuf );
}

#undef prtflag

#endif
