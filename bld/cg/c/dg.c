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



#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "cgstd.h"
#include "cg.h"
#include "bckdef.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "typclass.h"
#include "typedef.h"
#include "types.h"
#include "cfloat.h"
#include "cgaux.h"
#include "model.h"
#include "cgstub.h"
#include "dbcue.h"
#include "feprotos.h"

#include "stubdata.h"

extern  unsigned_16     TypeIdx;

extern  char            *ACopyOf(char *);
extern  void            VerTipe(cg_type ,cg_type *);
extern  char            *Tipe(cg_type );
extern  char            *FtnTipe(dbg_ftn_type );
extern  char            *Label(l *);
extern  void            Action(char *,... );
extern  void            TypDbg(char *,... );
extern  void            Code(char *,... );
extern  char            *Name(pointer );
extern  void            Put(char *,... );
extern  void            SymDbg(char *,... );
extern  void            VerBack(b *);
extern  char            *LToS(signed_32 );
extern  void            CGError(char *, ... );
extern  void            DDefLabel(l *);
extern  void            DRefLabel(l *);
extern  void            Find(char *,pointer *,pointer );



extern  void    DGAlign( uint algn ) {
//====================================

    Action( "DGAlign( %d )%n", algn );
    Locs[ CurSeg ] += (Locs[ CurSeg ] + algn - 1) % algn;
    Put("        align to %d byte boundry (%d)%n", algn, Locs[ CurSeg ] );
}
extern  uint    DGSeek( unsigned_32 where ) {
//====================================

    unsigned_32 old;

    Action( "DGSeek( %l )", where );
    Put( "        seek to location %l%n", where );
    old = Locs[ CurSeg ];
    Locs[ CurSeg ] = where;
    Action( " -> %l%n", old );
    return( old );
}
extern  void    DGUBytes( unsigned_32 len ) {
//===========================================

    Action( "DGUBytes( %l )%n", len );
    Put("        %l B(?)%n", len );
    Locs[ CurSeg ] += len;
}
extern  void    DGIBytes( unsigned_32 len, byte pat ) {
//=============================================

    Action( "DGIBytes( %l, %h )%n", len, pat );
    Put("        %d B(%h)%n", len, pat );
    Locs[ CurSeg ] += len;
}
extern  void    DGLabel( b *bk ) {
//================================

    Action( "DGLabel" );
    Action( "( %s = %s )%n", Label( bk->lp ), Name( bk->s ) );
    DDefLabel( bk->lp );
    VerBack(bk);
    PutName(bk);
    bk->loc = Locs[ CurSeg ];
    Put(":%n");
}
extern  unsigned_32     DGTell() {
//========================

    Action( "DGTell() -> %l%n", Locs[ CurSeg ] );
    return( Locs[ CurSeg ] );
}
extern  unsigned_32     DGBackTell( b *bk ) {
//===================================

    Action( "DGBackTell" );
    VerBack(bk);
    if( !bk->lp->ddef ) {
        CGError( "Label not yet defined in data segment" );
    }
    DRefLabel( bk->lp );
    Action( "( %s ) -> %l%n", Label( bk->lp ), bk->loc );
    return( bk->loc );
}
extern  void    DGFEPtr( sym s, cg_type t, signed_32 o ) {
//==========================================================

    b   *bk;

    Action( "DGFEPtr" );
    Action( "( %s, %s, %l )%n", Name( s ), Tipe( t ), o );
    bk = (b*)FEBack(s);
    VerBack(bk);
    if( !( FEAttr( s ) & FE_IMPORT ) ) DRefLabel( bk->lp );
    Put( "        A(" );
    PutName(bk);
    Put( ") + %l %s%n",o,Tipe(t) );
    Locs[ CurSeg ] += TypeAddress( t )->length;
}
extern  void    DGBackPtr( b *bk, segment_id s, signed_32 o, cg_type t ) {
//==========================================================================

    Action( "DGBackPtr" );
    Action( "( %s, %d, %l, %s )%n", Label( bk->lp ), s, o, Tipe( t ) );
    VerBack(bk);
    DRefLabel( bk->lp );
    Put( "        A(" );
    PutName(bk);
    Put( ") + %l (seg %d) %s%n",o,s,Tipe(t) );
    Locs[ CurSeg ] += TypeAddress( t )->length;
}
extern  void    DGBytes( unsigned_32 len, byte *bp ) {
//============================================

    Action( "DGBytes( %l, ... )%n", len );
    Put( "        " );
    while( len != 0 ) {
        Put( "%h ",*bp++ );
        --len;
    }
    Put( "%n" );
    Locs[ CurSeg ] += len;
}
extern  void    PutName( b *bk ) {
//================================

    Put( "%s", Label( bk->lp ) );
    if( bk->s != NULL ) {
        Put( " [ %s ]", Name( bk->s ) );
    }
}
extern  void    DGInteger(  unsigned_32 i,  cg_type  t ) {
//======================================================

    Action( "DGInteger" );
    VerTipe( t,DGIntTypes );
    Action( "( %l, %s )%n", i, Tipe( t ) );
    Put( "        I(%l)", i );
    Put( " %s%n", Tipe(t) );
    Locs[ CurSeg ] += TypeAddress( t )->length;
}
extern  void    DGCFloat( pointer f, cg_type t ) {
//================================================
    char        buffer[ 80 ];

    CFCnvFS( f, buffer, 80 );
    Put( "        F(%s)", buffer );
    Put( " %s%n", Tipe(t) );
    Locs[ CurSeg ] += TypeAddress( t )->length;
}
extern  void    DGFloat( char *f, cg_type t ) {
//=============================================

    Action( "DGFloat" );
    VerTipe( t,FloatTypes );
    Put( "        F(%s) %s%n", f, Tipe(t) );
    Action( "( %s, %s )%n", f, Tipe( t ) );
    Locs[ CurSeg ] += TypeAddress( t )->length;
}
extern  void    DGChar( char c ) {
//================================

    Action( "DGChar( %c )%n", c );
    Put( "        C(%c)%n",c );
    Locs[ CurSeg ] += sizeof( char );
}
extern  void    DGString( char *s, uint len ) {
//============================================

    Action( "DGString( ..., %d )%n", len );
    Locs[ CurSeg ] += len;
    Put( "        S(" );
    for( ; len != 0; --len ) {
        Put( "%c", *s++ );
    }
    Put( ")%n",s );
}
enum {
LOC_SYM,
LOC_CONST,
LOC_TEMP,
LOP_IND_2,
LOP_IND_4,
LOP_IND_ADDR286,
LOP_IND_ADDR386,
LOP_ZEB,
LOP_ZEW,
LOP_MK_FP,
LOP_POP,
LOP_XCHG,
LOP_ADD,
LOP_DUP
};
static char *LopNames[] = {
"",
"",
"IND_2",
"IND_4",
"IND_A286",
"IND_A386",
"ZEB",
"ZEW",
"MK_FP",
"POP",
"XCHG",
"ADD",
"DUP"
};
typedef struct location {
        struct location         *next;
        union {
            t                   *be_sym;
            cg_sym_handle       fe_sym;
            unsigned            stk;
            unsigned_32         val;
        }                       u;
        byte                    class;
} location;
static char *DBOpNames[] = {
"DB_OP_POINTS",
"DB_OP_ZEX",
"DB_OP_XCHG",
"DB_OP_MK_FP",
"DB_OP_ADD",
"DB_OP_DUP",
"DB_OP_POP"
};

