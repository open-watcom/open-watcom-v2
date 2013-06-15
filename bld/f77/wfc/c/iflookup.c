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
* Description:  look up an intrinsic function
*
****************************************************************************/


#include "ftnstd.h"
#include "iflookup.h"
#include "errcod.h"
#include "global.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"

#include <string.h>

extern  int             KwLookUp(char **,int,char *,int,bool);
extern  void            MarkIFUsed(IFF);
extern  sym_id          STSearch(char *,int,sym_id);
extern  sym_id          STNameSearch(char *,int);
extern  sym_id          STAdd(char *,int);

typedef struct iff_data {
    IFF     next;
    TYPE    ret_typ;
    TYPE    arg_typ;
    byte    flags;
} iff_data;

#define ONE_OR_TWO      0x00
#define ONE_ARG         0x01
#define TWO_ARG         0x02
#define TWO_OR_MORE     0x03
#define IF_COUNT_MASK   0x03

#define IF_EXTENSION    0x04
#define IF_IN_LINE      0x08
#define IF_ARG_OK       0x10
#define IF_NOT_ARG      0x00
#define IF_GENERIC      0x20
#define IF_NOT_GEN      0x00

#ifdef pick
#undef pick
#endif
#define pick(id,text,next,res,arg,flags) text,

static char __FAR *IFNames[] = {
#include "ifdefn.h"
    NULL
};

#undef pick
#define pick( id, text, next, res, arg, flags) {next, res, arg, flags},

static iff_data __FAR IFFlags[] = {
#include "ifdefn.h"
};

int     IFIndex( char *name ) {
//=============================

    return( KwLookUp( IFNames, IF_MAX_NAME, name, strlen( name ), TRUE ) );
}


IFF     IFLookUp( void ) {
//========================

    return( KwLookUp( IFNames, IF_MAX_NAME, CITNode->opnd, CITNode->opnd_size, TRUE ) );
}


static  sym_id  IFSymLookup( char *name, uint len ) {
//===================================================

    sym_id      sym;

    sym = STNameSearch( name, len );
    if( sym != NULL ) {
        if( ((sym->ns.flags & SY_CLASS) == SY_SUBPROGRAM) &&
               (sym->ns.flags & SY_INTRINSIC) ) {
            return( sym );
        }
    }
    sym = STSearch( name, len, IFList );
    if( sym == NULL ) {
        sym = STAdd( name, len );
        sym->ns.u3.address = NULL;
        sym->ns.link = IFList;
        IFList = sym;
    }
    return( sym );
}


bool    IFIsGeneric( IFF func ) {
//================================

    return( ( IFFlags[ func ].flags & IF_GENERIC ) != 0 );
}


bool    IFIsMagic( IFF func ) {
//==============================

    return( IFFlags[ func ].next == MAGIC );
}


TYPE    IFType( IFF func ) {
//===========================

    return( IFFlags[ func ].ret_typ );
}


IFF     IFSpecific( TYPE typ ) {
//==============================

    IFF         func;
    IFF         magic;
    sym_id      sym;

    magic = 0;
    func = CITNode->sym_ptr->ns.si.fi.index;
    if( IFFlags[ func ].next == MAGIC ) {
        magic = MAGIC;
    } else if( IFFlags[ func ].flags & IF_GENERIC ) {
        for( ; IFFlags[ func ].arg_typ != typ; ) {
            func = IFFlags[ func ].next;
            if( func == IF_NO_MORE ) {
                TypeErr( LI_NO_SPECIFIC, typ );
                return( magic );
            }
        }
        sym = IFSymLookup( IFNames[ func ], strlen( IFNames[ func ] ) );
        typ = IFFlags[ func ].ret_typ;
        // merge flags - don't assign them from CITNode->sym_ptr->ns.flags
        // since SY_IF_ARGUMENT may be set in sym->flags
        // Consider:        DOUBLE PRECISION X
        //                  INTRINSIC DSIN
        //                  CALL F( DSIN )
        //                  PRINT *, SIN( X )
        // when we process SIN( X ), the specific function DSIN already
        // has SY_IF_ARGUMENT set
        sym->ns.flags |= CITNode->sym_ptr->ns.flags | SY_REFERENCED;
        sym->ns.u1.s.typ = typ;
        sym->ns.xt.size = TypeSize( typ );
        sym->ns.si.fi.index = func;
        CITNode->sym_ptr = sym;
        if( IFFlags[ func ].flags & IF_IN_LINE ) {
            magic = MAGIC;
        } else {
            MarkIFUsed( func );
        }
    } else if( IFFlags[ func ].flags & IF_IN_LINE ) {
        magic = MAGIC;
    } else {
        MarkIFUsed( func );
    }
    typ = IFFlags[ func ].ret_typ;
    CITNode->typ = typ;
    CITNode->size = TypeSize( typ );
    return( magic );
}


bool    IsIFMax( IFF func ) {
//==========================

// Any changes here should be made in InlineCnvt in upscan.c

    return( (func == IF_AMAX0) || (func == IF_AMAX1) || (func == IF_DMAX1) ||
            (func == IF_QMAX1) || (func == IF_I1MAX0) || (func == IF_I2MAX0) ||
            ( func == IF_MAX0 ) || ( func == IF_MAX1 ) );
}


bool    IsIFMin( IFF func ) {
//==========================

// Any changes here should be made in InlineCnvt in upscan.c

    return( (func == IF_AMIN0) || (func == IF_AMIN1) || (func == IF_DMIN1) ||
            (func == IF_QMIN1) || (func == IF_I1MIN0) || (func == IF_I2MIN0) ||
            (func == IF_MIN0) || (func == IF_MIN1) );
}


bool    IsIntrinsic( unsigned_16 flags ) {
//========================================

    return( ( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) &&
            ( ( flags & SY_INTRINSIC ) != 0 ) );
}


bool    IFAsArg( IFF func ) {
//==============================

    return( IFFlags[ func ].flags & IF_ARG_OK );
}

bool    IFVarArgs( IFF func ) {
//==============================

    return( ( IFFlags[ func ].flags & IF_COUNT_MASK ) == TWO_OR_MORE );
}


void    IFChkExtension( IFF func ) {
//==================================

    if( IFFlags[ func ].flags & IF_EXTENSION ) {
        Extension( LI_IF_NOT_STANDARD, IFNames[ func ] );
    }
}


void    IFCntPrms( IFF func, int actual_cnt ) {
//===============================================

    int         need;

    need = IFFlags[ func ].flags & IF_COUNT_MASK;
    if( need == TWO_OR_MORE ) {
        if( actual_cnt >= 2 ) return;
    } else if( need == ONE_OR_TWO ) {
        if( actual_cnt <= 2 ) return;
    } else {
        if( need == actual_cnt ) return;
    }
    Error( AR_BAD_COUNT, IFNames[ func ] );
}


bool    IFGenInLine( IFF func ) {
//================================

    return( ( IFFlags[ func ].flags & IF_IN_LINE ) != 0 );
}


TYPE    IFArgType( IFF func ) {
//==============================

    return( IFFlags[ func ].arg_typ );
}

char    *IFName( IFF func ) {
//==============================

    return( IFNames[ func ] );
}
