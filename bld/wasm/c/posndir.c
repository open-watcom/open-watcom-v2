/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "asmglob.h"
#include "directiv.h"
#include "myassert.h"

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

static byte *NopLists[] = { NopList16, NopList32 };

bool ChangeCurrentLocation( bool relative, int_32 value, bool select_data )
/*************************************************************************/
{
    if( CurrSeg == NULL )
        return( RC_ERROR );
    if( relative ) {
        value += GetCurrAddr();
    }
    FlushCurrSeg( );
    if( select_data )
        OutSelect( true );
    CurrSeg->seg->e.seginfo->current_loc = value;
    CurrSeg->seg->e.seginfo->start_loc = value;

    if( CurrSeg->seg->e.seginfo->current_loc >= CurrSeg->seg->e.seginfo->length ) {
        CurrSeg->seg->e.seginfo->length = CurrSeg->seg->e.seginfo->current_loc;
    }

    return( RC_OK );
}

bool OrgDirective( token_buffer *tokbuf, token_idx i )
/****************************************************/
{
    asm_sym_handle  sym;
    int_32          value = 0;

    if( tokbuf->tokens[i+1].class == TC_NUM ) {
        return( ChangeCurrentLocation( false, tokbuf->tokens[i+1].u.value, false ) );
    } else if( tokbuf->tokens[i+1].class == TC_ID ) {
        sym = AsmLookup( tokbuf->tokens[i+1].string_ptr );
        if( tokbuf->tokens[i+2].class == TC_OP_SQ_BRACKET &&
            tokbuf->tokens[i+3].class == TC_NUM ) {
            value = tokbuf->tokens[i+3].u.value;
        }
        return( ChangeCurrentLocation( false, sym->offset + value, false ) );
    }
    AsmError( EXPECTING_NUMBER );
    return( RC_ERROR );
}

static void fill_in_objfile_space( uint_32 size )
/***********************************************/
{
    uint_32     i;
    uint_32     nop_type;

    /* first decide whether to output nulls or nops - is it a code seg? */
    if( ! SEGISCODE( CurrSeg ) ) {
        /* just output nulls */
        for( i = 0; i < size; i++ ) {
            AsmByte( 0x00 );
        }
    } else {
        /* output appropriate NOP type instructions to fill in the gap */
        int idx32 = ( Use32 ) ? 1 : 0;

        while( size > NopLists[idx32][0] ) {
            for( i = 1; i <= NopLists[idx32][0]; i++ ) {
                AsmByte( NopLists[idx32][i] );
            }
            size -= NopLists[idx32][0];
        }
        if( size == 0 ) return;

        i=1; /* here i is the index into the NOP table */
        for( nop_type = NopLists[idx32][0]; nop_type > size ; nop_type-- ) {
            i+=nop_type;
        }
        /* i now is the index of the 1st part of the NOP that we want */
        for( ; nop_type > 0; nop_type--,i++ ) {
            AsmByte( NopLists[idx32][i] );
        }
    }
}

bool AlignDirective( asm_token directive, token_buffer *tokbuf, token_idx i )
/***************************************************************************/
{
    uint_32     align_val = 0;
    unsigned    seg_align;

    switch( directive ) {
    case T_ALIGN:
        if( tokbuf->tokens[i+1].class == TC_NUM ) {
            uint_32 power;

            align_val = tokbuf->tokens[i+1].u.value;
            /* check that the parm is a power of 2 */
            for( power = 1; power < align_val; power <<= 1 );
            if( power != align_val ) {
                AsmError( POWER_OF_2 );
                return( RC_ERROR );
            }
        } else {
            if( tokbuf->count == i + 1 ) {
                align_val = GetCurrSegAlign();
            } else {
                AsmError( EXPECTING_NUMBER );
                return( RC_ERROR );
            }
        }
        break;
    case T_EVEN:
        align_val = 2;
        break;
    }
    seg_align = GetCurrSegAlign(); // # of bytes
    if( seg_align == 0 ) {
        AsmError( NO_SEGMENT_OPENED );
        return( RC_ERROR );
    }
    if( align_val > seg_align ) {
        AsmWarn( 1, ALIGN_TOO_HIGH );
        return( RC_ERROR );
    }
    /* find out how many bytes past alignment we are & add the remainder */
    //store temp. value
    seg_align = GetCurrAddr() % align_val;
    if( seg_align ) {
        align_val -= seg_align;
        fill_in_objfile_space( align_val );
    }
    return( RC_OK );
}
