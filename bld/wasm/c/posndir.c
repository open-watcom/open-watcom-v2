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


#include <stdlib.h>
#include <string.h>

#include "asmglob.h"
#include "asmerr.h"
#include "asmops1.h"
#include "asmops2.h"
#include "directiv.h"

#include "womp.h"
#include "pcobj.h"
#include "objrec.h"
#include "myassert.h"

extern void             AsmError( int );
extern direct_idx       FindClassLnameIdx( char *name );
extern uint_32          GetCurrSegAlign( void );
extern void             AsmByte( unsigned char );
extern void             FlushCurrSeg( void );
extern bool             CheckHaveSeg( void );

/* stupid global vars that we may as well use since they are there */
extern int_8            Use32;          // if 32-bit code is use
extern seg_list         *CurrSeg;       // points to stack of opened segments
extern  struct asm_tok  *AsmBuffer[];   // buffer to store token
extern int              Token_Count;    // number of tokens on line

typedef unsigned char   byte;

static byte NopList16[] = {
    2,                  /* objlen of first NOP pattern */
    0x89, 0xc0,         /* MOV AX,AX */
    0xfc                /* CLD */
};
static byte NopList32[] = {
    6,
    0x8d,0x80,0x00,0x00,0x00,0x00,  // lea     eax,+00000000H[eax]
    0x8d,0x40,0x00,                 // lea     eax,+00H[eax]
    0x8b,0xc9,                      // mov     ecx,ecx
    0x8d,0x44,0x20,0x00,            // lea     eax,+00H[eax+no_index_reg]
    0x8d,0x40,0x00,                 // lea     eax,+00H[eax]
    0x8b,0xc0,                      // mov     eax,eax
    0x90                            // nop
};
byte *NopLists[] = { NopList16, NopList32 };

int ChangeCurrentLocation( bool relative, int_32 value  )
/*******************************************************/
{
    if( CurrSeg == NULL ) return( ERROR );
    if( relative ) {
        value += GetCurrAddr();
    }
    FlushCurrSeg();

//    CurrSeg->seg->e.seginfo->segrec->d.segdef.seg_length = value;

    CurrSeg->seg->e.seginfo->current_loc = value;
    CurrSeg->seg->e.seginfo->start_loc = value;

    if( CurrSeg->seg->e.seginfo->current_loc >=
        CurrSeg->seg->e.seginfo->segrec->d.segdef.seg_length ) {
        CurrSeg->seg->e.seginfo->segrec->d.segdef.seg_length = CurrSeg->seg->e.seginfo->current_loc;
    }

    return( NOT_ERROR );
}

int OrgDirective( int i )
/***********************/
{
    struct asm_sym  *sym;
    int_32          value = 0;

    if( AsmBuffer[i+1]->token == T_NUM ) {
        return( ChangeCurrentLocation( FALSE, AsmBuffer[i+1]->value ) );
    } else if( AsmBuffer[i+1]->token == T_ID ) {
        sym = AsmLookup( AsmBuffer[i+1]->string_ptr );
        if( AsmBuffer[i+2]->token == T_OP_SQ_BRACKET &&
            AsmBuffer[i+3]->token == T_NUM ) {
            value = AsmBuffer[i+3]->value;
        }
        return( ChangeCurrentLocation( FALSE, sym->offset + value ) );
    }
    AsmError( EXPECTING_NUMBER );
    return( ERROR );
}

static bool is_code_seg( void )
/*****************************/
{
    direct_idx code;

    code = FindClassLnameIdx( "CODE" );
    if( CurrSeg->seg->e.seginfo->segrec->d.segdef.class_name_idx == code ) {
        return( TRUE );
    }
    return( FALSE );
}

static void fill_in_objfile_space( uint size )
/********************************************/
{
    int i;
    int nop_type;

    /* first decide whether to output nulls or nops - is it a code seg? */
    if( !is_code_seg() ) {
        /* just output nulls */
        for( i = 0; i < size; i++ ) {
            AsmByte( 0x00 );
        }
    } else {
        /* output appropriate NOP type instructions to fill in the gap */
        /**/ myassert( Use32 == 0 || Use32 == 1 );

        while( size > NopLists[Use32][0] ) {
            for( i = 1; i <= NopLists[Use32][0]; i++ ) {
                AsmByte( NopLists[Use32][i] );
            }
            size -= NopLists[Use32][0];
        }
        if( size == 0 ) return;

        i=1; /* here i is the index into the NOP table */
        for( nop_type = NopLists[Use32][0]; nop_type > size ; nop_type-- ) {
            i+=nop_type;
        }
        /* i now is the index of the 1st part of the NOP that we want */
        for( ; nop_type > 0; nop_type--,i++ ) {
            AsmByte( NopLists[Use32][i] );
        }
    }
}

int AlignDirective( uint_16 directive, int i )
/********************************************/
{
    int_32 align_val;
    int seg_align;

    switch( directive ) {
    case T_ALIGN:
        if( AsmBuffer[i+1]->token == T_NUM ) {
            int power;

            align_val = AsmBuffer[i+1]->value;
            /* check that the parm is a power of 2 */
            for( power = 1; power < align_val; power <<= 1 );
            if( power != align_val ) {
                AsmError( POWER_OF_2 );
                return( ERROR );
            }
        } else {
            if( Token_Count == i + 1 ) {
                align_val = GetCurrSegAlign();
            } else {
                AsmError( EXPECTING_NUMBER );
                return( ERROR );
            }
        }
        break;
    case T_EVEN:
        align_val = 2;
        break;
    }
    seg_align = GetCurrSegAlign(); // # of bytes
    if( seg_align <= 0 ) {
        AsmError( NO_SEGMENT_OPENED );
        return( ERROR );
    }
    if( align_val > seg_align ) {
        AsmWarn( 1, ALIGN_TOO_HIGH );
        return( ERROR );
    }
    /* find out how many bytes past alignment we are & add the remainder */
    //store temp. value
    seg_align = GetCurrAddr() % align_val;
    if( seg_align ) {
        align_val -= seg_align;
        fill_in_objfile_space( align_val );
    }
    return( NOT_ERROR );
}
