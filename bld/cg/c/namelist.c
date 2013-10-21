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
* Description:  Manage a list of names.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "hwreg.h"
#include "freelist.h"
#include "cfloat.h"
#include "zoiks.h"
#include "namelist.h"
#include "data.h"

extern  void            FreeTable(sym_handle);
extern  type_class_def  RegClass(hw_reg_set);
extern  type_def        *ClassType(type_class_def);
extern  pointer         LkAddBack(sym_handle,pointer);
extern  uint_8          RegTrans( hw_reg_set );

static  pointer         *FrlHead[N_INDEXED+1];
static  pointer         *ConstDefnFrl;
static  name            *NullReg;
static  name            *ConstZero;
static  name            *ConstOne;

static  int     Size[] = {
        sizeof( const_name ),
        sizeof( memory_name ),
        sizeof( temp_name ),
        sizeof( register_name ),
        sizeof( indexed_name )
};

static  type_length     OneClass[] = {
        U1,             /* U1*/
        U1,             /* I1*/
        U2,             /* U2*/
        U2,             /* I2*/
        U4,             /* U4*/
        U4,             /* I4*/
        U8,             /* U8*/
        U8,             /* U8*/
        CP,             /* CP*/
        PT,             /* PT*/
        FS,             /* FS*/
        FD,             /* FD*/
        FL,             /* FL*/
        XX };           /* XX*/

type_length     TypeClassSize[XX+1];

static  name    *AllocName( int class,
                            type_class_def type_class, type_length size ) {
/*************************************************************************/

    name        *new;

    new = AllocFrl( &FrlHead[  class  ], Size[  class  ] );
    new->n.class = class;
    new->n.next_name = Names[ class ];
    Names[ class ] = new;
    new->n.name_class = type_class;
    new->n.size = TypeClassSize[ type_class ];
    if( new->n.size == 0 ) {
        new->n.size = size;
    }
    return( new );
}

static  name    *findConst64( unsigned_32 low, unsigned_32 high, pointer cf_value ) {
/*******************************************************************************/

    name        *new_c;
    name        **last;

    last = &Names[  N_CONSTANT  ];
    new_c = Names[  N_CONSTANT  ];
    while( new_c != NULL ) {
        if( new_c->c.const_type == CONS_ABSOLUTE ) {
            if( new_c->c.int_value == low && new_c->c.int_value_2 == high ) {
                if( CFCompare( new_c->c.value, cf_value ) == 0 ) {
                    // move constant found to front of list
                    *last = new_c->n.next_name;
                    new_c->n.next_name = Names[ N_CONSTANT ];
                    Names[ N_CONSTANT ] = new_c;
                    break;
                }
            }
        }
        last = &new_c->n.next_name;
        new_c = new_c->n.next_name;
    }
    return( new_c );
}


static void ZapXX( name *xx, type_class_def class, type_length size ) {
/***********************************************************************/

    if( class != XX ) { /* if he's making a type with same size as xx */
        xx->n.name_class = class; /* zap the XX one to be a real type */
        xx->n.size = TypeClassSize[  class  ];
    } else if( size != 0 ) {
        xx->n.size = size;
    }
}


extern  name    *AllocConst( pointer value ) {
/********************************************/

    name        *new_c;
    name        **last;
    signed_32   int_value;
    int         test;
    int_value = CFCnvF32( value );
    test = CFTest( value );
    if( test == 0 && ConstZero != NULL ) {
        CFFree( value );
        return( ConstZero );
    }
    if( int_value == 1 && ConstOne != NULL ) {
        CFFree( value );
        return( ConstOne );
    }
    last = &Names[  N_CONSTANT  ];
    new_c = Names[  N_CONSTANT  ];
    while( new_c != NULL ) {
        if( new_c->c.const_type == CONS_ABSOLUTE ) {
            if( new_c->c.int_value == int_value ) {
                if( CFCompare( new_c->c.value, value ) == 0 ) {
                    CFFree( value );
                    // move constant found to front of list
                    *last = new_c->n.next_name;
                    new_c->n.next_name = Names[ N_CONSTANT ];
                    Names[ N_CONSTANT ] = new_c;
                    return( new_c );
                }
            }
        }
        last = &new_c->n.next_name;
        new_c = new_c->n.next_name;
    }
    new_c = AllocName( N_CONSTANT, XX, 0 );
    new_c->c.value = value;
    new_c->c.int_value = int_value;
    if( test < 0 ){
        new_c->c.int_value_2 = -1; //sign extend
    }else{
        new_c->c.int_value_2 = 0;
    }
    new_c->c.static_defn = NULL;
    new_c->c.const_type = CONS_ABSOLUTE;
    if( ConstOne == NULL && new_c->c.int_value == 1 ) {
        ConstOne = new_c;
    }
    if( ConstZero == NULL && new_c->c.int_value == 0 &&  test == 0 ) {
        ConstZero = new_c;
    }
    if( int_value == 0 ) {
        if( CFIsI64( value ) || CFIsU64( value ) ) {
            unsigned_64         i64val;

            i64val = CFCnvF64( value );
            new_c->c.int_value   = i64val.u._32[ I64LO32 ];
            new_c->c.int_value_2 = i64val.u._32[ I64HI32 ];
        }
    }
    return( new_c );
}

