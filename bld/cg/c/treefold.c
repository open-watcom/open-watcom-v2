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
#include "model.h"
#include "coderep.h"
#include "cgdefs.h"
#include "addrname.h"
#include "tree.h"
#include "opcodes.h"
#include "cfloat.h"
#include "zoiks.h"
#include "i64.h"
#include "feprotos.h"

typedef union i32 { signed_32 s; unsigned_32 u;
} i32;
extern  type_def        *TypeInteger;
extern  type_def        *TypeBoolean;

extern  void            BGGenCtrl(cg_op,bn,label_handle,bool);
extern  an              BGInteger(signed_32,type_def*);
extern  void            FlowOff(an);
extern  an              TreeGen(tn);
extern  name            *AllocIntConst(int);
extern  tn              TGCompare(cg_op,tn,tn,type_def*);
extern  unsigned_32     Mask(btn);
extern  unsigned_32     U32ModDiv(unsigned_32*,unsigned_32);
extern  tn              TGUnary(cg_op,tn,type_def*);
extern  tn              TGBinary(cg_op,tn,tn,type_def*);
extern  void            BurnTree(tn);
extern  tn              TGConst(cfloat *,type_def*);
extern  tn              TGConvert(tn,type_def*);
extern  uint            Length(char*);
extern  tn              TGTrash(tn);
extern  tn              TGNode( tn_class, cg_op, tn, tn, type_def * );


#define _HasBigConst( t )       ( ( (t)->attr & TYPE_FLOAT ) || (t)->length == 8 )

static cg_op RevOpcode[] = {
        O_EQ,    /* O_EQ*/
        O_NE,    /* O_NE*/
        O_LT,    /* O_GT*/
        O_GE,    /* O_LE*/
        O_GT,    /* O_LT*/
        O_LE     /* O_GE*/
};

static signed_32 CFConvertByType( cfloat *cf, type_def *tipe )
/************************************************************/
{
    signed_32   data;

    data = CFCnvF32( cf );
    switch( tipe->length ) {
    case 1:
        if( tipe->attr & TYPE_SIGNED ) {
            data = (signed_8)data;
        } else {
            data = (unsigned_8)data;
        }
        break;
    case 2:
        if( tipe->attr & TYPE_SIGNED ) {
            data = (signed_16)data;
        } else {
            data = (unsigned_16)data;
        }
        break;
    }
    return( data );
}

static signed_64 CFGetInteger64Value( cfloat *cf )
/************************************************/
{
    signed_64   value;
    int         neg;
    cfloat      *trunc;

    trunc = CFTrunc( cf );
    neg = CFTest( trunc );
    if( neg < 0 ) CFNegate( trunc );
    value = CFCnvF64( trunc );
    if( neg < 0 ) U64Neg( &value, &value );
    CFFree( trunc );
    return( value );
}

static  cfloat  *IntToCF( signed_64 value, type_def *tipe ) {
/**********************************************************/

    signed_8    s8;
    unsigned_8  u8;
    signed_16   s16;
    unsigned_16 u16;
    signed_32   s32;
    unsigned_32 u32;

    if( tipe->attr & TYPE_SIGNED ) {
        switch( tipe->length ) {
        case 1:
            s8 = value.u._8[0];
            return( CFCnvI32F( s8 ) );
        case 2:
            s16 = value.u._16[0];
            return( CFCnvI32F( s16 ) );
        case 4:
        case 6:
            s32 = value.u._32[0];
            return( CFCnvI32F( s32 ) );
        case 8:
            return( CFCnvI64F( value.u._32[0], value.u._32[1] ) );
        default:
            _Zoiks( ZOIKS_112 );
            return( NULL );
        }
    } else {
        switch( tipe->length ) {
        case 1:
            u8 = value.u._8[0];
            return( CFCnvU32F( u8 ) );
        case 2:
            u16 = value.u._16[0];
            return( CFCnvU32F( u16 ) );
        case 4:
        case 6:
            u32 = value.u._32[0];
            return( CFCnvU32F( u32 ) );
        case 8:
            return( CFCnvU64F( value.u._32[0], value.u._32[1] ) );
        default:
            _Zoiks( ZOIKS_112 );
            return( NULL );
        }
    }
}

static  tn      IntToType( signed_32 value, type_def *tipe ) {
/************************************************************/

    signed_64   temp;

    I32ToI64( value, &temp );
    return( TGConst( IntToCF( temp, tipe ), tipe ) );
}


static  tn      CFToType( cfloat *cf, type_def *tipe ) {
/******************************************************/

    tn          result;

    if( ( tipe->attr & TYPE_FLOAT ) == EMPTY ) {
        result = TGConst( IntToCF( CFGetInteger64Value( cf ), tipe ), tipe );
        CFFree( cf );
    } else {
        result = TGConst( cf, tipe );
    }
    return( result );
}



