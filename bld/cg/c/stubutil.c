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


// (jww) 93/12/13 -- force recompile

#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <process.h>
#include "standard.h"
#include "cg.h"
#include "bckdef.h"
#include "cgdefs.h"
#include "typclass.h"
#include "typedef.h"
#include "cfloat.h"
#include "cgaux.h"
#include "model.h"
#include "hostsys.h"
#include "cgstub.h"
#include "feprotos.h"

#include "stubdata.h"

extern  pointer         CGAlloc(int );

extern  type_def        *TypeAddress(cg_type);
extern  void            DumpTree(pointer);
extern  type_def        *TypeDef(cg_type,type_length);
extern  uint            Length(char*);
extern  void            TypeFini();
extern  char            *CopyStr(char*,char*);
extern  void            TypeInit();
extern  type_def        *TypeAlias(cg_type,cg_type);
extern  byte            *Copy(void*,void*,uint);
extern  void            CGFree(pointer);
extern  void            BECloseFiles();
extern  void            exit(int);
extern  void            Action(char * str, ... );
extern  pointer         SafeRecurse(pointer(*)(),pointer);

extern  char    *EnvVar( char *env ) {
//====================================

    return( getenv( env ) );
}


extern  int     FStdOut() {
//=========================

    return( 1 );
}


extern  int     FCreate( char *file ) {
//=====================================

    return( creat( file, 0 ) );
}

extern  void    FShut( int io ) {
//================================

    close( io );
}


extern  void    FPut( int io, char *str, int len ) {
//==================================================

    write( io, str, len );
}

extern  char    *DoIToHS( char * buff, int buff_len, int i ) {
//=============================================================

    char        *hexdigits;
    int         length;

    buff_len = buff_len;
    length = 2*sizeof( int );
    buff += length + 3;
    *buff = NULLCHAR;
    hexdigits = "0123456789abcdef";
    while( --length >= 0 ) {
        *--buff = hexdigits[ i & 0x0f ];
        i = (unsigned)i >> 4;
    }
    *--buff = 'x';
    *--buff = '0';
    return( buff );
}