extern  name    *AllocAddrConst( name *value, int seg,
                                 constant_class class,
                                 type_class_def name_class ) {
/************************************************************/

    name        *new_c;

    for( new_c = Names[N_CONSTANT]; new_c != NULL; new_c = new_c->n.next_name ) {
        if( new_c->c.const_type == class
         && new_c->c.value == value
         && new_c->n.name_class == name_class
         && new_c->c.int_value == seg ) return( new_c );
    }
    new_c = AllocName( N_CONSTANT, name_class, 0 );
    new_c->c.value = value;
    new_c->c.int_value = seg;
    new_c->c.static_defn = NULL;
    new_c->c.const_type = class;
    return( new_c );
}


extern  name    *FindIntValue( signed_32 value ) {
/************************************************/

    if( value == 0 && ConstZero != NULL ) return( ConstZero );
    if( value == 1 && ConstOne != NULL ) return( ConstOne );
    return( NULL );
}


extern  name    *AllocIntConst( int value ) {
/*******************************************/

    name        *konst;

    konst = FindIntValue( value );
    if( konst != NULL ) return( konst );
    return( AllocConst( CFCnvIF( value ) ) );
}


extern  name    *AllocS32Const( signed_32 value ) {
/*******************************************/

    name        *konst;

    konst = FindIntValue( value );
    if( konst != NULL ) return( konst );
    return( AllocConst( CFCnvI32F( value ) ) );
}

extern  name    *AllocS64Const( unsigned_32 low, unsigned_32 high ) {
/*******************************************************************/

    name        *new_c;
    pointer     cf_value = CFCnvI64F( low, high );

    new_c = findConst64( low, high, cf_value );
    if( new_c == NULL ){
        new_c = AllocName( N_CONSTANT, XX, 0 );
        new_c->c.value = cf_value;
        new_c->c.int_value = low;
        new_c->c.int_value_2 = high;
        new_c->c.static_defn = NULL;
        new_c->c.const_type = CONS_ABSOLUTE;
    } else {
        CFFree( cf_value );
    }
    return( new_c );
}

extern  name    *AllocU64Const( unsigned_32 low, unsigned_32 high ) {
/*******************************************************************/

    name        *new_c;
    pointer     cf_value = CFCnvU64F( low, high );

    new_c = findConst64( low, high, cf_value );
    if( new_c == NULL ){
        new_c = AllocName( N_CONSTANT, XX, 0 );
        new_c->c.value = cf_value;
        new_c->c.int_value = low;
        new_c->c.int_value_2 = high;
        new_c->c.static_defn = NULL;
        new_c->c.const_type = CONS_ABSOLUTE;
    } else {
        CFFree( cf_value );
    }
    return( new_c );
}

extern  name    *AllocUIntConst( uint value ) {
/*********************************************/

    name        *konst;

    konst = FindIntValue( value );
    if( konst != NULL ) return( konst );
    return( AllocConst( CFCnvUF( value ) ) );
}


extern  constant_defn   *GetFloat( name *cons, type_class_def class ) {
/*********************************************************************/

    constant_defn       *defn;

    defn = cons->c.static_defn;
    for(;;) {
        if( defn == NULL ) break;
        if( defn->const_class == class ) return( defn );
        defn = defn->next_defn;
    }
    defn = AllocFrl( &ConstDefnFrl, sizeof( constant_defn ) );
    defn->const_class = class;
    defn->label = NULL;
    CFCnvTarget( cons->c.value, (flt*)&defn->value, TypeClassSize[ class ] );
    defn->next_defn = cons->c.static_defn;
    cons->c.static_defn = defn;
    return( defn );
}


