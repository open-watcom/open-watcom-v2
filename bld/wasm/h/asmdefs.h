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


#define IS_CALL( inst )     ( inst == T_CALL || inst == T_CALLF )
#define IS_JMP( inst )      ( inst >= T_JA && inst <= T_JZ )
#define IS_BRANCH( inst )   ( IS_JMP( inst ) || IS_CALL( inst ) )

#define MOD_00          0x00
#define MOD_01          0x40
#define MOD_10          0x80
#define MOD_11          0xC0

#define W_BIT           0x01
#define NOT_W_BIT       0xFE

#define OPND1           0
#define OPND2           1

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

#define addr_32( code )     ( code->use32 ? ( code->adrsiz == EMPTY ) : ( code->adrsiz == NOT_EMPTY ) )

#ifdef _WASM_
    #define     Address         ( GetCurrAddr() )
    #define MEM_TYPE( op, typ ) ( (op) == T_##typ || (op) == T_S##typ )
#else
    extern uint_32              Address;
    #define MEM_TYPE( op, typ ) ( (op) == T_##typ )
#endif

/* global variables */
extern char             *CodeBuffer;
extern struct asm_tok   *AsmBuffer[];
extern struct AsmCodeName AsmOpcode[];
extern struct asmfixup  *InsFixups[3];
extern struct asmfixup  *FixupHead;
extern struct asm_code  *Code;
extern int_8            Frame;
extern uint_8           Frame_Datum;
extern char             Parse_Pass;     // phase of parsing
extern unsigned char    Opnd_Count;
extern char             Modend;         // end of module is reached
extern int_8            Use32;          // if 32-bit code is use
extern uint             LineNumber;

extern void             add_frame( void );
extern struct asmfixup  *AddFixup( struct asm_sym *sym, int fixup_type );
extern int              BackPatch( struct asm_sym *sym );
extern void             mark_fixupp( unsigned long determinant, int index );
extern struct fixup     *CreateFixupRec( int index );
extern int              store_fixup( int index );
extern int              MakeFpFixup( struct asm_sym *sym );
extern int              match_phase_1( void );
extern void             AsmByte( char );
#ifdef _WASM_
    extern void         AsmCodeByte( char );
    extern void         AsmDataByte( char );
#else
    #define AsmCodeByte( c )        AsmByte( c )
    #define AsmDataByte( c )        AsmByte( c )
#endif
