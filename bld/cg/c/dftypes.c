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


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "symdbg.h"
#include "model.h"
#include "typedef.h"
#include "types.h"
#include "zoiks.h"
#include "dbgstrct.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include "dw.h"
#include "dwarf.h"
#include "utils.h"
#include "dftypes.h"
#include "cgprotos.h"

extern  type_length     NewBase(name*);
extern dw_loc_handle    DBGLoc2DF( dbg_loc loc );
extern  void            DBLocFini( dbg_loc loc );
extern  uint            DFRegMap( hw_reg_set hw_reg );
extern  void            DFOutReg( dw_loc_id locid, name *reg );
extern  void            DFOutRegInd( dw_loc_id locid, name *reg );


extern  dw_client       Client;

extern  dbg_type        DFFtnType( const char *name, dbg_ftn_type tipe ) {
/************************************************************************/

    dbg_type    ret;
    unsigned    size;

    size = (tipe & 0x0f)+1;
    ret = DWFundamental( Client, name, DW_FT_COMPLEX_FLOAT, size );
    return( ret );
}


extern  dbg_type        DFScalar( const char *name, cg_type tipe ) {
/******************************************************************/
    type_def    *tipe_addr;
    int          class;
    dbg_type     ret;

    tipe_addr = TypeAddress( tipe );
    if( tipe_addr->attr & TYPE_FLOAT ){
        class = DW_FT_FLOAT;
    }else if( strcmp( "char", name ) == 0 ){
        if( tipe_addr->attr & TYPE_SIGNED ){
            class = DW_FT_SIGNED_CHAR;
        }else{
            class = DW_FT_UNSIGNED_CHAR;
        }
    }else if( strcmp( "unsigned char", name ) == 0 ){
        class = DW_FT_UNSIGNED_CHAR;
    }else if( strcmp( "signed char", name ) == 0 ){
        class = DW_FT_SIGNED_CHAR;
    }else if( tipe_addr->attr & TYPE_SIGNED ){
        class = DW_FT_SIGNED;
    }else{
        class = DW_FT_UNSIGNED;
    }

    ret = DWFundamental( Client, name, class, tipe_addr->length );
    return( ret );
}

enum scope_name {
    SCOPE_TYPEDEF = 0,
    SCOPE_STRUCT  = 1,
    SCOPE_UNION   = 2,
    SCOPE_ENUM    = 3,
    SCOPE_CLASS   = 4,
    SCOPE_MAX
};
static char const ScopeNames[SCOPE_MAX][7] = {
    "",
    "struct",
    "union",
    "enum",
    "class"
};

extern char const *DFScopeName( dbg_type scope ){
    return( ScopeNames[scope] );
}

extern  dbg_type        DFScope( const char *name ) {
/***************************************************/

    enum scope_name index;

    for( index = 0; index < SCOPE_MAX; ++index ){
        if( strcmp( name, ScopeNames[index] ) == 0 )break;
    }
    return( index );
}


extern  void    DFDumpName( name_entry *name, dbg_type tipe ) {
/***********************************************************/

    if( name->scope == SCOPE_TYPEDEF ){
        tipe = DWTypedef( Client, tipe, name->name, 0, 0 );
    }
   name->refno = tipe; /* link in  typedef sym to type */
}

extern void DFBackRefType( name_entry *name, dbg_type tipe ){
/******************************************************/
    name = name;
    tipe = tipe;
    Zoiks( ZOIKS_108 );
}

extern  dbg_type        DFCharBlock( unsigned_32 len ) {
/******************************************************/

    dbg_type    ret;

    ret = DWString( Client, NULL, len, NULL, 0, 0 );
    return( ret );
}

extern  dbg_type    DFCharBlockNamed( const char *name, unsigned_32 len ) {
/*************************************************************************/

    dbg_type    ret;

    ret = DWString( Client, NULL, len, name, 0, 0 );
    return( ret );
}

