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


#define _SixBits( x )   ( (x) & 0x003f )
#define _SevenBits( x ) ( (x) & 0x007f )
#define _FiveBits( x )  ( (x) & 0x001f )
#define _EightBits( x ) ( (x) & 0x00ff )
#define _ElevenBits( x )( (x) & 0x07ff )

// these correspond to letters in section 3.3 of the Alpha Architecture Manual
#define _A( x )         ( _FiveBits(x) << 21 )
#define _B( x )         ( _FiveBits(x) << 16 )
#define _C( x )         ( _FiveBits(x) << 0 )
#define _LIT( x )       ( _EightBits(x) << 13 )

#define _Opcode( x )    ( _SixBits(x) << 26 )
#define _Function( x )  ( _SevenBits(x) << 5 )
#define _FPFunction( x )( _ElevenBits(x) << 5 )

// sixteen bit signed immediate
#define _SignedImmed( x )       ( (x) & 0xffff )

typedef uint_32                 axp_ins;

#define AXP_ZERO_SINK           31
#define AXP_FLOAT_SCRATCH       30
#define AXP_RETURN_ADDR         26
#define AXP_STACK_REG           30
#define AXP_FRAME_REG           15
#define AXP_GPR_SCRATCH         28

#define AXP_MAX_OFFSET          0x7fff
