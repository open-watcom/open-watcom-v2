/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include <stdarg.h>
#include <stdlib.h>
#include "wio.h"
#include "_cg.h"
#include "bckdef.h"
#include "typclass.h"
#include "types.h"
#include "cfloat.h"
#include "cgaux.h"
#include "model.h"
#include "cgstub.h"
#include "cgmem.h"
#include "utils.h"
#include "stackcg.h"
#include "stubdata.h"
#include "dumptree.h"
#include "feprotos.h"


extern  void            BECloseFiles();
extern  void            Action(char * str, ... );


char    *EnvVar( char *env )
//==========================
{
    return( getenv( env ) );
}


int     FStdOut( void )
//=====================
{
    return( 1 );
}


int     FCreate( char *file )
//===========================
{
    return( creat( file, 0 ) );
}

void    FShut( int io )
//=====================
{
    close( io );
}


void    FPut( int io, char *str, int len )
//========================================
{
    write( io, str, len );
}

char    *DoIToHS( char * buff, int buff_len, int i )
//==================================================
{
    char        *hexdigits;
    int         length;

    buff_len = buff_len;
    length = 2*sizeof( int );
    buff += length + 3;
    *buff = NULLCHAR;
    hexdigits = "0123456789abcdef";
    while( --length >= 0 ) {
        *--buff = hexdigits[i & 0x0f];
        i = (unsigned)i >> 4;
    }
    *--buff = 'x';
    *--buff = '0';
    return( buff );
}

char    *DoIToS( char * buff, int buff_len, signed_32 i )
//=======================================================
{
    char        *p;
    bool        neg;

    p = buff + buff_len;
    *p = NULLCHAR;
    if( i < 0 ) {
        neg = true;
        i = -i;
    } else {
        neg = false;
    }
    while( i != 0 ) {
        *--p = i % 10 + '0';
        i /= 10;
    }
    if( neg ) {
        *--p = '-';
    } else {
        *--p = '0';
    }
    return( p );
}

void    PutFmt( int out, const char *str, va_list args )
{
    char        *str2;
    char        buff[80];
    char        c;

    while( *str ) {
        if( *str == '%' ) {
            ++str;
            switch( *str ) {
            case '%':
                FPut( out, str, 1 );
                break;
            case 'd':
                str2 = DoIToS( buff, 79, va_arg( args, int ) );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
            case 'l':
                str2 = DoIToS( buff, 79, va_arg( args, signed_32 ) );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
            case 'p':
            case 'h':
                str2 = DoIToHS( buff, 79, va_arg( args, int ) );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
            case 'c':
                c = va_arg( args, char );
                FPut( out, &c, 1 );
                break;
            case 's':
                str2 = va_arg( args, char * );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
            case 'n':
                FPut( out, "\n", 1 );
                break;
            case 't':
                {
                    n   *nd;

                    nd = va_arg( args, n * );
                    if( nd != NULL ) {
                        VerNode( nd );
                    }
                    str2 = nd != NULL ? DoIToS( buff, 79, nd->id ) : "NULL";
                    FPut( out, "c", 1 );
                    FPut( out, "g", 1 );
                    FPut( out, "(", 1 );
                    while( *str2 ) {
                        FPut( out, str2++, 1 );
                    }
                    FPut( out, ")", 1 );
                    break;
                }
            default:
                FPut( out, "ZOIKS", 5 );
            }
            ++str;
        } else {
            FPut( out, str++, 1 );
        }
    }
}


void    Code(char * str, ... )
{
    va_list     args;
    int         old;

    va_start( args, str );
    old = SetFile( CodeSeg );
    PutFmt( Out, str, args );
    SetFile( old );
    va_end( args );
}

void    Put(char * str, ... )
{
    va_list     args;

    va_start( args, str );
    PutFmt( Out, str, args );
    va_end( args );
}

void    Action(char * str, ... )
{
    va_list     args;

    va_start( args, str );
    PutFmt( Actions, str, args );
    va_end( args );
}

void    TypDbg(char * str, ... )
{
    va_list     args;

    va_start( args, str );
    PutFmt( TypDebug, str, args );
    va_end( args );
}

