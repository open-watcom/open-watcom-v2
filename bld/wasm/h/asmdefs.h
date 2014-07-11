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

#ifndef _ASMDEFS_H_INCLUDED
#define _ASMDEFS_H_INCLUDED

#define IS_CALL( inst )     ( inst == T_CALL || inst == T_CALLF )
#define IS_JMP( inst )      ( inst >= T_JA && inst <= T_JZ )
#define IS_BRANCH( inst )   ( IS_JMP( inst ) || IS_CALL( inst ) )
#define IS_JMPCALLF( inst )  ( inst == T_CALLF || inst == T_JMPF )
#define IS_JMPCALLN( inst )  ( inst == T_CALL || inst == T_JMP )
#define IS_ANY_BRANCH( inst )    \
            ( IS_BRANCH( inst ) || ( ( inst >= T_LOOP ) && ( inst <= T_LOOPZW ) ) )

#define MOD_00          0x00
#define MOD_01          0x40
#define MOD_10          0x80
#define MOD_11          0xC0

#define W_BIT           0x01
#define NOT_W_BIT       0xFE

#define ADRSIZ          0x67
#define OPSIZ           0x66
#define OP_WAIT         0x9B
#define EXTENDED_OPCODE 0x0F
#define OP_NOP          0x90

#define S_I_B           0x04
#define D32             0x05
#define D16             0x06

#define ESP             0x04
#define EBP             0x05
#define BP              0x06

#define MEM_BX_SI       0x00
#define MEM_BX_DI       0x01
#define MEM_BP_SI       0x02
#define MEM_BP_DI       0x03

#define SCALE_FACTOR_1  0x00
#define SCALE_FACTOR_2  0x40
#define SCALE_FACTOR_4  0x80
#define SCALE_FACTOR_8  0xC0

#define FPE_MIN         0xD8
#define FPE_MAX         0xDF

#define SET_ADRSIZ( s, x ) ( s->prefix.adrsiz = (( x ) ^ ( s->use32 )) ? TRUE : FALSE )
#define SET_ADRSIZ_32( s ) ( s->prefix.adrsiz = ( s->use32 ) ? FALSE : TRUE )
#define SET_ADRSIZ_16( s ) ( s->prefix.adrsiz = ( s->use32 ) ? TRUE : FALSE )
#define SET_ADRSIZ_NO( s ) ( s->prefix.adrsiz = FALSE )
#define SET_OPSIZ( s, x ) ( s->prefix.opsiz = (( x ) ^ ( s->use32 )) ? TRUE : FALSE )
#define SET_OPSIZ_32( s ) ( s->prefix.opsiz = ( s->use32 ) ? FALSE : TRUE )
#define SET_OPSIZ_16( s ) ( s->prefix.opsiz = ( s->use32 ) ? TRUE : FALSE )
#define SET_OPSIZ_NO( s ) ( s->prefix.opsiz = FALSE )

#define addr_32( s )     ( s->use32 ? ( s->prefix.adrsiz == FALSE ) : ( s->prefix.adrsiz == TRUE ))
#define oper_32( s )     ( s->use32 ? ( s->prefix.opsiz == FALSE ) : ( s->prefix.opsiz == TRUE ))

#if defined( _STANDALONE_ )

#define MEM_TYPE( op, typ ) ( (op) == MT_##typ || (op) == MT_S##typ )

#else

#define MEM_TYPE( op, typ ) ( (op) == MT_##typ )

#endif

/* global variables */
extern asm_tok          *AsmBuffer[];
extern struct asm_code  *Code;
extern struct asm_sym   *Frame;
extern char             Parse_Pass;     // phase of parsing
extern operand_idx      Opnd_Count;
extern bool             Modend;         // end of module is reached
extern bool             Use32;          // if 32-bit code is use
extern token_idx        Token_Count;    // number of tokens on line

extern void             AsmByte( unsigned char );
extern token_idx        AsmScan( const char * );

#if defined( _STANDALONE_ )

extern void             AsmCodeByte( unsigned char );
extern void             AsmDataByte( unsigned char );

#else

#define AsmCodeByte( c )    AsmByte( c )
#define AsmDataByte( c )    AsmByte( c )

#endif

#endif