static char *DoLocation( char *str, dbg_loc loc ) {
/*************************************************/

    if( loc == NULL ) return( str );
    str = DoLocation( str, loc->next );
    *str++ = ' ';
    switch( loc->class ) {
    case LOC_SYM:
        sprintf( str, "{%s}", FEName( (pointer)loc->u.fe_sym ) );
        break;
    case LOC_TEMP:
        sprintf( str, "{T%ld}", loc->u.be_sym->i );
        break;
    case LOC_CONST:
        sprintf( str, "{%ld}", loc->u.val );
        break;
    case LOP_XCHG:
        sprintf( str, "XCHG[%d]", loc->u.stk );
        break;
    default:
        strcpy( str, LopNames[loc->class] );
        break;
    }
    str += strlen( str );
    return( str );
}
static char *Location(dbg_loc loc) {
/**********************************/

    static char buff[256];
    char        *p;

    buff[0] = '(';
    p = DoLocation( buff+1, loc );
    *p++ = ')';
    *p = '\0';
    return( buff );
}
static  dbg_loc         LocCreate( dbg_loc loc, unsigned typ ) {
/**************************************************************/

    dbg_loc     new;

    new = CGAlloc( sizeof( location ) );
    new->next = loc;
    new->class = typ;
    return( new );
}
extern  dbg_loc         DBLocInit() {
/***********************************/

    Action( "DBLocInit ==> NULL%n" );
    return( NULL );
}
extern dbg_loc          DBLocSym( dbg_loc loc, cg_sym_handle sym ) {
/****************************************************************/


    Action( "DBLocSym( %p, %s ) ==>", loc, FEName((pointer)sym) );
    loc = LocCreate( loc, LOC_SYM );
    loc->u.fe_sym = sym;
    Action( "%p%n", loc );
    return( loc );
}
extern dbg_loc          DBLocTemp( dbg_loc loc, t *tm  ) {
/****************************************************************/


    Action( "DBLocTemp" );
    Find( "back end temp", (pointer *)TempList, tm );
    Action( "( %p, T%ld ) ==>", loc, tm->i );
    loc = LocCreate( loc, LOC_TEMP );
    loc->u.be_sym = tm;
    Action( "%p%n", loc );
    return( loc );
}

