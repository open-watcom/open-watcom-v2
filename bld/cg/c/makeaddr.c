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


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "cgdefs.h"
#include "model.h"
#include "addrname.h"
#include "procdef.h"
#include "zoiks.h"
#include "freelist.h"
#include "cfloat.h"
#include "feprotos.h"
#include "types.h"
#include "addrfold.h"
#include "makeins.h"
#include "display.h"

static    pointer       *AddrNameFrl;

extern    an            AddrList;
extern    name          *Names[];
extern    proc_def      *CurrProc;
extern    block         *CurrBlock;

extern  type_class_def  TypeClass(type_def*);
extern  name            *SAllocTemp(type_class_def,type_length);
extern  name            *SAllocUserTemp(pointer,type_class_def,type_length);
extern  name            *AllocConst(pointer);
extern  void            AddIns(instruction*);
extern  name            *BGNewTemp(type_def*);
extern  name            *AllocIntConst(int);
extern  name            *AllocS32Const(signed_32);
extern  name            *SAllocMemory(pointer,type_length,cg_class,type_class_def,type_length);
extern  name            *AllocTemp(type_class_def);
extern  void            AllocALocal(name*);
extern  byte            *Copy(void*,void*,uint);
extern  void            BGDone(an);
extern  cg_type         NamePtrType( name *op );
extern  name            *AllocRegName( hw_reg_set );


static  void    CopyAddr( an src, an dst )
/****************************************/
{
    an  link;

    link = dst->link;
    Copy( src, dst, sizeof( address_name ) );
    dst->link = link;
}


extern  an      NewAddrName( void )
/*********************************/
{
    an  addr;

    addr = AllocFrl( &AddrNameFrl, sizeof( address_name ) );
    addr->link = AddrList;
    AddrList = addr;
    addr->tipe = NULL;
    addr->index = NULL;
    addr->offset = 0;
    addr->u.name = NULL;
    addr->format = NF_ADDR;
    addr->flags = 0;
    addr->base = NULL;
    addr->alignment = 0;
    return( addr );
}


extern  an      MakeTypeTempAddr( name *op, type_def *tipe )
/**********************************************************/
{
    an          addr;

    addr = NewAddrName();
    addr->tipe = tipe;
    addr->format = NF_ADDR;
    addr->u.name = op;
    addr->class = CL_ADDR_TEMP;
    return( addr );
}


extern  an      MakeTempAddr( name *op, type_def *tipe )
/******************************************************/
{
    tipe = tipe;
    return( MakeTypeTempAddr( op, TypeAddress( TY_NEAR_POINTER ) ) );
}


extern  void    InitMakeAddr( void )
/**********************************/
{
    InitFrl( &AddrNameFrl );
    AddrList = NULL;
}


extern  name    *GenIns( an addr )
/********************************/
{
    return( GetValue( addr, NULL ) );
}


extern  void    AddrFree( an node )
/*********************************/
{
    an  *owner;

    owner = &AddrList;
    for( ;; ) {
        if( *owner == node ) {
            *owner = node->link;
            break;
        }
        owner = &(*owner)->link;
    }
    if( node->format == NF_INS ) {
        FreeIns( node->u.ins );
    }
    FrlFreeSize( &AddrNameFrl, (pointer *)node, sizeof( address_name ) );
}


extern  void    InsToAddr( an addr )
/**********************************/
{
    an          new;
    instruction *ins;

    if( addr->format == NF_INS ) {
        ins = addr->u.ins;
        ins->result = BGNewTemp( addr->tipe );
        new = AddrName( ins->result, addr->tipe );
        new->flags = addr->flags;
        new->base = addr->base;
        new->alignment = addr->alignment;
        CopyAddr( new, addr );
        AddrFree( new );
    }
}


