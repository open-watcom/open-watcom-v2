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


#include <stdarg.h>
#include "cgstd.h"
#include "cg.h"
#include "bckdef.h"
#include "cgdefs.h"
#include "typclass.h"
#include "typedef.h"
#include "types.h"
#include "cfloat.h"
#include "cgaux.h"
#include "model.h"
#include "cgstub.h"
#include "feprotos.h"
#include "cgmem.h"

#include "stubdata.h"

extern  int             TempId;

extern  void            DumpTree(n *);
extern  char            *CopyStr(char*,char*);

extern  char            *ACopyOf(char *);
extern  void            VerTipe(cg_type ,cg_type *);
extern  n               *Binary(cg_op ,n *,n *,cg_type );
extern  char            *Tipe(cg_type );
extern  void            VerLabel(l *);
extern  char            *Label(l *);
extern  void            Action(char *,... );
extern  void            NoDanglers(void);
extern  n               *NewNode(nclass ,cg_type );
extern  void            Code(char *,... );
extern  void            VerAuto(a *);
extern  void            VerNode(n *);
extern  char            *Name(pointer );
extern  char            *Op(cg_op );
extern  void            NotDefault(cg_type );
extern  void            CGError(char *,... );
extern  void            VerBack(b *);
extern  char            *LToS(signed_32 );
extern  void            DumpT(n *);
extern  void            VerOp(cg_op ,cg_op *);
extern  void            Find(char *,pointer *,pointer );
extern  n               *Unary(cg_op ,n *,cg_type );
extern  void            CRefLabel(l *lb);
extern  void            CDefLabel(l *lb);
extern  void            DRefLabel(l *lb);
extern  void            DDefLabel(l *lb);
extern  pointer         LkAddBack(sym_handle,pointer);
extern  unsigned_32     BETypeLength( cg_type );

/********************************************************************/
/* Handle feedback for auto locations                               */
/********************************************************************/


typedef struct auto_locn AUTO_LOCN;
struct auto_locn {
    AUTO_LOCN* next;
    sym s;
    unsigned offset;
};

#define AUTO_PACK 4

static AUTO_LOCN*auto_locations;
static unsigned next_auto_offset;

static void addAutoLocn( sym s, cg_type type )
{
    AUTO_LOCN* curr;

    curr = CGAlloc( sizeof( AUTO_LOCN ) );
    curr->s = s;
    curr->offset = next_auto_offset;
    curr->next = auto_locations;
    auto_locations = curr;
    next_auto_offset += ( BETypeLength( type ) + AUTO_PACK - 1 )
                      & ( - AUTO_PACK );
}


static void dumpAutoLocn( void )
{
    AUTO_LOCN* curr;

    if( auto_locations ) {
        for( ; ; ) {
            curr = auto_locations;
            if( ! curr ) break;
            auto_locations = curr->next;
            if( ((pointer)TEMP_LOC_YES) == FEAuxInfo( curr->s, TEMP_LOC_NAME ) ) {
                Action( "TEMP_LOC_TELL offset=%h symbol=%s%n"
                      , curr->offset
                      , Name( curr->s ) );
                FEAuxInfo( (sym_handle)curr->offset, TEMP_LOC_TELL );
            }
            CGFree( curr );
        }
        Action( "%n" );
    }
}

#undef AUTO_PACK


/********************************************************************/
/* end of auto locations                                            */
/********************************************************************/


/* verification */
extern  void    CGDone( n *nd ) {
/*******************************/

//  CGError( "OOPS!" );
    Action( "CGDone( %t )%n%n", nd );
    VerNode( nd );
    DumpTree( nd );
    NoDanglers();
}

extern  void    CGTrash( n *nd ) {
/********************************/

    Action( "CGTrash( %t )%n%n", nd );
    VerNode( nd );
    DumpT( nd );
}

static  int     EvalNo = { 0 };
extern  n       *CGEval( n *nd ) {
/********************************/

    n           *new;
    char        buff[80];
    char        *endptr;

    Action( "CGEval( %t )", nd );
    VerNode( nd );
    endptr = CopyStr( "[eval", buff );
    endptr = CopyStr( LToS( ++EvalNo ), endptr );
    endptr = CopyStr( "]", endptr );
    new = NewNode( LEAF, nd->t );
    new->l = (n *)ACopyOf( buff );
    DumpTree( nd );
    Code( "evaluate ===> %s%n", new->l );
    Action( " -> %t%n%n", new );
    return( new );
}

