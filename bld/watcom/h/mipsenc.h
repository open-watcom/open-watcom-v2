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
* Description:  Instruction encoding for MIPS architecture.
*
****************************************************************************/


#define _FiveBits( x )      ( (x) & 0x0000001f )
#define _SixBits( x )       ( (x) & 0x0000003f )
#define _SixteenBits( x )   ( (x) & 0x0000ffff )
#define _TwentySixBits( x ) ( (x) & 0x03ffffff )

// MIPS operand encodings, from Appendix A of MIPS RISC Architecture
#define _Rs( x )            ( _FiveBits(x)      << 21 )
#define _Rt( x )            ( _FiveBits(x)      << 16 )
#define _Rd( x )            ( _FiveBits(x)      << 11 )
#define _Shift( x )         ( _FiveBits(x)      << 6 )
#define _Ft( x )            ( _FiveBits(x)      << 16 )
#define _Fs( x )            ( _FiveBits(x)      << 11 )
#define _Fd( x )            ( _FiveBits(x)      << 6 )
#define _Immed( x )         ( _SixteenBits(x)   << 0 )
#define _Target( x )        ( _TwentySixBits(x) << 0 )

#define _Opcode( x )        ( _SixBits(x)       << 26 )
#define _Function( x )      ( _SixBits(x)       << 0 )
#define _FPFormat( x )      ( _FiveBits(x)      << 21 )

// sixteen bit signed immediate
#define _SignedImmed( x )   ( _Immed( x ) )

typedef uint_32                 mips_ins;

#define MIPS_ZERO_SINK          0       // $zero
#define MIPS_RETURN_ADDR        31      // $ra
#define MIPS_STACK_REG          29      // $sp
#define MIPS_FRAME_REG          30      // $fp
#define MIPS_GPR_SCRATCH        1       // $at

#define MIPS_MAX_OFFSET         0x7fff  // immediate displacement range

#define MIPS_NOP                0       // actually 'sll $zero,$zero,0'
