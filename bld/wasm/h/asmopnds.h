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


#define OP_NONE     0
#define OP_CL       0x00000001
#define OP_AL       0x00000002
#define OP_R8_GEN   0x00000004
#define OP_R8       ( OP_CL | OP_AL | OP_R8_GEN )

#define OP_AX       0x00000008
#define OP_DX       0x00000010
#define OP_R16_GEN  0x00000020
#define OP_R16      ( OP_AX | OP_DX | OP_R16_GEN )

#define OP_EAX      0x00000040
#define OP_R32_GEN  0x00000080
#define OP_R32      ( OP_EAX | OP_R32_GEN )

#define OP_R        ( OP_R8 | OP_R16 | OP_R32 )
#define OP_R1632    ( OP_R16 | OP_R32 )
#define OP_A        ( OP_AL | OP_AX | OP_EAX )
#define OP_MMX      ( OP_EAX | OP_DX | OP_CL )  //Kludge because of no bits

#define OP_I8       0x00000100
#define OP_I_1      0x00000200
#define OP_I_3      0x00000400
#define OP_I8_U     0x00000800
#define OP_I16      0x00001000
#define OP_I32      0x00002000
#define OP_J32      0x00004000
#define OP_J48      0x00008000
#define OP_I        ( OP_I8 | OP_I_1 | OP_I_3 | OP_I8_U | OP_I16 | OP_I32 )
#define OP_GE_8     ( OP_I8 | OP_I8_U | OP_I16 | OP_I32 )
#define OP_GE_16    ( OP_I16 | OP_I32 )
#define OP_GE_U8    ( OP_I8_U | OP_I16 | OP_I32 )

#define OP_M_B      0x00010000
#define OP_M_W      0x00020000
#define OP_M_DW     0x00040000
#define OP_M_QW     0x00080000
#define OP_M_TB     0x00100000

#define OP_M8       0x00200000
#define OP_M16      0x00400000
#define OP_M32      0x00800000

// we are out of space so ...
#define OP_M16_IND  ( OP_M8 | OP_M16 )          // indirect jmp - word ptr
#define OP_M32_IND  ( OP_M16 | OP_M32 )         // indirect jmp - dword ptr
#define OP_M48_IND  ( OP_M32 | OP_M8 )          // indirect jmp - fword ptr

#define OP_M        ( OP_M8 | OP_M16 | OP_M32 )
#define OP_M_ANY    ( OP_M_B | OP_M_W | OP_M_DW | OP_M_QW | OP_M_TB | OP_M )
#define OP_M8_R8    ( OP_M8 | OP_R8 )
#define OP_M16_R16    ( OP_M16 | OP_R16 )
#define OP_M32_R32    ( OP_M32 | OP_R32 )

#define OP_CR       0x01000000
#define OP_DR       0x02000000
#define OP_TR       0x04000000
#define OP_SPEC_REG ( OP_CR | OP_DR | OP_TR )

#define OP_SR2      0x08000000
#define OP_SR3      0x10000000
#define OP_SR       ( OP_SR2 | OP_SR3 )

#define OP_ST       0x20000000
#define OP_ST_REG   0x40000000
#define OP_STI    ( OP_ST | OP_ST_REG )

#define OP_SPECIAL  0x80000000      /* this includes the following cases, most
                                       of which are used in asmscan */

/* we need some kind of magic comparison fcn. to handle these
   this is ok, since they are not used in the parser, and rarely at all
 */

/* make all of these things OP_SPECIAL, and then add another value to
   differentiate between them */
/* store OP_SPECIAL in operand slot, this const. somewhere else - RM_BYTE slot */
#define OP_LABEL            0x1
#define OP_REGISTER         0x2
#define OP_RES_ID           0x4
#define OP_DIRECTIVE        0x8
#define OP_DIRECT_EXPR      0x10
#define OP_ARITHOP          0x20
#define OP_PTR_MODIFIER     0x40
#define OP_UNARY_OPERATOR   0x80

/* fix these comments up -- they are from the old stuff */
 /*     OP_NONE         no operands */
 /*     OP_LABEL        Label for JMP, CALL, etc */
 /*     OP_R            register */
 /*     OP_R8           8-bit register */
 /*     OP_CL           CL register */
 /*     OP_AL           AL register ( 8-bit accumulator ) */
 /*     OP_A            accumulator ( 8/16/32-bit ) */
 /*     OP_AX           AX register ( 16-bit accumulator ) */
 /*     OP_DX           DX register ( port )*/
 /*     OP_R16          16-bit register */
 /*     OP_R1632        16/32-bit register */
 /*     OP_EAX          EAX register ( 32-bit accumulator ) */
 /*     OP_R32          32-bit register */
 /*     OP_M            memory, 8/16/32-bit */
 /*     OP_M8_R8        8-bit memory or 8-bit register */
 /*                     difference between this and OP_M_B is that OP_M8 */
 /*                     assume the address mode is 8-bit if the user has */
 /*                     not specified the memory mode. On the other hand, */
 /*                     OP_M_B will treat that as error. */
 /*     OP_M16          memory, 16-bit */
 /*                     difference between this and OP_M_W is that OP_M16 */
 /*                     assume the address mode is 16-bit if the user has */
 /*                     not specified the memory mode. On the other hand, */
 /*                     OP_M_W will treat that as error. */
 /*     OP_M_B          memory ptr to byte */
 /*     OP_M_W          memory ptr to word */
 /*     OP_M_DW         memory ptr to dword */
 /*     OP_M_QW         memory ptr to qword */
 /*     OP_M_TB         memory ptr to tbyte */
 /*     OP_I8           immediate, 8 bit */
 /*     OP_I_1          immediate, 8 bit, value = 1 */
 /*     OP_I_3          immediate, 8 bit, value = 3 */
 /*     OP_I8_U         immediate, unsigned 8 bit, value between 0 - 255 */
 /*     OP_I            immediate, 8/16/32 bit */
 /*     OP_I16          immediate, 16 bit */
 /*     OP_I32          immediate, 32 bit */
 /*     OP_J32          immediate, 32 bit (for direct far calls/jmps) */
 /*     OP_J48          immediate, 48 bit (for direct far calls/jmps) */
 /*     OP_CR           Control Register */
 /*     OP_DR           Debug Register */
 /*     OP_TR           Test Register */
 /*     OP_SR            segreg, 16-bit */
 /*     OP_SR3   segreg, include 16/32 bit */
 /*     OP_ST            Stack Top */
 /*     OP_STI           registers in stack */
 /*     OP_REGISTER      designates a reserved register name, eg. AX */
 /*     OP_RES_ID        designates a reserved id, eg. BYTE, WORD */
 /*     OP_DIRECTIVE     designates a directive */
 /*     OP_DIRECT_EXPR  designates a directive which has an expression */
 /*     OP_ARITHOP          arithmetic operator */