extern  dbg_type        DFIndCharBlock( back_handle len, cg_type len_type,
                                        int off ) {
/************************************************************************/

    dbg_type    ret;
    dw_loc_id   len_locid;
    dw_loc_handle   len_loc;
    type_def    *tipe_addr;

    len_locid = DWLocInit( Client );
    DWLocSym( Client, len_locid, (dw_sym_handle)len, DW_W_LABEL );
    DWLocOp( Client, len_locid, DW_LOC_plus_uconst, off );
    len_loc = DWLocFini( Client, len_locid );
    tipe_addr = TypeAddress( len_type );
    ret = DWString( Client, len_loc, tipe_addr->length, NULL, 0, 0 );
    DWLocTrash( Client, len_loc );
    return( ret );
}

extern  dbg_type        DFLocCharBlock( dbg_loc loc, cg_type len_type ) {
/***********************************************************************/
    dw_loc_handle   len_loc;
    type_def    *tipe_addr;
    dbg_type    ret;

    //NYI: damned if I know what to do.
    len_loc = DBGLoc2DF( loc );
    tipe_addr = TypeAddress( len_type );
    ret = DWString( Client, len_loc, tipe_addr->length, NULL, 0, 0 );
    if( len_loc != NULL ){
        DWLocTrash( Client, len_loc );
    }
    return( ret );
}


extern  dbg_type        DFArray( dbg_type idx, dbg_type base ) {
/**************************************************************/

    dbg_type    ret;
    dw_dim_info info;

/* need subrange types in dwarf library */
    ret = DWBeginArray( Client, base, 0, NULL, 0, 0 );
    info.index_type  = idx;
    info.lo_data  = 0;
    info.hi_data  = 0;
    DWArrayDimension( Client, &info );
    DWEndArray( Client );
    return( ret );
}

extern  dbg_type        DFIntArray( unsigned_32 hi, dbg_type base ) {
/*******************************************************************/

    dbg_type    ret;

    ret = DWSimpleArray( Client, base, hi+1 );
    return( ret );
}

static  dw_handle   MKBckVar( back_handle bck, int off, dw_handle tipe ){
    dw_loc_id       locid;
    dw_loc_handle   dw_loc;
    dw_loc_handle   dw_segloc;
    dw_handle       obj;

    locid = DWLocInit( Client );
    DWLocSym( Client, locid, (dw_sym_handle)bck, DW_W_LABEL );
    DWLocOp( Client, locid, DW_LOC_plus_uconst, off );
    dw_loc = DWLocFini( Client, locid );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    if( _IsTargetModel( FLAT_MODEL ) ) {
        dw_segloc = NULL;
    }else{
        locid = DWLocInit( Client );
        DWLocSym( Client, locid, (dw_sym_handle)bck, DW_W_LABEL_SEG );
        dw_segloc = DWLocFini( Client, locid );
    }
#else
    dw_segloc = NULL;
#endif
    obj = DWVariable( Client, tipe, dw_loc,
                0, dw_segloc, "__bck", 0, 0 );

    DWLocTrash( Client, dw_loc );
    if( dw_segloc != NULL ){
        DWLocTrash( Client, dw_segloc );
    }
    return( obj );
}

extern  dbg_type    DFEndArray( array_list *ar ){
/************************************************/
    dw_dim_info    info;
    dw_vardim_info varinfo;
    dbg_type       lo_tipe;
    dbg_type       count_tipe;
    type_def       *tipe_addr;
    dim_any        *dim;
    dbg_type       ret;

    ret = DWBeginArray( Client, ar->base, 0, NULL, 0, 0 );
    lo_tipe = DBG_NIL_TYPE;
    tipe_addr = NULL;
    count_tipe = DBG_NIL_TYPE;
    for(;;) {
        dim = ar->list;
        if( dim == NULL ) break;
        switch( dim->entry.kind ) {
        case DIM_CON:
            info.index_type = dim->con.idx;
            info.lo_data  = dim->con.lo;
            info.hi_data  = dim->con.hi;
            DWArrayDimension( Client, &info );
            break;
        case DIM_VAR:
            if( lo_tipe == DBG_NIL_TYPE ){
                tipe_addr = TypeAddress( dim->var.lo_bound_tipe );
                lo_tipe = DFScalar( "", dim->var.lo_bound_tipe );
                count_tipe = DFScalar( "",  dim->var.num_elts_tipe );
            }
            varinfo.index_type = lo_tipe;
            varinfo.lo_data  =  MKBckVar( dim->var.dims, dim->var.off, lo_tipe);
            varinfo.count_data  = MKBckVar( dim->var.dims, dim->var.off + tipe_addr->length, count_tipe);
            DWArrayVarDim( Client, &varinfo );
            break;

        }
        ar->list = dim->entry.next;
        CGFree( dim );
    }
    DWEndArray( Client );
    return( ret );
}

