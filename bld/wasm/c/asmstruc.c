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


#include "asmglob.h"
#include <ctype.h>

#include "asmalloc.h"
#include "directiv.h"
#include "asminput.h"
#include "asmstruc.h"

a_definition_struct Definition = { 0, NULL, NULL };

struct asm_sym *FindStructureMember( asm_sym *symbol, const char *name )
{
    field_list      *field;
    struct asm_sym  *sym;
    size_t          len;

    len = strlen( name ) + 1;
    for( field = ((dir_node *)symbol)->e.structinfo->head ; field != NULL; field = field->next ) {
        if( (sym = field->sym) != NULL ) {
            if( memcmp( name, sym->name, len ) == 0 ) {
                return( sym );
            }
        }
    }
    return( NULL );
}

bool StructDef( token_idx i )
/***************************/
{
    char        *name;
    dir_node    *dir;
    token_idx   n;

    if( Options.mode & MODE_IDEAL ) {
        n = i + 1;
        if( ( AsmBuffer[i].u.token == T_STRUC ) &&
            ( AsmBuffer[n].class != TC_ID ) ) {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
    } else {
        if( i > 0 ) {
            n = i - 1;
        } else {
            n = INVALID_IDX;
        }
        if( ( n == INVALID_IDX ) || ( AsmBuffer[n].class != TC_ID ) ) {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
    }
    name = AsmBuffer[n].string_ptr;
    switch( AsmBuffer[i].u.token ) {
    case T_STRUC:
    case T_STRUCT:
        dir = (dir_node *)AsmGetSymbol( name );
        if( Parse_Pass == PASS_1 ) {
            if( dir == NULL ) {
                dir = dir_insert( name, TAB_STRUCT );
            } else if( dir->sym.state == SYM_UNDEFINED ) {
                dir_change( dir, TAB_STRUCT );
            } else if( ( dir->sym.state == SYM_STRUCT ) && (Options.mode & MODE_IDEAL) ) {
                /* Redefinition of structure */
                FreeInfo( dir );
                dir_init( dir, TAB_STRUCT );
            } else {
                AsmError( SYMBOL_ALREADY_DEFINED );
                return( RC_ERROR );
            }
        }
        /* even if the current is null */
        push( &Definition.struct_stack, Definition.curr_struct );
        Definition.curr_struct = dir;
        Definition.struct_depth++;
        break;
    case T_ENDS:
        if( Options.mode & MODE_IDEAL ) {
            switch( AsmBuffer[n].class ) {
            case TC_FINAL:   /* Name absent */
                name = Definition.curr_struct->sym.name;
            case TC_ID:
                break;
            default:
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
        }
        if( Definition.curr_struct != NULL &&
            strcmp( name, Definition.curr_struct->sym.name ) == 0 ) {
            /* this is the right struct ... so end it */
            Definition.curr_struct = pop( &Definition.struct_stack );
            Definition.struct_depth--;
        } else {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

int InitializeStructure( asm_sym *sym, asm_sym *struct_symbol, token_idx i )
/**************************************************************************/
{
    /* input: a line that looks like : sym_name struct_name { init. values }
     * where i marks the struct_name
     */

    char            buffer[MAX_LINE_LEN];
    const char      *ptr;
    const char      *ptr1;
    const char      *ptr2;
    dir_node        *dir;
    field_list      *f;
    char            *p;
    size_t          len;

    dir = (dir_node *)struct_symbol;

    PushLineQueue();
    if( AsmBuffer[i].class != TC_STRING ) {
        AsmError( SYNTAX_ERROR ); // fixme
        return( ERROR );
    }
    if( sym != NULL ) {
        sym->total_size   = struct_symbol->total_size;
        sym->total_length = struct_symbol->total_length;
        sym->first_size   = struct_symbol->first_size;
        sym->first_length = struct_symbol->first_length;
    }

    ptr = AsmBuffer[i].string_ptr;
    for( f = dir->e.structinfo->head; f != NULL; f = f->next ) {
        /* put the lines to define the fields of the structure in,
         * using the values specified ( if any ) or the default ones otherwise
         */
        len = strlen( f->initializer );
        p = CATSTR( buffer, f->initializer, len );
        *p++ = ' ' ;
        len = strlen( f->value );
        if( ptr == NULL ) {
            p = CATSTR( p, f->value, len );
        } else if( (ptr2 = strpbrk( ptr, "," )) != NULL ) {
            for( ptr1 = ptr; isspace( *ptr1 ); ) {
                ++ptr1;
            }
            if( ptr1 == ptr2 ) {
                p = CATSTR( p, f->value, len );
            } else {
                len = ptr2 - ptr1;
                p = CATSTR( p, ptr1, len );
            }
            ptr = ptr2 + 1;   // go past the comma
        } else {
            for( ; isspace( *ptr ); ptr++ )
                ;
            if( *ptr == '\0' ) {
                p = CATSTR( p, f->value, len );
            } else {
                len = strlen( ptr );
                p = CATSTR( p, ptr, len );
            }
            ptr = NULL;
        }
        *p = '\0';
        InputQueueLine( buffer );
    }

    return( NOT_ERROR );
}

int AddFieldToStruct( asm_sym *sym, token_idx loc )
/*************************************************/
{
    int         offset;
    size_t      count;
    token_idx   i;
    struct_info *the_struct;
    field_list  *f;
    size_t      len;

    the_struct = Definition.curr_struct->e.structinfo;

    offset = the_struct->size; // offset of this element

    f = AsmAlloc( sizeof( field_list ) );

    if( loc == INVALID_IDX ) {
        for( loc = 0; AsmBuffer[loc].class != TC_FINAL; ++loc ) {
            /* nothing to do */
        }
    }
    if( Options.mode & MODE_IDEAL ) {
        f->sym = sym;   /* add the members symbol to the structure's list */
    } else {
        f->sym = NULL;
    }
    /* now add the initializer to the structure's list */
    len = strlen( AsmBuffer[loc].string_ptr ) + 1;
    f->initializer = AsmAlloc( len );
    memcpy( f->initializer, AsmBuffer[ loc ].string_ptr, len );

    /* now add the value to initialize the struct to */
    count = 0;
    for( i = loc + 1; AsmBuffer[i].class != TC_FINAL; i++ ) {
        if( AsmBuffer[i].string_ptr != NULL ) {
            count += strlen( AsmBuffer[i].string_ptr ) + 1;
        }
        if( AsmBuffer[i].class == TC_STRING ) {
            count += 2;
        }
    }

    f->value = AsmAlloc( count + 1 );
    f->value[0] = '\0';

    for( i = loc + 1; AsmBuffer[i].class != TC_FINAL; i++ ) {
        if( AsmBuffer[i].class == TC_STRING ) {
            strcat( f->value, "<" );
        }
        if( AsmBuffer[i].string_ptr != NULL ) {
            strcat( f->value, AsmBuffer[i].string_ptr );
        }
        if( AsmBuffer[i].class == TC_STRING ) {
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

int GetStructSize( asm_sym *struct_sym )
/**************************************/
{
    return( ((dir_node *)struct_sym)->e.structinfo->size );
}
