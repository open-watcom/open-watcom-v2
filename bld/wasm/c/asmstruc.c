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
#include <ctype.h>

#include "asmglob.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmops1.h"
#include "directiv.h"

a_definition_struct Definition = { 0, NULL, NULL };

extern  char            Parse_Pass;

extern  struct asm_tok  *AsmBuffer[];   // buffer to store token

extern dir_node         *dir_insert( char *, int );
extern void             InputQueueLine( char * );
extern void             PushLineQueue(void);

extern void             AsmError( int );

int StructDef( int i )
/********************/
{
    char *name;
    struct asm_sym *sym;
    dir_node *dir;

    if( i < 0 ) {
        AsmError( SYNTAX_ERROR );
        return( ERROR );
    }
    name = AsmBuffer[i]->string_ptr;
    switch( AsmBuffer[i+1]->value ) {
    case T_STRUC:
    case T_STRUCT:
        sym = AsmGetSymbol( name );
        if( Parse_Pass == PASS_1 ) {
            if( sym != NULL  ) {
                AsmError( SYMBOL_ALREADY_DEFINED );
                return( ERROR );
            }
            dir = dir_insert( name, TAB_STRUCT );
        } else {
            dir = (dir_node *)sym;
        }
        /* even if the current is null */
        push( &( Definition.struct_stack ), Definition.curr_struct );
        Definition.curr_struct = dir;
        Definition.struct_depth++;
        break;
    case T_ENDS:
        if( Definition.curr_struct != NULL &&
            strcmp( name, Definition.curr_struct->sym.name ) == 0 ) {
            /* this is the right struct ... so end it */
            Definition.curr_struct = pop( &( Definition.struct_stack ) );
            Definition.struct_depth--;
        } else {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
    }
    return( NOT_ERROR );
}

int InitializeStructure( asm_sym *sym, int i )
/********************************************/
{
    /* input: a line that looks like : sym_name struct_name { init. values }
     * where i marks the struct_name
     */

    char            buffer[MAX_LINE_LEN];
    char            *ptr;
    int             count = 0;
    struct asm_sym  *struct_symbol;
    dir_node        *dir;
    field_list      *f;

    struct_symbol = AsmGetSymbol( AsmBuffer[ i ]->string_ptr );

    dir = (dir_node *)struct_symbol;

    PushLineQueue();
    if( AsmBuffer[i+1]->token != T_STRING ) {
        AsmError( SYNTAX_ERROR ); // fixme
        return( ERROR );
    }
    if( sym != NULL ) {
        sym->total_size = struct_symbol->total_size;
        sym->total_length       = struct_symbol->total_length;
        sym->first_size = struct_symbol->first_size;
        sym->first_length       = struct_symbol->first_length;
    }

    ptr = AsmBuffer[i+1]->string_ptr;
    for( f = dir->e.structinfo->head; f != NULL; f = f->next ) {
        /* put the lines to define the fields of the structure in,
         * using the values specified ( if any ) or the default ones otherwise
         */
        strcpy( buffer, f->initializer );
        strcat( buffer, " " );
        if( ptr == NULL ) {
            strcat( buffer, f->value );
        } else if( strpbrk( ptr, "," ) != NULL ) {
            count = (int)( strpbrk( ptr, "," ) - ptr );
            strncat( buffer, ptr, count );
            ptr += min( count + 1, strlen( ptr ) ); // go past the comma
        } else {
            for( ; *ptr != '\0' && isspace(*ptr); ptr++ );
            if( *ptr == '\0' ) {
                strcat( buffer, f->value );
            } else {
                strcat( buffer, ptr );
            }
            ptr = NULL;
        }
        InputQueueLine( buffer );
    }

    return( NOT_ERROR );
}

int AddFieldToStruct( int loc )
/*****************************/
{
    int offset;
    int count = 0;
    int i;
    struct_info *the_struct;
    field_list  *f;

    the_struct = Definition.curr_struct->e.structinfo;

    offset = the_struct->size; // offset of this element

    f = AsmAlloc( sizeof( field_list ) );

    if( loc == -1 ) {
        for( loc = 0; AsmBuffer[loc]->token != T_FINAL; ++loc ) {
            /* nothing to do */
        }
    }

    /* now add the initializer to the structure's list */
    f->initializer = AsmAlloc( strlen( AsmBuffer[loc]->string_ptr ) + 1 );
    strcpy( f->initializer, AsmBuffer[ loc ]->string_ptr );

    /* now add the value to initialize the struct to */
    for( i = loc + 1; AsmBuffer[i]->token != T_FINAL; i++ ) {
        if( AsmBuffer[i]->string_ptr != NULL ) {
            count += strlen( AsmBuffer[i]->string_ptr ) + 1;
        }
        if( AsmBuffer[i]->token == T_STRING ) count += 2;
    }

    f->value = AsmAlloc( count + 1 );
    f->value[0] = '\0';

    for( i = loc + 1; AsmBuffer[i]->token != T_FINAL; i++ ) {
        if( AsmBuffer[i]->token == T_STRING ) {
            strcat( f->value, "<" );
        }
        if( AsmBuffer[i]->string_ptr != NULL ) {
            strcat( f->value, AsmBuffer[i]->string_ptr );
        }
        if( AsmBuffer[i]->token == T_STRING ) {
            strcat( f->value, ">" );
        }
        strcat( f->value, " " );
    }

    f->next = NULL;
    if( the_struct->head == NULL ) {
        the_struct->head = the_struct->tail = f;
    } else {
        the_struct->tail->next = f;
        the_struct->tail = f;
    }

    return( offset );
}

int GetStructSize( int loc )
/**************************/
{
    dir_node *dir;

    dir = (dir_node *)(AsmGetSymbol( AsmBuffer[loc]->string_ptr ) );

    return( dir->e.structinfo->size );
}
