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
* Description:  Symbol table management for ssl.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ssl.h"
#include "sslint.h"


extern char     TokenBuff[];
extern unsigned TokenLen;
extern FILE     *PrsFile;
extern token    CurrToken;


symbol          *SymLst;


static char LookupAndAdd( class typ, symbol **ptr )
{
    symbol      **owner;
    symbol      *curr;

    if( CurrToken != T_NAME && CurrToken != T_LITERAL ) {
        Error( "expecting name" );
    }
    owner = &SymLst;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        if( strcmp( curr->name, TokenBuff ) == 0 ) {
            *ptr = curr;
            return( 0 );
        }
        if( curr->alias != NULL && strcmp( curr->alias, TokenBuff ) == 0 ) {
            *ptr = curr;
            return( 0 );
        }
        owner = &curr->link;
    }
    curr = calloc( 1, sizeof( symbol ) );
    if( curr == NULL ) Error( "out of memory" );
    *owner = curr;
    curr->link = NULL;
    curr->name = malloc( TokenLen + 1 );
    if( curr->name == NULL ) Error( "out of memory" );
    strcpy( curr->name, TokenBuff );
    curr->alias = NULL;
    curr->typ = typ;
    *ptr = curr;
    return( 1 );
}


symbol  *NewSym( class typ )
{
    symbol      *curr;
    if( !LookupAndAdd( typ, &curr ) ) {
        Error( "'%s' previously defined", TokenBuff );
    }
    return( curr );
}

void NewAlias( symbol *sym )
{
    sym->alias = malloc( TokenLen + 1 );
    if( sym->alias == NULL ) Error( "out of memory" );
    strcpy( sym->alias, TokenBuff );
}


symbol *Lookup( class typ )
{
    symbol      *curr;

    if( LookupAndAdd( typ, &curr ) ) {
        if( typ == CLASS_RULE ) {
            curr->v.rule.lbl = GenNewLbl();
        } else {
            Error( "'%s' not previously defined", TokenBuff );
        }
    } else {
        if( typ == CLASS_ANY ) {
            ; /* ok */
        } else if( typ == curr->typ ) {
            ; /* ok */
        } else if( (typ == CLASS_INPUT || typ == CLASS_OUTPUT)
                && curr->typ == CLASS_INOUT ) {
            ; /* ok */
        } else {
            Error( "'%s' has improper type", TokenBuff );
        }
    }
    return( curr );
}

typedef struct token_entry {
    struct token_entry  *link;
    char                *name;
    unsigned            value;
    unsigned            len;
} token_entry;

typedef struct {
    token_entry *head;
    unsigned    len;
} token_list;

token_list      Keywords;
token_list      Delimiters;
token_list      Exports;

static void AddToList( token_list *list, char *name, unsigned value )
{
    unsigned    len;
    token_entry **owner;
    token_entry *curr;
    token_entry *new;

    len = strlen( name )+(sizeof(char) + sizeof(unsigned short));
    list->len += len;
    owner = &list->head;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        if( len > curr->len ) break;
        owner = &curr->link;
    }
    new = malloc( sizeof( token_entry ) );
    new->link = curr;
    *owner = new;
    new->name = name;
    new->value = value;
    new->len = len;
}


static void AddToken( char *name, unsigned value )
{
    if( name == NULL ) return;
    if( *name == '_' || isalnum( *name ) ) {
        AddToList( &Keywords, name, value );
    } else {
        AddToList( &Delimiters, name, value );
    }
}


static void OutList( char *name, token_list *list )
{
    token_entry *curr;
    char        *ptr;

    OutStartSect( name, list->len + 1 );
    for( curr = list->head; curr != NULL; curr = curr->link ) {
        for( ptr = curr->name; *ptr != '\0'; ++ptr ) {
            OutByte( *ptr );
        }
        OutByte( '\0' );
        OutWord( curr->value );
    }
    OutByte( '\0' );
    OutEndSect();
}


void DumpSymTbl(void)
{
    symbol      *curr;

    for( curr = SymLst; curr != NULL; curr = curr->link ) {
        switch( curr->typ ) {
        case CLASS_INPUT:
            Dump( "INPUT:  [%4d] %-20s '%s'\n", curr->v.token,
                        curr->name,
                        (curr->alias == NULL) ? "" : curr->alias );
            AddToken( curr->alias, curr->v.token );
            break;
        case CLASS_OUTPUT:
            Dump( "OUTPUT: [%4d] %-20s '%s'\n", curr->v.token,
                        curr->name,
                        (curr->alias == NULL) ? "" : curr->alias );
            break;
        case CLASS_INOUT:
            Dump( "INOUT:  [%4d] %-20s '%s'\n", curr->v.token,
                        curr->name,
                        (curr->alias == NULL) ? "" : curr->alias );
            AddToken( curr->alias, curr->v.token );
            break;
        case CLASS_ERROR:
            Dump( "ERROR:  [%4d] %s\n", curr->v.token,  curr->name );
            break;
        case CLASS_TYPE:
            Dump( "TYPE:   %s\n", curr->name );
            break;
        case CLASS_SEM:
            Dump( "SEM:    [%4d] %s", curr->v.sem.value,
                        curr->name );
            if( curr->v.sem.parm != NULL ) {
                Dump( "( %s )", curr->v.sem.parm->name );
            }
            if( curr->v.sem.ret != NULL ) {
                Dump( " >> %s", curr->v.sem.ret->name );
            }
            Dump( "\n" );
            break;
        case CLASS_RULE:
            if( curr->v.rule.lbl->location == NO_LOCATION ) {
                Error( "rule '%s' has not been defined", curr->name );
            }
            Dump( "RULE: %c[L%.4x] %s",
                        curr->v.rule.exported ? '*' : ' ',
                        curr->v.rule.lbl->location, curr->name );
            if( curr->v.rule.ret != NULL ) {
                Dump( " >> %s", curr->v.rule.ret->name );
            }
            Dump( "\n" );
            if( curr->v.rule.exported ) {
                AddToList( &Exports, curr->name, curr->v.rule.lbl->location );
            }
            break;
        case CLASS_ENUMS:
           Dump( "ENUM:   [%4d] %s >> %s\n", curr->v.enums.value,
                curr->name, curr->v.enums.type->name );
            break;
        }
    }
    OutList( "Keywords", &Keywords );
    OutList( "Delims", &Delimiters );
    OutList( "Exports", &Exports );
}