extern  dbg_type        DFFtnArray( back_handle dims, cg_type lo_bound_tipe,
                                    cg_type num_elts_tipe, int off,
                                    dbg_type base ) {
/***************************************************************************/

    dw_vardim_info varinfo;
    dbg_type       lo_tipe;
    dbg_type       count_tipe;
    type_def       *tipe_addr;
    dbg_type       ret;

    ret = DWBeginArray( Client, base, 0,NULL, 0, 0 );
    tipe_addr = TypeAddress( lo_bound_tipe );
    lo_tipe = DFScalar( "", lo_bound_tipe );
    count_tipe = DFScalar( "", num_elts_tipe );
    varinfo.index_type = lo_tipe;
    varinfo.lo_data  =  MKBckVar( dims, off, lo_tipe);
    varinfo.count_data  = MKBckVar( dims, off+tipe_addr->length, count_tipe);
    DWArrayVarDim( Client, &varinfo );
    DWEndArray( Client );
    return( ret );
}


extern  dbg_type        DFSubRange( signed_32 lo, signed_32 hi,
                                    dbg_type base ) {
/***************************************************/
/* need some dwarflib support */
    lo = lo;
    hi = hi;
    base = base;
    return( 0 );
}

static  uint   DFPtrClass( cg_type ptr_type ){
/*******************************************************************/
    type_def    *tipe_addr;
    uint        flags;

    if( (ptr_type == TY_POINTER || ptr_type == TY_CODE_PTR)
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
      && _IsTargetModel( FLAT_MODEL )  ) {
#else
      ) {
#endif
        flags = DW_PTR_TYPE_DEFAULT;
    } else {
        flags = 0;
        tipe_addr = TypeAddress( ptr_type );
        switch( tipe_addr->refno ) {
        case TY_HUGE_POINTER:
            flags = DW_PTR_TYPE_HUGE16;
        //  flags = DW_PTR_TYPE_FAR16;
            break;
        case TY_LONG_POINTER:
        case TY_LONG_CODE_PTR:
            if( tipe_addr->length == 6 ) {
                flags = DW_PTR_TYPE_FAR32;
            } else {
                flags = DW_PTR_TYPE_FAR16;
            }
            break;
        case TY_NEAR_POINTER:
        case TY_NEAR_CODE_PTR:
            if( tipe_addr->length == 4 ) {
                flags = DW_PTR_TYPE_NEAR32;
            } else {
                flags = DW_PTR_TYPE_NEAR16;
            }
            break;
        }
    }
    return( flags );
}


extern  dbg_type        DFDereference( cg_type ptr_type, dbg_type base ) {
/************************************************************************/
    dbg_type    ret;
    uint        flags;

    flags = DFPtrClass( ptr_type );
    ret = DWPointer( Client, base, flags | DW_FLAG_REFERENCE  );
    return( ret );
}

extern  dbg_type        DFPtr( cg_type ptr_type, dbg_type base ) {
/****************************************************************/
    dbg_type    ret;
    uint        flags;

    flags = DFPtrClass( ptr_type );
    ret = DWPointer( Client, base, flags );
    return( ret );
}


extern  void      DFBegStruct( struct_list  *st ){
/******************************************************/
    dbg_type    ret;
    uint        class;

    if( st->is_struct ){
        class = DW_ST_STRUCT;
    }else{
        class = DW_ST_UNION;
    }
    ret = DWStruct( Client, class );
    st->me = ret;
}


typedef struct {
    dw_loc_id       locid;
    bool            addr_seg:1;
    bool            seg     :1;
    bool            offset  :1;
}loc_state;