extern  dbg_loc         DBLocConst( dbg_loc loc, unsigned_32 val ) {
/******************************************************************/

    Action( "DBLocConst( %p, %l ) ==>", loc, val );
    loc = LocCreate( loc, LOC_CONST );
    loc->u.val = val;
    Action( "%p%n", loc );
    return( loc );
}
extern  dbg_loc         DBLocOp(dbg_loc loc, dbg_loc_op op, unsigned other) {
/***************************************************************************/

    unsigned    stkop;

    Action( "DBLocOp( %p, %s, %d ) ==>", loc, DBOpNames[ op ], other );
    stkop = 0;
    switch( op ) {
    case DB_OP_POINTS:
        VerTipe( other, NULL );
        switch( TypeAddress( other )->refno ) {
#if  !(_TARGET & _TARG_80386)
        case TY_NEAR_POINTER:
        case TY_NEAR_CODE_PTR:
#endif
        case TY_UINT_2:
        case TY_INT_2:
            stkop = LOP_IND_2;
            break;
#if  _TARGET & _TARG_80386
        case TY_NEAR_POINTER:
        case TY_NEAR_CODE_PTR:
#endif
        case TY_UINT_4:
        case TY_INT_4:
            stkop = LOP_IND_4;
            break;
        case TY_LONG_POINTER:
        case TY_HUGE_POINTER:
        case TY_LONG_CODE_PTR:
#if  _TARGET & _TARG_80386
            stkop = LOP_IND_ADDR386;
#else
            stkop = LOP_IND_ADDR286;
#endif
            break;
        default:
            CGError( "Invalid DB points type %d\n", other );
            break;
        }
        loc = LocCreate( loc, stkop );
        break;
    case DB_OP_ZEX:
        VerTipe( other, NULL );
        switch( TypeAddress( other )->length ) {
        case 1:
            stkop = LOP_ZEB;
            break;
        case 2:
            stkop = LOP_ZEW;
            break;
        default:
            CGError( "invalid ZEX size with type %d\n", other );
            break;
        }
        loc = LocCreate( loc, stkop );
        break;
    case DB_OP_XCHG:
        loc = LocCreate( loc, LOP_XCHG );
        loc->u.stk = other;
        break;
    case DB_OP_MK_FP:
        loc = LocCreate( loc, LOP_MK_FP );
        break;
    case DB_OP_ADD:
        loc = LocCreate( loc, LOP_ADD );
        break;
    case DB_OP_DUP:
        loc = LocCreate( loc, LOP_DUP );
        break;
    case DB_OP_POP:
        loc = LocCreate( loc, LOP_POP );
        break;
    default:
        CGError( "invalid location expression operator" );
        break;
    }
    Action( "%p%n", loc );
    return( loc );
}
extern  void _CGAPI             DBLocFini( dbg_loc loc ) {
/********************************************************/

    Action( "DBLocFini( %p )%n", loc );
}

static fname_lst  *SrcFiles;

static  void    SrcFileNoInit( void ){
/*****************************/
    SrcFiles = NULL;
}

static  void    DBSrcFileFini( void ){
/*****************************/
    fname_lst   *curr, *old;
    curr = SrcFiles;
    while( curr != NULL ){
        old = curr;
        curr = curr->next;
        CGFree( old );
    }
    SrcFiles = NULL;
}

extern  uint    DBSrcFile( char *fname ){
/****************************************/
    int          index;
    int          len;
    fname_lst   *curr, **lnk;

    lnk  = &SrcFiles;
    curr = *lnk;
    index = 0;
    while( (curr = *lnk) != NULL ){
       if( strcmp( fname, curr->fname ) == 0 ){
            goto found;
       }
       ++index;
       lnk = &curr->next;
    }
    len = strlen( fname );
    curr = CGAlloc( sizeof( *curr )+len );
    curr->next = NULL;
    strcpy( curr->fname, fname );
    *lnk = curr;
found:
    return( index );
}

extern  void    InitDbgInfo() {
/******************************/
    char       *fname;
    uint        fno;

    SrcFileNoInit();
    fname = FEAuxInfo( NULL, SOURCE_NAME );
    fno = DBSrcFile( fname );
}


extern  void    FiniDbgInfo() {
/******************************/

    DBSrcFileFini();
}