extern  n       *CGVolatile( n *nd ) {
/************************************/

    Action( "CGVolatile( %t ) -> %t%n", nd, nd );
    VerNode( nd );
    return( nd );
}

extern  n       *CGAttr( n *nd, cg_sym_attr attr ) {
/**************************************************/

    Action( "CGAttr( %t, %l ) -> %t%n", nd, attr, nd );
    VerNode( nd );
    // should check the attr?
    return( nd );
}

static  int     DupNo = { 0 };
static  n       *CGDuplicateArray[ 2 ];
extern  n       **CGDuplicate( n *nd ) {
/************************************/

    n           *new1;
    n           *new2;
    char        buff[80];
    char        *endptr;

    Action( "CGDuplicate( %t )", nd );
    VerNode( nd );
    endptr = CopyStr( "[dup", buff );
    endptr = CopyStr( LToS( ++DupNo ), endptr );
    endptr = CopyStr( "]", endptr );
    new1 = NewNode( LEAF, nd->t );
    new1->l = (n *)ACopyOf( buff );
    endptr = CopyStr( "[dup", buff );
    endptr = CopyStr( LToS( ++DupNo ), endptr );
    endptr = CopyStr( "]", endptr );
    new2 = NewNode( LEAF, nd->t );
    new2->l = (n *)ACopyOf( buff );
    DumpT( nd );
    Code( "duplicate ===> %s %s%n", new1->l, new2->l );
    CGDuplicateArray[ 0 ] = new1;
    CGDuplicateArray[ 1 ] = new2;
    Action( " -> %t %t%n", new1, new2 );
    return( CGDuplicateArray );
}

static int inlineTypeEquivalent( cg_type t1, cg_type t2 )
{
    return TypeAddress( t1 )->refno == TypeAddress( t2 )->refno;
}

