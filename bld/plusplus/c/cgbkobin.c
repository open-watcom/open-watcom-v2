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
* Description:  Operations on OBJ_INIT and DTREG_OBJ.
*
****************************************************************************/


#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "carve.h"
#include "vstk.h"
#include "ring.h"
#include "initdefs.h"
#include "errdefns.h"


static VSTK_CTL stack_object_init;  // stack: array initializations
static carve_t carveDTREG_OBJ;      // allocations for DTREG_OBJ
static DTREG_OBJ* ringDtregObj;     // DTREG_OBJ entries (function)
static DTREG_OBJ* ringDtregObjMod;  // DTREG_OBJ entries (module)


static TYPE arrayOrStructType   // PUT TYPE INTO CONICAL FORM
    ( TYPE type )               // - the type
{
    TYPE retn = StructType( type );
    if( NULL == retn ) {
        retn = ArrayType( type );
    }
    return retn;
}


DTREG_OBJ* DtregObj(            // LOCATE RW_DTREG_OBJ SYMBOL
    FN_CTL* fctl )              // - current function information
{
    DTREG_OBJ* curr;            // - current entry
    DTREG_OBJ* obj;             // - entry for function

    obj = NULL;
    RingIterBeg( ringDtregObj, curr ) {
        if( ! curr->in_use ) {
            obj = curr;
            break;
        }
    } RingIterEnd( curr )
    if( obj == NULL ) {
        obj = RingCarveAlloc( carveDTREG_OBJ, &ringDtregObj );
        obj->cg_sym = NULL;
    }
    if( ! DtmTabular( fctl ) ) {
        obj->sym = NULL;
    } else {
        if( obj->cg_sym == NULL ) {
            obj->cg_sym = CgVarRw( 3 * CgbkInfo.size_data_ptr
                                 , SC_AUTO );
            AutoRelRegister( obj->cg_sym, &obj->offset );
        }
        obj->sym = obj->cg_sym;
    }
    obj->in_use = TRUE;
    return obj;
}


DTREG_OBJ* DtregActualBase(     // REGISTER AN ACTUAL BASE
    FN_CTL* fctl )              // - current function information
{
    return DtregObj( fctl );
}


static void dtregObjFree(       // FREE A DTREG_OBJ
    DTREG_OBJ* reg )            // - registration
{
    if( reg != NULL ) {
        reg->in_use = FALSE;
    }
}


void FreeDtregObjs(             // FREE OBJECT REGISTRATIONS (FUNCTION)
    void )
{
    DTREG_OBJ* curr;            // - current entry

    RingIterBegSafe( ringDtregObj, curr ) {
        curr->offset = CgOffsetRw( curr->offset );
        RingAppend( &ringDtregObjMod, curr );
    } RingIterEndSafe( curr )
    ringDtregObj = NULL;
}


static cg_name objInitField(    // GET EXPRESSION FOR OBJ_INIT FIELD
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init,             // - initialization element
    target_offset_t offset )    // - field offset
{
    if( NULL == init->reg ) {
        init->reg = DtregObj( fctl );
    }
    return CgSymbolPlusOffset( init->reg->sym, offset );
}


static
cg_name objInitAssignBaseExpr(  // ASSIGN BASE REGISTRATION, FROM EXPR'N
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init,             // - initialization element
    cg_name expr )              // - expression
{
    return CGLVAssign( objInitField( fctl, init, 0 ), expr, TY_POINTER );
}


static OBJ_INIT* objInitNext(   // GET NEXT INITIALIZATION OBJECT
    OBJ_INIT* curr )            // - current entry
{
    return VstkNext( &stack_object_init, curr );
}


OBJ_INIT* ObjInitArray(         // GET OBJ_INIT FOR INDEXING
    void )
{
    OBJ_INIT* init;             // - current element
    OBJ_INIT* array;            // - array element
    TYPE base_type;             // - base type
    TYPE init_base_type;        // - base type of current

    array = NULL;
    base_type = NULL;
    for( init = ObjInitTop(); ; init = objInitNext( init ) ) {;
        init_base_type = ObjInitArrayBaseType( init );
        if( init_base_type == NULL ) {
            if( base_type == NULL ) {
                base_type = StructType( init->obj_type );
            } else {
                break;
            }
        } else {
            if( base_type == NULL ) {
                base_type = init_base_type;
                array = init;
            } else if( base_type == init_base_type ) {
                array = init;
            } else {
                break;
            }
        }
    }
    DbgVerify( array != NULL, "objInitArray -- not array" );
    return array;
}


TYPE ObjInitArrayBaseType(      // GET BASE TYPE FOR ARRAY
    OBJ_INIT* curr )            // - current entry
{
    TYPE base_type;             // - base type of an array element

    if( curr == NULL ) {
        base_type = NULL;
    } else {
        base_type = ArrayType( curr->obj_type );
        if( base_type != NULL ) {
            base_type = ArrayBaseType( base_type );
        }
    }
    return base_type;
}