extern  void    DBLineNum( uint no ) {
//====================================

    Action( "%nDBLineNum( %d )%n", no );
    if( CodeSeg != -1 ) {
        Code( "<%d> ", no );
    }
}


extern  void    DBSrcCue( uint fno, uint line, uint col ) {
/************************************/

    Action( "%nDBSrcCue( %d, %d, %d )%n", fno, line, col );
}

extern  void    DBModSym( pointer sym, cg_type tipe ) {
//=====================================================

    Action( "DBModSym" );
    Action( "( %s, %s )%n", FEName(sym), Tipe( tipe ) );
    if( (FEAttr(sym) & FE_IMPORT) == 0 ) {
        /* Should remember and print FEDbgType(sym) later! */
        SymDbg( "ModSym '%s' %d (%s)%n", FEName(sym), 0,
                Tipe(tipe) );
    }
}
extern  void    DBLocalSym( pointer sym, cg_type tipe ) {
//=======================================================

    Action( "DBLocalSym" );
    Action( "( %s )%n", FEName(sym) );
    if( (FEAttr(sym) & FE_IMPORT) == 0 ) {
        /* Should remember and print FEDbgType(sym) later! */
        SymDbg( "LocalSym '%s' %d (%s)%n", FEName(sym), 0,
                Tipe(tipe));
    }
}
extern  void            DBTypeDef( char *nm, dbg_type  tipe ){
/********************************************************/
    Action( "DBTypeDef( %s, %l ) ==>", nm, tipe );
}

void            DBLocalType(pointer sym, bool kind ){
/***************************************************/
    Action( "DBLocalType( %s, %d ) ==>", FEName((pointer)sym), kind );
}