extern  void    NamesCrossBlocks( void )
/**************************************/
{
    an          addr;
    an          new;
    an          next;
    name        *temp;

    addr = AddrList;
    while( addr != NULL ) { /* Careful. The list shifts under our feet.*/
        next = addr->link;
        if( addr->flags & ADDR_OK_ACROSS_BLOCKS ) {
            addr->flags |= ADDR_CROSSED_BLOCKS;
        } else if( addr->format == NF_INS ) {
            InsToAddr( addr );
            addr->u.name->v.usage |= USE_IN_ANOTHER_BLOCK;
        } else if( addr->format != NF_CONS
             && addr->format != NF_BOOL
             && ( addr->format != NF_ADDR
                || ( addr->class != CL_ADDR_GLOBAL
                  && addr->class != CL_ADDR_TEMP ) ) ) {
            temp = GenIns( addr );
            if( temp->n.class == N_TEMP ) {
                temp->v.usage |= USE_IN_ANOTHER_BLOCK;
            } else if( temp->n.class == N_INDEXED ) {
                if( temp->i.index->n.class == N_TEMP ) {
                    temp->i.index->v.usage
                        |= USE_IN_ANOTHER_BLOCK;
                }
            }
            new = AddrName( temp, addr->tipe );
            CopyAddr( new, addr );
            AddrFree( new );
        }
        addr = next;
    }
}


extern  bool    AddrFrlFree( void )
/*********************************/
{
    return( FrlFreeAll( &AddrNameFrl, sizeof( address_name ) ) );
}


static  name    *Display( sym_handle symbol, int level )
/******************************************************/
{
    proc_def    *proc;
    name        *op;
    name        *names;

    proc = CurrProc;
    while( level != CurrProc->lex_level ) {
        CurrProc = CurrProc->next_proc;
    }
    names = Names[  N_TEMP  ];
    Names[  N_TEMP  ] = CurrProc->names[  N_TEMP  ];
    op = Names[  N_TEMP  ];
    while( op->v.symbol != symbol ) {
        op = op->n.next_name;
    }
    op->v.usage |= ( NEEDS_MEMORY | USE_MEMORY | USE_IN_ANOTHER_BLOCK);
    AllocALocal( op );
    CurrProc = proc;
    Names[  N_TEMP  ] = names;
    return( MakeDisplay( op, level ) );
}


extern  an      MakeGets( an dst, an src, type_def *tipe )
/********************************************************/
{
    name                *dst_name;
    name                *src_name;
    instruction         *ins;
    type_class_def      class;
    name                *temp;

    InsToAddr( dst );
    dst_name = Points( dst, tipe );
    ins = src->u.ins;
    if( src->format == NF_INS && CurrBlock->ins.hd.prev == ins ) {
        ins->result = dst_name;
        src->format = NF_ADDR;  /*% so instruction doesn't get freed!*/
    } else {
        src_name = GetValue( src, dst_name );
        if( src_name != dst_name ||
         (( src_name->n.class == N_MEMORY ) && ( src_name->v.usage & VAR_VOLATILE )) ) {
            class = TypeClass( tipe );
            src_name = GenIns( src );
            if( dst_name->n.class == N_INDEXED &&
             !( dst_name->i.index_flags & X_VOLATILE ) ) {
                /* don't give him back an indexed name - it extends the life of*/
                /* a pointer*/
                temp = SAllocTemp( dst_name->n.name_class, dst_name->n.size );
                AddIns( MakeMove( src_name, dst_name, class ) );
                AddIns( MakeMove( dst_name, temp, class ) );
                dst_name = temp;
            } else {
                AddIns( MakeMove( src_name, dst_name, class ) );
            }
        }
    }
    BGDone( src );
    BGDone( dst );
    return( AddrName( dst_name, tipe ) );
}


extern  an      MakeConst( pointer cf, type_def *tipe )
/*****************************************************/
{
    return( AddrName( AllocConst( cf ), tipe ) );
}


extern  an      MakePoints( an name, type_def *tipe )
/***************************************************/
{
    an  new;

    new = AddrName( Points( name, tipe ), tipe );
    BGDone( name );
    return( new );
}

extern  an      RegName( hw_reg_set reg, type_def *tipe )
/*******************************************************/
{
    an  addr;

    addr = NewAddrName();
    addr->format = NF_NAME;
    addr->tipe = tipe;
    addr->u.name = AllocRegName( reg );
    return( addr );
}

