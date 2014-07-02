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
* Description:  MIPS specific definitions for #pragma aux.
*
****************************************************************************/


hw_reg_set DefaultParms[] = {
    HW_D( HW_EMPTY )
};

hw_reg_set DefaultVarParms[] = {
    HW_D( HW_EMPTY )
};

hw_reg_set DefaultLinkage[] = {
    HW_D( HW_R4 ),
    HW_D( HW_R5 ),
    HW_D( HW_R6 ),
    HW_D( HW_R7 ),
};


#define REGS_MAP \
REG_PICK( "zero",  0 ) \
REG_PICK( "at",    1 ) \
REG_PICK( "v0",    2 ) \
REG_PICK( "v1",    3 ) \
REG_PICK( "a0",    4 ) \
REG_PICK( "a1",    5 ) \
REG_PICK( "a2",    6 ) \
REG_PICK( "a3",    7 ) \
REG_PICK( "t0",    8 ) \
REG_PICK( "t1",    9 ) \
REG_PICK( "t2",    10 ) \
REG_PICK( "t3",    11 ) \
REG_PICK( "t4",    12 ) \
REG_PICK( "t5",    13 ) \
REG_PICK( "t6",    14 ) \
REG_PICK( "t7",    15 ) \
REG_PICK( "s0",    16 ) \
REG_PICK( "s1",    17 ) \
REG_PICK( "s2",    18 ) \
REG_PICK( "s3",    19 ) \
REG_PICK( "s4",    20 ) \
REG_PICK( "s5",    21 ) \
REG_PICK( "s6",    22 ) \
REG_PICK( "s7",    23 ) \
REG_PICK( "t8",    24 ) \
REG_PICK( "t9",    25 ) \
REG_PICK( "k0",    26 ) \
REG_PICK( "k1",    27 ) \
REG_PICK( "gp",    28 ) \
REG_PICK( "sp",    29 ) \
REG_PICK( "s8",    30 ) \
REG_PICK( "fp",    30 ) \
REG_PICK( "ra",    31 )

char Registers[] = {
    #define REG_PICK(t,r) t "\0"
    REGS_MAP
    "\0"
    #undef REG_PICK
};

unsigned char RegMap[] = {
    #define REG_PICK(t,r) r,
    REGS_MAP
    #undef REG_PICK
};

hw_reg_set RegBits[] = {
    HW_D( HW_R0 ),
    HW_D( HW_R1 ),
    HW_D( HW_R2 ),
    HW_D( HW_R3 ),
    HW_D( HW_R4 ),
    HW_D( HW_R5 ),
    HW_D( HW_R6 ),
    HW_D( HW_R7 ),
    HW_D( HW_R8 ),
    HW_D( HW_R9 ),
    HW_D( HW_R10 ),
    HW_D( HW_R11 ),
    HW_D( HW_R12 ),
    HW_D( HW_R13 ),
    HW_D( HW_R14 ),
    HW_D( HW_R15 ),
    HW_D( HW_R16 ),
    HW_D( HW_R17 ),
    HW_D( HW_R18 ),
    HW_D( HW_R19 ),
    HW_D( HW_R20 ),
    HW_D( HW_R21 ),
    HW_D( HW_R22 ),
    HW_D( HW_R23 ),
    HW_D( HW_R24 ),
    HW_D( HW_R25 ),
    HW_D( HW_R26 ),
    HW_D( HW_R27 ),
    HW_D( HW_R28 ),
    HW_D( HW_R29 ),
    HW_D( HW_R30 ),
    HW_D( HW_R31 ),
};