extern  int     GetLog2( unsigned_32 value ) {
/*****************************************/

    unsigned_32 count;
    int         log;

    if( _IsPowerOfTwo( value ) && value != 0 ) {
        log = 0;
        count = 1;
        for(;;) {
            if( count == value ) return( log );
            count += count;
            ++log;
        }
    }
    return( -1 );
}


extern  tn      FoldTimes( tn left, tn rite, type_def *tipe ) {
/*************************************************************/

    tn          temp;
    tn          fold;
    int         test;
    int         log;
    cfloat      *lv;
    cfloat      *rv;
    unsigned_32 li;
    unsigned_32 ri;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
    }
    if( rite->class != TN_CONS ) return( NULL );
    if( left->class == TN_BINARY && left->op == O_TIMES &&
        tipe == left->tipe && !_HasBigConst( tipe ) ) {
        if( left->u.left->class == TN_CONS ) {
            left->u.left = FoldTimes( left->u.left, rite, tipe );
            return( left );
        }
        if( left->rite->class == TN_CONS ) {
            left->rite = FoldTimes( left->rite, rite, tipe );
            return( left );
        }
    }
    if( left->class==TN_BINARY && left->op==O_LSHIFT && tipe==left->tipe ) {
        if( !_HasBigConst( tipe ) && left->rite->class == TN_CONS ) {
            log = left->rite->u.name->c.int_value;
            li = 1;
            while( --log >= 0 ) {
                li <<= 1;
            }
            BurnTree( left->rite );
            left->rite = CFToType( CFCnvU32F( li ), tipe );
            left->op = O_TIMES;
            left->rite = FoldTimes( left->rite, rite, tipe );
            return( left );
        }
    }
    rv = rite->u.name->c.value;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( !_HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
            li = CFConvertByType( lv, tipe );
            ri = CFConvertByType( rv, tipe );
            ri = li * ri;
            fold = IntToType( ri, tipe );
        } else {
            fold = CFToType( CFMul( left->u.name->c.value, rv ), tipe );
        }
        BurnTree( left );
        BurnTree( rite );
        return( fold );
    }
    if( !_HasBigConst( tipe ) && rite->u.name->c.int_value == 1 ) {
        BurnTree( rite );
        return( left );
    }
    test = CFTest( rv );
    if( test == 0 ) {
        return( TGBinary( O_COMMA, TGTrash( left ), rite, tipe ) );
    }
    if( _HasBigConst( tipe ) ) return( NULL );
    if( test < 0 ) {
        CFNegate( rv );
    }
    fold = NULL;
    if( CFIsU32( rv ) ) {
        log = GetLog2( CFConvertByType( rv, tipe ) );
        if( log != -1 ) {
            fold = TGBinary( O_LSHIFT, left,
                          IntToType( log, TypeInteger ), tipe );
            if( test < 0 ) {
                fold = TGUnary( O_UMINUS, fold, tipe );
            }
            BurnTree( rite );
        }
    }
    if( test < 0 ) {
        CFNegate( rv );
    }
    return( fold );
}


extern  cfloat  *OkToNegate( cfloat *value, type_def *tipe ) {
/***********************************************************/

/* make sure we don't negate an unsigned and get out of range*/
/* for example -MAX_LONG is no longer an integer type*/

    cfloat      *neg;

    if( _HasBigConst( tipe ) && ( tipe->attr & TYPE_FLOAT ) == 0 ) return( NULL );
    neg = CFCopy( value );
    CFNegate( neg );
    if( _HasBigConst( tipe ) ) return( neg );
    if( tipe->attr & TYPE_SIGNED ) return( neg );
    if( CFIsSize( neg,   tipe->length ) ) return( neg );
    CFFree( neg );
    return( NULL );
}

extern  tn      FoldMinus( tn left, tn rite, type_def *tipe ) {
/*************************************************************/

    tn          fold;
    cfloat      *lv;
    cfloat      *rv;
    unsigned_32 li;
    unsigned_32 ri;

    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( rite->class == TN_CONS ) {
            rv = rite->u.name->c.value;
            if( !_HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                li = CFConvertByType( lv, tipe );
                ri = CFConvertByType( rv, tipe );
                ri = li - ri;
                fold = IntToType( ri, tipe );
            } else {
                fold = CFToType( CFSub( lv, rite->u.name->c.value ),
                                  tipe );
            }
            BurnTree( rite );
            BurnTree( left );
        } else if( CFTest( lv ) == 0 ) {
            fold = TGUnary( O_UMINUS, rite, tipe );
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        rv = OkToNegate( rite->u.name->c.value, tipe );
        if( rv != NULL ) {
            fold = TGBinary( O_PLUS, left, TGConst( rv, rite->tipe ),
                              tipe );
            BurnTree( rite );
        }
    } else if( rite->class == TN_UNARY && rite->op == O_UMINUS ) {
        fold = TGBinary( OP_ADD, left, rite->u.left, tipe );
        rite->u.left = NULL;
        BurnTree( rite );
    }
    return( fold );
}