extern  memory_name     *SAllocMemory( pointer symbol, type_length offset,
                                       cg_class class, type_class_def nclass,
                                       type_length size ) {
/*********************************************************/

    name        *new_m;
    name        *other;
    name        *xx;

    other = NULL;
    xx = NULL;
    for( new_m = Names[N_MEMORY]; new_m != NULL; new_m = new_m->n.next_name ) {
        if( new_m->v.symbol == symbol && new_m->m.memory_type == class ) {
            if( new_m->v.offset != offset ) {
                other = new_m;
                new_m->v.usage |= USE_MEMORY | NEEDS_MEMORY;
            } else {
                if( nclass == XX && size == 0 ) return( &( new_m->m ) ); /* 89-07-07 */
                if( new_m->n.name_class == nclass && nclass != XX ) {/*exact!*/
                    return( &( new_m->m ) );
                }
                if( new_m->n.name_class == XX && new_m->n.size == size ) {
                    xx = new_m;
                }
                other = new_m;
                new_m->v.usage |= USE_MEMORY | NEEDS_MEMORY;
            }
        }
    }
    if( xx != NULL ) {
        ZapXX( xx, nclass, size );
        return( &( xx->m ) );
    }
    new_m = AllocName( N_MEMORY, nclass, size );
    new_m->v.symbol = symbol;
    new_m->v.offset = offset;
    new_m->m.memory_type    = class;
    new_m->m.alignment = 0;
    if( other != NULL ) {
        if( other->m.same_sym != NULL ) {
            new_m->m.same_sym = other->m.same_sym;
        } else {
            new_m->m.same_sym = other;
        }
        other->m.same_sym = new_m;
        new_m->v.usage = USE_MEMORY | NEEDS_MEMORY;
    } else {
        new_m->v.usage = NEEDS_MEMORY;
        new_m->m.same_sym = NULL;
    }
    if( class == CG_FE && _IsModel( NO_OPTIMIZATION ) ) {
        new_m->v.usage |= USE_MEMORY;
    }
    new_m->v.block_usage = 0;
    new_m->v.conflict = NULL;
    if( class == CG_LBL || class == CG_CLB ) {
        new_m->v.usage |= USE_MEMORY; /* so not put in conflict graph*/
    }
    return( &( new_m->m ) );
}


extern  name    *AllocMemory( pointer symbol, type_length offset,
                              cg_class class, type_class_def type_class ) {
/*************************************************************************/

    return( (name *) SAllocMemory( symbol, offset, class, type_class, 0 ) );
}


extern  name    *STempOffset( name *temp, type_length offset,
                              type_class_def class, type_length size ) {
/**********************************************************************/


    name        *new_t;
    name        *xx;

    class = OneClass[  class  ];
    offset += temp->v.offset;
    new_t = temp->t.alias;
    xx = NULL;
    for( ;; ) {
        if( new_t->t.v.id == temp->t.v.id && new_t->v.offset == offset ) {
            if( class == XX && size == 0 ) return( new_t ); /* 89-07-07 */
            if( new_t->n.name_class == class && class != XX ) {
                return( new_t ); /* exact match */
            }
            if( new_t->n.name_class == XX && new_t->n.size == size ) {
                xx = new_t; /* an XX with the right size */
            }
        }
        if( new_t == temp ) break;
        new_t = new_t->t.alias;
    }
    if( xx != NULL ) {
        ZapXX( xx, class, size );
        return( xx );
    }
    new_t = AllocName( N_TEMP, class, size );
    new_t->t.v.id = temp->t.v.id;
    new_t->v.symbol = temp->v.symbol;
    new_t->v.offset = offset;
    new_t->v.usage = temp->v.usage;
    new_t->v.block_usage = 0;
    new_t->v.conflict = temp->v.conflict;
    new_t->t.u.block_id = temp->t.u.block_id;
    new_t->t.alias = temp->t.alias;
    temp->t.alias = new_t;
    new_t->t.temp_flags = ALIAS;
    new_t->t.temp_flags |= temp->t.temp_flags & PERM_TEMP_FLAGS;
    if( temp->t.location == NO_LOCATION ) {
        new_t->t.location = NO_LOCATION;
    } else {
        new_t->t.location = temp->t.location + offset;
    }
    return( new_t );
}


