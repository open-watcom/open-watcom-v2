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
* Description:  Label directive, anonymous labels
*
****************************************************************************/


#include "asmglob.h"
#include "asmalloc.h"
#include "asmfixup.h"
#include "asmlabel.h"
#if defined( _STANDALONE_ )
#include "directiv.h"
#include "asmstruc.h"
#include "queues.h"
#endif

#if defined( _STANDALONE_ )

static unsigned   AnonymousCounter = 0;

void PrepAnonLabels( void )
/*************************/
{
    asm_sym_handle  sym;
    char            buffer[20];

    sprintf( buffer, "L&_%d", AnonymousCounter );
    AsmChangeName( "@B", buffer  );
    AnonymousCounter = 0;

    sym = AsmGetSymbol( "L&_0" );
    if( sym != NULL ) {
        AsmChangeName( sym->name, "@F" );
    }

}

bool IsLabelStruct( const char *name )
/************************************/
{
    asm_sym_handle  sym;

    sym = AsmGetSymbol( name );
    return( ( sym != NULL && sym->state == SYM_STRUCT ) );
}
#endif

bool MakeLabel( token_buffer *tokbuf, token_idx i, memtype mem_type )
/*******************************************************************/
{
    asm_sym_handle  sym;
    char            *symbol_name;
#if defined( _STANDALONE_ )
    uint_32         addr = 0;
    char            buffer[20];
    asm_sym_handle  newsym;
//    proc_info       *info;
#endif

    symbol_name = tokbuf->tokens[i].string_ptr;
#if defined( _STANDALONE_ )
    if( CurrSeg == NULL )
        AsmError( LABEL_OUTSIDE_SEGMENT );
    if( symbol_name[0] == '@' && symbol_name[1] == '@' && symbol_name[2] == '\0' ) {
        /* anonymous label */
        /* find any references to @F and mark them to here as @B */
        /* find the old @B */
        sym = AsmGetSymbol( "@B" );
        if( sym != NULL ) {
            /* change it to some magical name */
            sprintf( buffer, "L&_%d", AnonymousCounter++ );
            AsmChangeName( sym->name, buffer );
        }
        sym = AsmLookup( "@B" );
        /* change all forward anon. references to this location */
        newsym = AsmGetSymbol( "@F" );
        if( newsym != NULL ) {
            sym->fixup = newsym->fixup;
            newsym->fixup = NULL;
        }
        AsmTakeOut( "@F" );
        sym->state = SYM_INTERNAL;
        sym->mem_type = mem_type;  // fixme ??
        GetSymInfo( sym );
        BackPatch( sym );
        /* now point the @F marker at the next anon. label if we have one */
        sprintf( buffer, "L&_%d", AnonymousCounter + 1 );
        sym = AsmGetSymbol( buffer );
        if( sym != NULL ) {
            AsmChangeName( sym->name, "@F" );
        }
        return( RC_OK );
    }
    if( (Options.mode & MODE_TASM) && Options.locals_len ) {
        if( strncmp( symbol_name, Options.locals_prefix, Options.locals_len ) == 0
            && symbol_name[Options.locals_len] != '\0' ) {
            if( CurrProc == NULL ) {
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
//            info = CurrProc->e.procinfo;
            sym = AsmLookup( symbol_name );
            if( sym == NULL )
                return( RC_ERROR );
            GetSymInfo( sym );
            BackPatch( sym );
            return( RC_OK );
        }
    }
    sym = AsmLookup( symbol_name );
    if( sym == NULL )
        return( RC_ERROR );
    if( Parse_Pass == PASS_1 ) {
        if( sym->state == SYM_EXTERNAL && ((dir_node_handle)sym)->e.extinfo->global ) {
            dir_to_sym( (dir_node_handle)sym );
            AddPublicData( (dir_node_handle)sym );
            if( sym->mem_type != mem_type ) {
                AsmErr( SYMBOL_TYPE_DIFF, sym->name );
                return( RC_ERROR );
            }
        } else if( sym->state != SYM_UNDEFINED ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, symbol_name );
            return( RC_ERROR );
        }
    } else {
        /* save old offset */
        addr = sym->offset;
    }
    if( Definition.struct_depth != 0 ) {
        if( Parse_Pass == PASS_1 ) {
            sym->offset = AddFieldToStruct( sym, tokbuf, INVALID_IDX );
            sym->state = SYM_STRUCT_FIELD;
        }
    } else {
        sym->state = SYM_INTERNAL;
        GetSymInfo( sym );
    }
    sym->mem_type = mem_type;  // fixme ??
    if( Parse_Pass != PASS_1 && sym->offset != addr ) {
        PhaseError = true;
    }
#else
    sym = AsmLookup( symbol_name );
    if( sym == NULL )
        return( RC_ERROR );
    if( sym->state != SYM_UNDEFINED ) {
        AsmError( SYMBOL_ALREADY_DEFINED );
        return( RC_ERROR );
    }
    sym->state = SYM_INTERNAL;
    sym->addr = AsmCodeAddress;
    //  it should define label type ?????
    sym->mem_type = mem_type;  // fixme ??
#endif
    BackPatch( sym );
    return( RC_OK );
}

#if defined( _STANDALONE_ )
bool LabelDirective( token_buffer *tokbuf, token_idx i )
/******************************************************/
{
    token_idx   n;

    if( Options.mode & MODE_IDEAL ) {
        n = ++i;
    } else if( i > 0 ) {
        n = i - 1;
    } else {
        n = INVALID_IDX;
    }
    if( ISINVALID_IDX( n ) || ( tokbuf->tokens[n].class != TC_ID ) ) {
        AsmError( INVALID_LABEL_DEFINITION );
        return( RC_ERROR );
    }
    if( tokbuf->tokens[++i].class == TC_ID ) {
        if( IsLabelStruct( tokbuf->tokens[i].string_ptr ) ) {
            return( MakeLabel( tokbuf, n, MT_STRUCT ) );
        }
    }
    if( ( tokbuf->tokens[i].class != TC_RES_ID ) &&
        ( tokbuf->tokens[i].class != TC_DIRECTIVE ) ) {
        AsmError( INVALID_LABEL_DEFINITION );
        return( RC_ERROR );
    }
    switch( tokbuf->tokens[i].u.token ) {
    case T_NEAR:
        return( MakeLabel( tokbuf, n, MT_NEAR ));
    case T_FAR:
        return( MakeLabel( tokbuf, n, MT_FAR ));
    case T_BYTE:
        return( MakeLabel( tokbuf, n, MT_BYTE ));
    case T_WORD:
        return( MakeLabel( tokbuf, n, MT_WORD ));
    case T_DWORD:
        return( MakeLabel( tokbuf, n, MT_DWORD ));
    case T_FWORD:
        return( MakeLabel( tokbuf, n, MT_FWORD ));
    case T_PWORD:
        return( MakeLabel( tokbuf, n, MT_FWORD ));
    case T_QWORD:
        return( MakeLabel( tokbuf, n, MT_QWORD ));
    case T_TBYTE:
        return( MakeLabel( tokbuf, n, MT_TBYTE ));
    case T_OWORD:
        return( MakeLabel( tokbuf, n, MT_OWORD ));
    case T_PROC:
        return( MakeLabel( tokbuf, n, CurrProc->e.procinfo->mem_type ));
    default:
        AsmError( INVALID_LABEL_DEFINITION );
        return( RC_ERROR );
    }
}
#endif
