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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define _FiveBits( x )          ( (x) & 0x0000001f )
#define _SixBits( x )           ( (x) & 0x0000003f )
#define _SevenBits( x )         ( (x) & 0x0000007f )
#define _EightBits( x )         ( (x) & 0x000000ff )
#define _ElevenBits( x )        ( (x) & 0x000007ff )
#define _FourteenBits( x )      ( (x) & 0x00003fff )
#define _SixteenBits( x )       ( (x) & 0x0000ffff )
#define _TwentyOneBits( x )     ( (x) & 0x001fffff )
/*
 * these correspond to letters in section 3.3 of the Alpha Architecture Manual
 */
#define _Ra( x )                ( _FiveBits(x) << 21 )
#define _Rb( x )                ( _FiveBits(x) << 16 )
#define _Rc( x )                ( _FiveBits(x) << 0 )
#define _LIT( x )               ( _EightBits(x) << 13 )
#define _LIT_bit                ( 1 << 12 )

#define _Opcode( x )            ( _SixBits(x) << 26 )
#define _Function( x )          ( _SevenBits(x) << 5 )
#define _FPFunction( x )        ( _ElevenBits(x) << 5 )
/*
 * sixteen bit signed immediate
 */
#define _SignedImmed( x )       _SixteenBits( x )

typedef uint_32                 axp_ins;

#define ZERO_REG_IDX            31      // Contains 0
#define FP_REG_IDX              15      // Contains the Frame pointer (if needed)
#define RA_REG_IDX              26      // Contains the Return address
#define AT_REG_IDX              28      // Reserved for the temporary
#define GP_REG_IDX              29      // Contains the Global pointer
#define SP_REG_IDX              30      // Contains the Stack pointer

#define AXP_MAX_OFFSET          0x7fff
