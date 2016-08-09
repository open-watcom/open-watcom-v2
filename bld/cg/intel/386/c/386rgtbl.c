/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Register usage table for 386.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "data.h"
#include "i87data.h"
#include "rgtbl.h"


/* some short forms*/
#define RL_DP   RL_DBL_OR_PTR
#define RL_WS   RL_ANYWORD
#define RL_2B   RL_TWOBYTE
#define RL_IX   RL_INDEX
#define RL_TI   RL_TEMP_INDEX
#define RL_WD   RL_WORD
#define RL_SG   RL_SEG

/*
 * Note: MAX_RG in "rg.h" must be not less then number of elements
 *       in longest of these tables!
 */

static  hw_reg_set      Reg64Order[] = {
    HW_D_1( HW_EAX ),
    HW_D_1( HW_EBX ),
    HW_D_1( HW_ESI ),
    HW_D_1( HW_EDI ),
    HW_D_1( HW_EDX ),
    HW_D_1( HW_ECX ),
    HW_D_1( HW_BP ),
    HW_D_1( HW_SP ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      Empty[] = {
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      ALReg[] = {
    HW_D_1( HW_AL ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      AHReg[] = {
    HW_D_1( HW_AH ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      AXReg[] = {
    HW_D_1( HW_AX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      EAXReg[] = {
    HW_D_1( HW_EAX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      ECXReg[] = {
    HW_D_1( HW_ECX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      CLReg[] = {
    HW_D_1( HW_CL ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      CXReg[] = {
    HW_D_1( HW_CX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      DXReg[] = {
    HW_D_1( HW_DX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      EDXReg[] = {
    HW_D_1( HW_EDX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      DXAXReg[] = {
    HW_D_2( HW_DX, HW_AX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      DXEAXReg[] = {
    HW_D_2( HW_DX, HW_EAX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      EDXEAXReg[] = {
    HW_D_2( HW_EDX, HW_EAX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      FPParm2Reg[] = {
    HW_D_1( HW_EMPTY ),         /* filled in by InitRegTbl */
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      ByteRegs[] = {
    HW_D_1( HW_AL ),
    HW_D_1( HW_AH ),
    HW_D_1( HW_DL ),
    HW_D_1( HW_DH ),
    HW_D_1( HW_BL ),
    HW_D_1( HW_BH ),
    HW_D_1( HW_CL ),
    HW_D_1( HW_CH ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      WordOrSegReg[] = {
    HW_D_1( HW_AX ),
    HW_D_1( HW_DX ),
    HW_D_1( HW_BX ),
    HW_D_1( HW_CX ),
    HW_D_1( HW_SI ),
    HW_D_1( HW_DI ),
    HW_D_1( HW_DS ),
    HW_D_1( HW_ES ),
    HW_D_1( HW_FS ),
    HW_D_1( HW_GS ),
    HW_D_1( HW_CS ),
    HW_D_1( HW_SS ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      WordRegs[] = {
    HW_D_1( HW_AX ),
    HW_D_1( HW_DX ),
    HW_D_1( HW_BX ),
    HW_D_1( HW_CX ),
    HW_D_1( HW_SI ),
    HW_D_1( HW_DI ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      TwoByteRegs[] = {
    HW_D_1( HW_AX ),
    HW_D_1( HW_DX ),
    HW_D_1( HW_BX ),
    HW_D_1( HW_CX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      SegRegs[] = {
    HW_D_1( HW_DS ),
    HW_D_1( HW_ES ),
    HW_D_1( HW_FS ),
    HW_D_1( HW_GS ),
    HW_D_1( HW_CS ),
    HW_D_1( HW_SS ),
    HW_D_1( HW_EMPTY )
};
static hw_reg_set       FarPointerRegs[] = {

    HW_D_2( HW_DX, HW_EAX ),
    HW_D_2( HW_CX, HW_EBX ),
    HW_D_2( HW_CX, HW_EAX ),
    HW_D_2( HW_CX, HW_ESI ),
    HW_D_2( HW_DX, HW_EBX ),
    HW_D_2( HW_DI, HW_EAX ),
    HW_D_2( HW_CX, HW_EDI ),
    HW_D_2( HW_DX, HW_ESI ),
    HW_D_2( HW_DI, HW_EBX ),
    HW_D_2( HW_SI, HW_EAX ),
    HW_D_2( HW_CX, HW_EDX ),
    HW_D_2( HW_DX, HW_EDI ),
    HW_D_2( HW_DI, HW_ESI ),
    HW_D_2( HW_SI, HW_EBX ),
    HW_D_2( HW_BX, HW_EAX ),

    HW_D_2( HW_DS, HW_EAX ),
    HW_D_2( HW_DS, HW_EDX ),
    HW_D_2( HW_DS, HW_EBX ),
    HW_D_2( HW_DS, HW_ECX ),
    HW_D_2( HW_DS, HW_ESI ),
    HW_D_2( HW_DS, HW_EDI ),
    HW_D_2( HW_DS, HW_BP ),

    HW_D_2( HW_ES, HW_EAX ),
    HW_D_2( HW_ES, HW_EDX ),
    HW_D_2( HW_ES, HW_EBX ),
    HW_D_2( HW_ES, HW_ECX ),
    HW_D_2( HW_ES, HW_ESI ),
    HW_D_2( HW_ES, HW_EDI ),
    HW_D_2( HW_ES, HW_BP ),

    HW_D_2( HW_FS, HW_EAX ),
    HW_D_2( HW_FS, HW_EDX ),
    HW_D_2( HW_FS, HW_EBX ),
    HW_D_2( HW_FS, HW_ECX ),
    HW_D_2( HW_FS, HW_ESI ),
    HW_D_2( HW_FS, HW_EDI ),
    HW_D_2( HW_FS, HW_BP ),

    HW_D_2( HW_GS, HW_EAX ),
    HW_D_2( HW_GS, HW_EDX ),
    HW_D_2( HW_GS, HW_EBX ),
    HW_D_2( HW_GS, HW_ECX ),
    HW_D_2( HW_GS, HW_ESI ),
    HW_D_2( HW_GS, HW_EDI ),
    HW_D_2( HW_GS, HW_BP ),

    HW_D_2( HW_SS, HW_EAX ),
    HW_D_2( HW_SS, HW_EDX ),
    HW_D_2( HW_SS, HW_EBX ),
    HW_D_2( HW_SS, HW_ECX ),
    HW_D_2( HW_SS, HW_ESI ),
    HW_D_2( HW_SS, HW_EDI ),
    HW_D_2( HW_SS, HW_BP ),

    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      LongIndexRegs[] = {
    HW_D_2( HW_DS, HW_EAX ),
    HW_D_2( HW_DS, HW_EDX ),
    HW_D_2( HW_DS, HW_EBX ),
    HW_D_2( HW_DS, HW_ECX ),
    HW_D_2( HW_DS, HW_ESI ),
    HW_D_2( HW_DS, HW_EDI ),
    HW_D_2( HW_DS, HW_BP ),

    HW_D_2( HW_ES, HW_EAX ),
    HW_D_2( HW_ES, HW_EDX ),
    HW_D_2( HW_ES, HW_EBX ),
    HW_D_2( HW_ES, HW_ECX ),
    HW_D_2( HW_ES, HW_ESI ),
    HW_D_2( HW_ES, HW_EDI ),
    HW_D_2( HW_ES, HW_BP ),

    HW_D_2( HW_FS, HW_EAX ),
    HW_D_2( HW_FS, HW_EDX ),
    HW_D_2( HW_FS, HW_EBX ),
    HW_D_2( HW_FS, HW_ECX ),
    HW_D_2( HW_FS, HW_ESI ),
    HW_D_2( HW_FS, HW_EDI ),
    HW_D_2( HW_FS, HW_BP ),

    HW_D_2( HW_GS, HW_EAX ),
    HW_D_2( HW_GS, HW_EDX ),
    HW_D_2( HW_GS, HW_EBX ),
    HW_D_2( HW_GS, HW_ECX ),
    HW_D_2( HW_GS, HW_ESI ),
    HW_D_2( HW_GS, HW_EDI ),
    HW_D_2( HW_GS, HW_BP ),

    HW_D_2( HW_SS, HW_EAX ),
    HW_D_2( HW_SS, HW_EDX ),
    HW_D_2( HW_SS, HW_EBX ),
    HW_D_2( HW_SS, HW_ECX ),
    HW_D_2( HW_SS, HW_ESI ),
    HW_D_2( HW_SS, HW_EDI ),
    HW_D_2( HW_SS, HW_BP ),

    HW_D_2( HW_CS, HW_EAX ),
    HW_D_2( HW_CS, HW_EDX ),
    HW_D_2( HW_CS, HW_EBX ),
    HW_D_2( HW_CS, HW_ECX ),
    HW_D_2( HW_CS, HW_ESI ),
    HW_D_2( HW_CS, HW_EDI ),
    HW_D_2( HW_CS, HW_BP ),

    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      ABCDRegs[] = {
    HW_D_1( HW_EAX ),
    HW_D_1( HW_EDX ),
    HW_D_1( HW_EBX ),
    HW_D_1( HW_ECX ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      DoubleRegs[] = {
    HW_D_1( HW_EAX ),
    HW_D_1( HW_EDX ),
    HW_D_1( HW_ECX ),
    HW_D_1( HW_EBX ),
    HW_D_1( HW_ESI ),
    HW_D_1( HW_EDI ),
    HW_D_1( HW_BP ),
    HW_D_1( HW_SP ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      DoubleParmRegs[] = {
    HW_D_1( HW_EAX ),
    HW_D_1( HW_EDX ),
    HW_D_1( HW_EBX ),
    HW_D_1( HW_ECX ),
    HW_D_1( HW_ESI ),
    HW_D_1( HW_EDI ),
    HW_D_1( HW_BP ),
    HW_D_1( HW_SP ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      QuadReg[] = {
    HW_D_2( HW_EDX, HW_EAX ),
    HW_D_2( HW_ECX, HW_EBX ),
    HW_D_2( HW_ECX, HW_EAX ),
    HW_D_2( HW_ECX, HW_ESI ),
    HW_D_2( HW_EDX, HW_EBX ),
    HW_D_2( HW_EDI, HW_EAX ),
    HW_D_2( HW_ECX, HW_EDI ),
    HW_D_2( HW_EDX, HW_ESI ),
    HW_D_2( HW_EDI, HW_EBX ),
    HW_D_2( HW_ESI, HW_EAX ),
    HW_D_2( HW_ECX, HW_EDX ),
    HW_D_2( HW_EDX, HW_EDI ),
    HW_D_2( HW_EDI, HW_ESI ),
    HW_D_2( HW_ESI, HW_EBX ),
    HW_D_2( HW_EBX, HW_EAX ),
    HW_D_2( HW_BP,  HW_EAX ),
    HW_D_2( HW_BP,  HW_EDX ),
    HW_D_2( HW_BP,  HW_EBX ),
    HW_D_2( HW_BP,  HW_ECX ),
    HW_D_2( HW_BP,  HW_ESI ),
    HW_D_2( HW_BP,  HW_EDI ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      ST0Reg[] = {
    HW_D_1( HW_ST0 ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      STIReg[] = {
    HW_D_1( HW_ST1 ),
    HW_D_1( HW_ST2 ),
    HW_D_1( HW_ST3 ),
    HW_D_1( HW_ST4 ),
    HW_D_1( HW_ST5 ),
    HW_D_1( HW_ST6 ),
    HW_D_1( HW_ST7 ),
    HW_D_1( HW_EMPTY )
};
static  hw_reg_set      STParmReg[] = {
    HW_D_1( HW_ST1 ),
    HW_D_1( HW_ST2 ),
    HW_D_1( HW_ST3 ),
    HW_D_1( HW_ST4 ),
    HW_D_1( HW_ST5 ),
    HW_D_1( HW_ST6 ),
    HW_D_1( HW_ST7 ),
    HW_D_1( HW_EMPTY ),
    HW_D_1( HW_EMPTY )
};

hw_reg_set      *RegSets[] = {
/*****************************
    define the possible register lists
*/
    #define RL(a,b,c,d) a
    #include "rl.h"
    #undef RL
    NULL
};

op_regs RegList[] = {
/********************
    define the possible combinations of register lists for operands/results
*/
    #define RG( a,b,c,d,e,f ) {a,b,c,d,e}
    #include "rg.h"
    #undef RG
};

static  reg_set_index   ClassSets[] = {
/**************************************
       define the class set associated with a given register set
       RL_ indicates that the class is already identified
       (e.g. I1 already identified as U1)
*/
    RL_BYTE,                /* U1 */
    RL_,                    /* I1 */
    RL_WORD,                /* U2 */
    RL_SEG,                 /* I2 */ /* (put segs here so their size is OK) */
    RL_DOUBLE,              /* U4 */
    RL_DX_AX,               /* I4 */ /* for IDIV instructions */
    RL_8,                   /* U8 */
    RL_8,                   /* I8 */
    RL_FAR_POINTER,         /* CP */
    RL_,                    /* PT */
    RL_,                    /* FS */
    RL_8,                   /* FD */
    RL_8,                   /* FL */ // FIXME - long double
    RL_                     /* XX */
};

static  hw_reg_set      *ParmSets[] = {
/**************************************
    define the set of register that a parameter of a given class could use
*/
    ByteRegs,               /* U1 */
    ByteRegs,               /* I1 */
    WordRegs,               /* U2 */
    WordRegs,               /* I2 */
    DoubleParmRegs,         /* U4 */
    DoubleParmRegs,         /* I4 */
    QuadReg,                /* U8 */
    QuadReg,                /* I8 */
    FarPointerRegs,         /* CP */
    FarPointerRegs,         /* PT */
    DoubleParmRegs,         /* FS */
    QuadReg,                /* FD */
    Empty,                  /* FL */
    Empty                   /* XX */
};

static  hw_reg_set      *ParmSets8087[] = {
/******************************************
    define the set of register that a parameter of a given class could use
    when generating 8087 code
*/
    ByteRegs,               /* U1 */
    ByteRegs,               /* I1 */
    WordRegs,               /* U2 */
    WordRegs,               /* I2 */
    DoubleParmRegs,         /* U4 */
    DoubleParmRegs,         /* I4 */
    QuadReg,                /* U8 */
    QuadReg,                /* I8 */
    FarPointerRegs,         /* CP */
    FarPointerRegs,         /* PT */
    STParmReg,              /* FS */
    STParmReg,              /* FD */
    STParmReg,              /* FL */
    Empty                   /* XX */
};

static  reg_set_index   IsSets[] = {
/***********************************
    define the normal register list associated with a given class
*/
    RL_BYTE,                /* U1 */
    RL_BYTE,                /* I1 */
    RL_WORD,                /* U2 */
    RL_WORD,                /* I2 */
    RL_DOUBLE,              /* U4 */
    RL_DOUBLE,              /* I4 */
    RL_8,                   /* U8 */
    RL_8,                   /* I8 */
    RL_FAR_POINTER,         /* CP */
    RL_FAR_POINTER,         /* PT */
    RL_DOUBLE,              /* FS */
    RL_8,                   /* FD */
    RL_,                    /* FL */
    RL_                     /* XX */
};

static  reg_set_index   ReturnSets[] = {
/***************************************
    define the set a register normally used to return a given class
*/
    RL_AL,                  /* U1 */
    RL_AL,                  /* I1 */
    RL_AX,                  /* U2 */
    RL_AX,                  /* I2 */
    RL_EAX,                 /* U4 */
    RL_EAX,                 /* I4 */
    RL_EDX_EAX,             /* U8 */
    RL_EDX_EAX,             /* I8 */
    RL_DX_EAX,              /* CP */
    RL_,                    /* PT */
    RL_EAX,                 /* FS */
    RL_EDX_EAX,             /* FD */
    RL_,                    /* FL */
    RL_                     /* XX */
};

static  reg_set_index   Return8087[] = {
/***************************************
    define the set a register normally used to return a given class with 8087
*/
    RL_AL,                  /* U1 */
    RL_AL,                  /* I1 */
    RL_AX,                  /* U2 */
    RL_AX,                  /* I2 */
    RL_EAX,                 /* U4 */
    RL_EAX,                 /* I4 */
    RL_EDX_EAX,             /* U8 */
    RL_EDX_EAX,             /* I8 */
    RL_DX_EAX,              /* CP */
    RL_,                    /* PT */
    RL_ST0,                 /* FS */
    RL_ST0,                 /* FD */
    RL_ST0,                 /* FL */
    RL_                     /* XX */
};

static  reg_set_index   IndexSets[] = {
/**************************************
    define the sets of index registers of a given class
*/
    RL_,                    /* U1 */
    RL_,                    /* I1 */
    RL_,                    /* U2 */
    RL_,                    /* I2 */
    RL_DOUBLE,              /* U4 */
    RL_DOUBLE,              /* I4 */
    RL_,                    /* U8 */
    RL_,                    /* I8 */
    RL_LONG_INDEX,          /* CP */
    RL_LONG_INDEX,          /* PT */
    RL_,                    /* FS */
    RL_,                    /* FD */
    RL_,                    /* FL */
    RL_                     /* XX */
};

/*
   Information for register set intersections
       if sets are of different classes
            intersection is empty
       else
            intersection given in square matrix for class
 */

typedef enum {
    ONE_BYTE,
    TWO_BYTE,
    FOUR_BYTE,
    SIX_BYTE,
    EIGHT_BYTE,
    FLOATING,
    OTHER
} intersect_class;


typedef struct reg_class {
    intersect_class class;
    byte            index;
} reg_class;

static  reg_class       IntersectInfo[] = {
    #define RL(a,b,c,d) {c,d}
    #include "rl.h"
    #undef RL
    {OTHER, 0}          /* RL_NUMBER_OF_SETS */
};

static  byte    Width[] = {
    4,              /* ONE_BYTE   */
    7,              /* TWO_BYTE   */
    6,              /* FOUR_BYTE  */
    3,              /* SIX_BYTE   */
    3,              /* EIGHT_BYTE */
    2,              /* FLOATING   */
    0               /* OTHER      */
};

static  reg_set_index   OneByteInter[] = {
/*  AL      AH      CL      BYTE                */
    RL_AL,  RL_,    RL_,    RL_AL,      /* AL   */
    RL_,    RL_AH,  RL_,    RL_AH,      /* AH   */
    RL_,    RL_,    RL_CL,  RL_CL,      /* CL   */
    RL_AL,  RL_AH,  RL_CL,  RL_BYTE     /* BYTE */
};


/* short forms used in this table*/
/* - RL_WD == RL_WORD*/
/* - RL_2B == RL_TWOBYTE*/
/* - RL_WS == RL_ANYWORD*/
/* - RL_SG == RL_SEG*/

static  reg_set_index   TwoByteInter[] = {
/*  AX     CX     DX     WD     2B     SG     WS            */
    RL_AX, RL_,   RL_,   RL_AX, RL_AX, RL_,   RL_AX,  /* AX */
    RL_,   RL_CX, RL_,   RL_CX, RL_CX, RL_,   RL_CX,  /* CX */
    RL_,   RL_,   RL_DX, RL_DX, RL_DX, RL_,   RL_DX,  /* DX */
    RL_AX, RL_CX, RL_DX, RL_WD, RL_2B, RL_,   RL_WD,  /* WD */
    RL_AX, RL_CX, RL_CX, RL_2B, RL_2B, RL_,   RL_2B,  /* 2B */
    RL_,   RL_,   RL_,   RL_,   RL_,   RL_SG, RL_SG,  /* SG */
    RL_AX, RL_CX, RL_DX, RL_WD, RL_2B, RL_SG, RL_WS   /* WS */
};

static  reg_set_index   FourByteInter[] = {
/*  RL_EAX     RL_EDX      RL_DX_AX    RL_DOUBLE   RL_ABCD  RL_ECX              */
    RL_EAX,    RL_,        RL_,        RL_EAX,     RL_EAX,  RL_,   /* RL_EAX    */
    RL_,       RL_EDX,     RL_,        RL_EDX,     RL_EDX,  RL_,   /* RL_EDX    */
    RL_,       RL_,        RL_DX_AX,   RL_,        RL_,     RL_,   /* RL_DX_AX  */
    RL_EAX,    RL_EDX,     RL_,        RL_DOUBLE,  RL_ABCD, RL_,   /* RL_DOUBLE */
    RL_EAX,    RL_EDX,     RL_,        RL_ABCD,    RL_ABCD, RL_,   /* RL_ABCD   */
    RL_,       RL_,        RL_,        RL_,        RL_,     RL_ECX /* RL_ECX    */
};

static  reg_set_index   SixByteInter[] = {
/*  RL_DX_EAX    RL_LONG_INDEX     RL_FAR_POINTER                    */
    RL_DX_EAX,   RL_,              RL_DX_EAX,      /* RL_DX_EAX      */
    RL_,         RL_LONG_INDEX,    RL_LONG_INDEX,  /* RL_LONG_INDEX  */
    RL_DX_EAX,   RL_LONG_INDEX,    RL_FAR_POINTER  /* RL_FAR_POINTER */
};


static  reg_set_index   EightByteInter[] = {
/*  RL_EDX_EAX      RL_FPPARM2      RL_8                      */
    RL_EDX_EAX,     RL_,            RL_EDX_EAX, /* RL_EDX_EAX */
    RL_,            RL_FPPARM2,     RL_FPPARM2, /* RL_FPPARM2 */
    RL_EDX_EAX,     RL_FPPARM2,     RL_8        /* RL_8       */
};

static  reg_set_index   FloatingInter[] = {
/*  RL_ST0  RL_STI               */
    RL_ST0, RL_,      /*  RL_ST0 */
    RL_,    RL_STI    /*  RL_STI */
};

static  reg_set_index   OtherInter[] = {
    RL_
};

static  reg_set_index   *InterTable[] = {
    OneByteInter,           /* ONE_BYTE   */
    TwoByteInter,           /* TWO_BYTE   */
    FourByteInter,          /* FOUR_BYTE  */
    SixByteInter,           /* SIX_BYTE   */
    EightByteInter,         /* EIGHT_BYTE */
    FloatingInter,          /* FLOATING   */
    OtherInter              /* others     */
};

void            InitRegTbl( void )
/*********************************
    Initialize the tables.
*/
{
    if( _FPULevel( FPU_87 ) ) {
        HW_CAsgn( STParmReg[Max87Stk], HW_EMPTY );
    }
    if( _IsTargetModel( INDEXED_GLOBALS ) ) {
        HW_CAsgn( FPParm2Reg[0], HW_ECX_ESI );
    } else {
        HW_CAsgn( FPParm2Reg[0], HW_ECX_EBX );
    }
}


reg_set_index   RegIntersect( reg_set_index s1, reg_set_index s2 )
/*****************************************************************
    return the intersection of two register lists
*/
{
    reg_class           *set1;
    reg_class           *set2;
    intersect_class     class;
    reg_set_index       result;

    set1 = & IntersectInfo[s1];
    set2 = & IntersectInfo[s2];
    class = set1->class;
    if( class == set2->class ) {
        result = InterTable[class][set1->index * Width[class] + set2->index];
    } else if( s1 == RL_NUMBER_OF_SETS ) {
        result = s2;
    } else if( s2 == RL_NUMBER_OF_SETS ) {
        result = s1;
    } else {
        result = RL_;
    }
    return( result );
}


hw_reg_set      *ParmChoices( type_class_def class )
/***************************************************
    return a list of register which could be used to return type "class"
*/
{
    hw_reg_set  *list;

    if( _FPULevel( FPU_87 ) ) {
        list = ParmSets8087[class];
    } else {
        list = ParmSets[class];
    }
    return( list );
}


hw_reg_set      InLineParm( hw_reg_set regs, hw_reg_set used )
/*************************************************************
    for parm [8087]. returns the next available 8087 parameter register
*/
{
    if( HW_COvlap( regs, HW_FLTS ) ) {
        HW_CAsgn( regs, HW_EMPTY );
        if( HW_COvlap( used, HW_ST4 ) ) {
            /*regs = EMPTY;*/
        } else if( HW_COvlap( used, HW_ST3 ) ) {
            HW_CTurnOn( regs, HW_ST4 );
        } else if( HW_COvlap( used, HW_ST2 ) ) {
            HW_CTurnOn( regs, HW_ST3 );
        } else if( HW_COvlap( used, HW_ST1 ) ) {
            HW_CTurnOn( regs, HW_ST2 );
        } else {
            HW_CTurnOn( regs, HW_ST1 );
        }
    }
    return( regs );
}


hw_reg_set      StructReg( void )
/*******************************/
{
    return( HW_ESI );
}


hw_reg_set      ReturnReg( type_class_def class, bool use_87 )
/*************************************************************
    return the "normal" return register used for type "class"
*/
{
    hw_reg_set          *list;

    if( _FPULevel( FPU_87 ) && use_87 ) {
        list = RegSets[Return8087[class]];
    } else {
        list = RegSets[ReturnSets[class]];
    }
    return( *list );
}


reg_set_index   SegIndex( void )
/******************************/
{
    return( RL_SEG );
}


reg_set_index   NoSegments( reg_set_index idx )
/**********************************************
    return a register list like "idx" that doesn't include any segment regs
*/
{
    if( idx == RL_ANYWORD ) {
        idx = RL_WORD;
    }
    return( idx );
}


reg_set_index   IndexIntersect( reg_set_index curr,
                                       type_class_def class,
                                       bool is_temp_index )
/***********************************************************
    return the intersection of "curr" with the set of index
    registers of type "class"
*/
{
    is_temp_index = is_temp_index;
    return( RegIntersect( curr, IndexSets[class] ) );
}


bool    IsIndexReg( hw_reg_set reg, type_class_def class,
                            bool is_temp_index )
/****************************************************************
    return true if "reg" can be used as an index of type "class"
*/
{
    hw_reg_set  *list;

    is_temp_index = is_temp_index;
    for( list = RegSets[IndexSets[class]]; !HW_CEqual( *list, HW_EMPTY ); ++list ) {
        if( HW_Equal( *list, reg ) ) {
            break;
        }
    }
    return( HW_Equal( *list, reg ) );
}

static  type_class_def  NotFloatRegClass( hw_reg_set regs )
/*********************************************************/
{
    hw_reg_set          *possible;
    type_class_def      class;

    for( class = U1; class < XX; ++class ) {
        for( possible = RegSets[ClassSets[class]]; !HW_CEqual( *possible, HW_EMPTY ); ++possible ) {
            if( HW_Equal( *possible, regs ) ) {
                return( class );
            }
        }
    }
    if( HW_COvlap( regs, HW_SEGS ) ) { // a kludge to get GS:[EAX+EDX]
        HW_CTurnOff( regs, HW_SEGS );
        if( NotFloatRegClass( regs ) == FD ) {
            return( CP );
        }
    }
    return( XX );
}

type_class_def  RegClass( hw_reg_set regs )
/******************************************
    return the type associated with "regs".
*/
{
    hw_reg_set          *possible;

    if( HW_COvlap( regs, HW_FLTS ) ) {
        if( HW_CEqual( regs, HW_ST0 ) )
            return( FD );
        for( possible = STIReg; !HW_CEqual( *possible, HW_EMPTY ); ++possible ) {
            if( HW_Equal( regs, *possible ) ) {
                return( FD );
            }
        }
        return( XX );
    } else {
        return( NotFloatRegClass( regs ) );
    }
}

bool    IndexRegOk( hw_reg_set reg, bool is_temp_index )
/*******************************************************
    return true if "reg" is ok to be used as in index reg.
    "is_temp_index" means we'll also need to incorporate the
    AR/BP register into the index, since we're indexing an auto
*/
{
    hw_reg_set  *list;

    is_temp_index = is_temp_index;
    if( RegClass( reg ) == U4 ) {
        list = RegSets[RL_DOUBLE];
    } else {
        list = RegSets[RL_LONG_INDEX];
    }
    for( ; !HW_CEqual( *list, HW_EMPTY ); ++list ) {
        if( HW_Equal( *list, reg ) ) {
            break;
        }
    }
    return( HW_Equal( *list, reg ) );
}


bool    IsSegReg( hw_reg_set regs )
/**********************************
    return true if "regs" is a segment register
*/
{
    hw_reg_set  tmp;

    tmp = regs;
    HW_COnlyOn( tmp, HW_SEGS );
    return( HW_Equal( tmp, regs ) );
}


hw_reg_set      Low16Reg( hw_reg_set regs )
/******************************************
    return the low order part of 16 bit register "regs"
*/
{
    HW_COnlyOn( regs, HW_AL_BL_CL_DL );
    return( regs );
}


hw_reg_set      High16Reg( hw_reg_set regs )
/*******************************************
    return the high order part of 16 bit register "regs"
*/
{
    HW_COnlyOn( regs, HW_AH_BH_CH_DH );
    return( regs );
}


hw_reg_set      Low32Reg( hw_reg_set regs )
/******************************************
    return the low order part of 32 bit register "regs"
*/
{
    if( HW_CEqual( regs, HW_DX_AX ) ) {
        return( HW_AX );
    }
    HW_COnlyOn( regs, HW_AX_BX_CX_DX_SI_DI );
    return( regs );
}


hw_reg_set      High32Reg( hw_reg_set regs )
/*******************************************
    return the high order part of 32 bit register "regs"
*/
{
    if( HW_CEqual( regs, HW_DX_AX ) ) {
        return( HW_DX );
    }
    return( HW_EMPTY );
}

hw_reg_set      FullReg( hw_reg_set regs )
/*****************************************
    given a register (eg AL), return the full register (eg EAX)
*/
{
    if( HW_COvlap( regs, HW_EAX ) ) {
        HW_CTurnOn( regs, HW_EAX );
    }
    if( HW_COvlap( regs, HW_EBX ) ) {
        HW_CTurnOn( regs, HW_EBX );
    }
    if( HW_COvlap( regs, HW_ECX ) ) {
        HW_CTurnOn( regs, HW_ECX );
    }
    if( HW_COvlap( regs, HW_EDX ) ) {
        HW_CTurnOn( regs, HW_EDX );
    }
    if( HW_COvlap( regs, HW_EDI ) ) {
        HW_CTurnOn( regs, HW_EDI );
    }
    if( HW_COvlap( regs, HW_ESI ) ) {
        HW_CTurnOn( regs, HW_ESI );
    }
    return( regs );
}

hw_reg_set      Low48Reg( hw_reg_set regs )
/******************************************
    return the low order part of 48 bit register "regs"
*/
{
    if( HW_CEqual( regs, HW_EMPTY ) ) return( HW_EMPTY );
    if( HW_COvlap( regs, HW_SEGS ) ) {
        HW_CTurnOff( regs, HW_SEGS );
        return( regs );
    }
    HW_COnlyOn( regs, HW_32 );
    return( FullReg( regs ) );
}


hw_reg_set      High48Reg( hw_reg_set regs )
/*******************************************
    return the high order part of 48 bit register "regs"
*/
{
    hw_reg_set  high;

    high = Low48Reg( regs );
    if( !HW_CEqual( high, HW_EMPTY ) ) {
        HW_TurnOff( regs, high );
        return( regs );
    }
    return( high );
}

hw_reg_set      Low64Reg( hw_reg_set regs )
/******************************************
    return the low order part of 64 bit register "regs"
*/
{
    hw_reg_set  low;
    hw_reg_set  *order;

    if( HW_CEqual( regs, HW_EMPTY ) ) return( HW_EMPTY );
    order = Reg64Order;
    for( ;; ) {
        if( HW_Ovlap( *order, regs ) ) break;
        ++order;
    }
    low = regs;
    HW_OnlyOn( low, *order );
    if( HW_Equal( low, regs ) ) {
        low = HW_EMPTY;
    }
    return( low );
}


hw_reg_set      High64Reg( hw_reg_set regs )
/*******************************************
    return the high order part of 64 bit register "regs"
*/
{
    hw_reg_set  high;

    high = Low64Reg( regs );
    if( !HW_CEqual( high, HW_EMPTY ) ) {
        HW_TurnOff( regs, high );
        return( regs );
    }
    return( high );
}



hw_reg_set      HighReg( hw_reg_set regs )
/*****************************************
    return the high order portion of "regs"
*/
{
    switch( RegClass( regs ) ) {
    case CP:
        HW_COnlyOn( regs, HW_SEGS );
        return( regs );
    case FD:
    case U8:
    case I8:
        return( High64Reg( regs ) );
    case U4:
    case I4:
        return( High32Reg( regs ) );
    case U2:
        return( High16Reg( regs ) );
    default:
        return( HW_EMPTY );
    }
}

hw_reg_set      HighOffsetReg( hw_reg_set regs )
/***********************************************
    return the portion of "regs" which would occupy the high memory address
*/
{
    return( HighReg( regs ) );
}


hw_reg_set      LowReg( hw_reg_set regs )
/****************************************
    return the low order portion of "regs"
*/
{
    switch( RegClass( regs ) ) {
    case CP:
        HW_CTurnOff( regs, HW_SEGS );
        return( regs );
    case U8:
    case I8:
    case FD:
        return( Low64Reg( regs ) );
    case U4:
        return( Low32Reg( regs ) );
    case U2:
        return( Low16Reg( regs ) );
    default:
        return( HW_EMPTY );
    }
}

hw_reg_set      LowOffsetReg( hw_reg_set regs )
/**********************************************
    return the portion of "regs" which would occupy the low memory address
*/
{
    return( LowReg( regs ) );
}



bool    IsRegClass( hw_reg_set regs, type_class_def class )
/**********************************************************
    return true if "regs" has type "class" (eg I4, U4, etc)
*/
{
    hw_reg_set  *list;

    for( list = RegSets[IsSets[class]]; !HW_Equal( *list, HW_EMPTY ); ++list ) {
        if( HW_Equal( *list, regs ) ) {
            break;
        }
    }
    return( !HW_Equal( *list, HW_EMPTY ) );
}


hw_reg_set      ActualParmReg( hw_reg_set reg )
/**********************************************
    given a register "reg", to be used to pass a parameter,
    decide which register name should really be used in
    the instruction generated to load it.
*/
{
    if( HW_COvlap( reg, HW_FLTS ) ) {
        HW_CAsgn( reg, HW_ST0 );
    }
    return( reg );
}

hw_reg_set      FixedRegs( void )
/********************************
    return the set of register which may not be modified within this routine
*/
{
    hw_reg_set  fixed;

    HW_CAsgn( fixed, HW_SP );
    HW_CTurnOn( fixed, HW_BP );
    HW_CTurnOn( fixed, HW_SS );
    HW_CTurnOn( fixed, HW_CS );
    if( _IsntTargetModel( FLOATING_DS ) ) HW_CTurnOn( fixed, HW_DS );
    if( _IsntTargetModel( FLOATING_ES ) ) HW_CTurnOn( fixed, HW_ES );
    if( _IsntTargetModel( FLOATING_FS ) ) HW_CTurnOn( fixed, HW_FS );
    if( _IsntTargetModel( FLOATING_GS ) ) HW_CTurnOn( fixed, HW_GS );
    if( _IsTargetModel( INDEXED_GLOBALS ) ) HW_CTurnOn( fixed, HW_EBX );
    return( fixed );
}


bool    IsStackReg( name *sp )
/****************************/
{
    if( sp == NULL ) return( false );
    if( sp->n.class != N_REGISTER ) return( false );
    if( !HW_CEqual( sp->r.reg, HW_SP ) ) return( false );
    return( true );
}


hw_reg_set      StackReg( void )
/******************************/
{
    return( HW_SP );
}


hw_reg_set      DisplayReg( void )
/********************************/
{
    if( CurrProc->targ.sp_frame ) return( HW_SP );
    return( HW_BP );
}


int     SizeDisplayReg( void )
/*****************************
    return the size of the "pascal" display register entry on the stack
*/
{
    return( WORD_SIZE );
}


hw_reg_set      AllCacheRegs( void )
/***********************************
    return the set of all registers that could be used to cache values
*/
{
    hw_reg_set  all;

    HW_CAsgn( all, HW_FLTS );
    HW_CTurnOn( all, HW_EAX );
    HW_CTurnOn( all, HW_EDX );
    HW_CTurnOn( all, HW_EBX );
    HW_CTurnOn( all, HW_ECX );
    HW_CTurnOn( all, HW_ESI );
    HW_CTurnOn( all, HW_EDI );
    HW_CTurnOn( all, HW_ES );
    HW_CTurnOn( all, HW_FS );
    HW_CTurnOn( all, HW_GS );
    if( _IsTargetModel( FLOATING_DS ) ) HW_CTurnOn( all, HW_DS );
    if( _IsTargetModel( INDEXED_GLOBALS ) ) HW_CTurnOff( all, HW_EBX );
    return( all );
}

hw_reg_set      *IdxRegs( void )
/*******************************
    return a pointer to the set of "indexable" registers
*/
{
    return( DoubleRegs );
}

hw_reg_set      FirstReg( reg_set_index index )
/**********************************************
    The table RTInfo[] uses reg_set_indexes instead of hw_reg_sets since
    they are only one byte long.  This retrieves the first hw_reg_set
    from the reg_set table "index".

    the tables above use RL_ consts rather that hw_reg_sets cause
    it cheaper. This just picks off the first register from a
    register list and returns it.
*/
{
    return( *RegSets[index] );
}
