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
#include "asmops1.h"//
#include "asmops2.h"
#include "asmins1.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmglob.h"

extern struct AsmCodeName           AsmOpcode[];
extern char                         AsmChars[];

#include "hash.h"

struct AsmCodeName *inst_table[ HASH_TABLE_SIZE ] = { NULL };


static struct AsmCodeName **find( char *name )
/********************************************/
/* find an instruction in the hash table */
{
    struct AsmCodeName **inst;

    inst = &inst_table[ hashpjw( name ) ];

    for( ; *inst; inst = &((*inst)->next) ) {
        /* check if the name matches the entry for this inst in AsmChars */
        if( strnicmp( name, &AsmChars[ (*inst)->index ], (*inst)->len ) == 0
            && name[ (*inst)->len ] == '\0' ) {
            return( inst );
        }
    }
    return( inst );
}

static struct AsmCodeName *add( struct AsmCodeName *inst )
/********************************************************/
{
    struct AsmCodeName  **location;
    char buffer[20];
    strncpy( buffer, (char *)&(AsmChars[inst->index]), inst->len );
    buffer[ inst->len ] = '\0';

    location = find( buffer );

    if( *location != NULL ) {
        /* we already have one */
        AsmError( SYMBOL_ALREADY_DEFINED ); // fixme
        return( NULL );
    }

    inst->next = *location;
    *location = inst;

    return( inst );
}

/* entry points */

int get_instruction_position( char *string )
/******************************************/
{
    struct AsmCodeName **inst;

    inst = find( string );

    if( *inst ) return( (*inst)->position );
    return( EMPTY );
}

// fixme -- make this whole table static? use indices instead of pointers

void make_inst_hash_table( void )
/*******************************/
{
    unsigned short i;

    for( i=0; i != T_NULL; i++ ) {
        add( &AsmOpcode[i] );
    }
    return;
}
