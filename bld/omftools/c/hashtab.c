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
* Description:  symbol hash function.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtab.h"

#define HASH_TABLE_SIZE 211

static unsigned int hashpjw( char *s )
/************************************/
{
    unsigned h;
    unsigned g;

    for( h = 0; *s; ++s ) {
        /* ( h & ~0x0fff ) == 0 is always true here */
        h = (h << 4) + (*(unsigned char *)s | ' ');
        g = h & ~0x0fff;
        h ^= g;
        h ^= g >> 12;
    }
    return( h % HASH_TABLE_SIZE );
}

void AddSymbol( sym_table sym_tbl, char *name, char *value )
/**********************************************************/
{
    symbol      **sym_ptr;
    symbol      *sym;

    sym_ptr = &sym_tbl[ hashpjw( name ) ];
    for( sym = *sym_ptr; sym; sym = sym->next ) {
        if( stricmp( name, sym->name ) == 0 ) {
            return;
        }
    }
    if( sym == NULL ) {
        sym = malloc( sizeof( symbol ) );
        sym->name = malloc( strlen( name ) + 1 );
        strcpy( sym->name, name );
        if( value != NULL ) {
            sym->value = malloc( strlen( value ) + 1 );
            strcpy( sym->value, value );
        } else {
            sym->value = NULL;
        }
        sym->next = *sym_ptr;
        *sym_ptr = sym;
    }
    return;
}

char *SymbolExists( sym_table sym_tbl, char *name )
/*************************************************/
{
    symbol      *sym;

    for( sym = sym_tbl[ hashpjw( name ) ]; sym; sym = sym->next ) {
        if( stricmp( name, sym->name ) == 0 ) {
            if( sym->value != NULL ) {
                return( sym->value );
            } else {
                return( "" );
            }
        }
    }
    return( NULL );
}

sym_table SymbolInit( void )
/**************************/
{
    sym_table sym_tbl;

    sym_tbl = malloc( sizeof( symbol * ) * HASH_TABLE_SIZE );
    memset( sym_tbl, 0, sizeof( symbol * ) * HASH_TABLE_SIZE );
    return( sym_tbl );
}

void SymbolFini( sym_table sym_tbl )
/**********************************/
{
    struct symbol      *sym;
    struct symbol      *next;
    unsigned            i;

    for( i = 0; i < HASH_TABLE_SIZE; i++ ) {
        for( sym = sym_tbl[i]; sym != NULL; sym = next ) {
            next = sym->next;
            free( sym->name );
            free( sym->value );
            free( sym );
        }
    }
}
