/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS architecture registers.
*
****************************************************************************/


#define MakeReg( c, i ) ( ( (i) & 0xff ) | ( (int)(c) << 8 ) )
#define RegClass( r )   ( ( (r) >> 8 ) & 0xff )
#define RegIndex( r )   ( (r) & 0xff )

#define NULL_REG_IDX    0
#define NULL_REG        MakeReg( RC_NOTHING, NULL_REG_IDX )

#define ZERO_REG_IDX    0   // Contains 0
#define AT_REG_IDX      1   // Reserved for the assembler temporary
#define GP_REG_IDX      28  // Contains the Global pointer
#define SP_REG_IDX      29  // Contains the Stack pointer
#define FP_REG_IDX      30  // Contains the Frame pointer
#define RA_REG_IDX      31  // Contains the Return address

#define AT_REG          MakeReg( RC_GPR, AT_REG_IDX )
#define ZERO_REG        MakeReg( RC_GPR, ZERO_REG_IDX )

typedef uint_16 reg;

typedef enum {
    #define PICK( a, b )    RC_##a,
    #include "regclass.inc"
    #undef PICK
    RC_LAST
} reg_class;

extern char *AsRegName( reg );