static  dw_loc_id   DoLocCnv( dbg_loc loc, loc_state *state ) {
/****************************************/

    type_length     offset;
    dw_loc_id       locid;
    uint            dref_op;
    uint            size;
    dw_sym_handle   sym;

    if( loc->next != NULL ) {
        locid = DoLocCnv( loc->next, state );
    }else{
        locid = state->locid;
    }
    switch( loc->class & 0xf0 ) {
    case LOC_CONSTANT:
        if( loc->class == LOC_MEMORY ) {
            sym = (dw_sym_handle)loc->u.fe_sym;
            if( state->seg == TRUE ){
                DWLocSegment( Client, locid, sym );
            }
            if( state->offset == TRUE ){
                DWLocStatic( Client, locid, sym );
                if( state->seg ){
                    state->addr_seg = TRUE;
                }
            }
        } else {
            offset = loc->u.val;
            DWLocConstS( Client, locid, offset );
        }
        break;
    case LOC_BP_OFFSET:
        offset = NewBase( loc->u.be_sym );
        DWLocOp( Client, locid, DW_LOC_fbreg, offset );
        break;
    case LOC_REG:
        DFOutReg( locid, loc->u.be_sym );
        break;
    case LOC_IND_REG:
        if( HW_CEqual( loc->u.be_sym->r.reg, HW_EMPTY ) ) {
            //NYI: structured return value on the stack. Have to do something
            //       suitable. For now, output a no location.
            DWLocOp( Client,locid,DW_LOC_breg, 0, 0 );
        } else {
            DFOutRegInd( locid, loc->u.be_sym );
        }
        break;
    case LOC_OPER:
        switch( loc->class & 0x0f ) {
        case LOP_IND_2:
        case LOP_IND_4:
            if( state->addr_seg ){
                dref_op =  DW_LOC_xderef_size;
            }else{
                dref_op =  DW_LOC_deref_size;
            }
            if( (loc->class & 0x0f) == LOP_IND_2 ){
                size = 2;
            }else{
                size = 4;
            }
            DWLocOp( Client, locid, dref_op, size );
            state->addr_seg = FALSE;  /* ate offset seg */
            break;
        case LOP_IND_ADDR286:
        case LOP_IND_ADDR386:
            if( (loc->class & 0x0f) == LOP_IND_ADDR286 ){
                size = 2;
            }else{
                size = 4;
            }
            if( state->addr_seg ){
                dref_op =  DW_LOC_xderef_size;
                DWLocOp0( Client, locid, DW_LOC_pick );  /* dup seg */
                DWLocOp0( Client, locid, DW_LOC_pick );  /* dup offset */
                DWLocOp( Client, locid, DW_LOC_plus_uconst, size ); /* seg offset*/
                DWLocOp( Client, locid, dref_op, 2 );  /* push seg */
                DWLocOp0( Client, locid, DW_LOC_rot );  /* seg at bottom */
                DWLocOp( Client, locid, dref_op, size ); /* push offset */
                /* now have offset seg on stack */
            }else{
                dref_op =  DW_LOC_deref_size;
                DWLocOp0( Client, locid, DW_LOC_dup );    /* dup offset */
                DWLocOp( Client, locid, DW_LOC_plus_uconst, size ); /* seg offset*/
                DWLocOp( Client, locid, dref_op, 2 );  /* push seg */
                DWLocOp0( Client, locid, DW_LOC_swap ); /* get dup offset */
                DWLocOp( Client, locid, dref_op, size ); /* push offset */
                /* now have offset seg on stack */
            }
            state->addr_seg = TRUE; /* we have offset seg on stack */
            break;
        case LOP_ZEB:
            DWLocConstU( Client, locid, 0xff );
            DWLocOp0( Client, locid, DW_LOC_and );
            break;
        case LOP_ZEW:
            DWLocConstU( Client, locid, 0xffff );
            DWLocOp0( Client, locid, DW_LOC_and );
            break;
        case LOP_MK_FP:
            state->addr_seg = TRUE;
            break;
        case LOP_POP:
            DWLocOp0( Client, locid, DW_LOC_drop );
            break;
        case LOP_XCHG:
            DWLocOp0( Client, locid, DW_LOC_swap );
            break;
        case LOP_ADD:
            DWLocOp0( Client, locid, DW_LOC_plus );
            break;
        case LOP_DUP:
            DWLocOp0( Client, locid, DW_LOC_dup );
            break;
        case LOP_NOP:
            DWLocOp0( Client, locid, DW_LOC_not );
            break;
        }
        break;
    default:
        break;
    }
    return( locid );
}

