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
#include "asmsym.h"
#include "asmops1.h"//
#include "asmops2.h"

#ifdef _WASM_
    #include "directiv.h"

    #include "womp.h"
    #include "pcobj.h"
    #include "objrec.h"
    #include "myassert.h"

    #define     Address         ( GetCurrAddr() )
    extern char                 Parse_Pass;     // phase of parsing
    extern int_8                PhaseError;

    extern int                  AddFieldToStruct( int );
#else
    extern uint_32              Address;
#endif

extern void             AsmError( int );
extern int              BackPatch( struct asm_sym * );

extern  struct asm_tok  *AsmBuffer[];   // buffer to store token

#ifdef _WASM_
    static unsigned             AnonymousCounter = 0;
#endif

#ifdef _WASM_
void PrepAnonLabels( void )
/*************************/
{
    struct asm_sym *sym;
    char buffer[20];

    sprintf( buffer, "L&_%d", AnonymousCounter );
    AsmChangeName( "@B", buffer  );
    AnonymousCounter = 0;

    sym = AsmGetSymbol( "L&_0" );
    if( sym != NULL ) {
        AsmChangeName( sym->name, "@F" );
    }

}
#endif

#pragma off (unreferenced )
int MakeLabel( char *symbol_name, int mem_type )
/**********************************************/
{
    struct asm_sym      *sym;
    #ifdef _WASM_
        int                     addr;
        char                    buffer[20];

    if( strcmp( symbol_name, "@@" ) == 0 ) {
        struct asm_sym          *newsym;
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
        sprintf( buffer, "L&_%d", AnonymousCounter+1 );
        sym = AsmGetSymbol( buffer );
        if( sym != NULL ) {
            AsmChangeName( sym->name, "@F" );
        }

        return( NOT_ERROR );
    }
    #endif
    sym = AsmLookup( symbol_name );

    if( sym == NULL ) return( ERROR );

    #ifdef _WASM_
        if( Parse_Pass == PASS_1 ) {
            if( sym->state != SYM_UNDEFINED ) {
                AsmErr( SYMBOL_PREVIOUSLY_DEFINED, symbol_name );
                return( ERROR );
            }
        } else {
            /* save old offset */
            addr = sym->offset;
        }
        if( Definition.struct_depth != 0 ) {
            if( Parse_Pass == PASS_1 ) {
                sym->offset = AddFieldToStruct( -1 );
                sym->state = SYM_STRUCT_FIELD;
            }
        } else {
            sym->state = SYM_INTERNAL;
            GetSymInfo( sym );
        }
        sym->mem_type = mem_type;  // fixme ??
        if( Parse_Pass != PASS_1 && sym->offset != addr ) {
            PhaseError = TRUE;
        }
    #else
        if( sym->state != SYM_UNDEFINED ) {
            AsmError( SYMBOL_ALREADY_DEFINED );
            return( ERROR );
        }
        sym->state = SYM_INTERNAL;
        sym->addr = Address;
    #endif
    BackPatch( sym );
    return( NOT_ERROR );
}

int LabelDirective( int i )
/*************************/
{
    if( i != 1 ) {
        AsmError( INVALID_LABEL_DEFINITION );
        return( ERROR );
    }
#ifdef _WASM_
    if( AsmBuffer[i+1]->token == T_ID ) {
        asm_sym *sym;

        sym = AsmGetSymbol( AsmBuffer[i+1]->string_ptr );
        if( sym != NULL && sym->state == SYM_STRUCT ) {
            return( MakeLabel( AsmBuffer[i-1]->string_ptr, T_STRUCT ) );
        }
    }
#endif
    switch( AsmBuffer[i+1]->value ) {
    case T_NEAR:
    case T_FAR:
    case T_BYTE:
    case T_WORD:
    case T_DWORD:
    case T_FWORD:
    case T_PWORD:
    case T_QWORD:
    case T_TBYTE:
        if( AsmBuffer[i+1]->token == T_RES_ID ) {
            return( MakeLabel( AsmBuffer[i-1]->string_ptr, AsmBuffer[i+1]->value ));
        }
        /* fall through */
    default:
        AsmError( INVALID_LABEL_DEFINITION );
        return( ERROR );
    }

}