extern  name    *SAllocTemp( type_class_def class, type_length size ) {
/*********************************************************************/

    name        *new_t;

    class = OneClass[  class  ];
    new_t = AllocName( N_TEMP, class, size );
    new_t->t.v.id = ++TempId;
    new_t->v.symbol = NULL;
    new_t->v.offset = 0;
    new_t->v.usage = 0;
    new_t->v.block_usage = 0;
    new_t->v.conflict = NULL;
    new_t->t.u.block_id = NO_BLOCK_ID;
    new_t->t.location = NO_LOCATION;
    new_t->t.alias = new_t;
    new_t->t.temp_flags = 0;
    return( new_t );
}


extern  name    *AllocTemp( type_class_def class ) {
/**************************************************/

    return( SAllocTemp( class, 0 ) );
}


extern  name    *TempOffset( name *temp, type_length offset,
                             type_class_def class ) {
/***************************************************************************/

    return( STempOffset( temp, offset, class, 0 ) );
}


extern  name    *SAllocUserTemp( pointer symbol,
                                 type_class_def class, type_length size ) {
/*************************************************************************/

    name        *new_t;

    class = OneClass[  class  ];
    new_t = LkAddBack( symbol, NULL );
    if( new_t == NULL ) {
        new_t = AllocTemp( class );
        new_t->v.symbol = symbol;
        if( size != 0 ) {
            new_t->n.size = size;
        }
        LkAddBack( symbol, new_t );
        return( new_t );
    } else {
        if( new_t->n.name_class == class && class != XX ) return( new_t );
        return( STempOffset( new_t, 0, class, size ) );
    }
}


extern  name    *AllocUserTemp( pointer symbol, type_class_def class ) {
/**********************************************************************/

    return( SAllocUserTemp( symbol, class, 0 ) );
}


extern  name    *DeAlias( name *temp ) {
/**************************************/

    while( temp->t.temp_flags & ALIAS ) {
        temp = temp->t.alias;
    }
    return( temp );
}



extern  name    *AllocRegName( hw_reg_set regs ) {
/************************************************/

    name        *new_r;

    if( HW_CEqual( regs, HW_EMPTY ) ) { /* here for speed. we grab this a lot! */
        if( NullReg == NULL ) {
            new_r = AllocName( N_REGISTER, RegClass( regs ), 0 );
            HW_CAsgn( new_r->r.reg, HW_EMPTY );
            new_r->r.reg_index = -1;
            NullReg = new_r;
        }
        return( NullReg );
    }
    for( new_r = Names[N_REGISTER]; new_r != NULL; new_r = new_r->n.next_name ) {
        if( HW_Equal( new_r->r.reg, regs ) ) {
            return( new_r );
        }
    }
    new_r = AllocName( N_REGISTER, RegClass( regs ), 0 );
    new_r->r.reg = regs;
    new_r->r.reg_index = -1;
#if _TARGET & _TARG_RISC
    new_r->r.arch_index = RegTrans( regs );
#endif
    return( new_r );
}


extern  name    *ScaleIndex( name *index, name *base, type_length offset,
                             type_class_def class,
                             type_length size, int scale, i_flags flags ) {
/*************************************************************************/

    name        *new_x;

    class = OneClass[  class  ];
    for( new_x = Names[N_INDEXED]; new_x != NULL; new_x = new_x->n.next_name ) {
        if( new_x->i.base == base
         && new_x->i.index == index
         && new_x->i.constant == offset
         && new_x->i.scale == scale
         && new_x->i.index_flags == flags
         && ( ( new_x->n.name_class == class
              && new_x->n.name_class != XX )
            || ( new_x->n.name_class == XX
              && new_x->n.size == size ) ) ) {
            if( class != XX ) {
                new_x->n.name_class = class;
                new_x->n.size = TypeClassSize[  class  ];
            }
            if( index->n.class == N_TEMP ) {
                index->t.temp_flags |= INDEXED;
            }
            return( new_x );
        }
    }
    new_x = AllocName( N_INDEXED, class, size );
    new_x->i.index = index;
    new_x->i.base = base;
    new_x->i.constant = offset;
    new_x->i.index_flags = flags;
    new_x->i.scale = scale;
    if( index->n.class == N_TEMP ) {
        index->t.temp_flags |= INDEXED;
    }
    return( new_x );
}