extern  void    DBObject( dbg_type tipe, dbg_loc loc ) {
//=====================================================

    Action( "DBObject" );
    Action( "( %d, %s )%n", tipe, Location( loc ) );
    SymDbg( "Object %d (%s)%n", tipe, Location( loc ) );
}
extern  void    DBBegBlock( ) {
//=============================

    Action( "DBBegBlock()%n" );
    SymDbg( "DBBegBlock%n" );
}
extern  void    DBEndBlock() {
//============================

    Action( "DBEndBlock()%n" );
    SymDbg( "DBEndBlock%n" );
}
extern  dbg_type        DBFtnType( char *nm, dbg_ftn_type tipe ) {
//================================================================

    Action( "DBFtnType" );
    Action( "( %s, %d )", nm, tipe );
    TypDbg( "(%d) Fortran '%s' == %s%n", ++TypeIdx, nm, FtnTipe( tipe ) );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBScalar( char *nm, cg_type tipe ) {
//==========================================================

    Action( "DBScalar" );
    Action( "( %s, %s )", nm, Tipe( tipe ) );
    TypDbg( "(%d) Scalar '%s' == %s%n", ++TypeIdx, nm, Tipe( tipe ) );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBScope( char *nm ) {
//===========================================

    Action( "DBScope( %s )", nm );
    TypDbg( "(%d) Scope Name '%s'%n", ++TypeIdx, nm );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_name        DBBegName( const char *nm, dbg_type scope ) {
//===================================================================

    name_entry  *name;

    Action( "DBBegName( %s, %d )", nm, scope );
    name = CGAlloc( sizeof( name_entry ) );
    name->name = (char*)ACopyOf( nm );
    name->scope = scope;
    name->refno = DBG_NIL_TYPE;
    Action( " -> %p%n", name );
    return( name );
}
extern  void    DumpName( name_entry *nm, dbg_type tipe ) {
//=========================================================

    TypDbg( "(%d) Type Name '%s' scope==%d, type==%d%n", ++TypeIdx,
            nm->name, nm->scope, tipe );
    nm->refno = TypeIdx;
}
extern  dbg_type        DBForward( name_entry *nm ) {
//===================================================

    Action( "DBForward( %p )", nm );
    if( nm->refno == DBG_NIL_TYPE ) {
       DumpName( nm, DBG_FWD_TYPE );
    }
    Action( " -> %d%n", nm->refno );
    return( nm->refno );
}
extern  dbg_type        DBEndName( name_entry *nm, dbg_type tipe ) {
//==================================================================

    dbg_type    retv;

    Action( "DBEndName( %p, %d )", nm, tipe );
    if( nm->refno == DBG_NIL_TYPE ) {
        DumpName( nm, tipe );
    } else {
        TypDbg( "Patch refno %d with %d%n", nm->refno, tipe );
    }
    retv = nm->refno;
    CGFree( nm->name );
    CGFree( nm );
    Action( " -> %d%n", retv );
    return( retv );
}
extern  dbg_type        DBCharBlock( unsigned_32 len ) {
//======================================================

    Action( "DBCharBlock( %l )", len );
    TypDbg( "(%d) CharBlock len==%l%n", ++TypeIdx, len );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBIndCharBlock( b *len, cg_type len_type, int off ) {
//=========================================================

    Action( "DBIndCharBlock" );
    Action( "( %s, %s, %d )", Label( len->lp ), Tipe( len_type ), off );
    TypDbg( "(%d) IndCharBlock len==%s, len_type==%s, offset==%d%n", ++TypeIdx,
            Label( len->lp ), Tipe( len_type ), off );
    Action( " -> %d%n", TypeIdx );
    if( !len->lp->ddef ) {
        CGError( "Label not defined in data segment" );
    }
    DRefLabel( len->lp );
    return( TypeIdx );
}
extern  dbg_type        DBLocCharBlock( dbg_loc loc, cg_type len_type ) {
//=======================================================================

    Action( "DBLocCharBlock" );
    Action( "( %s, %s )", Location( loc ), Tipe( len_type ) );
    TypDbg( "(%d) LocCharBlock len==%s, len_type==%s", ++TypeIdx,
            Location( loc ), Tipe( len_type ) );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBDereference( cg_type ptr_type, dbg_type base ) {
//=======================================================================

    Action( "DBDereference" );
    Action( "( %s, %d )", Tipe( ptr_type ), base );
    TypDbg( "(%d) DeReference ptr_type==%s, base==%d%n", ++TypeIdx,
            Tipe(ptr_type), base );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBFtnArray( b *bk, cg_type lo_bound,
                                    cg_type num_elts, int dim_off,dbg_type base ) {
//=======================================================================

    Action( "DBFtnArray" );
    Action( "( %s, %s, %s, %d, %d )", Label( bk->lp ),
            Tipe( lo_bound ), Tipe( num_elts ), dim_off, base );
    TypDbg( "(%d) ADV==%s, lo_bound=%s, num_elts=%s, offset=%d, base==%d%n",
            ++TypeIdx, Label( bk->lp ),
            Tipe( lo_bound ), Tipe( num_elts ),dim_off, base );
    Action( " -> %d%n", TypeIdx );
    if( !bk->lp->ddef ) {
        CGError( "Label not defined in data segment" );
    }
    DRefLabel( bk->lp );
    return( TypeIdx );
}
extern  dbg_type        DBArray( dbg_type idx, dbg_type base ) {
//==============================================================

    Action( "DBArray( %d, %d )", idx, base );
    TypDbg( "(%d) Array index==%d, base==%d%n", ++TypeIdx, idx, base );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBIntArray( unsigned_32 hi, dbg_type base ) {
//===================================================================

    Action( "DBIntArray( %l, %d )", hi, base );
    TypDbg( "(%d) Int Array hi==%l, base==%d%n", ++TypeIdx, hi, base );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBIntArrayCG( cg_type tipe, unsigned_32 hi, dbg_type base ) {
/*******************************************************************/

    Action( "DBIntArrayCg( %s, %l, %d )", Tipe(tipe), hi, base );
    TypDbg( "(%d) Int Array hi==%l, base==%d%n", ++TypeIdx, hi, base );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}

extern  array_list  *DBBegArray(  dbg_type base, cg_type tipe, bool is_col_major ){
/******************************************************************************/
    array_list *ar;
    type_def   *tipe_addr;

    Action( "DBBegArray( %l, %s, %d )", base, Tipe( tipe ),
                     is_col_major );
    ar = CGAlloc( sizeof( *ar ) );
    tipe_addr = TypeAddress( tipe );
    ar->num = 0;
    ar->size = tipe_addr->length;
    ar->list = NULL;
    ar->base = base;
    ar->is_col_major = is_col_major;
    Action( " -> %p%n", ar );
    return( ar );
}

static  void    AddDim( array_list *ar, dim_any *dim ){
/******************************************************/

    dim_entry *curr;
    dim_entry **owner;

    owner = &ar->list;
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
        owner = &curr->next;
    }
    dim->entry.next = NULL;
    *owner = dim;
    ar->num++;
}

extern  void DBDimCon( array_list *ar, dbg_type idx, signed_32 lo, signed_32 hi ){
/***************************************************/
    dim_con *dim;

    Action( "DBDimCon( %p %d, %l, %l, %d)%n", ar, idx, lo, hi );
    dim = CGAlloc(  sizeof( *dim ) );
    dim->entry.kind = DIM_CON;
    dim->lo = lo;
    dim->hi = hi;
    dim->idx = idx;
    AddDim( ar, dim );
}

extern  void  DBDimVar( array_list *ar,
                        b      *dims, int off,
                        cg_type lo_bound_tipe,
                        cg_type num_elts_tipe ){
/*************************************************/
    dim_var *dim;

    Action( "DBDimVar" );
    Action( "( %p, %s, %s, %s, %d )%n", ar, Label( dims->lp ),
            Tipe( lo_bound_tipe ), Tipe( num_elts_tipe ), off );
    dim = CGAlloc( sizeof( *dim ) );
    dim->entry.kind = DIM_VAR;
    dim->dims = (back_handle)dims;
    dim->off = off;
    dim->lo_bound_tipe = lo_bound_tipe;
    dim->num_elts_tipe = num_elts_tipe;
    AddDim( ar, dim );
}

extern  dbg_type        DBEndArray( array_list  *ar ) {
/*******************************************************/


    Action( "DBEndArray( %p )", ar );
    TypDbg( "(%d) array #dim == %d%n", ++TypeIdx, ar->num );
    CGFree( ar );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}

extern  dbg_type        DBSubRange( signed_32 lo, signed_32 hi, dbg_type base ) {
//=============================================================================

    Action( "DBSubRange( %l, %l, %d )", lo, hi, base );
    TypDbg( "(%d) Subrange  lo==%l, hi==%l, base==%d%n", ++TypeIdx,
             lo, hi, base );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  dbg_type        DBPtr( cg_type ptr_tipe, dbg_type base ) {
//================================================================

    Action( "DBPtr" );
    Action( "( %s, %d )", Tipe( ptr_tipe ), base );
    TypDbg( "(%d) Pointer <%s>, base==%d%n", ++TypeIdx, Tipe(ptr_tipe),
            base );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}

struct_list     *DBBegNameStruct( const char *nm, cg_type tipe, bool is_struct ) {
/********************************************************************************/
    struct_list *st;

    is_struct = is_struct;
    tipe = tipe;
    Action( "DBBegNameStruct(%s)", nm );
    st = CGAlloc( sizeof( struct_list ) );
    st->num = 0;
    st->list = NULL;
    // st->me = DBG_NIL_TYPE;
    Action( " -> %p%n", st );
    return( st );
}

struct_list     *DBBegStruct( cg_type tipe, bool is_struct ) {
//============================================================

    struct_list *st;

    is_struct = is_struct;
    tipe = tipe;
    Action( "DBBegStruct()" );
    st = CGAlloc( sizeof( struct_list ) );
    st->num = 0;
    st->list = NULL;
    // st->me = DBG_NIL_TYPE;
    Action( " -> %p%n", st );
    return( st );
}

static  field_member     *CreateMember( char *nm, byte strt, byte len,
                                        dbg_type base, uint attr ) {
/******************************************************************/
    uint          n_len;
    field_member *field;

    n_len = strlen( nm );
    field = CGAlloc( sizeof( field_member ) + n_len );
    strcpy( field->name, nm );
    field->attr = attr;
    field->len = n_len;
    field->base = base;
    field->b_strt = strt;
    field->b_len  = len;
    return( field );
}

static  void    AddField( struct_list *st, field_any *field ){
/***************************************************************/

    field_entry *curr;
    field_entry **owner;

    owner = &st->list;
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
        owner = &curr->next;
    }
    field->entry.next = NULL;
    *owner = field;
    st->num++;
}

extern  void    DBAddBitField(struct_list *st,unsigned_32 off,byte strt,byte len,char *nm,dbg_type base) {
    field_member *field;

    Action( "DBAddBitField( %p, %l, %d, %d, %s, %d )%n",
                          st, off, strt, len, nm, base );
    field = CreateMember( nm, strt, len, base, 0 );
    field->entry.field_type = FIELD_OFFSET;
    field->u.off= off;
    AddField( st, field );
}
extern  void    DBAddLocField( struct_list *st, dbg_loc loc, uint attr,
                            byte strt, byte len, char *nm, dbg_type base ) {
/******************************************************************/
    field_member *field;

    Action( "DBAddLocField( %p, %s, %d, %d, %d, %s, %d )%n",
                  st, Location(loc), attr, strt, len, nm, base );
    field = CreateMember( nm, strt, len, base, attr );
    field->entry.field_type = FIELD_LOC;
    field->u.loc  = loc;
    AddField( st, field );
}


extern void DBAddField(struct_list *st,unsigned_32 off,char *nm,dbg_type base) {
//==============================================================================

    field_member *field;

    Action( "DBAddField( %p, %l, %s, %d )%n", st, off, nm, base );
    field = CreateMember( nm, 0, 0, base, 0 );
    field->entry.field_type = FIELD_OFFSET;
    field->u.off= off;
    AddField( st, field );
}

extern  void    DBAddStField( struct_list *st, dbg_loc loc, char *nm, uint attr,
                                             dbg_type base ) {
/*************************************************************/

    uint          n_len;
    field_stfield *field;

    Action( "DBAddStField( %p, %s, %s, %d, %d )%n",
                  st, Location(loc),nm, attr, base );
    n_len = strlen( nm );
    field = CGAlloc( sizeof( field_stfield ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_STFIELD;
    field->attr = attr;
    field->base = base;
    AddField( st, field );
}

extern  void    DBAddMethod( struct_list *st, dbg_loc loc,
                             uint attr, uint kind,
                             char *nm, dbg_type base ) {
/******************************************************************/

    uint          n_len;
    field_method *field;

    Action( "DBAddMethod( %p, %s, %d, %s, %d )%n",
                  st, Location(loc), attr, nm, base );
    n_len = strlen( nm );
    field = CGAlloc( sizeof( field_method ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_METHOD;
    field->u.loc =  loc;
    field->attr = attr;
    field->kind = kind;
    field->len = n_len;
    field->base = base;
    AddField( st, field );
}

extern  void    DBAddNestedType( struct_list *st, char *nm,
                                                  dbg_type base ) {
/******************************************************************/

    uint          n_len;
    field_nested *field;

    Action( "DBAddNestedType( %p, %s, %d )%n",
                  st, nm, base );
    n_len = strlen( nm );
    field = CGAlloc( sizeof( field_nested ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_NESTED;
    field->base = base;
    AddField( st, field );
    st->is_cnested = TRUE;
}


extern  void        DBAddInheritance( struct_list *st, dbg_type inherit,
                                  uint attr, uint kind,  dbg_loc loc ) {
//===========================================================================
    field_bclass *field;


    Action( "DBAddInheritance( %p, %d, %p )%n", st, inherit, loc );
    field = CGAlloc( sizeof( field_bclass ) );
    field->entry.field_type = FIELD_INHERIT;
    field->attr = attr;
    field->base = inherit;
    field->attr = attr;
    field->kind = kind;
    field->u.adjustor  = loc;
    AddField( st, field );
}

extern  void    DBAddBaseInfo( struct_list  *st, unsigned_32 vb_off,  unsigned_32 esize,
                                              dbg_type vtbl, cg_type ptr_type ){
/*******************************************************************/
    st->vtbl_off    = vb_off;
    st->vtbl_type  = vtbl;
    st->ptr_type  =  ptr_type;
    st->vtbl_esize = esize;
     Action( "DBAddBaseInfo( %p, %d, %d, %d,%s )%n", st, vb_off,
                             esize, vtbl, Tipe( ptr_type) );
}

extern  void    DBAddVFuncInfo( struct_list  *st, unsigned_32 vfptr_off,
                                                  int size,
                                                  cg_type vft_cgtype ){
//======================================================================
     Action( "DBAddVFuncInfo( %p, %d, %d,%s )%n", st, vfptr_off,
                             size,Tipe( vft_cgtype) );
}

extern  dbg_type        DBEndStruct( struct_list *st ) {
//======================================================

    field_any  *curr;
    uint        toff;

     Action( "DBEndStruct( %p )", st );
     TypDbg( "(%d) Struct #fields == %d%n", ++TypeIdx, st->num );
     for(;;) {
         curr = st->list;
         if( curr == NULL ) break;
         switch( curr->entry.field_type ) {
         case FIELD_OFFSET:{
             field_member *field = curr;

             toff = field->u.off;
             TypDbg( "    '%s' base==%d, offset==%d", field->name, field->base,
                     toff );
             if( field->len != 0 ) {
                 TypDbg( "{%d:%d}", field->b_strt, field->b_len );
             }
            break;
         }
         case FIELD_LOC:{
             field_member *field = curr;

             TypDbg( "    '%s' base==%d, attr==%h, location==%s", field->name, field->base,
                     field->attr, Location( field->u.loc ) );
             if( field->len != 0 ) {
                 TypDbg( "{%d:%d}", field->b_strt, field->b_len );
             }
            break;
         }
         case FIELD_INHERIT:{
             field_bclass *field = curr;

             TypDbg( "    inherit base==%d, adjust==%s", field->base,
                     Location( field->u.adjustor ) );
             break;
         }
         }
         TypDbg( "%n" );
         st->list = curr->entry.next;
         curr->entry.next = NULL;
         CGFree( curr );
     }
     CGFree( st );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}

bool            DBNested( bool flag ){
/************************************/
     Action( "DBNested( %d )", flag );
     return( 0 );
}

extern  dbg_type      DBStructForward( struct_list  *st ) {
//===========================================================
     Action( "DBStructForward( %p )", st );
     return( DBG_NIL_TYPE  );
}

extern  enum_list       *DBBegEnum( cg_type tipe ) {
//==================================================

    enum_list   *en;

    Action( "DBBegEnum" );
    Action( "( %s )%n", Tipe( tipe ) );
    en = CGAlloc( sizeof( enum_list ) );
    en->num = 0;
    en->list = NULL;
    en->tipe = tipe;
    Action( " -> %p%n", en );
    return( en );
}

void    DBAddConst( enum_list *en, const char *nm, signed_32 val ) {
//====================================================================

    const_entry *cons;

    Action( "DBAddConst( %p, %s, %l )%n", en, nm, val );
    cons = CGAlloc( sizeof( const_entry ) );
    cons->next = en->list;
    en->list = cons;
    en->num++;
    cons->name = ACopyOf( nm );
    cons->val  = val;
}
extern  dbg_type        DBEndEnum( enum_list *en ) {
//==================================================

    const_entry *cons;
    int tval;

    Action( "DBEndEnum( %p )", en );
    TypDbg( "(%d) Enumerated base==%s, &consts==%d%n", ++TypeIdx,
            Tipe( en->tipe ), en->num );
    for(;;) {
        cons = en->list;
        if( cons == NULL ) break;
        tval = cons->val;
        TypDbg( "    '%s' == %d%n", cons->name, tval );
        en->list = cons->next;
        cons->next = NULL;
        CGFree( cons->name );
        CGFree( cons );
    }
    CGFree( en );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
extern  proc_list       *DBBegProc( cg_type call_type, dbg_type ret ) {
//=====================================================================

    proc_list   *pr;

    Action( "DBBegProc" );
    Action( "( %s, %d )", Tipe( call_type ), ret );
    pr = CGAlloc( sizeof( proc_list ) );
    pr->num = 0;
    pr->list = NULL;
    pr->call = call_type;
    pr->ret = ret;
    Action( " -> %p%n", pr );
    return(pr);
}
extern  void  DBAddMethParms(  proc_list *pr,  dbg_type  cls, dbg_type this   ) {
/************************************************************************/

    Action( "DBAddMethParms" );
    Action( "(%d %d, %d )%n", pr, cls, this );
}
extern  void    DBAddParm( proc_list *pr, dbg_type tipe ) {
//=========================================================

    parm_entry  *parm;
    parm_entry  **owner;

    Action( "DBAddParm( %p, %d )", pr, tipe );
    parm = CGAlloc( sizeof( parm_entry ) );
    pr->num++;
    owner = &pr->list;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    *owner = parm;
    parm->tipe = tipe;
    parm->next = NULL;
}
extern  dbg_type        DBEndProc( proc_list *pr ) {
//==================================================

    parm_entry  *parm;

    Action( "DBEndProc( %p )", pr );
    TypDbg( "(%d) Procedure %s, return==%d, &parms==%d%n", ++TypeIdx,
            Tipe(pr->call), pr->ret, pr->num );
    for(;;) {
        parm = pr->list;
        if( parm == NULL ) break;
        TypDbg( "    type == %d%n", parm->tipe );
        pr->list = parm->next;
        parm->next = NULL;
        CGFree( parm );
    }
    CGFree( pr );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}

extern  void            DBGenStMem(cg_sym_handle sym,dbg_loc loc) {
//=================================================================

    Action( "DBGenStMem" );
    Action( "( %s, %s )%n", FEName((pointer)sym), Location( loc ) );
}

extern  void            DBGenSym(cg_sym_handle sym,dbg_loc loc,int scope) {
//=========================================================================

    Action( "DBGenSym" );
    Action( "( %s )%n", FEName((pointer)sym) );
    if( (FEAttr((pointer)sym) & FE_IMPORT) == 0 ) {
        /* Should remember and print FEDbgType(sym) later! */
        SymDbg( "GenSym %s'%s' ==> %s%n",
                scope ? "(scoped) " : "", FEName((pointer)sym),
                Location( loc ) );
    }
}
extern  dbg_type        DBBasedPtr(cg_type ptr_tipe ,dbg_type base,dbg_loc loc) {
//===============================================================================

    Action( "DBBasedPtr" );
    Action( "( %s, %d )", Tipe( ptr_tipe ), base );
    TypDbg( "(%d) BPointer <%s>, base==%d%n seg_loc=%s", ++TypeIdx, Tipe(ptr_tipe),
            base, Location( loc ) );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}

extern  dbg_type        DBBasedPtrK(cg_type ptr_tipe ,dbg_type base,
                                  cg_sym_handle sym,  int kind ){
//==================================================================

    Action( "DBBasedPtrK" );
    Action( "( %s, %d %s %d)", Tipe( ptr_tipe ), base,
                          kind, FEName((pointer)sym) );
    Action( " -> %d%n", TypeIdx );
    return( TypeIdx );
}