static  dbg_loc   SkipMkFP( dbg_loc loc ) {
/****************************************/
//skip a MkFP and operand

    if( (loc->class & 0xf0) == LOC_OPER ) {
        if( (loc->class & 0x0f) == LOP_MK_FP  ){
            loc = loc->next;  /* skip MK_FP */
            loc = loc->next;   /* skip operand */
        }
    }
    return( loc );
}

extern dw_loc_id DBGLoc2DFCont( dbg_loc loc, dw_loc_id df_locid ){
/*****************************************/
/* Convert Brian to a dwarf              */
/* in a continious fasion                */
/*****************************************/
    loc_state       state;

    state.seg = FALSE;
    state.offset = TRUE;
    state.addr_seg = FALSE;
    state.locid = df_locid;
    if( loc != NULL ){
        df_locid = DoLocCnv( loc, &state );
    }
    return( df_locid );

}
extern dw_loc_handle DBGLoc2DF( dbg_loc loc ){
/*****************************************/
/* Convert Brian to a dwarf                */
/*****************************************/
    dw_loc_id       df_locid;
    dw_loc_handle   df_loc;
    loc_state       state;

    state.seg = FALSE;
    state.offset = TRUE;
    state.addr_seg = FALSE;
    state.locid = DWLocInit( Client );
    if( loc != NULL ){
        df_locid = DoLocCnv( loc, &state );
        df_loc = DWLocFini( Client, df_locid );
    }else{
        df_locid = state.locid;
        df_loc = DWLocFini( Client, df_locid );
    }
    return( df_loc );

}

extern dw_loc_handle DBGLocBase2DF( dbg_loc loc_seg ){
/*****************************************/
/* Convert Brian to a dwarf                */
/*****************************************/
    dw_loc_id       df_locid;
    dw_loc_handle   df_loc;
    loc_state       state;

    state.seg = TRUE;
    state.offset = TRUE;
    state.addr_seg = FALSE;
    state.locid = DWLocInit( Client );
    loc_seg = SkipMkFP( loc_seg );
    if( loc_seg != NULL ){
        df_locid = DoLocCnv( loc_seg, &state );
        df_loc = DWLocFini( Client, df_locid );
    }else{
        df_locid = state.locid;
        df_loc = DWLocFini( Client, df_locid );
    }
    return( df_loc );

}
extern  dbg_type        DFBasedPtr( cg_type ptr_type, dbg_type base,
                                        dbg_loc loc_segment ) {
/****************************************************************/
/* need support to get segment value */
    dbg_type        ret;
    uint            flags;
    dw_loc_handle   dw_segloc;

    dw_segloc = DBGLocBase2DF( loc_segment );
    flags = DFPtrClass( ptr_type );
    ret = DWBasedPointer( Client, base, dw_segloc, flags );
    if( dw_segloc != NULL ){
        DWLocTrash( Client, dw_segloc );
    }
    return( ret );
}

static int WVDFAccess( uint attr ){
    int ret;

    if( attr & FIELD_INTERNAL ){
        attr &= ~FIELD_INTERNAL;
        ret = DW_FLAG_ARTIFICIAL;
    }else{
        ret = 0;
    }
    switch( attr ){
    case FIELD_PUBLIC:
        ret |= DW_FLAG_PRIVATE;
        break;
    case FIELD_PROTECTED:
        ret |= DW_FLAG_PROTECTED;
        break;
    case FIELD_PRIVATE:
        ret |= DW_FLAG_PRIVATE;
        break;
    }
    return( ret );
}

