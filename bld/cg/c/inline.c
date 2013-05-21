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
#include "cgdefs.h"
#include "coderep.h"
#include "addrname.h"
#include "cgmem.h"
#include "zoiks.h"
#include "feprotos.h"
#include "makeins.h"

extern  name            *GenIns(an);
extern  name            *SAllocUserTemp(pointer,type_class_def,type_length);
extern  void            EnLink(label_handle,bool);
extern  void            AddIns(instruction*);
extern  type_class_def  TypeClass(type_def*);
extern  void            AddTarget(label_handle,bool);
extern  void            GenBlock( block_class, int );
extern  an              MakeTempAddr(name*,type_def*);
extern  name            *BGNewTemp(type_def*);
extern  void            BGDone(an);
extern  an              BGCopy(an);
extern  an              BGAssign(an,an,type_def*);

extern  type_def        *TypeInteger;
extern  bool            HaveCurrBlock;

typedef struct inline_parm {
        struct inline_parm      *next;
        an                      addr;
} inline_parm;

typedef struct inline_stack {
        struct inline_stack     *next;
        struct inline_parm      *parms;
        sym_handle              proc_sym;
        type_def                *tipe;
        an                      addr;
} inline_stack;

static inline_stack     *InlineStack = NULL; // fix this!

extern  void    BGStartInline( sym_handle proc_sym ) {
/****************************************************/

    inline_stack        *stk;

    stk = CGAlloc( sizeof( *stk ) );
    stk->parms = NULL;
    stk->tipe = NULL;
    stk->addr = NULL;
    stk->proc_sym = proc_sym;
    stk->next = InlineStack;
    InlineStack = stk;
}


extern  void    BGAddInlineParm( an addr ) {
/******************************************/

    inline_parm *parm;

    parm = CGAlloc( sizeof( *parm ) );
    parm->addr = addr;
    parm->next = InlineStack->parms;
    InlineStack->parms = parm;
}


extern  an      BGStopInline( cg_name handle, type_def *tipe ) {
/*********************************************************/

    // works if we're in the middle of a conditional???

    label_handle        lbl;
    inline_stack        *junk;
    an                  retv;

    if( HaveCurrBlock == FALSE ) {
        EnLink( AskForNewLabel(), TRUE );
        HaveCurrBlock = TRUE;
    }
    lbl = AskForNewLabel();
    GenBlock( JUMP, 1 );
    AddTarget( lbl, FALSE );
    EnLink( lbl, TRUE );
    InlineStack->tipe = tipe;
    FEGenProc( InlineStack->proc_sym, handle );
    retv = InlineStack->addr;
    junk = InlineStack;
    InlineStack = InlineStack->next;
    CGFree( junk );
    return( retv );
}


static bool NotEquiv( type_def *a, type_def *b )
/**********************************************/
{
    if( TypeClass( a ) != TypeClass( b ) ) return( TRUE );
    if( a->length != b->length ) return( TRUE );
    return( FALSE );
}


extern  void    BGProcInline( sym_handle proc_sym, type_def *tipe ) {
/*******************************************************************/

    if( InlineStack->proc_sym != proc_sym || NotEquiv( InlineStack->tipe, tipe ) ) {
        _Zoiks( ZOIKS_072 );
    }
}


extern  void    BGParmInline( sym_handle sym, type_def *tipe ) {
/**************************************************************/

    name                *temp;
    name                *parm_value;
    inline_parm         *parm;

    parm = InlineStack->parms;
    InlineStack->parms = InlineStack->parms->next;
//  if( NotEquiv( parm->addr->tipe, tipe ) ) {
//      _Zoiks( ZOIKS_070 );
//  }
    temp = SAllocUserTemp( sym, TypeClass( tipe ), tipe->length );
    temp->v.usage |= USE_IN_ANOTHER_BLOCK;
    parm_value = GenIns( parm->addr );
    BGDone( parm->addr );
    AddIns( MakeMove( parm_value, temp, temp->n.name_class ) );
    CGFree( parm );
}


extern  void    BGRetInline( an addr, type_def *tipe ) {
/********************************************************/

    an  tempaddr;

    if( NotEquiv( tipe, InlineStack->tipe ) ) {
        _Zoiks( ZOIKS_070 );
    }
    if( addr == NULL ) {
        InlineStack->addr = MakeTempAddr( BGNewTemp(TypeInteger), TypeInteger );
    } else {
        tempaddr = MakeTempAddr( BGNewTemp( tipe ), tipe );
        InlineStack->addr = BGCopy( tempaddr );
        BGDone( BGAssign( tempaddr, addr, tipe ) );
    }
}


extern  bool    BGInInline() {
/****************************/

    return( InlineStack != NULL );
}

extern  int     BGInlineDepth() {
/*******************************/

    int                 depth;
    inline_stack        *curr;

    depth = 0;
    curr = InlineStack;
    while( curr != NULL ) {
        depth++;
        curr = curr->next;
    }
    return( depth );
}