extern  an      InsName( instruction *ins, type_def *tipe )
/*********************************************************/
{
    an  addr;

    addr = NewAddrName();
    addr->u.ins = ins;
    addr->tipe = tipe;
    addr->format = NF_INS;
    return( addr );
}

extern  name    *LoadTemp( name *temp, type_class_def class )
/***********************************************************/
{
    instruction *ins;

    ins = MakeMove( temp, AllocTemp( class ), class );
    temp = ins->result;
    AddIns( ins );
    return( temp );
}


static  name    *Temporary( name *temp, type_def *tipe )
/******************************************************/
{
    if( temp->n.class != N_TEMP ) {
        temp = LoadTemp( temp, TypeClass( tipe ) /*temp->n.name_class*/ );
    }
    return( temp );
}


extern  an      AddrEval( an addr )
/*********************************/
{
    an  new;

    new = AddrName( Temporary( GenIns( addr ), addr->tipe ), addr->tipe );
    AddrFree( addr );
    return( new );
}


extern  void    MoveAddress(  an src,  an  dest )
/***********************************************/
{
    dest->format = src->format;
    dest->class = src->class;
    dest->u.name  = src->u.name;
    dest->index = src->index;
    dest->offset= src->offset;
}


extern  void    Convert( an addr, type_class_def class )
/******************************************************/
{
    instruction *ins;

    if( addr->offset != 0 ) {
        ins = MakeBinary( OP_ADD, addr->u.name,
                                AllocIntConst( addr->offset ),
                                AllocTemp( addr->u.name->n.name_class ),
                                TypeClass( addr->tipe ) );
        addr->u.name = ins->result;
        AddIns( ins );
    }
    ins = MakeUnary( OP_CONVERT, addr->u.name, AllocTemp( class ), class );
    addr->u.name = ins->result;
    addr->offset = 0;
    AddIns( ins );
}


extern  bool    PointLess( an l_addr, an r_addr )
/***********************************************/
{
    if( l_addr->class != CL_POINTER && r_addr->class != CL_POINTER )
        return( FALSE );
    if( l_addr->offset != 0 || r_addr->offset != 0 ) return( FALSE );
    return( TRUE );
}


extern  an      AddrToIns( an addr )
/**********************************/
{
    instruction *ins;
    an          new;

    if( addr->format != NF_INS ) {
        ins = MakeMove( GenIns( addr ), NULL, TypeClass( addr->tipe ) );
        new = InsName( ins, addr->tipe );
        new->flags = addr->flags;
        new->alignment = addr->alignment;
        BGDone( addr );
        AddIns( ins );
    } else {
        new = addr;
    }
    return( new );
}


extern  an      AddrDuplicate( an node )
/**************************************/
{
    an          new;
    name        *op;

    InsToAddr( node );
    op = GenIns( node );
    new = AddrName( op, node->tipe );
    CopyAddr( new, node );
    return( new );
}

extern  an      AddrCopy( an node )
/*********************************/
{
    an  new;

    InsToAddr( node );
    new = NewAddrName();
    CopyAddr( node, new );
    return( new );
}


extern  an      AddrSave( an node )
/*********************************/
{
    InsToAddr( node );
    return( node );
}


extern  void    AddrDemote( an node )
/***********************************/
{
    node->flags |= ADDR_DEMOTED;
    if( node->format == NF_INS ) {
        node->u.ins->ins_flags |= INS_DEMOTED;
    }
}


extern  name    *MaybeTemp( name *op, type_class_def kind )
/*********************************************************/
{
    if( op == NULL ) {
        op = AllocTemp( kind );
    }
    return( op );
}


extern  void    CheckPointer( an addr )
/*************************************/
{
    InsToAddr( addr );
    if( addr->format == NF_NAME
     && ( addr->tipe->attr & TYPE_POINTER ) ) {
        addr->index = Temporary( addr->u.name, addr->tipe );
        addr->u.name = NULL;
        addr->offset = 0;
        addr->class = CL_POINTER;
        addr->format = NF_ADDR;
    }
}