static type_def *FixAddType( tn left, tn rite, type_def *tipe )
{
    if( left->tipe != tipe ) {
        if( left->tipe == rite->tipe ) {
            tipe = left->tipe;
        }
    }
    return( tipe );
}


extern  tn      FoldPlus( tn left, tn rite, type_def *tipe ) {
/************************************************************/

    tn          fold;
    tn          temp;
    cfloat      *lv;
    cfloat      *rv;
    unsigned_32 li;
    unsigned_32 ri;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( rite->class == TN_CONS ) {
            rv = rite->u.name->c.value;
            if( !_HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                li = CFConvertByType( lv, tipe );
                ri = CFConvertByType( rv, tipe );
                ri = li + ri;
                fold = IntToType( ri, tipe );
            } else {
                fold = CFToType( CFAdd( lv, rite->u.name->c.value ),
                                  tipe );
            }
            BurnTree( rite );
            BurnTree( left );
        } else if( CFTest( lv ) == 0 ) {
            fold = TGConvert( rite, tipe );
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        if( CFTest( rite->u.name->c.value ) == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( left->class == TN_BINARY && left->op == O_PLUS &&
                   tipe == left->tipe && !_HasBigConst( tipe ) ) {
            if( left->u.left->class == TN_CONS ) {
                tipe = FixAddType( left->u.left, rite, tipe );
                if( left->u.left->tipe == tipe ) {
                    fold = FoldPlus( left->u.left, rite, tipe );
                    left->u.left = fold;
                    fold = left;
                }
            } else if( left->rite->class == TN_CONS ) {
                tipe = FixAddType( left->rite, rite, tipe );
                if( left->rite->tipe == tipe ) {
                    fold = FoldPlus( left->rite, rite, tipe );
                    left->rite = fold;
                    fold = left;
                }
            }
        }
    } else if( rite->class == TN_UNARY && rite->op == O_UMINUS ) {
        fold = TGBinary( OP_SUB, left, rite->u.left, tipe );
        rite->u.left = NULL;
        BurnTree( rite );
    } else if( left->class == TN_UNARY && left->op == O_UMINUS ) {
        fold = TGBinary( OP_SUB, rite, left->u.left, tipe );
        left->u.left = NULL;
        BurnTree( left );
    }
    return( fold );
}


static  tn      Halve( tn left, type_def *tipe ) {
/************************************************/

#define ONE_HALF "0.5"
    char        *value;

    value = ONE_HALF;
    return( TGBinary( OP_MUL, left,
                      TGConst( CFCnvSF( value, value+sizeof(ONE_HALF)-1 ), tipe ),
                      tipe ) );
}

extern  tn      FoldPow( tn left, tn rite, type_def *tipe ) {
/************************************************************/

    tn          fold;

    fold = NULL;
    if( left->class == TN_UNARY && left->op == OP_SQRT ) {
        fold = Halve( rite, tipe );
        fold = TGBinary( OP_POW, left->u.left, fold, tipe );
        left->u.left = NULL;
        BurnTree( left );
    } else if( left->class == TN_UNARY && left->op == OP_EXP ) {
        left->u.left = TGBinary( OP_MUL, left->u.left, rite, tipe );
        fold = left;
    } else if( left->class == TN_BINARY && left->op == OP_POW ) {
        left->rite = TGBinary( OP_MUL, left->rite, rite, tipe );
        fold = left;
    }
    return( fold );
}


extern  tn      FoldAnd( tn left, tn rite, type_def *tipe ) {
/***********************************************************/

    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    unsigned_32 and;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        rv = rite->u.name->c.value;
        if( CFIs32( lv ) && CFIs32( rv ) ) {
            and = CFConvertByType( rv, tipe ) & CFConvertByType( lv, tipe );
            fold = IntToType( and, tipe );
            BurnTree( left );
            BurnTree( rite );
        }
    } else if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( CFTest( rv ) == 0 ) {
            left = TGTrash( left );
            fold = TGBinary( O_COMMA, left, IntToType( 0, tipe ), tipe );
            BurnTree( rite );
        } else if( !_HasBigConst( tipe ) && rite->u.name->c.int_value == -1 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldOr( tn left, tn rite, type_def *tipe ) {
/**********************************************************/

    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    unsigned_32 or;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        rv = rite->u.name->c.value;
        if( CFIs32( lv ) && CFIs32( rv ) ) {
            or = CFConvertByType( rv, tipe ) | CFConvertByType( lv, tipe );
            fold = IntToType( or, tipe );
            BurnTree( left );
            BurnTree( rite );
        }
    } else if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( CFTest( rv ) == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( !_HasBigConst( tipe ) && rite->u.name->c.int_value == -1 ) {
            left = TGTrash( left );
            fold = TGBinary( O_COMMA, left, IntToType( -1, tipe ), tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldXor( tn left, tn rite, type_def *tipe ) {
/***********************************************************/

    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    unsigned_32 li;
    unsigned_32 ri;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        rv = rite->u.name->c.value;
        if( CFIs32( lv ) && CFIs32( rv ) ) {
            li = CFConvertByType( lv, tipe );
            ri = CFConvertByType( rv, tipe );
            ri = li ^ ri;
            fold = IntToType( ri, tipe );
            BurnTree( left );
            BurnTree( rite );
        }
    } else if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( CFTest( rv ) == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( !_HasBigConst( tipe ) && rite->u.name->c.int_value == -1 ) {
            fold = TGUnary( O_COMPLEMENT, left, tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldRShift( tn left, tn rite, type_def *tipe ) {
/**************************************************************/

    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    signed_32   li;
    signed_32   ri;
    unsigned_32 shft;

    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        ri = CFConvertByType( rv, tipe );
        if( left->class == TN_CONS ) {
            lv = left->u.name->c.value;
            if( CFIs32( lv ) && CFIs32( rv ) ) {
                li = CFConvertByType( lv, tipe );
                if( tipe->attr & TYPE_SIGNED ) {
                    shft = li >> ri;
                } else {
                    shft = (unsigned_32)li >> (unsigned_32)ri;
                }
                fold = IntToType( shft, tipe );
                BurnTree( left );
                BurnTree( rite );
            }
        } else if( ri == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( ri >= ( tipe->length * 8 ) && ( tipe->attr & TYPE_SIGNED ) == 0 ) {
            fold = TGBinary( O_COMMA, TGTrash( left ), IntToType( 0, tipe ), tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldLShift( tn left, tn rite, type_def *tipe ) {
/**************************************************************/

    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    signed_32   li;
    signed_32   ri;

    if( _HasBigConst( tipe ) ) return( NULL );
    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        ri = CFConvertByType( rv, tipe );
        if( left->class == TN_CONS ) {
            lv = left->u.name->c.value;
            if( CFIs32( lv ) && CFIs32( rv ) ) {
                li = CFConvertByType( lv, tipe );
                fold = IntToType( li << ri, tipe );
                BurnTree( left );
                BurnTree( rite );
            }
        } else if( ri == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( ri >= ( tipe->length * 8 ) ) {
            fold = TGBinary( O_COMMA, TGTrash( left ), IntToType( 0, tipe ), tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldDiv( tn left, tn rite, type_def *tipe ) {
/***********************************************************/


    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    cfloat      *tmp;
    unsigned_32 ri;
    int         log;
    unsigned_32 li;

    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( _HasBigConst( tipe ) ) {
            if( CFTest( rv ) != 0 && left->class == TN_CONS ) {
                fold = CFToType( CFDiv( left->u.name->c.value, rv ), tipe );
                BurnTree( left );
                BurnTree( rite );
            } else if( ( tipe->attr & TYPE_FLOAT ) &&
                ( _IsModel( FP_UNSTABLE_OPTIMIZATION ) ||
                ( CFIsU32( rv ) && GetLog2( CFConvertByType( rv, tipe ) ) != -1 ) ) ) {
                if( CFTest( rv ) != NULL ) {
                    tmp = CFInverse( rv );
                    if( tmp != NULL ) {
                        fold = TGBinary( OP_MUL, left, TGConst( tmp, tipe ), tipe );
                        BurnTree( rite );
                    }
                }
            }
        } else if( CFTest( rv ) != 0 && left->class == TN_CONS ) {
            lv = left->u.name->c.value;
            if( tipe->attr & TYPE_SIGNED ) {
                if( CFIsI32( lv ) && CFIsI32( rv ) ) {
                    li = CFConvertByType( lv, tipe );
                    ri = CFConvertByType( rv, tipe );
                    fold = IntToType( (signed_32)li / (signed_32)ri, tipe );
                    BurnTree( left );
                    BurnTree( rite );
                }
            } else {
                if( CFIsU32( lv ) && CFIsU32( rv ) ) {
                    li = CFCnvF32( lv );
                    ri = CFCnvF32( rv );
                    U32ModDiv( &li, ri );
                    fold = IntToType( li, tipe );
                    BurnTree( left );
                    BurnTree( rite );
                }
            }
        } else if( rite->u.name->c.int_value == 1 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( !_HasBigConst( tipe )
              && ( left->tipe->attr & TYPE_SIGNED ) == 0 ) {
            if( CFIsU32( rv ) ) {
                log = GetLog2( rite->u.name->c.int_value );
                if( log != -1 ) {
                    fold = TGBinary( O_RSHIFT, left,
                                  IntToType( log, TypeInteger ), tipe );
                    BurnTree( rite );
                }
            }
        }
    } else if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            rite = TGTrash( rite );
            fold = TGBinary( O_COMMA, rite, TGConvert( left, tipe ), tipe );
        }
    }
    return( fold );
}


extern  tn      FoldMod( tn left, tn rite, type_def *tipe ) {
/***********************************************************/


    tn          fold;
    cfloat      *rv;
    cfloat      *lv;
    unsigned_32 ri;
    int         log;
    unsigned_32 li;

    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( CFTest( rv ) != 0
         && left->class == TN_CONS && !_HasBigConst( tipe ) ) {
            lv = left->u.name->c.value;
            if( tipe->attr & TYPE_SIGNED ) {
                if( CFIsI32( lv ) && CFIsI32( rv ) ) {
                    li = CFConvertByType( lv, tipe );
                    ri = CFConvertByType( rv, tipe );
                    fold = IntToType( (signed_32)li % (signed_32)ri, tipe );
                    BurnTree( left );
                    BurnTree( rite );
                }
            } else {
                if( CFIsU32( lv ) && CFIsU32( rv ) ) {
                    li = CFConvertByType( lv, tipe );
                    ri = CFConvertByType( rv, tipe );
                    li = U32ModDiv( &li, ri );
                    fold = IntToType( li, tipe );
                    BurnTree( left );
                    BurnTree( rite );
                }
            }
        } else if( !_HasBigConst( tipe ) && rite->u.name->c.int_value == 1 ) {
            fold = CFToType( CFCnvIF( 0 ), tipe );
            fold = TGBinary( O_COMMA, left, fold, tipe );
            BurnTree( rite );
        } else if( !_HasBigConst( tipe )
              && ( left->tipe->attr & TYPE_SIGNED ) == 0 ) {
            if( CFIsU32( rv ) ) {
                ri = CFConvertByType( rv, tipe );
                log = GetLog2( ri );
                if( log != -1 ) {
                    fold = TGBinary( O_AND, left,
                                  IntToType( ri-1, tipe ), tipe );
                    BurnTree( rite );
                }
            }
        }
    } else if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            rite = TGTrash( rite );
            fold = TGBinary( O_COMMA, rite, IntToType( 0, tipe ), tipe );
            BurnTree( left );
        }
    }
    return( fold );
}


extern  tn      Fold1sComp( tn left, type_def *tipe ) {
/*****************************************************/

    tn          new;
    cfloat      *lv;

    new = NULL;
    if( !_HasBigConst( tipe ) && left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( CFIs32( lv ) ) {
            new = IntToType( ~CFConvertByType( lv, tipe ), tipe );
            BurnTree( left );
        }
    }
    return( new );
}


extern  tn      FoldUMinus( tn left, type_def *tipe ) {
/*****************************************************/

    tn          new;
    cfloat      *lv;

    new = NULL;
    if( left->class == TN_CONS ) {
        lv = OkToNegate( left->u.name->c.value, tipe );
        if( lv != NULL ) {
            new = CFToType( lv, tipe );
            BurnTree( left );
        }
    } else if( left->class == TN_UNARY && left->op == O_UMINUS ) {
        new = left->u.left;
        left->u.left = NULL;
        BurnTree( left );
    }
    return( new );
}


extern  tn      FoldSqrt( tn left, type_def *tipe ) {
/***************************************************/

    tn          fold;

    fold = NULL;
    if( left->class == TN_UNARY && left->op == OP_EXP ) {
        left->u.left = Halve( left->u.left, tipe );
        fold = left;
    } else if( left->class == TN_BINARY && left->op == OP_POW ) {
        left->rite = Halve( left->rite, tipe );
        fold = left;
    }
    return( fold );
}


extern  tn      FoldLog( cg_op op, tn left, type_def *tipe ) {
/************************************************************/

    tn          fold;

    fold = NULL;
    if( left->class == TN_UNARY && left->op == OP_SQRT ) {
        fold = TGUnary( op, left->u.left, tipe );
        fold = Halve( fold, tipe );
        left->u.left = NULL;
        BurnTree( left );
    } else if( left->class == TN_BINARY && left->op == OP_POW ) {
        fold = TGUnary( op, left->u.left, tipe );
        fold = TGBinary( O_TIMES, left->rite, fold, tipe );
        left->u.left = NULL;
        left->rite = NULL;
        BurnTree( left );
    }
    return( fold );
}


extern  tn      FoldFlAnd( tn left, tn rite ) {
/*********************************************/

    tn  fold;

    fold = NULL;
    if( left->class == TN_CONS ) {
        if( rite->class == TN_CONS ) {
            if( CFTest( left->u.name->c.value ) && CFTest( rite->u.name->c.value ) ) {
                fold = IntToType( FETrue(), TypeInteger );
            } else {
                fold = IntToType( 0, TypeInteger );
            }
            BurnTree( left );
            BurnTree( rite );
        } else if( CFTest( left->u.name->c.value ) == 0 ) {
            // ( 0 && expr ) -> 0
            fold = IntToType( 0, TypeInteger );
            BurnTree( left );
            BurnTree( rite );
        } else {
            // ( 1 && expr ) -> ( expr )
            fold = rite;
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        if( CFTest( rite->u.name->c.value ) ) {
            // ( expr && 1 ) -> ( expr )
            fold = left;
            BurnTree( rite );
        } else {
            // ( expr && 0 ) -> ( expr, 0 )
            fold = TGNode( TN_COMMA, OP_NOP, left, IntToType( 0, TypeInteger ), TypeInteger );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldFlOr( tn left, tn rite ) {
/********************************************/

    tn  fold;

    fold = NULL;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) ) {
            // ( 1 || expr ) -> ( TRUE )
            fold = IntToType( FETrue(), TypeInteger );
            BurnTree( left );
            BurnTree( rite );
        } else {
            // ( 0 || expr ) -> ( expr )
            fold = rite;
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        if( CFTest( rite->u.name->c.value ) ) {
            // ( expr || 1 ) -> ( expr, TRUE )
            fold = TGNode( TN_COMMA, OP_NOP, left, IntToType( FETrue(), TypeInteger ), TypeInteger );
            BurnTree( rite );
        } else {
            // ( expr || 0 ) -> ( expr )
            fold = left;
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  tn      FoldFlNot( tn left ) {
/************************************/

    tn          fold;
    int         result;


    fold = NULL;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            result = FETrue();
        } else {
            result = 0;
        }
        fold = IntToType( result, TypeInteger );
        BurnTree( left );
    }
    return( fold );
}

extern  tn      FoldBitCompare( opcode_defs op, tn_btn left, tn rite ) {
/*******************************************************************/

    tn          fold;
    unsigned_32 new_cons;
    unsigned_32 mask;

    if( left.t->class == TN_CONS ) {
        fold = left.t;
        left.t = rite;
        rite = fold;
        op = RevOpcode[  op - O_EQ  ];
    }
    fold = NULL;
    if( rite->class == TN_CONS
     && left.t->class == TN_BIT_RVALUE
     && !(left.b->is_signed)
     && !_HasBigConst( left.b->tipe ) ) {
        new_cons = rite->u.name->c.int_value;
        new_cons <<= left.b->start;
        mask = Mask( left.b );
        if( ( new_cons & ~mask ) == 0 ) { /* idiot comparing out of range*/
            fold = TGUnary( O_POINTS, left.b->u.left, left.b->tipe );
            fold = TGBinary( O_AND, fold, IntToType( mask, left.b->tipe ),
                              left.b->tipe );
            fold = TGCompare( op, fold, IntToType( new_cons, left.b->tipe ),
                               left.b->tipe );
            left.b->u.left = NULL; /* so no recursion*/
            BurnTree( left.t );
            BurnTree( rite );
        }
    }
    return( fold );
}


extern  cfloat *CnvCFToType( cfloat *cf, type_def *tipe ) {
/*********************************************************/


    if( ( tipe->attr & TYPE_FLOAT ) == EMPTY ) {
        cf = IntToCF( CFGetInteger64Value( cf ), tipe );
    } else {
        cf = CFCopy( cf );
    }
    return( cf );
}

static  tn      FindBase( tn tree, bool op_eq ) {
/***********************************************/

    type_attr           child_attr;
    type_attr           this_attr;

    for(;;) {
        if( tree->class != TN_UNARY ) break;
        if( tree->op != O_CONVERT ) break;
        if( tree->u.left->tipe->length > tree->tipe->length ) break;
        child_attr = tree->u.left->tipe->attr;
        this_attr = tree->tipe->attr;
        if( op_eq ) {
            // if we are doing a EQ/NE comparison we can ignore sign changes
            child_attr &= ~TYPE_SIGNED;
            this_attr  &= ~TYPE_SIGNED;
        }
        if( child_attr != this_attr ) break;
        tree = tree->u.left;
    }
    return( tree );
}

static  void    BurnToBase( tn root, tn base ) {
/**********************************************/

    tn          next;
    tn          curr;

    for( curr = root; curr != base; curr = next ) {
        assert( curr->class == TN_UNARY && curr->op == O_CONVERT );
        next = curr->u.left;
        curr->u.left = NULL;
        BurnTree( curr );
    }
}

extern  tn      FoldCompare( opcode_defs op, tn left,
                             tn rite, type_def *tipe ) {
/*****************************************************/

    int         compare;
    tn          temp;
    int         result;
    int         true_value;
    cfloat      *lv;
    cfloat      *rv;
    tn          base_r;
    tn          base_l;
    bool        op_eq;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
        op = RevOpcode[  op - O_EQ  ];
    }
    op_eq = FALSE;
    if( ( op == O_EQ ) || ( op == O_NE ) ) {
        op_eq = TRUE;
    }
    true_value = FETrue();
    if( left->class == TN_CONS ) {
        lv = CnvCFToType( left->u.name->c.value, tipe );
        rv = CnvCFToType( rite->u.name->c.value, tipe );
        compare = CFCompare( lv, rv );
        CFFree( lv );
        CFFree( rv );
        result = 0;
        switch( op ) {
        case O_EQ:
            if( compare == 0 ) {
                result = true_value;
            }
            break;
        case O_NE:
            if( compare != 0 ) {
                result = true_value;
            }
            break;
        case O_GT:
            if( compare > 0 ) {
                result = true_value;
            }
            break;
        case O_LT:
            if( compare < 0 ) {
                result = true_value;
            }
            break;
        case O_GE:
            if( compare >= 0 ) {
                result = true_value;
            }
            break;
        case O_LE:
            if( compare <= 0 ) {
                result = true_value;
            }
            break;
        }
        BurnTree( left );
        BurnTree( rite );
        return( IntToType( result, TypeInteger ) );
    } else if( rite->class == TN_CONS ) {
        if( left->class != TN_BINARY ) {
            base_l = FindBase( left, op_eq );
            if( base_l != left && ( ( op == O_NE ) || ( op == O_EQ ) ) ) {
                // got rid of some lame converts
                BurnToBase( left, base_l );
                return( TGNode( TN_COMPARE, op, base_l, rite, TypeBoolean ) );
            }
            return( NULL );
        }
#if _TARGET == _TARG_370
        if( left->rite->class != TN_CONS ) return( NULL );
#endif
        if( left->op != O_AND ) return( NULL );
        if( op != O_EQ && op != O_NE ) return( NULL );
        if( left->u.left->class == TN_CONS ) {
            temp = left->u.left;
            left->u.left = left->rite;
            left->rite = temp;
        }
        if( left->rite->class == TN_CONS
         && left->rite->u.left == rite->u.left
         && !_HasBigConst( tipe )
         && GetLog2( rite->u.name->c.int_value ) != -1 ) {
            rite->u.name = AllocIntConst( 0 );
            if( op == O_EQ ) {
                op = O_NE;
            } else {
                op = O_EQ;
            }
        }
        if( CFTest( rite->u.name->c.value ) != 0 ) return( NULL );
        BurnTree( rite );
        left->class = TN_COMPARE;
        left->tipe = TypeBoolean;
        if( op == O_EQ ) {
            left->op = OP_BIT_TEST_FALSE;
        } else {
            left->op = OP_BIT_TEST_TRUE;
        }
        return( left );
    } else {
        base_r = FindBase( rite, op_eq );
        base_l = FindBase( left, op_eq );
        if( base_r != rite || base_l != left ) {
            if( ( base_r->tipe == base_l->tipe ) && ( ( op == O_EQ ) || ( op == O_NE ) ) ) {
                BurnToBase( rite, base_r );
                BurnToBase( left, base_l );
                return( TGNode( TN_COMPARE, op, base_l, base_r, TypeBoolean ) );
            }
        }
    }
    return( NULL );
}

static  bool    SimpleLeaf( tn tree ) {
/*************************************/

    if( tree->class == TN_UNARY && tree->op == O_POINTS ) {
        tree = tree->u.left;
    }
    return( tree->class == TN_LEAF );
}


extern  tn      FoldPostGetsCompare( opcode_defs op, tn left, tn rite, type_def *tipe ) {
/***************************************************************************************/

    tn          compare;
    tn          temp;
    signed_32   ri;
    signed_32   li;
    signed_32   value;
    cfloat      *rv;
    cfloat      *lv;

    compare = NULL;
    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
        op = RevOpcode[  op - O_EQ  ];
    }

    if( op == O_GT && ( ( tipe->attr & TYPE_SIGNED ) == EMPTY ) ) {
        if( rite->class == TN_CONS ) {
            if( CFTest( rite->u.name->c.value ) == 0 ) {
                op = O_NE;
            }
        }
    }

    /*
     * This is a little sick - basically, we have some code which looks
     * like "while( n-- ) {" and we don't want it to mess up our dataflo
     * so we do a little tree re-write here to smooth things over.
     */

    if( op != O_EQ && op != O_NE ) return( NULL );
    if( rite->class == TN_CONS &&
        left->class == TN_POST_GETS &&
        ( left->op == OP_SUB || left->op == OP_ADD ) ) {
        if( left->rite->class == TN_CONS && SimpleLeaf( left->u.left ) ) {
            if( tipe == left->tipe ) {
                rv = rite->u.name->c.value;
                lv = left->rite->u.name->c.value;
                if( !_HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                    li = CFConvertByType( lv, tipe );
                    ri = CFConvertByType( rv, tipe );
                    value = ( left->op == OP_SUB ) ? ( ri - li ) : ( ri + li );
                    temp = IntToType( value, tipe );
                    left->class = TN_PRE_GETS;
                    left->optipe = left->tipe;
                    temp = TGNode( TN_COMPARE, op, left, temp, TypeBoolean );
                    BurnTree( rite );
                    return( temp );
                }
            }
        }
    }
    return( NULL );
}


/* routines above here are called while building the tree*/
/* routines below here are called while tearing the tree apart*/

static  an Flip( an name, bool op_false, bool op_true ) {
/************************************************************/

    label_handle        temp;

    if( op_false ) {
        if( op_true ) {
            FlowOff( name );
            name = BGInteger( FETrue(), TypeInteger );
        } else {
            temp = (*(bn *)&name)->f;
            (*(bn *)&name)->f = (*(bn *)&name)->t;
            (*(bn *)&name)->t = temp;
        }
    } else {
        if( op_true ) {
            /* nothing*/
        } else {
            FlowOff( name );
            name = BGInteger( 0, TypeInteger );
        }
    }
    return( name );
}



extern  an FoldConsCompare( opcode_defs op, tn left,
                                 tn rite, type_def *tipe ) {
/**********************************************************/

    tn          temp;
    an          fold;
    int         compare;
    int         compare_true;
    cfloat      *f;
    cfloat      *t;
    cfloat      *rv;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
        op = RevOpcode[  op - O_EQ  ];
    }
    fold = NULL;
    if( rite->class == TN_CONS ) {
        if( left->class == TN_FLOW_OUT ) {
            f = CFCnvIF( 0 );
            t = CFCnvIF( FETrue() );
            rv = CnvCFToType( rite->u.name->c.value, tipe );
            compare = CFCompare( f, rv );
            compare_true = CFCompare( t, rv );
            CFFree( rv );
            CFFree( f );
            CFFree( t );
            fold = TreeGen( left->u.left );
            switch( op ) {
            case O_EQ:
                fold = Flip( fold, compare == 0, compare_true == 0 );
                break;
            case O_NE:
                fold = Flip( fold, compare != 0, compare_true != 0 );
                break;
            case O_GT:
                fold = Flip( fold, compare > 0, compare_true > 0 );
                break;
            case O_LT:
                fold = Flip( fold, compare < 0, compare_true < 0 );
                break;
            case O_GE:
                fold = Flip( fold, compare >= 0, compare_true >= 0 );
                break;
            case O_LE:
                fold = Flip( fold, compare <= 0, compare_true <= 0 );
                break;
            case OP_BIT_TEST_TRUE:
                if( !_HasBigConst( tipe ) ) {
                    fold = Flip( fold, FALSE,
                            ( rite->u.name->c.int_value & FETrue() ) != 0 );
                }
                break;
            case OP_BIT_TEST_FALSE:
                if( !_HasBigConst( tipe ) ) {
                    fold = Flip( fold, TRUE,
                            ( rite->u.name->c.int_value & FETrue() ) == 0 );
                }
                break;
            }
            BurnTree( rite );
            left->u.left = NULL;
            BurnTree( left );
        }
    }
    return( fold );
}


extern  bool    FoldIfTrue( tn left, label_handle lbl ) {
/*******************************************************/

    bool        folded;

    folded = FALSE;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) != 0 ) {
            BGGenCtrl( O_GOTO, NULL, lbl, TRUE );
        }
        folded = TRUE;
        BurnTree( left );
    }
    return( folded );
}


extern  bool    FoldIfFalse( tn left, label_handle lbl ) {
/********************************************************/

    bool        folded;

    folded = FALSE;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            BGGenCtrl( O_GOTO, NULL, lbl, TRUE );
        }
        folded = TRUE;
        BurnTree( left );
    }
    return( folded );
}
