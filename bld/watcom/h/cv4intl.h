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


/* Intel reg defn  */
/* 8 bit regs */
_CVREG( AL,       1)
_CVREG( CL,       2)
_CVREG( DL,       3)
_CVREG( BL,       4)
_CVREG( AH,       5)
_CVREG( CH,       6)
_CVREG( DH,       7)
_CVREG( BH,       8)
/* 16 bit regs */
_CVREG( AX,       9)
_CVREG( CX,      10)
_CVREG( DX,      11)
_CVREG( BX,      12)
_CVREG( SP,      13)
_CVREG( BP,      14)
_CVREG( SI,      15)
_CVREG( DI,      16)
/* 32 bit regs */
_CVREG( EAX,     17)
_CVREG( ECX,     18)
_CVREG( EDX,     19)
_CVREG( EBX,     20)
_CVREG( ESP,     21)
_CVREG( EBP,     22)
_CVREG( ESI,     23)
_CVREG( EDI,     24)
/* segment regs */
_CVREG( ES,      25)
_CVREG( CS,      26)
_CVREG( SS,      27)
_CVREG( DS,      28)
_CVREG( FS,      29)
_CVREG( GS,      30)
/* Special cases */
_CVREG( IP,      31)
_CVREG( FLAGS,   32)
_CVREG( EIP,     33)
_CVREG( EFLAGS,  34)
/* Pcode */
_CVREG( TEMP,    40)
_CVREG( TEMPH,   41)
_CVREG( QUOTE,   42)
/* 43-47 Reseved */
/* System regs   */
_CVREG( CR0,     80)
_CVREG( CR1,     81)
_CVREG( CR2,     82)
_CVREG( CR3,     83)
_CVREG( DR0,     90)
_CVREG( DR1,     91)
_CVREG( DR2,     92)
_CVREG( DR3,     93)
_CVREG( DR4,     94)
_CVREG( DR5,     95)
_CVREG( DR6,     96)
_CVREG( DR7,     97)
/* reg extensions for 8087 */
_CVREG( ST0,    128)
_CVREG( ST1,    129)
_CVREG( ST2,    130)
_CVREG( ST3,    131)
_CVREG( ST4,    132)
_CVREG( ST5,    133)
_CVREG( ST6,    134)
_CVREG( ST7,    135)
_CVREG( CONTROL,136)
_CVREG( STATUS, 137)
_CVREG( TAG,    138)
_CVREG( FPIP,   139)
_CVREG( FPCS,   140)
_CVREG( FPDO,   141)
_CVREG( FPDS,   142)
_CVREG( ISEM,   143)
_CVREG( FPEIP,  144)
_CVREG( FPEDO,  145)