void    SymDbg(char * str, ... )
{
    va_list     args;

    va_start( args, str );
    PutFmt( SymDebug, str, args );
    va_end( args );
}

void    PutError( int out, const char *str, va_list args )
{
    FPut( out, "\nError! ", 8 );
    PutFmt( out, str, args );
    FPut( out, "\n", 1 );
}

void    CGError(const char *str, ... )
{
    va_list     args;
    int         old;

    old = SetFile( CodeSeg );

    va_start( args, str );
    PutError( 2,       str, args );
    va_end( args );
    va_start( args, str );
    PutError( Actions, str, args );
    va_end( args );
    va_start( args, str );
    PutError( Out,     str, args );
    va_end( args );

    BECloseFiles();
    SetFile( old );
    exit( 2010 );
}

char    *Name( pointer sym )
//==========================
{
    static char buff[256];
    char        hexbuf[20];
    char        *end,*hex;

    end = CopyStr( "[", CopyStr( FEName( sym ), buff ) );
    hex = DoIToHS( hexbuf, 20, (int)sym );
    end = CopyStr( "]", CopyStr( hex, end ) );
    return( buff );
}

char    *FtnTipe( dbg_ftn_type tipe )
//===================================
{
    switch( tipe ) {
    case T_DBG_COMPLEX: return( "T_DBG_COMPLEX" );
    case T_DBG_DCOMPLEX: return( "T_DBG_DCOMPLEX" );
    default:   CGError( "Undefined FORTRAN debug type %d", tipe );
    }
    return( NULL );
}

char    *LToS( signed_32 i )
//==========================
{
    return( DoIToS( UBuff, UBUFF_LEN, i ) );
}

char    *Tipe( cg_type tipe )
//===========================
{
    char        *res;
    type_def    *t;

    if( tipe >= TY_FIRST_FREE ) {
        VerTipe( tipe, NULL );
        t = TypeAddress( tipe );
        res = LToS( t->refno );
        *--res = '_';
        *--res = 'Y';
        *--res = 'T';
    } else {
        res = Tipes[tipe];
    }
    return( res );
}

char    *Label( l *lb )
//=====================
{
    char *res;

    VerLabel( lb );
    res = LToS( lb->i );
    *--res = 'L';
    return( res );
}

void    VDefLabel( l *lb )
//========================
{
    if( ( lb->cref && !lb->cdef ) || ( lb->dref && !( lb->ddef||lb->idef) ) ) {
        CGError( "Label %s used but not defined\n", Label(lb) );
    }
}

void    CRefLabel( l *lb )
//========================
{
    if( lb->cdef != NULL && lb->cdef != InProc ) {
        CGError( "Referencing label %s outside its procedure\n", Label(lb) );
    }
    if( lb->cref != NULL && lb->cref != InProc ) {
        CGError( "Referencing label %s outside its procedure\n", Label(lb) );
    }
    if( lb->ddef || lb->dref ) {
        CGError( "Illegal control flow reference to code label %s\n", Label(lb) );
    }
    lb->cref = InProc;
}

void    CDefLabel( l *lb )
//========================
{
    if( lb->cref != NULL && lb->cref != InProc ) {
        CGError( "Referencing label %s outside its procedure\n", Label(lb) );
    }
    if( lb->cdef != NULL || lb->ddef ) {
        CGError( "Defining label %s twice\n", Label(lb) );
    }
    if( lb->dref ) {
        CGError( "Defining label %s in both code and data\n", Label(lb) );
    }
    lb->cdef = InProc;
}

void    DRefLabel( l *lb )
//========================
{
    if( lb->cdef || lb->cref ) {
        CGError( "Using label %s in both code and data\n", Label(lb) );
    }
    lb->dref = 1;
}

void    DDefLabel( l *lb )
//========================
{
    if( lb->cdef || lb->cref ) {
        CGError( "Using label %s in both code and data\n", Label(lb) );
    }
    if( lb->ddef ) {
        CGError( "Defining label %s twice\n", Label(lb) );
    }
    lb->ddef = 1;
}

void    DDefILabel( l *lb )
//=========================
{
    if( lb->cdef || lb->cref ) {
        CGError( "Using label %s in both code and data\n", Label(lb) );
    }
    lb->idef = 1;
}

