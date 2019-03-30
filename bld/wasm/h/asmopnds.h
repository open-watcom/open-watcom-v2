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

#ifndef ASMOPNDS_H
#define ASMOPNDS_H

#ifdef _M_I86
#define ENUMU32(x)      x ## UL
#else
#define ENUMU32(x)      x ## U
#endif

typedef enum operand_type {
    OP_NONE     = ENUMU32( 0x00000000 ),
    OP_R8       = ENUMU32( 0x00000001 ),
    OP_R16      = ENUMU32( 0x00000002 ),
    OP_R32      = ENUMU32( 0x00000004 ),
    OP_MMX      = ENUMU32( 0x00000008 ),
    OP_XMM      = ENUMU32( 0x00000010 ),
    OP_A        = ENUMU32( 0x00000020 ),  // AL, AX, EAX registers
    OP_C        = ENUMU32( 0x00000040 ),  // CL register
    OP_D        = ENUMU32( 0x00000080 ),  // DX register

    OP_AL       = ( OP_A | OP_R8 ),
    OP_AX       = ( OP_A | OP_R16 ),
    OP_EAX      = ( OP_A | OP_R32 ),
    OP_CL       = ( OP_C | OP_R8 ),
    OP_DX       = ( OP_D | OP_R16 ),
    OP_R1632    = ( OP_R16 | OP_R32 ),
    OP_R        = ( OP_R8 | OP_R16 | OP_R32 ),
    OP_RMX      = ( OP_MMX | OP_XMM ),

    OP_I8       = ENUMU32( 0x00000100 ),
    OP_I_1      = ENUMU32( 0x00000200 ),
    OP_I_3      = ENUMU32( 0x00000400 ),
    OP_U8       = ENUMU32( 0x00000800 ),
    OP_I16      = ENUMU32( 0x00001000 ),
    OP_I32      = ENUMU32( 0x00002000 ),
    OP_J32      = ENUMU32( 0x00004000 ),
    OP_J48      = ENUMU32( 0x00008000 ),
    OP_I        = ( OP_I8 | OP_I_1 | OP_I_3 | OP_U8 | OP_I16 | OP_I32 ),
    OP_GE_8     = ( OP_I8 | OP_U8 | OP_I16 | OP_I32 ),
    OP_GE_16    = ( OP_I16 | OP_I32 ),
    OP_GE_U8    = ( OP_U8 | OP_I16 | OP_I32 ),

    OP_M_B      = ENUMU32( 0x00010000 ),
    OP_M_W      = ENUMU32( 0x00020000 ),
    OP_M_DW     = ENUMU32( 0x00040000 ),
    OP_M_FW     = ENUMU32( 0x00080000 ),
    OP_M_QW     = ENUMU32( 0x00100000 ),
    OP_M_TB     = ENUMU32( 0x00200000 ),
    OP_M_OW     = ENUMU32( 0x00400000 ),
    OP_M_DFT    = ENUMU32( 0x00800000 ),

    OP_M8       = ( OP_M_B | OP_M_DFT ),
    OP_M16      = ( OP_M_W | OP_M_DFT ),
    OP_M32      = ( OP_M_DW | OP_M_DFT ),
    OP_M64      = ( OP_M_QW | OP_M_DFT ),
    OP_M128     = ( OP_M_OW | OP_M_DFT ),

    OP_M        = ( OP_M_B | OP_M_W | OP_M_DW | OP_M_DFT ),
    OP_M_ANY    = ( OP_M_B | OP_M_W | OP_M_DW | OP_M_FW | OP_M_QW | OP_M_TB | OP_M_OW | OP_M_DFT ),
    OP_M8_R8    = ( OP_M_B | OP_M_DFT | OP_R8 ),
    OP_M16_R16  = ( OP_M_W | OP_M_DFT | OP_R16 ),
    OP_M32_R32  = ( OP_M_DW | OP_M_DFT | OP_R32 ),

    OP_CR       = ENUMU32( 0x01000000 ),
    OP_DR       = ENUMU32( 0x02000000 ),
    OP_TR       = ENUMU32( 0x04000000 ),
    OP_SPEC_REG = ( OP_CR | OP_DR | OP_TR ),

    OP_SR2      = ENUMU32( 0x08000000 ),
    OP_SR3      = ENUMU32( 0x10000000 ),
    OP_SR       = ( OP_SR2 | OP_SR3 ),

    OP_ST       = ENUMU32( 0x20000000 ),
    OP_ST_REG   = ENUMU32( 0x40000000 ),
    OP_STI      = ( OP_ST | OP_ST_REG ),

    OP_SPECIAL  = ENUMU32( 0x80000000 )     /* this includes the following cases, most
                                                of which are used in asmscan */
} OPNDTYPE;