extern  char    *DoIToS( char * buff, int buff_len, signed_32 i ) {
//=================================================================

    char        *p;
    bool        neg;

    p = buff + buff_len;
    *p = NULLCHAR;
    if( i < 0 ) {
        neg = TRUE;
        i = -i;
    } else {
        neg = FALSE;
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

extern  void    PutFmt( int out, char * str, va_list list ) {

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
                str2 = DoIToS( buff, 79, va_arg( list, int ) );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
            case 'l':
                str2 = DoIToS( buff, 79, va_arg( list, signed_32 ) );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
#ifndef __386__
            case 'p':
                {
                    unsigned            offset, segment;

                    offset = va_arg( list, unsigned );
                    segment = va_arg( list, unsigned );
                    str2 = DoIToHS( buff, 79, segment );
                    while( *str2 ) FPut( out, str2++, 1 );
                    FPut( out, ":", 1 );
                    str2 = DoIToHS( buff, 79, offset );
                    while( *str2 ) FPut( out, str2++, 1 );
                    break;
                }
#else
            case 'p':
#endif
            case 'h':
                str2 = DoIToHS( buff, 79, va_arg( list, int ) );
                while( *str2 ) {
                    FPut( out, str2++, 1 );
                }
                break;
            case 'c':
                c = va_arg( list, char );
                FPut( out, &c, 1 );
                break;
            case 's':
                str2 = va_arg( list, char * );
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

                    nd = va_arg( list, n * );
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


extern  void    Code(char * str, ... ) {

    va_list     list;
    int         old;

    va_start( list, str );
    old = SetFile( CodeSeg );
    PutFmt( Out, str, list );
    SetFile( old );
    va_end( list );
}

extern  void    Put(char * str, ... ) {

    va_list     list;

    va_start( list, str );
    PutFmt( Out, str, list );
    va_end( list );
}

extern  void    Action(char * str, ... ) {

    va_list     list;

    va_start( list, str );
    PutFmt( Actions, str, list );
    va_end( list );
}

extern  void    TypDbg(char * str, ... ) {

    va_list     list;

    va_start( list, str );
    PutFmt( TypDebug, str, list );
    va_end( list );
}

extern  void    SymDbg(char * str, ... ) {

    va_list     list;

    va_start( list, str );
    PutFmt( SymDebug, str, list );
    va_end( list );
}

extern  void    PutError( int out, char * str, va_list list ) {

    FPut( out, "\nError! ", 8 );
    PutFmt( out, str, list );
    FPut( out, "\n", 1 );
}
extern  void    CGError(char * str, ... ) {

    va_list     list;
    int         old;

    old = SetFile( CodeSeg );

    va_start( list, str );
    PutError( 2,       str, list );
    va_end( list );
    va_start( list, str );
    PutError( Actions, str, list );
    va_end( list );
    va_start( list, str );
    PutError( Out,     str, list );
    va_end( list );

    BECloseFiles();
    SetFile( old );
    exit( 2010 );
}

extern  char    *Name( pointer sym ) {
//====================================

    static char buff[256];
    char        hexbuf[20];
    char        *end,*hex;

    end = CopyStr( "[", CopyStr( FEName( sym ), buff ) );
    #ifdef __386__
        hex = DoIToHS( hexbuf, 20, (int)sym );
        end = CopyStr( "]", CopyStr( hex, end ) );
    #else
        hex = DoIToHS( hexbuf, 20, (unsigned long)sym >> 16 );
        end = CopyStr( ":", CopyStr( hex, end ) );
        hex = DoIToHS( hexbuf, 20, (unsigned long)sym & 0xFFFF );
        end = CopyStr( "]", CopyStr( hex, end ) );
    #endif
    return( buff );
}

extern  char    *FtnTipe( dbg_ftn_type tipe ) {
//=============================================

    switch( tipe ) {
    case 0x47: return( "T_DBG_COMPLEX" );
    case 0x4f: return( "T_DBG_DCOMPLEX" );
    case 0x53: return( "T_DBG_NEAR_SCB" );
    case 0x55: return( "T_DBG_FAR_SCB" );
    default:   CGError( "Undefined FORTRAN debug type %d", tipe );
    }
    return( NULL );
}

extern  char    *LToS( signed_32 i ) {
//====================================

    return( DoIToS( UBuff, UBUFF_LEN, i ) );
}

extern  char    *Tipe( cg_type tipe ) {
//=====================================

    char        *res;
    type_def    *t;

    if( tipe >= T_FIRST_FREE ) {
        VerTipe( tipe, NULL );
        t = TypeAddress( tipe );
        res = LToS( t->refno );
        *--res = '_';
        *--res = 'T';
    } else {
        res = Tipes[ tipe ];
    }
    return( res );
}
extern  char    *Label( l *lb ) {
//===============================

    char *res;

    VerLabel( lb );
    res = LToS( lb->i );
    *--res = 'L';
    return( res );
}
extern  void    VDefLabel( l *lb ) {
//==================================

    if( ( lb->cref && !lb->cdef ) || ( lb->dref && !( lb->ddef||lb->idef) ) ) {
        CGError( "Label %s used but not defined\n", Label(lb) );
    }
}
extern  void    CRefLabel( l *lb ) {
//=================================

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
extern  void    CDefLabel( l *lb ) {
//=================================

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
extern  void    DRefLabel( l *lb ) {
//=================================

    if( lb->cdef || lb->cref ) {
        CGError( "Using label %s in both code and data\n", Label(lb) );
    }
    lb->dref = 1;
}
extern  void    DDefLabel( l *lb ) {
//=================================

    if( lb->cdef || lb->cref ) {
        CGError( "Using label %s in both code and data\n", Label(lb) );
    }
    if( lb->ddef ) {
        CGError( "Defining label %s twice\n", Label(lb) );
    }
    lb->ddef = 1;
}
extern  void    DDefILabel( l *lb ) {
//=================================

    if( lb->cdef || lb->cref ) {
        CGError( "Using label %s in both code and data\n", Label(lb) );
    }
    lb->idef = 1;
}
extern  char    *Op( cg_op op ) {
//===============================

    char        *res;
    if( op >= STUB_MAX_OP ) {
        CGError( "Illegal operator" );
    } else {
        res = Ops[ op ];
    }
    return( res );
}
extern  n       *NewNode( nclass c, cg_type t ) {
//===============================================

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
    nd->burnt = FALSE;
    NodeList = nd;
    return( nd );
}

#define FE_TYPE( x )    ( ( (x) > T_FIRST_FREE ) && ( (x) < T_LAST_FREE ) )
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
        if( o == *l++ ) return( TRUE );
    }
    return( FALSE );
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
        while( *l != T_DEFAULT ) {
            if( t == *l++ ) return;
        }
        CGError( "Illegal type for given routine %s", Tipe(t) );
    }
}
extern  void    Find( char *msg, pointer *list, pointer nd ) {
//============================================================

    while( list != NULL ) {
        if( list == nd ) return;
        list = *list;
    }
    CGError( "Unknown %s %p", msg, nd );
}
extern  char    *ACopyOf( char *s ) {
//===================================

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

    if( NodeList != NULL ) {
        Code( "Dangling trees ========================%n" );
        while( NodeList != NULL ) {
            Code( "%t %p\n", NodeList, NodeList );
            NodeFree( NodeList );
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
        if( p->r == r ) return( p );
        p = p->r;
    }
//  return( NULL );
}
extern void DumpCallTree( n *t ) {
//================================

    n   *parm;
    n   *junk;

    DumpSubTree( t->l );
    Code( "(" );
    if( t->r == NULL ) return;
    parm = t->r;
    for( ;; ) {
        DumpSubTree( parm->l );
        junk = parm;
        parm = parm->r;
        NodeFree( junk );
        if( parm == NULL ) break;
        Code( "," );
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

    if( !( *(call_class*)FEAuxInfo( t->h, CALL_CLASS ) & MAKE_CALL_INLINE ) ) {
        return( FALSE );
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
    parm = t->r;
    while( parm != NULL ) {
        iparm = CGAlloc( sizeof( ip ) );
        iparm->t = parm->t;
        iparm->n = NULL;
        *padd = iparm;
        padd = &iparm->n;
        icall->c++;
        parm = parm->r;
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
    return TRUE;
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
    o = &NodeList;
    while( *o != nd ) {
        o = (n**)*o;
    }
    *o = nd->n;
    CGFree( nd );
}
extern  segment_id      SetFile( segment_id seg ) {
//=================================================

    segment_id  old;
    int         i;

    old = CurSeg;
    CurSeg = seg;
    if( seg > MAX_SEG || seg < MIN_SEG || SegOk[ seg ] == FALSE ) {
        CGError( "BESetSeg - bad segment (%d)", seg );
    } else {
        if( Files[ seg ].hdl == 0 ) {
            if( FilesOpen > 10 ) {
                for( i = 0; Files[ i ].hdl == 0; ++i );
                FShut( Files[ i ].hdl );
                --FilesOpen;
            }
            if( Files[ seg ].exists ) {
                Files[ seg ].hdl = open( Files[ seg ].name, O_RDWR );
                lseek( Files[ seg ].hdl, 0, SEEK_END );
                ++FilesOpen;
            } else {
                Files[ seg ].hdl = FCreate( Files[ seg ].name );
                if( Files[ seg ].hdl != -1 ) {
                    Files[ seg ].exists = TRUE;
                    ++FilesOpen;
                } else {
                    Files[ seg ].hdl = 0;
                }
            }
        }
        Out = Files[ seg ].hdl;
    }
    return( old );
}
extern  void    NotDefault( cg_type  t ) {
//========================================

    if( t == T_DEFAULT ) {
        CGError( "T_DEFAULT not allowed as type to routine" );
    }
}

extern  void    CFCnvFS( cfloat *f ) {
//====================================

    int         len;
    char        *buffer;

    buffer = UBuff;
    len = f->len - 1;
    if( f->sign == -1 ) {
        *buffer++ = '-';
    }
    *buffer++ = f->mant[0];
    *buffer++ = '.';
    Copy( &f->mant[1], buffer, len );
    buffer += len;
    *buffer++ = 'E';
    len = f->exp - 1;
    buffer[ 2 ] = len % 10;
    len /= 10;
    buffer[ 1 ] = len % 10;
    len /= 10;
    buffer[ 0 ] = len % 10;
}

pointer SafeRecurse( pointer (* rtn)(), pointer arg ) {
/*****************************************************/

    return( rtn( arg ) );
}