extern  dbg_type        DFEndStruct( struct_list  *st ) {
/*******************************************************/

    field_any      *field;
    dbg_type        ret;
    dw_loc_id       locid;
    dw_loc_handle   loc;
    char           *name;
    uint            flags;

    ret = st->me;
    if( st->name[0] != '\0' ){
        name = st->name;
    }else{
        name = NULL;
    }
    DWBeginStruct( Client, ret, st->size, name, 0, 0 );
    for(;;) {
        field = st->list;
        if( field == NULL ) break;
        switch( field->entry.field_type ) {
        case FIELD_INHERIT:
            flags = WVDFAccess(field->bclass.attr );
            loc = DBGLoc2DF( field->bclass.u.adjustor );
            DBLocFini( field->bclass.u.adjustor );
            if( field->bclass.kind == INHERIT_VBASE ){
                flags |= DW_FLAG_VIRTUAL;
            }
            DWAddInheritance( Client, field->bclass.base, loc, flags );
            DWLocTrash( Client, loc );
            break;
        case FIELD_METHOD:
            DBLocFini( field->method.u.loc );
            break;
        case FIELD_NESTED:
            break;
        case FIELD_LOC:
        case FIELD_OFFSET:
            /* some loc thing */
            flags = WVDFAccess(field->member.attr );
            if( field->entry.field_type == FIELD_LOC ){
                loc = DBGLoc2DF( field->member.u.loc );
                 DBLocFini( field->member.u.loc );
            }else{
                locid = DWLocInit( Client );
                DWLocConstU( Client, locid, field->member.u.off );
                DWLocOp0( Client, locid, DW_LOC_plus );
                loc = DWLocFini( Client, locid );
            }
            if( field->member.b_strt == 0 && field->member.b_len == 0 ){
                DWAddField( Client, field->member.base, loc,
                    field->member.name, flags );
            }else{
                int bit_start;

                bit_start = 4*8-( field->member.b_strt+field->member.b_len);
                DWAddBitField( Client, field->member.base, loc, 4,
                               bit_start, field->member.b_len,
                               field->member.name, flags );
            }
            DWLocTrash( Client, loc );
            break;
        case FIELD_STFIELD:
            /* some loc thing */
            flags = WVDFAccess( field->stfield.attr );
            loc = DBGLoc2DF( field->stfield.loc );
            DBLocFini( field->stfield.loc );
            DWAddField( Client, field->stfield.base, loc,
                         field->stfield.name, flags | DW_FLAG_STATIC );
            DWLocTrash( Client, loc );
            break;
        }
        st->list = field->entry.next;
        CGFree( field );
    }
    DWEndStruct( Client );
    return( ret );
}


extern  dbg_type        DFEndEnum( enum_list *en ) {
/**************************************************/
    dbg_type    ret;
    type_def    *tipe_addr;
    const_entry *cons;
    signed_64   val;

    tipe_addr = TypeAddress( en->tipe );
    ret = DWBeginEnumeration( Client, tipe_addr->length, NULL, 0, 0 );
    for(;;) {
        cons = en->list;
        if( cons == NULL ) break;
        val = cons->val;
        if( val.u._32[I64HI32] == 0 || val.u._32[I64HI32] == -1 ){
            DWAddConstant( Client, val.u._32[I64LO32], cons->name );
        }else{
            DWAddConstant( Client, val.u._32[I64LO32], cons->name );
        }
        en->list = cons->next;
        CGFree( cons );
    }
    DWEndEnumeration( Client );
    return( ret );
}


extern  dbg_type        DFEndProc( proc_list  *pr ) {
/***************************************************/

    parm_entry  *parm;
    dbg_type    proc_type;
    uint        flags;


//  flags = DFPtrClass( pr->call );
    flags = DW_FLAG_PROTOTYPED | DW_FLAG_DECLARATION;
    proc_type = DWBeginSubroutineType( Client, pr->ret, NULL, 0, flags );
    parm = pr->list;
    if( parm == NULL ){
        DWAddEllipsisToSubroutineType( Client );
    }
    for(;;) {
        if( parm == NULL ) break;
        DWAddParmToSubroutineType( Client, parm->tipe, NULL, NULL, NULL );
        pr->list = parm->next;
        CGFree( parm );
        parm = pr->list;
    }
    DWEndSubroutineType( Client );
    return( proc_type );
}