enum operand3_type {
    OP3_NONE = 0x00,
    OP3_CL   = 0x01,
    OP3_I8_U = 0x02,
    OP3_I    = 0x03,
    OP3_HID  = 0x08
};

/* we need some kind of magic comparison fcn. to handle these
   this is ok, since they are not used in the parser, and rarely at all
 */

/* make all of these things OP_SPECIAL, and then add another value to
   differentiate between them */
/* store OP_SPECIAL in operand slot, this const. somewhere else - RM_BYTE slot */

enum oper_type {
    OP_DIRECTIVE = 1,
    OP_REGISTER,
    OP_RES_ID,
    OP_RES_ID_PTR_MODIF,
    OP_DIRECT_EXPR,
    OP_RELATION_OPERATOR,
    OP_ARITH_OPERATOR,
    OP_UNARY_OPERATOR,
};

/* fix these comments up -- they are from the old stuff */
 /*     OP_NONE         no operands */
 /*     OP_R            register 8/16/32-bit */
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
 /*     OP_MMX          MMX 64-bit register */
 /*     OP_XMM          XMM 128-bit register */

 /*     OP_M            memory, 8/16/32-bit */
 /*     OP_M8           memory, 8-bit */
 /*                     difference between this and OP_M_B is that OP_M8 */
 /*                     assume the address mode is 8-bit if the user has */
 /*                     not specified the memory mode. On the other hand, */
 /*                     OP_M_B will treat that as error. */
 /*     OP_M16          memory, 16-bit */
 /*                     difference between this and OP_M_W is that OP_M16 */
 /*                     assume the address mode is 16-bit if the user has */
 /*                     not specified the memory mode. On the other hand, */
 /*                     OP_M_W will treat that as error. */
 /*     OP_M32          memory, 32-bit */
 /*                     difference between this and OP_M_DW is that OP_M32 */
 /*                     assume the address mode is 32-bit if the user has */
 /*                     not specified the memory mode. On the other hand, */
 /*                     OP_M_DW will treat that as error. */
 /*     OP_M8_R8        8-bit memory or 8-bit register */
 /*     OP_M16_R16      16-bit memory or 16-bit register */
 /*     OP_M32_R32      32-bit memory or 32-bit register */
 /*     OP_M_B          memory ptr to byte */
 /*     OP_M_W          memory ptr to word */
 /*     OP_M_DW         memory ptr to dword */
 /*     OP_M_FW         memory ptr to fword, pword */
 /*     OP_M_QW         memory ptr to qword */
 /*     OP_M_TB         memory ptr to tbyte */
 /*     OP_M_OW         memory ptr to oword */

 /*     OP_I8           immediate, 8 bit */
 /*     OP_I_1          immediate, 8 bit, value = 1 */
 /*     OP_I_3          immediate, 8 bit, value = 3 */
 /*     OP_U8           immediate, unsigned 8 bit, value between 0 - 255 */
 /*     OP_I            immediate, 8/16/32 bit */
 /*     OP_I16          immediate, 16 bit */
 /*     OP_I32          immediate, 32 bit */
 /*     OP_J32          immediate, 32 bit (for direct far calls/jmps) */
 /*     OP_J48          immediate, 48 bit (for direct far calls/jmps) */

 /*     OP_CR           Control Register */
 /*     OP_DR           Debug Register */
 /*     OP_TR           Test Register */
 /*     OP_SR           segreg, 16-bit */
 /*     OP_SR3          segreg, include 16/32 bit */
 /*     OP_ST           x87 Stack Top */
 /*     OP_STI          x87 registers in stack */

 /*     OP_LABEL        Label for JMP, CALL, etc */
 /*     OP_REGISTER     designates a reserved register name, eg. AX */
 /*     OP_RES_ID       designates a reserved id, eg. BYTE, WORD */
 /*     OP_DIRECTIVE    designates a directive */
 /*     OP_DIRECT_EXPR  designates a directive which has an expression */
 /*     OP_ARITHOP      arithmetic operator */

#endif