OBJ_INIT* ObjInitClass(         // GET OBJ_INIT FOR A CLASS
    void )
{
    OBJ_INIT* init;             // - current element
    OBJ_INIT* clss;             // - class element

    clss = ObjInitTop();
    DbgVerify( clss != NULL, "ObjInitClass -- no class element" );
    DbgVerify( StructType( clss->obj_type ) != NULL
             , "ObjInitClass -- not class type" );
    for( ; ; ) {
        init = objInitNext( clss );
        if( init == NULL ) break;
        if( init->obj_offset != clss->obj_offset ) break;
        if( init->obj_type   != clss->obj_type   ) break;
        if( init->obj_sym    != clss->obj_sym    ) break;
        clss = init;
    }
    return clss;
}


OBJ_INIT* ObjInitPush(          // PUSH INITIALIZATION OBJECT (HAS COMPONENTS)
    TYPE obj_type )             // - type of object
{
    OBJ_INIT* init;             // - new object

    init = VstkPush( &stack_object_init );
    init->obj_type = arrayOrStructType( obj_type );
    init->obj_se = NULL;
    init->ctor_test = NULL;
    init->reg = NULL;
    init->defn = NULL;
    init->obj_sym = NULL;
    init->obj_offset = 0;
    init->patch = 0;
    return init;
}


OBJ_INIT* ObjInitPop(           // POP INITIALIZATION OBJECT (HAS COMPONENTS)
    void )
{
    OBJ_INIT* init;             // - new object

    init = VstkPop( &stack_object_init );
    DbgVerify( init != NULL, "ObjInitPop -- init object stack empty" );
    dtregObjFree( init->reg );
    return init;
}


OBJ_INIT* ObjInitTop(           // GET TOP INITIALIZATION OBJECT
    void )
{
    return VstkTop( &stack_object_init );
}


cg_name ObjInitAssignBase(      // ASSIGN BASE REGISTRATION
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init )            // - initialization element
{
    return objInitAssignBaseExpr( fctl
                                , init
                                , CgSymbolPlusOffset( init->obj_sym
                                                    , init->obj_offset ) );
}


cg_name ObjInitAssignIndex(     // ASSIGN INDEX TO RT_ARRAY_INIT
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init,             // - initialization element
    unsigned index )            // - index
{
    return CGLVAssign( objInitField( fctl, init, CgbkInfo.size_data_ptr * 2 )
                     , CgOffset( index )
                     , CgTypeOffset() );
}


cg_name ObjInitRegisterObj(     // CREATE AN OBJECT REGISTRATION
    FN_CTL* fctl,               // - current function information
    cg_name base_expr,          // - base expression
    boolean use_fun_cdtor )     // - TRUE ==> use CDTOR parm of function
{
    OBJ_INIT* init;             // - initialization element
    cg_name expr;               // - initialization expression
    cg_name cdtor;              // - CDTOR expression

    init = ObjInitTop();
    if( use_fun_cdtor && fctl->cdtor_sym != NULL ) {
        cdtor = CgFetchSym( fctl->cdtor_sym );
    } else {
        cdtor = CGInteger( 0, TY_UINT_1 );
    }
    expr = CGLVAssign( objInitField( fctl, init, CgbkInfo.size_data_ptr )
                     , cdtor
                     , TY_UINT_1 );
    expr = CgComma( objInitAssignBaseExpr( fctl, init, base_expr )
                  , expr
                  , TY_POINTER );
    return expr;
}


cg_name ObjInitRegActualBase    // REGISTER FOR AN ACTUAL BASE
    ( SE* se )                  // - component for actual base
{
    cg_name expr1;              // - initialization expression
    cg_name expr2;              // - initialization expression

    expr1 = CGLVAssign( CgSymbolPlusOffset( se->component.obj->sym
                                          , CgbkInfo.size_data_ptr )
                      , CGInteger( DTOR_COMPONENT, TY_UINT_1 )
                      , TY_UINT_1 );
#if 0
    expr2 = CGLVAssign( CgSymbolPlusOffset( se->component.obj->sym, 0 )
                      , CGBinary( O_PLUS
                                , IbpFetchRef( NULL )
                                , CgOffset( se->component.offset )
                                , TY_POINTER )
                      , TY_POINTER );
#else
    expr2 = CGLVAssign( CgSymbolPlusOffset( se->component.obj->sym, 0 )
                      , IbpFetchRef( NULL )
                      , TY_POINTER );
#endif
    expr2 =  CgComma( expr1, expr2, TY_POINTER );
    return expr2;
}


SE* ObjInitDtorAuto(            // UPDATE OBJ_INIT FOR AUTO DTOR
    SE* se,                     // - entry for symbol
    SYMBOL sym )                // - symbol needing dtor
{
    OBJ_INIT* init;             // - current initialization object
    OBJ_INIT* zap;              // - initialization object for symbol

    zap = NULL;
    for( init = ObjInitTop(); init != NULL; init = objInitNext( init ) ) {
        if( init->obj_sym == sym ) {
            zap = init;
        }
    }
    if( zap != NULL ) {
        zap->obj_se = se;
    }
    return se;
}


static void init(               // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    ringDtregObj = NULL;
    ringDtregObjMod = NULL;
    VstkOpen( &stack_object_init, sizeof( OBJ_INIT ), 4 );
    carveDTREG_OBJ = CarveCreate( sizeof( DTREG_OBJ ), 16 );
}


static void fini(               // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_object_init );
    RingCarveFree( carveDTREG_OBJ, &ringDtregObjMod );
    CarveDestroy( carveDTREG_OBJ );
}


INITDEFN( cg_obj_init, init, fini )