extern  void    CGProcDecl( sym s, cg_type t ) {
/**********************************************/

    b   *bk;
    Action( "%n==================================================%n" );
    Action( "CGProcDecl" );
    Action( "( %s, %s )", Name(s), Tipe(t) );
    Attrs(s);
    CClass(s);
    bk = FEBack(s);
    VerBack(bk);
    Code( "Routine %s%n", Name(s) );
    if( Inlines ) {
#if 0
        if( Inlines->t != t ) {
#else
        if( ! inlineTypeEquivalent( Inlines->t, t ) ) {
#endif
            CGError( "Declared type of inline '%s' does not match", Name(s) );
        }
        if( Inlines->h != s ) {
            char name[80];
            CopyStr( Name(Inlines->h), name );
            CGError( "Expecting inline for '%s', got '%s'", name, Name(s) );
        }
    } else {
        DDefLabel( bk->lp );
        InProc = s;
    }
}

extern  void    CGParmDecl(  sym s,  cg_type  t ) {
/*************************************************/

    ip  *iparm;
    int i;

    addAutoLocn( s, t );
    Action( "CGParmDecl" );
    Action( "( %s, %s )", Name(s), Tipe(t) );
    Attrs(s);
    Code( "Parm %s%n", Name(s) );
    if( Inlines ) {
        Inlines->d++;
        if( Inlines->d > Inlines->c ) {
            CGError( "Too many parameters for inline '%s'", Name(Inlines->h) );
        }
        iparm = Inlines->p;
        for( i = 1; i < Inlines->d; ++i ) {
            iparm = iparm->n;
        }
#if 0
        if( iparm->t != t ) {
#else
        if( ! inlineTypeEquivalent( iparm->t, t ) ) {
#endif
            CGError( "Type of parm %d to inline '%s' does not match",
                      Inlines->d, Name(Inlines->h) );
        }
    }
}

extern  void    CGAutoDecl( sym s, cg_type t ) {
/**********************************************/

    addAutoLocn( s, t );
    Action( "CGAutoDecl" );
    Action( "( %s, %s )", Name(s), Tipe(t) );
    Attrs(s);
    Code( "Auto %s%n", Name(s) );
}

extern  label_handle CGLastParm() {
/*********************************/

    Action( "CGLastParm()%n" );
    return( NULL );
}

extern  void    Attrs( sym s ) {
/******************************/

    fe_attr     a;
    Action( "\n----" );
    a = FEAttr( s );
    if( a & FE_PROC ) Action( " FE_PROC" );
    if( a & FE_STATIC ) Action( " FE_STATIC" );
    if( a & FE_GLOBAL ) Action( " FE_GLOBAL" );
    if( a & FE_IMPORT ) Action( " FE_IMPORT" );
    if( a & FE_CONSTANT ) Action( " FE_CONSTANT" );
    if( a & FE_MEMORY ) Action( " FE_MEMORY" );
    if( a & FE_VISIBLE ) Action( " FE_VISIBLE" );
    if( a & FE_NOALIAS ) Action( " FE_NOALIAS" );
    if( a & FE_UNIQUE ) Action( " FE_UNIQUE" );
    if( a & FE_COMMON ) Action( " FE_COMMON" );
    if( a & FE_ADDR_TAKEN ) Action( " FE_ADDR_TAKEN" );
    if( a & FE_VOLATILE ) Action( " FE_VOLATILE" );
    Action( " seg id %d%n", FESegID( s ) );
}

extern  void    DumpCClass( call_class c ) {
/******************************************/

#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
    if( c & FAR_CALL ) Action( "FAR " );
    if( c & ROUTINE_RETURN ) Action( "ROUTINE_RETURN " );
    if( c & SPECIAL_RETURN ) Action( "SPECIAL_RETURN " );
#endif
    if( c & CALLER_POPS ) Action( "CALLER_POPS " );
    if( c & NO_MEMORY_CHANGED ) Action( "NO_MEMORY_CHANGED " );
    if( c & NO_MEMORY_READ ) Action( "NO_MEMORY_READ " );
    if( c & SUICIDAL ) Action( "SUICIDAL " );
}

extern  void    CClass( sym  s ) {
/********************************/

    call_class  *pc;

    pc = FEAuxInfo( FEAuxInfo( s, AUX_LOOKUP ), CALL_CLASS );
    DumpCClass( *pc );
    Action( "%n" );
}

extern  void    CG3WayControl( n *e, l *lt, l *eq, l *gt ) {
/**********************************************************/

    Action( "CG3WayControl" );
    Action( "( %s, ", Label(lt) );
    Action( "%s, ", Label( eq ) );
    Action( "%s )%n", Label( gt ) );
    Code( "IF_ARITH %s", Label( lt ) );
    Code( " %s", Label( eq ) );
    Code( " %s ", Label( gt ) );
    CRefLabel( lt );
    CRefLabel( eq );
    CRefLabel( gt );
    VerNode( e );
    DumpTree( e );
    NoDanglers();
}
extern  void    CGControl( cg_op o, n *e, l  *lb ) {
/**************************************************/

    Action( "CGControl" );
    VerCGCtrl( TRUE, o, e, lb );
}
extern  void    VerCGCtrl( bool ver, cg_op o, n *e, l *lb ) {
/***********************************************************/

    if( ver ) {
        VerOp( o, ControlOps );
    }
    if( o == O_IF_TRUE || o == O_IF_FALSE ) {
        VerNode( e );
    } else if( e != NULL ) {
        CGError( "3rd Parm to CGControl must be NULL if 2nd parm !== O_IF_ ... " );
    }
    if( o != O_LABEL_RETURN ) {
        Action( "( %s, %t, %s )%n", Op(o), e, Label(lb) );
        Code( "%s %s  ", Op(o), Label(lb) );
    } else {
        Action( "( %s, %t, NULL )%n", Op(o), e );
        Code( "%s%n", Op(o) );
    }
    if( e != NULL ) {
        DumpTree( e );
    } else {
        Code( "%n" );
    }
    switch( o ) {
    case O_GOTO:
    case O_IF_TRUE:
    case O_IF_FALSE:
    case O_INVOKE_LABEL:
        CRefLabel( lb );
        break;
    case O_LABEL:
        CDefLabel( lb );
        break;
    }
    Action( "%n" );
    NoDanglers();
}
extern  n       *CGCompare( cg_op o, n *l, n *r, cg_type t ) {
/************************************************************/

    n   *new;

    Action( "CGCompare" );
    VerNode(l);
    VerNode(r);
    VerOp(o,CompareOps);
    Action( "( %s, %t, %t, %s )", Op(o), l, r, Tipe( t ) );
    new = Binary(o,l,r,T_BOOLEAN);
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGFlow( cg_op o, n *l, n *r ) {
/**********************************************/

    n   *new;

    Action( "CGFlow" );
    VerNode(l);
    if( o != O_FLOW_NOT ) {
        VerNode(r);
    } else if( r != NULL ) {
        CGError( "3rd Parm to CGFlow( O_FLOW_NOT, ... must be null" );
    }
    VerOp(o,FlowOps);
    Action( "( %s, %t, %t )", Op(o), l, r );
    new = Binary(o,l,r,T_BOOLEAN);
    Action( " -> %t%n", new );
    return( new );
}
extern  sh      *CGSelInit() {
/****************************/

    sh  *s;

    Action( "CGSelInit()" );
    s=CGAlloc(sizeof(sh));
    s->o=NULL;
    s->r=NULL;
    s->i=++SelId;
    Action( " -> %d%n", SelId );
    return(s);
}
extern  void    CGSelCase( sh *s, l *lb, signed_32 v ) {
/******************************************************/

    Action( "CGSelCase" );
    Action( "( %d, %l, %s )%n", s->i, v, Label( lb ) );
    CRefLabel( lb );
    SelRange(s,v,v,lb);
}
extern  void    CGSelRange( sh *s, signed_32 lo, signed_32 hi, l *lb ) {
/**********************************************************************/

    Action( "CGSelRange" );
    Action( "( %d, %l, %l, %s )%n", s->i, lo, hi, Label( lb ) );
    CRefLabel( lb );
    SelRange(s,lo,hi,lb);
}
extern  void    SelRange( sh *s, signed_32 lo, signed_32 hi, l *lb ) {
/********************************************************************/

    rh  **or;
    rh  *n;

    VerLabel(lb);
    CRefLabel(lb);
    if(lo>hi) { CGError( "lo bound > hi bound" ); }
    or = &s->r;
    for(;;) {
        if( *or==NULL) break;
        if(hi< (*or)->l) break;
        if(lo<= (*or)->h) { CGError( "case range overlap" ); }
        or = &(*or)->n;
    }
    n=CGAlloc(sizeof( rh ));
    n->l=lo;
    n->h=hi;
    n->lb=lb;
    n->n= *or;
    *or=n;
}
extern  void    CGSelOther( sh *s, l *lb ) {
/******************************************/

    Action( "CGSelOther" );
    Action( "( %d, %s )%n", s->i, Label( lb ) );
    CRefLabel(lb);
    if(s->o!=NULL) { CGError( "Otherwise already defined" ); }
    s->o=lb;
}
extern  void    CGSelect( sh *s, n *e ) {
/***************************************/

    rh  *r;

    Action( "CGSelect" );
    VerNode(e);
    Action( "( %d, %t )%n", s->i, e );
    Code( "select: " );
    DumpTree(e);
    NoDanglers();
    while( s->r != NULL ) {
        r = s->r;
        Code( "range: %s,", LToS(r->l) );
        Code( "%s", LToS( r->h ) );
        Code( " %s%n", Label(r->lb) );
        s->r=r->n;
        CGFree(r);
    }
    if(s->o!=NULL) { Code( "default: %s%n", Label(s->o) ); }
    Code("}%n");
    CGFree(s);
}
extern  n       *CGInteger( signed_32 i, cg_type t ) {
/****************************************************/

    n   *in;

    Action( "CGInteger" );
    Action( "( %l, %s )", i, Tipe( t ) );
    in = NewNode( LEAF, t );
    in->l = (n *)ACopyOf( LToS(i) );
    VerTipe(t,CGIntTypes);
    Action( " -> %t%n", in );
    return( in );
}
extern  n       *CGCallback( pointer func, pointer parm ) {
/*********************************************************/
    n   *nd;
    call_back* cb;

    Action( "CGCallback" );
    nd = NewNode( CALLBACK, TY_DEFAULT );
    cb = CGAlloc( sizeof( call_back ) );
    nd->l = (n*)cb;
    cb->function = func;
    cb->data = parm;
    Action( "( %p, %p ) -> %t%n", func, parm, nd );
    return( nd );
}
extern  n       *CGPatchNode( pointer p, cg_type t ) {
/****************************************************/
    n   *l;

    Action( "CGPatchNode" );
    VerTipe( t, CGIntTypes );
    l = NewNode( LEAF, t );
    l->l = (n *)ACopyOf( LToS( -1 ) );
    Action( "( %p, %s ) -> %t%n", p, Tipe( t ), l );
    return( l );
}
extern  n       *CGFloat(char *s,cg_type t) {
/*******************************************/

    n   *l;

    Action( "CGFloat" );
    VerTipe( t,FloatTypes );
    l = NewNode( LEAF, t );
    l->l = (n*)ACopyOf( s );
    Action( "( %s, %s ) -> %t%n", s, Tipe(t), l );
    return( l );
}
static  cg_type PtrTipe( pointer s ) {
/************************************/

    fe_attr     a;

    if( s != NULL ) {
        a = FEAttr( s );
    } else {
        a = 0;
    }
    if( a & FE_PROC ) {
        return( TY_CODE_PTR );
    } else {
        return( TY_POINTER );
    }
}
extern  n       *CGFEName( pointer sym, cg_type t ) {
/*************************************************/

    n   *nd;
    a   *au;
    s   *st;
    b   *bk;

    Action( "CGFEName" );
    nd = NewNode( LEAF,PtrTipe( sym ) );
    nd->l = (n*)ACopyOf( FEName( sym ) );
    nd->src = sym;
//  Action( "( %s, %s ) -> %t", nd->l, Tipe(t), nd );
    Action( "( %s, %s ) -> %t", Name( sym ), Tipe(t), nd );
    Attrs( sym );
    if( FEAttr( sym ) & FE_STATIC ) {
        bk = FEBack( sym );
        VerBack( bk );
        if( !( FEAttr( sym ) & FE_IMPORT ) ) DRefLabel( bk->lp );
        for( st = StaticList; st; st = st->n ) {
            if( st->s == sym ) break;
        }
        if( st == NULL ) {
            st = CGAlloc( sizeof( s ) );
            st->n = StaticList;
            st->s = sym;
            st->a = FEAttr( sym );
            StaticList = st;
        }
    } else if( FELexLevel( sym ) != 0 ) {
        CGError( "Lex level for %s(%p) must be 0", FEName( sym ), sym );
    } else {
        au = LkAddBack( sym, NULL );
        if( au != NULL ) {
            VerAuto( au );
            if( au->s != sym ) {
                CGError( "Wrong auto back handle for %s(%p)",
                         FEName( sym ), sym );
            }
            if( au->o ) {
                CGError( "Outdated auto back handle for %s(%p)",
                FEName( sym ), sym );
            }
        } else {
            Action( "WARNING: Allocating back handle for lookup of %s(%p)%n",
                 FEName( sym ), sym );
            au = CGAlloc( sizeof( a ) );
            au->n = AutoList;
            au->s = sym;
            au->o = 0;
            AutoList = au;
            LkAddBack( sym, au );
        }
    }
    return( nd );
}
extern  n       *CGBackName( b *s, cg_type t ) {
/***************************************************/

    n   *nd;

    Action( "CGBackName" );
    nd = NewNode(LEAF,PtrTipe( NULL ));
    VerBack( s );
    DRefLabel( s->lp );
    CopyStr( Label( s->lp ), UBuff );
    nd->l = (n*)ACopyOf( UBuff );
    VerTipe(t,NULL);
    Action( "( %s, %s ) -> %t%n", nd->l, Tipe(t), nd );
    return( nd );
}
extern  t       *CGTemp( cg_type ty ) {
/*************************************/

    t   *tm;
    ty=ty;
    Action( "CGTemp" );
    tm = CGAlloc( sizeof( t ) );
    tm->n = TempList;
    tm->i = ++TempId;
    Action( "( %s ) ->%d%n", Tipe( ty ), tm->i );
    TempList = tm;
    return(tm);
}
extern  n       *CGTempName( t *tm, cg_type ty ) {
/************************************************/

    n   *nd;
    char *c;

    Action( "CGTempName" );
    Find( "back end temp", (pointer *)TempList, tm );
    nd = NewNode(LEAF,ty);
    c = LToS( tm->i );
    *--c = 'T';
    nd->l = (n*)ACopyOf( c );
    Action( "( %d, %s ) ->%t%n", tm->i, Tipe( ty ), nd );
    return(nd);
}
extern  n       *CGBitMask( n *r, byte st, byte ln, cg_type t ) {
/***************************************************************/

    n   *nd;

    Action( "CGBitMask" );
    Action( "( %t, %d, %d, %s )", r, st, ln, Tipe( t ) );
    NotDefault( t );
    Action( "%s", Tipe(t) );
    nd = Unary( OP_BIT_FIELD, r, t );
    nd->st = st;
    nd->ln = ln;
    Action( " -> %t%n", nd );
    return( nd );
}
extern  n       *DoCGAssign( n *l, n *r, cg_type t, int i ) {
/**************************************************/

    n   *new;

    NotDefault( t );
    VerNode( l ); VerNode( r );
    Action( "( %t, %t, %s )", l, r, Tipe(t) );
    new = Binary( i,l,r,t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGAssign( n *l, n *r, cg_type t ) {
/****************************************************/

    Action( "CGAssign" );
    return( DoCGAssign( l, r, t, O_GETS ) );
}
extern  n       *CGLVAssign( n *l, n *r, cg_type t ) {
/****************************************************/

    Action( "CGLVAssign" );
    return( DoCGAssign( l, r, t, O_LV_GETS ) );
}
extern  n       *DoCGPreGets( cg_op o, n *l, n *r, cg_type t, int i ) {
/************************************************************/

    n   *new;

    NotDefault( t );
    VerNode( l ); VerNode( r ); VerOp( o, BinaryOps );
    Action( "( %s, %t, %t, %s )", Op(o), l, r, Tipe(t) );
    new = Unary( i, Binary(o,l,r,t),t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGPreGets( cg_op o, n *l, n *r, cg_type t ) {
/***************************************************************/

    Action( "CGPreGets" );
    return( DoCGPreGets( o, l, r, t, O_PRE_GETS ) );
}
extern  n       *CGLVPreGets( cg_op o, n *l, n *r, cg_type t ) {
/**************************************************************/

    Action( "CGLVPreGets" );
    return( DoCGPreGets( o, l, r, t, O_LV_PRE_GETS ) );
}
extern  n       *CGPostGets( cg_op o, n *l, n *r, cg_type t ) {
/*************************************************************/

    n   *new;

    Action( "CGPostGets" );
    NotDefault( t );
    VerNode( l ); VerNode( r ); VerOp( o, BinaryOps );
    Action( "( %s, %t, %t, %s )", Op(o), l, r, Tipe(t) );
    new = Unary( O_POST_GETS,Binary(o,l,r,t),t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGUnary( cg_op o, n *r, cg_type t ) {
/****************************************************/

    n   *new;

    Action( "CGUnary" );
    VerNode( r ); VerOp( o, UnaryOps );
    Action( "( %s, %t, %s )", Op(o), r, Tipe(t) );
    if( o == O_POINTS ) { NotDefault( t ); }
    new = Unary( o, r, t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGBinary( cg_op o, n *l, n *r, cg_type t ) {
/***********************************************************/

    n   *new;

    Action( "CGBinary" );
    VerNode( r ); VerNode( l ); VerOp( o, BinaryOps );
    Action( "( %s, %t, %t, %s )", Op(o), l, r, Tipe(t) );
    new = Binary( o,l,r,t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGIndex( n *l, n *r, cg_type t, cg_type s ) {
/*************************************************/

    n   *new;

    s=s;
    Action( "CGIndex" );
    VerNode( l ); VerNode( r );
    Action( "( %t, %t, %s )", l, r, Tipe(t) );
    new = Binary( O_PLUS,l,r,t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGInitCall( n *l, cg_type t, sym_handle h ) {
/************************************************************/

    n   *new;

    Action( "CGInitCall" );
    VerNode(l);
//  Action( "( %t, %s )", l, Tipe(t) );
    Action( "( %t, %s, %s )", l, Tipe(t), FEName(h) );
    new = Binary(O_CALL,l,NULL,t);
    new->h = h;
    Action( " -> %t%n", new );
    return( new );
}
extern  void    CGAddParm( n *l, n *p, cg_type t ) {
/**************************************************/

    Action( "CGAddParm" );
    VerNode( l );
    VerNode( p );
    Action( "( %t, %t, %s )%n", l, p, Tipe(t) );
    l->r=Binary(O_PARM,p,l->r,t);
}
extern  n       *CGCall( n *r ) {
/*******************************/

    call_class  *pc;

    Action( "CGCall( %t )", r );
    pc = (call_class *)FEAuxInfo( r->h, CALL_CLASS );
    if( *pc & MAKE_CALL_INLINE ) {
        Action( " inline handle=%p%n", r );
    } else {
        DumpCClass( *pc );
        Action( "%n" );
    }
    VerNode( r );
    if( r->r != NULL ) r->r->i++;
    return( r );
}
extern  void    CGReturn( n *r, cg_type t ) {
/*******************************************/

    s   *st;
    ic  *icall;
    ip  *iparm;
    ip  *junk;

    Action("CGReturn");
    Action( "( %t, %s )%n%n", r, Tipe( t ) );
    VerTipe( t,NULL );
    if( r != NULL ) {
        VerNode( r );
        DumpTree( Unary( O_RETURN,r,t ) );
    }
    NoDanglers();
    if( Inlines ) {
#if 0
        if( Inlines->t != t ) {
#else
        if( ! inlineTypeEquivalent( Inlines->t, t ) ) {
#endif
            CGError( "Return type of inline '%s' does not match",
                      Name(Inlines->h) );
        }
        iparm = Inlines->p;
        while( iparm ) {
            junk = iparm;
            iparm = iparm->n;
            CGFree(junk);
        }
        icall = Inlines;
        Inlines = Inlines->n;
        CGFree(icall);
    } else {
        InProc = NULL;
        dumpAutoLocn();
        for( st = StaticList; st; st = st->n ) {
            if( st->a != FEAttr( st->s ) ) {
                CGError( "Symbol %p freed too soon", st->s );
            }
        }
    }
}
extern  n       *CGChoose( n *s, n *l, n *r, cg_type t ) {
/********************************************************/

    n   *new;

    Action( "CGChoose" );
    VerNode( s ); VerNode( r ); VerNode( l );
    Action( "( %t, %t, %t, %s )", s, l, r, Tipe(t) );
    new = Binary( OP_CHOOSE,s,Binary( OP_JOIN,l,r,t ),t );
    Action( " -> %t%n", new );
    return( new );
}
extern  n       *CGWarp( n *b4, l *lb, n *af ) {
/**********************************************/

    n   *r;

    Action( "CGWarp" );
    if( b4 != NULL ) {
        VerNode( b4 );
    }
    VerNode( af );
    Action( "( %t, %s, %t )", b4, Label(lb), af );
    CRefLabel(lb);
    r = Binary( OP_WARP,Binary( OP_JOIN,NULL,b4,T_DEFAULT ),af,af->t );
    r->l->l = (n*)lb;
    Action( " -> %t%n", r );
    return( r );
}
extern  cg_type CGType( n *nd ) {
/*******************************/

    Action( "CGType( %t )", nd );
    VerNode( nd );
    VerTipe( nd->t,NULL );
    Action( " -> %d%n", TypeAddress( nd->t )->refno );
    return( TypeAddress( nd->t )->refno );
}
extern  void    CGBigGoto( l *lb, int ll ) {
/*******************************************/

    Action( "CGBigGoto( %d )", ll );
    VerCGCtrl( FALSE, O_BIG_GOTO, NULL, lb );
}
extern  void    CGBigLabel( b *bk ) {
/************************************/

    Action( "CGBigLabel" );
    VerCGCtrl( FALSE, O_BIG_LABEL, NULL, bk->lp );
}