extern  name    *SAllocIndex( name *index, name *base, type_length offset,
                              type_class_def class, type_length size ) {
/**********************************************************************/

    return( ScaleIndex( index, base, offset, class, size, 0, EMPTY ) );
}


extern  name    *AllocIndex( name *index, name *base,
                             type_length offset, type_class_def class ) {
/***************************************************************/

    return( SAllocIndex( index, base, offset, class, 0 ) );
}


extern  void    InitNames() {
/***************************/

    int                 class;

    class = N_CONSTANT;
    for(;;) {
        InitFrl( &FrlHead[  class  ] );
        if( ++class > N_INDEXED ) break;
    }
    InitFrl( &ConstDefnFrl );
    for( class = U1; class <= XX; ++class ) {
        TypeClassSize[ class ] = ClassType( class )->length;
    }
    ReInitNames();
}


extern  void    ReInitNames() {
/*****************************/

    int class;

    TempId = 0;
    NullReg = NULL;
    ConstOne = NULL;
    ConstZero = NULL;
    LastTemp = NULL;
    DummyIndex = NULL;
    for( class = N_CONSTANT; class <= N_INDEXED; ++class ) {
        Names[class] = NULL;
    }
}


extern  void    FreeNames() {
/***************************/

    int         class;
    name        *temp;
    name        *next;

    for( class = N_CONSTANT; class <= N_INDEXED; ++class ) {
        temp = Names[class];
        Names[class] = NULL;
        for( ; temp != NULL; temp = next ) {
            next = temp->n.next_name;
            FreeAName( temp );
        }
    }
}


extern  void    FreeAName( name *op ) {
/***************************************/

    constant_defn       *defn;
    constant_defn       *junk;

    if( op->n.class == N_CONSTANT ) {
        if( op == ConstZero ) {
            ConstZero = NULL;
        }
        if( op == ConstOne ) {
            ConstOne = NULL;
        }
        if( op->c.const_type == CONS_ABSOLUTE ) {
            CFFree( op->c.value );
            defn = op->c.static_defn;
            while( defn != NULL ) {
                junk = defn;
                defn = defn->next_defn;
                FrlFreeSize( &ConstDefnFrl,
                             (pointer *)junk, sizeof( constant_defn ) );
            }
        }
    } else if( op->n.class == N_MEMORY ) {
        if( op->m.memory_type == CG_TBL ) {
            FreeTable( op->v.symbol );
        }
    } else if( op->n.class == N_REGISTER ) {
        if( op == NullReg ) {
            NullReg = NULL;
        }
    } else if( op->n.class == N_TEMP ) {
        if( op == LastTemp ) LastTemp = op->n.next_name;
        if( op == DummyIndex ) DummyIndex = NULL;
    }
    FrlFreeSize( &FrlHead[  op->n.class  ],
                 (pointer *)op, Size[  op->n.class  ] );
}


extern  bool    NameFrlFree() {
/*****************************/

    bool        freed;
    int         class;

    class = N_CONSTANT;
    freed = FALSE;
    for(;;) {
        freed |= FrlFreeAll( &FrlHead[  class  ], Size[  class  ] );
        if( ++class > N_INDEXED ) break;
    }
    freed |= FrlFreeAll( &ConstDefnFrl, sizeof( constant_defn ) );
    return( freed );
}

extern  i_flags AlignmentToFlags( type_length alignment ) {
/*********************************************************/

    i_flags             flags;

    flags = 0;
    switch( alignment ) {
    case 0:
        break;
    case 1:
        flags |= X_ALIGNED_1;
        break;
    case 2:
        flags |= X_ALIGNED_2;
        break;
    case 4:
        flags |= X_ALIGNED_4;
        break;
    case 8:
        flags |= X_ALIGNED_8;
        break;
    default:
        _Zoiks( ZOIKS_098 );
    }
    return( flags );
}

extern  type_length     FlagsToAlignment( i_flags flags ) {
/*********************************************************/

    type_length         alignment;

    alignment = 0;
    if( flags & X_ALIGNED_1 ) {
        alignment = 1;
    }
    if( flags & X_ALIGNED_2 ) {
        alignment = 2;
    }
    if( flags & X_ALIGNED_4 ) {
        alignment = 4;
    }
    if( flags & X_ALIGNED_8 ) {
        alignment = 8;
    }
    return( alignment );
}