extern  void    FixCodePtr( an addr )
/***********************************/
{
    instruction *ins;

    if( addr->format == NF_INS ) {
        ins = addr->u.ins;
        ins->ins_flags |= INS_CODE_POINTER;
    }
}


extern  bool    NeedPtrConvert( an addr, type_def * tipe )
/********************************************************/
{
    if( addr->format != NF_ADDR ) return( TRUE );
    if( addr->class == CL_ADDR_GLOBAL || addr->class == CL_ADDR_TEMP ) {
        if( tipe->refno == TY_NEAR_POINTER ) return( FALSE );
        if( tipe->refno == TY_LONG_POINTER ) return( FALSE );
        if( tipe->refno == TY_HUGE_POINTER ) return( FALSE );
    }
    return( TRUE );
}


extern  name    *LoadAddress( name *op, name *suggest, type_def *type_ptr )
/*************************************************************************/
{
    name                *new;
    type_class_def      class;

    if( op->n.class == N_INDEXED && !HasTrueBase( op ) ) {
        if( op->i.constant != 0 ) {
            class = op->i.index->n.name_class;
            new = MaybeTemp( suggest, class );
            AddIns( MakeBinary( OP_ADD, op->i.index,
                         AllocS32Const( op->i.constant ),
                         new, class ) );
        } else {
            new = op->i.index;
        }
    } else {
        if( suggest != NULL ) {
            class = suggest->n.name_class;
        } else {
            if( type_ptr->length == WORD_SIZE ) {
                class = WD;
            } else {
                class = CP;
            }
        }
        new = MaybeTemp( suggest, class );
        AddIns( MakeUnary( OP_LA, op, new, class ) );
    }
    return( new );
}


extern  an      MakeAddrName( cg_class class, sym_handle sym, type_def *tipe )
/****************************************************************************/
{
    an          addr;
    fe_attr     attr;
    int         level;
    name        *op;

    addr = NewAddrName();
    addr->format = NF_ADDR;
    if( class != CG_FE ) {
        op = SAllocMemory( sym, 0, class, TypeClass( tipe ), tipe->length );
        addr->u.name = op;
        addr->class = CL_ADDR_GLOBAL;
    } else {
        attr = FEAttr( sym );
        level = FELexLevel( sym );
        if( attr & FE_STATIC ) {
            op = SAllocMemory( sym, 0, class, TypeClass(tipe), tipe->length );
            if( ( attr & FE_MEMORY ) != EMPTY ) {
                op->v.usage |= NEEDS_MEMORY | USE_MEMORY;
            }
            addr->u.name = op;
            addr->class = CL_ADDR_GLOBAL;
            if( attr & FE_VOLATILE ) {
                op->v.usage |= VAR_VOLATILE | NEEDS_MEMORY | USE_MEMORY;
            }
            if( attr & FE_UNALIGNED ) {
                op->v.usage |= VAR_UNALIGNED;
            }
            if( attr & FE_CONSTANT ) {
                op->v.usage |= VAR_CONSTANT;
            }
        } else if( level != CurrProc->lex_level ) {
            op = Display( sym, level );
            addr->u.name = op;
            addr->format = NF_NAME;
        } else {
            op = SAllocUserTemp( sym, TypeClass( tipe ), tipe->length );
            if( attr & FE_MEMORY ) {
                op->v.usage |= NEEDS_MEMORY | USE_MEMORY;
            }
            if( attr & FE_ADDR_TAKEN ) {
                op->v.usage |= USE_ADDRESS;
            }
            addr->u.name = op;
            addr->class = CL_ADDR_TEMP;
            op->v.usage |= USE_IN_ANOTHER_BLOCK;
            if( attr & FE_VOLATILE ) {
                op->v.usage |= VAR_VOLATILE | NEEDS_MEMORY | USE_MEMORY;
            }
        }
    }
    addr->tipe = TypeAddress( NamePtrType( op ) );
    return( addr );
}