char    *Op( cg_op op )
//=====================
{
    char        *res;
    if( op >= STUB_MAX_OP ) {
        CGError( "Illegal operator" );
    } else {
        res = Ops[op];
    }
    return( res );
}

n       *NewNode( nclass c, cg_type t )
//=====================================
{
    n   *nd;
    nd = CGAlloc( sizeof( n ) );
    nd->c = c;
    nd->o = O_NOP;
    nd->l = NULL;
    nd->r = NULL;
    nd->n = NodeList;
    nd->i = 0;
    nd->t = t;
    nd->id = ++NodeId;
    nd->src = NULL;
    nd->burnt = false;
    NodeList = nd;
    return( nd );
}

#define FE_TYPE( x )    ( ( (x) >= TY_FIRST_FREE ) && ( (x) <= TY_LAST_FREE ) )
extern  n       *Binary( cg_op op, n *l, n *r, cg_type t ) {
//==========================================================

    n   *nd;

    Use( l );
    Use( r );
    if( op == O_CONVERT ) {
        if( FE_TYPE( l->t ) || FE_TYPE( r->t ) ) {
            Action( "***ILLEGAL CONVERSION***" );
        }
    }
    nd = NewNode( OP, t );
    nd->l = l;
    nd->r = r;
    nd->o = op;
    return( nd );
}
extern  n       *Unary( cg_op op, n *r, cg_type t ) {
//===================================================

    n   *nd;

    Use( r );
    nd = NewNode( OP, t );
    nd->r = r;
    nd->o = op;
    return( nd );
}
extern  void    Use( n *nd ) {
//============================

    if( nd != NULL ) {
        VerNode( nd );
        nd->i++;
        if( nd->i != 1 ) {
            Code( "Used twice ====================================%n" );
            DumpSubTree( nd );
            CGError( "A cg-name has been reused%n" );
        }
    }
}
extern  void    VerNode( n *nd ) {
//================================

    Find( "node", (pointer *)NodeList, nd );
}
extern  void    VerAuto( a *au ) {
//================================

    Find( "auto variable", (pointer *)AutoList, au );
}
extern  void    VerLabel( l *lb ) {
//=================================

    Find( "label", (pointer *)LblList, lb );
    if( lb->i == -1 ) {
        CGError( "Unknown label%n" );
    }
}
extern  void    VerBack( b *bk ) {
//================================

    Find( "back handle",(pointer *)BackList, bk );
}
extern  bool    LkUpOp( cg_op o, cg_op *l ) {
//==========================================

    while( *l != O_NOP ) {
        if( o == *l++ ) {
            return( true );
        }
    }
    return( false );
}
extern  void    VerOp( cg_op o, cg_op *l ) {
//==========================================

    Op(o);
    if( l != NULL ) {
        if( !LkUpOp( o, l ) ) {
            CGError( "Illegal operator for given CG routine %s", Op(o) );
        }
    }
}
extern  void    VerTipe( cg_type t, cg_type *l ) {
//================================================

    type_def    *a;

    a = TypeAddress( t );
    if( a == NULL ) {
        CGError( "Undefined type %d", t );
    }
    t = a->refno;
    if( l != NULL ) {
        while( *l != TY_DEFAULT ) {
            if( t == *l++ ) {
                return;
            }
        }
        CGError( "Illegal type for given routine %s", Tipe(t) );
    }
}
extern  void    Find( char *msg, pointer *list, pointer nd ) {
//============================================================

    for( ; list != NULL; list = *list ) {
        if( list == nd ) {
            return;
        }
    }
    CGError( "Unknown %s %p", msg, nd );
}
extern  char    *ACopyOf( const char *s ) {
//=========================================

    char        *c;
    c = CGAlloc( Length( s ) + 1 );
    CopyStr( s, c );
    return( c );
}
extern  void    DumpTree( n *t ) {
//================================

    NodeId = 0;
    DumpT( t );
}
extern  void    NoDanglers() {
//============================

    n *next;

    if( NodeList != NULL ) {
        Code( "Dangling trees ========================%n" );
        while( NodeList != NULL ) {
            next = NodeList->n;
            Code( "%t %p\n", NodeList, NodeList );
            NodeFree( NodeList );
            NodeList = next;
        }
        CGError( "Hanging on to cgnames too long!" );
    }
}
extern  void    DumpT( n *t ) {
//=============================

    Use( t );
    DumpSubTree( t );
    Code( "%n" );
}
extern n *FindParm( n *p, n *r ) {
//================================

    for( ;; ) {
        if( p->r == r )
            return( p );
        p = p->r;
    }
//  return( NULL );
}
extern void DumpCallTree( n *t ) {
//================================

    n   *parm;
    n   *next;
    n   *first;

    DumpSubTree( t->l );
    Code( "(" );
    first = t->r;
    for( parm = t->r; parm != NULL; parm = next ) {
        next = parm->r;
        if( parm != first ) {
            Code( "," );
        }
        DumpSubTree( parm->l );
        NodeFree( parm );
    }
    Code( ")" );
}
extern  bool    CheckInLine( n * t ) {
//====================================

    n   *nlist;
    ic  *icall;
    ip  *iparm;
    ip  **padd;
    n   *parm;
    b   *bk;

    if( (*(call_class*)FEAuxInfo( t->h, CALL_CLASS ) & MAKE_CALL_INLINE) == 0 ) {
        return( false );
    }
    icall = CGAlloc( sizeof( ic ) );
    icall->t = t->t;
    icall->p = NULL;
    icall->n = Inlines;
    icall->d = 0;
    icall->c = 0;
    icall->h = t->h;
    bk = FEBack( t->h );
    VerBack( bk );
    DDefILabel( bk->lp );
    Inlines = icall;
    padd = &icall->p;
    for( parm = t->r; parm != NULL; parm = parm->r ) {
        iparm = CGAlloc( sizeof( ip ) );
        iparm->t = parm->t;
        iparm->n = NULL;
        *padd = iparm;
        padd = &iparm->n;
        icall->c++;
    }
    DumpCallTree( t );
    nlist = NodeList;
    NodeList = NULL;
    Code( "%n======== Inline code for %s starts%n", FEName( t->h ) );
    Action( "%nFEGenProc( sym=%s, handle=%p )%n", FEName( t->h ), t );
    Action( "======== Inline code for %s starts%n", FEName( t->h ) );
    FEGenProc( t->h, t );
    Code( "%n======== Inline code for %s ends%n", FEName( t->h ) );
    Action( "%n======== Inline code for %s ends%n", FEName( t->h ) );
    NodeList = nlist;
    return( true );
}
extern  void    DumpSubTree( n *t ) {
//===================================

    if( t == NULL ) {
        ;
    } else {
        VerNode( t );
        if( t->c == LEAF ) {
            Code( "%s", t->l );
            if( NULL != t->src ) {
                Code( "[%p]", t->src );
            }
        } else if( t->c == CALLBACK ) {
            call_back* cb = (call_back*)t->l;
            Code( "%s", "<CallBack>" );
            t->l = NULL;
            Action( "CALLBACK( %p, %p )%n"
                  , cb->function
                  , cb->data );
            (cb->function)( cb->data );
            Action( "== completed CALLBACK( %p, %p )%n"
                  , cb->function
                  , cb->data );
            CGFree( cb );
        } else if( t->o == OP_BIT_FIELD ) {
            Code( "{%d:%d}", t->st, t->ln );
            DumpSubTree( t->r );
        } else if( t->o == OP_CHOOSE ) {
            DumpSubTree( t->l );
            Code( "?" );
            DumpSubTree( t->r->l );
            Code( ":" );
            DumpSubTree( t->r->r );
            NodeFree( t->r );
        } else if( t->o == OP_WARP ) {
            Code( "__Warp[ (" );
            if( t->l->r != NULL ) {
                DumpSubTree( t->l->r );
            } else {
                Code( "\\" );
            }
            Code( ") ,%s, (", Label((struct l*)t->l->l) );
            DumpSubTree( t->r );
            Code( ") ]" );
            NodeFree( t->l );
        } else if( t->o == O_GETS || t->o == O_LV_GETS ) {
            if( t->o == O_GETS ) Code( "@" );
            Code( "(" );
            DumpSubTree( t->l );
            Code( "=" );
            DumpSubTree( t->r );
            Code( ")" );
        } else if( t->o == O_POST_GETS ) {
            Code( "(" );
            DumpSubTree( t->r->l );
            Code( "," );
            DumpSubTree( t->r->r );
            Code( ")" );
            Code( "%s%s", Op( t->r->o ), Op( t->r->o ) );
            NodeFree( t->r );
        } else if( t->o == O_PRE_GETS || t->o == O_LV_PRE_GETS ) {
            if( t->o == O_PRE_GETS ) Code( "@" );
            Code( "(" );
            DumpSubTree( t->r->l );
            Code( "%s", Op( t->r->o ) );
            Code( "=" );
            DumpSubTree( t->r->r );
            Code( ")" );
            NodeFree( t->r );
        } else if( t->o == O_CALL ) {
            if( !CheckInLine( t ) ) DumpCallTree( t );
        } else {
            if( LkUpOp( t->o, FunkyOps ) ) {
                Code( "%s(", Op( t->o ) );
                if( t->l != NULL ) {
                    DumpSubTree( t->l );
                    Code( "," );
                }
                DumpSubTree( t->r );
                Code( ")" );
            } else {
                if( t->l != NULL && t->r != NULL ) {
                    Code( "(" );
                }
                DumpSubTree( t->l );
                Code( "%s", Op( t->o ) );
                DumpSubTree( t->r );
                if( t->l != NULL && t->r != NULL ) {
                    Code( ")" );
                }
            }
        }
        if( t->i == 1 ) {
            NodeFree( t );
        }
    }
}
extern  void    NodeFree( n *nd ) {
//=================================

    n   **o;
    for( o = &NodeList; *o != nd; ) {
        o = (n**)*o;
    }
    *o = nd->n;
    CGFree( nd );
}
extern segment_id   SetFile( segment_id segid ) {
//=================================================

    segment_id  old_segid;
    int         i;

    old_segid = CurSeg;
    CurSeg = segid;
    if( segid > MAX_SEG || segid < MIN_SEG || !SegOk[segid] ) {
        CGError( "BESetSeg - bad segment (%d)", segid );
    } else {
        if( Files[segid].hdl == 0 ) {
            if( FilesOpen > 10 ) {
                for( i = 0; Files[i].hdl == 0; ++i );
                FShut( Files[i].hdl );
                --FilesOpen;
            }
            if( Files[segid].exists ) {
                Files[segid].hdl = open( Files[segid].name, O_RDWR );
                lseek( Files[segid].hdl, 0, SEEK_END );
                ++FilesOpen;
            } else {
                Files[segid].hdl = FCreate( Files[segid].name );
                if( Files[segid].hdl != -1 ) {
                    Files[segid].exists = true;
                    ++FilesOpen;
                } else {
                    Files[segid].hdl = 0;
                }
            }
        }
        Out = Files[segid].hdl;
    }
    return( old_segid );
}
extern  void    NotDefault( cg_type  t ) {
//========================================

    if( t == TY_DEFAULT ) {
        CGError( "TY_DEFAULT not allowed as type to routine" );
    }
}

extern  char   *CFCnvFS( float_handle f, char *buffer, int maxlen ) {
//====================================

    int         len;

    len = f->len - 1;
    if( len + 10 > maxlen ) {
        len = maxlen - 10;
    }
    if( f->sign == -1 ) {
        *buffer++ = '-';
    }
    *buffer++ = f->mant[0];
    *buffer++ = '.';
    Copy( &f->mant[1], buffer, len );
    buffer += len;
    *buffer++ = 'E';
    len = f->exp - 1;
    if( len < 0 ) {
        *buffer++ = '-';
        len = -len;
    }
    buffer[2] = len % 10 + '0';
    len /= 10;
    buffer[1] = len % 10 + '0';
    len /= 10;
    buffer[0] = len + '0';
    buffer += 3;
    return( buffer );
}

void *SafeRecurseCG( func_sr rtn, void *arg ) {
/*********************************************/

    return( rtn( arg ) );
}

