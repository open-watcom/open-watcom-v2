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
#include "regset.h"
#include "procdef.h"
#include "model.h"

#include "s37rgtbl.def"

extern  proc_def        *CurrProc;

static  hw_reg_set      Empty[] = {
                EMPTY };
static  hw_reg_set      G0Reg[] = {
                HW_D_1( HW_G0 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      Y0Reg[] = {
                HW_D_1( HW_Y0 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      D0Reg[] = {
                HW_D_1( HW_D0 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      G11Reg[] = {
                HW_D_1( HW_G11 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      G15Reg[] = {
                HW_D_1( HW_G15 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      WordRegs[] = {
                HW_D_1( HW_G0 ),
                HW_D_1( HW_G1 ),
                HW_D_1( HW_G2 ),
                HW_D_1( HW_G3 ),
                HW_D_1( HW_G4 ),
                HW_D_1( HW_G5 ),
                HW_D_1( HW_G6 ),
                HW_D_1( HW_G7 ),
                HW_D_1( HW_G8 ),
                HW_D_1( HW_G9 ),
                HW_D_1( HW_G10 ),
                HW_D_1( HW_G11 ),
                HW_D_1( HW_G12 ),
                HW_D_1( HW_G13 ),
                HW_D_1( HW_G14 ),
                HW_D_1( HW_G15 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      IndexRegs[] = {
                HW_D_1( HW_G1 ),
                HW_D_1( HW_G2 ),
                HW_D_1( HW_G3 ),
                HW_D_1( HW_G4 ),
                HW_D_1( HW_G5 ),
                HW_D_1( HW_G6 ),
                HW_D_1( HW_G7 ),
                HW_D_1( HW_G8 ),
                HW_D_1( HW_G9 ),
                HW_D_1( HW_G10 ),
                HW_D_1( HW_G11 ),
                HW_D_1( HW_G12 ),
                HW_D_1( HW_G13 ),
                HW_D_1( HW_G14 ),
                HW_D_1( HW_G15 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      PairRegs[] = {
                HW_D_2( HW_G0,HW_G1 ),
                HW_D_2( HW_G2,HW_G3 ),
                HW_D_2( HW_G4,HW_G5 ),
                HW_D_2( HW_G6,HW_G7 ),
                HW_D_2( HW_G8,HW_G9 ),
                HW_D_2( HW_G10,HW_G11 ),
                HW_D_2( HW_G12,HW_G13 ),
                HW_D_2( HW_G14,HW_G15 ),
                HW_D_1( HW_EMPTY ) };

static  hw_reg_set      TwoPairRegs[] = {
                HW_D_4( HW_G0 ,HW_G1  , HW_G2 ,HW_G3  ),
                HW_D_4( HW_G0 ,HW_G1  , HW_G4 ,HW_G5  ),
                HW_D_4( HW_G0 ,HW_G1  , HW_G6 ,HW_G7  ),
                HW_D_4( HW_G0 ,HW_G1  , HW_G8 ,HW_G9  ),
                HW_D_4( HW_G0 ,HW_G1  , HW_G10,HW_G11 ),
                HW_D_4( HW_G0 ,HW_G1  , HW_G12,HW_G13 ),
                HW_D_4( HW_G0 ,HW_G1  , HW_G14,HW_G15 ),

                HW_D_4( HW_G2 ,HW_G3  , HW_G4 ,HW_G5  ),
                HW_D_4( HW_G2 ,HW_G3  , HW_G6 ,HW_G7  ),
                HW_D_4( HW_G2 ,HW_G3  , HW_G8 ,HW_G9  ),
                HW_D_4( HW_G2 ,HW_G3  , HW_G10,HW_G11 ),
                HW_D_4( HW_G2 ,HW_G3  , HW_G12,HW_G13 ),
                HW_D_4( HW_G2 ,HW_G3  , HW_G14,HW_G15 ),

                HW_D_4( HW_G4 ,HW_G5  , HW_G6 ,HW_G7  ),
                HW_D_4( HW_G4 ,HW_G5  , HW_G8 ,HW_G9  ),
                HW_D_4( HW_G4 ,HW_G5  , HW_G10,HW_G11 ),
                HW_D_4( HW_G4 ,HW_G5  , HW_G12,HW_G13 ),
                HW_D_4( HW_G4 ,HW_G5  , HW_G14,HW_G15 ),

                HW_D_4( HW_G6 ,HW_G7  , HW_G8 ,HW_G9  ),
                HW_D_4( HW_G6 ,HW_G7  , HW_G10,HW_G11 ),
                HW_D_4( HW_G6 ,HW_G7  , HW_G12,HW_G13 ),
                HW_D_4( HW_G6 ,HW_G7  , HW_G14,HW_G15 ),

                HW_D_4( HW_G8 ,HW_G9  , HW_G10,HW_G11 ),
                HW_D_4( HW_G8 ,HW_G9  , HW_G12,HW_G13 ),
                HW_D_4( HW_G8 ,HW_G9  , HW_G14,HW_G15 ),

                HW_D_4( HW_G10,HW_G11 , HW_G12,HW_G13 ),
                HW_D_4( HW_G10,HW_G11 , HW_G14,HW_G15 ),

                HW_D_4( HW_G12,HW_G13 , HW_G14,HW_G15 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      SingleRegs[] = {
                HW_D_1( HW_Y0 ),
                HW_D_1( HW_Y2 ),
                HW_D_1( HW_Y4 ),
                HW_D_1( HW_Y6 ),
                HW_D_1( HW_EMPTY ) };
static  hw_reg_set      DoubleRegs[] = {
                HW_D_1( HW_D0 ),
                HW_D_1( HW_D2 ),
                HW_D_1( HW_D4 ),
                HW_D_1( HW_D6 ),
                HW_D_1( HW_EMPTY ) };

hw_reg_set      *RegSets[] = {
        #undef RL
        #define RL(a,b,c,d) a
        #include "rl.h"
        NULL
};

op_regs RegList[] = {
        #undef RG
        #define RG( a,b,c,d,e,f ) a,b,c,d,e
        #include "rg.h"
        RL_
};

static  reg_set_index   ClassSets[] = {
/*       RL_ indicates that the class is already identified*/
/*       (e.g. I1 already identified as U1*/
/**/
        RL_,                    /* U1*/
        RL_,                    /* I1*/
        RL_,                    /* U2*/
        RL_,                    /* I2*//*(put segs here so their size is OK)*/
        RL_WORD,                /* U4*/
        RL_WORD,                /* I4*//* for IDIV instructions */
        RL_,                    /* CP*/
        RL_WORD,                /* PT*/
        RL_SINGLE,              /* FS*/
        RL_DOUBLE,              /* FD*/
        RL_PAIR };              /* XX*/

static  reg_set_index   IsSets[] = {
        RL_,                    /* U1*/
        RL_,                    /* I1*/
        RL_,                    /* U2*/
        RL_,                    /* I2*/
        RL_WORD,                /* U4*/
        RL_WORD,                /* I4*/
        RL_,                    /* CP*/
        RL_WORD,                /* PT*/
        RL_SINGLE,              /* FS*/
        RL_DOUBLE,              /* FD*/
        RL_ };                  /* XX*/

static  reg_set_index   ReturnSets[] = {
        RL_G11,                 /* U1*/
        RL_G11,                 /* I1*/
        RL_G11,                 /* U2*/
        RL_G11,                 /* I2*/
        RL_G11,                 /* U4*/
        RL_G11,                 /* I4*/
        RL_,                    /* CP*/
        RL_G11,                 /* PT*/
        RL_,                    /* FS*/
        RL_,                    /* FD*/
        RL_ };                  /* XX*/

static  reg_set_index   OSReturnSets[] = {
        RL_G15,                 /* U1*/
        RL_G15,                 /* I1*/
        RL_G15,                 /* U2*/
        RL_G15,                 /* I2*/
        RL_G15,                 /* U4*/
        RL_G15,                 /* I4*/
        RL_,                    /* CP*/
        RL_G15,                 /* PT*/
        RL_Y0,                  /* FS*/
        RL_D0,                  /* FD*/
        RL_ };                  /* XX*/

static  reg_set_index   IndexSets[] = {
        RL_,                    /* U1*/
        RL_,                    /* I1*/
        RL_,                    /* U2*/
        RL_,                    /* I2*/
        RL_INDEX,               /* U4*/
        RL_,                    /* I4*/
        RL_,                    /* CP*/
        RL_INDEX,               /* PT*/
        RL_,                    /* FS*/
        RL_,                    /* FD*/
        RL_ };                  /* XX*/

/*       Information for register set intersections*/
/*       if sets are of different classes*/
/*               intersection is empty*/
/*       } else {*/
/*               intersection given in square matrix for class*/
/*       }*/

typedef enum {
        FOUR_BYTE,
        EIGHT_BYTE,
        SINGLE,
        DOUBLE,
        OTHER
} intersect_class;


typedef struct reg_class {
        intersect_class class;
        byte            index;
} reg_class;

static  reg_class       IntersectInfo[] = {
        #undef RL
        #define RL(a,b,c,d) c,d
        #include "rl.h"
        OTHER,          0 };    /* RL_NUMBER_OF_SETS*/


static  byte    Width[] = {
        5,              /* FOUR_BYTE */
        1,              /* EIGHT_BYTE*/
        2,              /* SINGLE    */
        2,              /* DOUBLE    */
        0};             /* OTHER     */

static  reg_set_index   FourByteInter[] = {
/*RL_G0    RL_G11      RL_INDEX     RL_WORD      RL_G15*/

RL_G0,     RL_,        RL_,         RL_G0,       RL_,           /*RL_G0*/
RL_,       RL_G11,     RL_G11,      RL_G11,      RL_,           /*RL_G11*/
RL_,       RL_G11,     RL_INDEX,    RL_INDEX,    RL_G15,        /*RL_INDEX*/
RL_G0,     RL_G11,     RL_INDEX,    RL_WORD,     RL_G15,        /*RL_WORD*/
RL_,       RL_,        RL_G15,      RL_G15,      RL_G15         /*RL_G15*/

};


static  reg_set_index   EightByteInter[] = {
        RL_PAIR
};


static  reg_set_index   SingleInter[] = {
/*      RL_SINGLE       RL_Y0   */

        RL_SINGLE,      RL_Y0,          /* RL_SINGLE */
        RL_Y0,          RL_Y0           /* RL_Y0 */

};


static  reg_set_index   DoubleInter[] = {
/*      RL_DOUBLE       RL_D0   */

        RL_DOUBLE,      RL_D0,          /* RL_DOUBLE */
        RL_D0,          RL_D0           /* RL_D0 */

};


static  reg_set_index   OtherInter[] = {
        RL_
};


static  reg_set_index   *InterTable[] = {
        &FourByteInter,         /* FOUR_BYTE */
        &EightByteInter,        /* EIGHT_BYTE*/
        &SingleInter,           /* SINGLE    */
        &DoubleInter,           /* DOUBLE    */
        &OtherInter};           /* OTHERS    */

extern  void            InitRegTbl() {
/************************************/

}


extern  reg_set_index   RegIntersect( reg_set_index s1, reg_set_index s2 ) {
/**************************************************************************/


    reg_class           *set1;
    reg_class           *set2;
    intersect_class     class;
    reg_set_index       result;

    set1 = & IntersectInfo[ s1 ];
    set2 = & IntersectInfo[ s2 ];
    class = set1->class;
    if( class == set2->class ) {
        result = InterTable[ class ]
                        [  set1->index * Width[ class ] + set2->index  ];
    } else if( s1 == RL_NUMBER_OF_SETS ) {
        result = s2;
    } else if( s2 == RL_NUMBER_OF_SETS ) {
        result = s1;
    } else {
        result = RL_;
    }
    return( result );
}


extern  hw_reg_set      *ParmChoices( type_class_def class ) {
/************************************************************/
    class=class;
    return( RegSets[ RL_WORD ] );
}


extern  hw_reg_set      InLineParm( hw_reg_set regs, hw_reg_set used ) {
/**********************************************************************/

    regs = regs; used = used;
    return( HW_EMPTY );
}


extern  hw_reg_set      StructReg() {
/***********************************/

    return( HW_EMPTY );
}


extern  hw_reg_set      ReturnReg( type_class_def class,
                                   call_attributes attr ) {
/*********************************************************/

    hw_reg_set          *list;

    if( attr & ROUTINE_OS ) {
        list = RegSets[  OSReturnSets[  class  ]  ];
    } else {
        list = RegSets[  ReturnSets[  class  ]  ];
    }
    return( *list );
}


extern  reg_set_index   SegIndex() {
/**********************************/

    return( RL_  );
}


extern  reg_set_index   NoSegments( reg_set_index idx ) {
/*******************************************************/

    return( idx );
}


extern  reg_set_index   IndexIntersect( reg_set_index curr,
                                       type_class_def class,
                                       bool is_temp_index ) {
/***********************************************************/

    is_temp_index = is_temp_index;
    return( RegIntersect( curr, IndexSets[  class  ] ) );
}


extern  bool    IsIndexReg( hw_reg_set reg, type_class_def class,
                            bool is_temp_index ) {
/************************************************/

    hw_reg_set  *list;

    is_temp_index = is_temp_index;
    list = RegSets[  IndexSets[  class  ]  ];
    while( !HW_CEqual( *list, HW_EMPTY ) ) {
        if( HW_Equal( *list, reg ) ) break;
        ++ list;
    }
    return( HW_Equal( *list, reg ) );
}


extern  bool    IndexRegOk( hw_reg_set reg, bool is_temp_index ) {
/****************************************************************/

    hw_reg_set  *list;

    is_temp_index = is_temp_index;
    if( RegClass( reg ) == U4 ) {
        list = RegSets[  RL_INDEX  ];
    } else {
        list = RegSets[  RL_ ];
    }
    while( !HW_CEqual( *list, HW_EMPTY ) ) {
        if( HW_Equal( *list, reg ) ) break;
        ++ list;
    }
    return( HW_Equal( *list, reg ) );
}


extern  bool    IsSegReg( hw_reg_set regs ) {
/*******************************************/
    regs = regs;
    return( false );
}


extern  hw_reg_set      LowTreePart( hw_reg_set regs ) {
/*******************************************************/

    return( LowOffsetReg( regs ) );
}


extern  hw_reg_set      LowOffsetReg( hw_reg_set regs ) {
/*******************************************************/

    return( HighReg( regs ) );
}


extern  hw_reg_set      High64Reg( hw_reg_set regs ) {
/*****************************************************/

    return( HighReg( regs ) );
}


extern  hw_reg_set      HighReg( hw_reg_set regs ) {
/**************************************************/

    switch( RegClass( regs ) ) {
    case XX:
        HW_COnlyOn( regs, HW_EVEN );
        return( regs );
    case FD:
        HW_COnlyOn( regs, HW_SNGL );
        return( regs );
    }
    return( HW_EMPTY );
}


extern  hw_reg_set      HighTreePart( hw_reg_set regs ) {
/********************************************************/

    return( HighOffsetReg( regs ) );
}


extern  hw_reg_set      HighOffsetReg( hw_reg_set regs ) {
/********************************************************/

    return( LowReg( regs ) );
}


extern  hw_reg_set      Low64Reg( hw_reg_set regs ) {
/***************************************************/

    return( LowReg( regs ) );
}


extern  hw_reg_set      LowReg( hw_reg_set regs ) {
/*************************************************/

    switch( RegClass( regs ) ) {
    case XX:
        HW_COnlyOn( regs, HW_ODD );
        return( regs );
    }
    return( HW_EMPTY );
}

extern  hw_reg_set      FullReg( hw_reg_set regs ) {
/**************************************************/
    return( regs );
}


extern  type_class_def  RegClass( hw_reg_set regs ) {
/***************************************************/

    hw_reg_set          *possible;
    type_class_def      class;

    if( HW_COvlap( regs, HW_FLTS ) ) {
        class = FS;
    } else {
        class = U1;
    }
    for(;;) {
        possible = RegSets[  ClassSets[  class  ]  ];
        while( !HW_CEqual( *possible, HW_EMPTY ) ) {
            if( HW_Equal( *possible, regs ) ) return( class );
            ++ possible;
        }
        ++ class;
        if( class == XX ) break;
    }
    return( class );
}


extern  bool    IsRegClass( hw_reg_set regs, type_class_def class ) {
/*******************************************************************/

    hw_reg_set  *list;

    list = RegSets[  IsSets[  class  ]  ];
    while( !HW_CEqual( *list, HW_EMPTY ) ) {
        if( HW_Equal( *list, regs ) ) break;
        ++ list;
    }
    return( !HW_CEqual( *list, HW_EMPTY ) );
}


extern  hw_reg_set      ActualParmReg( hw_reg_set reg ) {
/*******************************************************/
    return( reg );
}


extern  hw_reg_set      FixedRegs() {
/***********************************/

    hw_reg_set  fixed;

    fixed = WordRegs[ CurrProc->state.regs.GP ];
    HW_TurnOn( fixed, WordRegs[ CurrProc->state.regs.SP ] );
    HW_TurnOn( fixed, WordRegs[ CurrProc->state.regs.RA ] );
    HW_TurnOn( fixed, WordRegs[ CurrProc->state.regs.BR ] );
    HW_TurnOn( fixed, WordRegs[ CurrProc->state.regs.AR ] );
    HW_TurnOn( fixed, WordRegs[ CurrProc->state.regs.PA ] );
    if( CurrProc->state.attr & ROUTINE_OS ) {
        HW_TurnOn( fixed, WordRegs[ CurrProc->state.regs.SA ] );
    }
    return( fixed );
}


extern  hw_reg_set      LNReg() {
/**********************************/

    return( WordRegs[ CurrProc->state.regs.LN ] );
}


extern  hw_reg_set      RAReg() {
/**********************************/

    return( WordRegs[ CurrProc->state.regs.RA ] );
}


extern  bool            IsStackReg( name *sp ) {
/**********************************************/
    sp = sp;
    return( false );
}


extern  hw_reg_set      StackReg() {
/**********************************/

    return( WordRegs[ CurrProc->state.regs.SP ] );
}


extern  hw_reg_set      GblReg() {
/********************************/

    return( WordRegs[ CurrProc->state.regs.GP ] );
}


extern  hw_reg_set      DisplayReg() {
/************************************/

    return( WordRegs[ CurrProc->state.regs.AR ] );
}


static  hw_reg_set      FirstReg( hw_reg_set regs, hw_reg_set *table ) {
/***********************************************************************

*/
    while( !HW_CEqual( *table, HW_EMPTY ) ) {
        if( HW_Subset( regs, *table ) ) return( *table );
        ++table;
    }
    return( HW_EMPTY );
}


extern hw_reg_set       FirstWordReg( hw_reg_set regs )
/*****************************************************/
{
    return( FirstReg( regs, WordRegs ) );
}


extern hw_reg_set       FirstSingleReg( hw_reg_set regs )
/*****************************************************/
{
    return( FirstReg( regs, SingleRegs ) );
}


extern hw_reg_set       FirstDoubleReg( hw_reg_set regs )
/*****************************************************/
{
    return( FirstReg( regs, DoubleRegs ) );
}


extern  hw_reg_set      WordReg( int i ) {
/****************************************/

    return( WordRegs[ i ] );
}


extern  int     SizeDisplayReg() {
/********************************/

    return( WORD_SIZE );
}


extern  hw_reg_set      AllCacheRegs() {
/**************************************/

    return( HW_EMPTY );
}

extern  hw_reg_set      *IdxRegs() {
/**********************************/

    return( IndexRegs );
}

extern  bool            IsRegPair( hw_reg_set reg ) {
/***************************************************/

    hw_reg_set  *curr;

    for( curr = PairRegs; !HW_CEqual( *curr, HW_EMPTY ); ++curr ) {
        if( HW_Equal( *curr, reg ) ) return( true );
    }
    return( false );
}
