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
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "asmglob.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmops1.h"
#include "asmops2.h"
#include "asmins1.h"
#include "namemgr.h"
#include "asmsym.h"

#include "womp.h"
#include "pcobj.h"
#include "objrec.h"
#include "myassert.h"
#include "fixup.h"
#include "queue.h"

#include "expand.h"
#include "directiv.h"
extern  char            StringBuf[];
extern int              Token_Count;    // number of tokens on line
extern  struct asm_tok  *AsmBuffer[];   // buffer to store token
extern dir_node         *CurrProc;
extern  char            Parse_Pass;     // phase of parsing
extern int              LineNumber;

extern void             InputQueueLine( char * );
extern void             PushLineQueue(void);
extern void             wipe_space( char *token );
extern int              AsmScan( char *, char * );
extern dir_node         *dir_insert( char *name, int tab );
extern int              EvalExpr( int, int, int );
extern void             GetInsString( enum asm_token , char *, int );
extern int              MakeLabel( char *symbol_name, int mem_type );

#define    MAX_EQU_NESTING      20

static label_list *label_cmp( char *name, label_list *head )
{
    label_list  *curr;

    for( curr = head; curr; curr = curr->next ) {
        if( stricmp( curr->label, name ) == 0 ) {
            return( curr );
        }
    }
    return( NULL );
}


void AddTokens( struct asm_tok **buffer, int start, int count )
/************************************************************/
{
    int i;

    switch( count ) {
    case 0:
        return;
    case -1:
        /* it's an empty expansion */
        for( i = start; i <= Token_Count; ++i ) {
            *buffer[i] = *buffer[i+1];
        }
        break;
    default:
        for( i=Token_Count; i >= start; i-- ) {
            *buffer[i+count] = *buffer[i];
        }
        break;
    }
    Token_Count += count;
}

int ExpandSymbol( int i, int early_only )
/***************************************/
{
    struct asm_sym      *sym;
    dir_node            *dir;
    int                 j;

    /* expand constant */
    sym = AsmGetSymbol( AsmBuffer[i]->string_ptr );
    if( sym == NULL ) return( NOT_ERROR );
    switch( sym->state ) {
    case SYM_CONST:
        dir = (dir_node *)sym;
        if( !dir->e.constinfo->expand_early && early_only ) return( NOT_ERROR );
        DebugMsg(( "Expand Constant: %s ->", sym->name ));
        /* insert the pre-scanned data for this constant */
        AddTokens( AsmBuffer, i, dir->e.constinfo->count - 1 );
        for( j = 0; j < dir->e.constinfo->count; j++ ) {
            AsmBuffer[i+j]->token = dir->e.constinfo->data[j].token;
            AsmBuffer[i+j]->value = dir->e.constinfo->data[j].value;
            AsmBuffer[i+j]->string_ptr = dir->e.constinfo->data[j].string_ptr;
            #ifdef DEBUG_OUT
            if( AsmBuffer[i+j]->token == T_NUM ) {
                DebugMsg(( " %d", AsmBuffer[i+j]->value ));
            } else {
                DebugMsg(( " %s", AsmBuffer[i+j]->string_ptr ));
            }
            #endif
        }
        DebugMsg(( "\n" ));
        Globals.expand_count++;
        if( Globals.expand_count >= MAX_EQU_NESTING ) {
            AsmError( NESTING_LEVEL_TOO_DEEP );
            return( ERROR );
        }
        return( STRING_EXPANDED );
    }
    return( NOT_ERROR );
}

int ExpandProcString( int index )
/*******************************/
{
    int_8               i;
    int_8               cnt;
    int_8               count = 0; /* number of words in the name string */
    char                *string;
    char                *word;
    char                *replace = NULL;
    char                buffer[MAX_LINE_LEN];
    label_list          *label;

    string = AsmTmpAlloc( strlen( AsmBuffer[index]->string_ptr ) + 1 );
    strcpy( string, AsmBuffer[index]->string_ptr );
    wipe_space( string );
    word = strtok( string, " \t" );
    while( word != NULL ) {
        count++;

        /**/myassert( CurrProc != NULL );
        label = label_cmp( word, CurrProc->e.procinfo->paralist );
        if( label == NULL ) {
            label = label_cmp( word, CurrProc->e.procinfo->locallist );
        }
        if( label != NULL ) {
            if( label->replace != NULL ) {
                replace = label->replace;
            }
        }

        if( replace != NULL ) {
            if( *replace == '\0' ) {
                AsmErr( CANT_ACCESS_MULTI_REG_PARMS, replace+1 );
                return( ERROR );
            }
            if( index > 0 && AsmBuffer[index-1]->token == T_DIRECTIVE ) {
                switch( AsmBuffer[index-1]->value ) {
                case T_IFDEF:
                case T_IFNDEF:
                    /* do NOT expand strings in IFDEF and IFNDEF ins.
                     * we want to know if they are defined, NOT their value
                     */
                    return( NOT_ERROR );
                }
            }
            if( AsmBuffer[index+1]->token == T_DIRECTIVE ) {
                /* this will never happen with multiple words in a string */
                switch( AsmBuffer[index+1]->value ) {
                case T_EQU:
                case T_EQU2:
                case T_TEXTEQU:
                    return( NOT_ERROR );
                }
            }
            break;
        }
        word = strtok( NULL, " \t" );
    }
    if( replace == NULL ) return( NOT_ERROR );

    DebugMsg(( "ExpandString: %s -> %s \n", word, replace ));

    /* now we need to build the new line string to pass through the scanner */
    buffer[0] = '\0';
    /* NOTE: if we have a T_DIRECTIVE, token_count is always set to 1 !??! */
    for( i=0; i < Token_Count; i++ ) {
        if( i != index ) {
            // if( expand_directive_string( buffer, i ) == ERROR ) return( ERROR );
            if( AsmBuffer[i]->token == T_STRING &&
                *AsmBuffer[i]->string_ptr == '\0' ) {
                strcat( buffer, "<>" );
            } else {
                strcat( buffer, AsmBuffer[i]->string_ptr );
            }
        } else {
            if( AsmBuffer[i]->token == T_PERCENT ) {
                /* don't save the % */
                i++;
            }
            // if( expand_directive_string( buffer, i ) == ERROR ) return( ERROR );

            /* copy the string in ... 1 word at a time */
            string = AsmTmpAlloc( strlen( AsmBuffer[index]->string_ptr ) + 1 );
            strcpy( string, AsmBuffer[index]->string_ptr );
            wipe_space( string );
            word = strtok( string, " \t" );
            for( cnt = 1; cnt < count; cnt++ ) {
                strcat( buffer, word );
                strcat( buffer, " " );
                word = strtok( NULL, " \t" );
            }
            strcat( buffer, replace );
            word = strtok( NULL, " \t" );
            while( word != NULL ) {
                strcat( buffer, " " );
                strcat( buffer, word );
                word = strtok( NULL, " \t" );
            }
        }
        strcat( buffer, " " );
    }
    /* make sure this line goes at the front of the queue */
    PushLineQueue();
    InputQueueLine( buffer );
    AsmBuffer[0]->token = 0;
    AsmBuffer[0]->string_ptr = NULL;
    AsmBuffer[0]->value = 0;
    return( STRING_EXPANDED );
}

int DefineConstant( int i, bool redefine, bool expand_early )
/***********************************************************/
/* if expand_early is TRUE, expand before doing any parsing */
{

    char                *name;

    if( i != 0 ) {
        AsmError( SYNTAX_ERROR );
        return( ERROR );
    }

    /* get the name */
    name = AsmBuffer[i++]->string_ptr;
    i++;

    return( createconstant( name, FALSE, i, redefine, expand_early ) );
}

int StoreConstant( char *name, char *value, int_8 redefine )
/**********************************************************/
{
    AsmScan( value, StringBuf );
    return( createconstant( name, FALSE, 0, redefine, FALSE ) );
}

void MakeConstant( long token )
/*****************************/
{
    char buffer[20];

    /* define a macro */
    buffer[0]='\0';
    strcat( buffer, "__" );
    GetInsString( (enum asm_token)token, buffer+2, 18 );
    strcat( buffer, "__" );
    strupr( buffer );
    createconstant( buffer, 1, 0, TRUE, FALSE );
}

static int createconstant( char *name, int value, int start, int_8 redefine, bool expand_early )
/**********************************************************************************************/
{
    struct asm_tok      *new;
    dir_node            *dir;
    struct asm_sym      *sym;
    int                 i;
    int                 count;

    sym = AsmGetSymbol( name );

    /* if we've never seen it before, put it in */
    if( sym == NULL ) {
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( ERROR );
        }
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = expand_early;
        dir->e.constinfo->data = NULL;
        dir->e.constinfo->count = 0;
        dir->sym.state = SYM_UNDEFINED; /* can't use the value yet */
    } else {
        /* check if it can be redefined */
        dir = (dir_node *)sym;
        if( sym->state == SYM_UNDEFINED ) {
            dir->e.constinfo = AsmAlloc( sizeof( const_info ) );
            dir->e.constinfo->redefine = redefine;
            dir->e.constinfo->expand_early = expand_early;
            sym->grpidx = sym->segidx = sym->offset = 0;
            dir->e.constinfo->data = NULL;
        } else  if( sym->state != SYM_CONST ||
            ( !dir->e.constinfo->redefine && Parse_Pass == PASS_1 ) ) {

            /* error */
            AsmError( LABEL_ALREADY_DEFINED );
            return( ERROR );
        }
    }

    if( value ) {
        /* just define it to be 1 and get out */
        new = AsmAlloc( sizeof( struct asm_tok ) );
        new[0].token = T_NUM;
        new[0].value = 1;
        new[0].string_ptr = NULL;

        if( dir->e.constinfo->data != NULL ) {
            FreeInfo( dir );
            dir->e.constinfo = AsmAlloc( sizeof( const_info ) );
            dir->e.constinfo->redefine = redefine;
            dir->e.constinfo->expand_early = expand_early;
        }
        dir->e.constinfo->count = 1;
        dir->e.constinfo->data = new;
        dir->sym.state = SYM_CONST;
        return( NOT_ERROR );
    }

    /* expand any constants */
    if( ExpandTheWorld( start, FALSE ) == ERROR ) return( ERROR );

    for( i=start; AsmBuffer[i]->token != T_FINAL; i++ );
    count = i-start;

    new = AsmAlloc( count * sizeof( struct asm_tok ) );
    for( i=0; i < count; i++ ) {
        switch( AsmBuffer[start+i]->token ) {
        case T_STRING:
            if( AsmBuffer[start+i]->value == 0 ) {
                i--;
                count--;
                continue;
            }
            break;
        case T_ID:
            if( AsmBuffer[start+i]->string_ptr[0] == '$'
             && AsmBuffer[start+i]->string_ptr[1] == '\0' ) {
                char            buff[40];
                /*
                    We want a '$' symbol to have the value at it's
                    point of definition, not point of expansion.
                */
                sprintf( buff, ".$%x/%lx", GetCurrSeg(), (unsigned long)GetCurrAddr() );
                AsmBuffer[start+i]->string_ptr = buff;
                sym = AsmGetSymbol( buff );
                if( sym == NULL ) MakeLabel( buff, T_NEAR );
            }
            break;
        }
        new[i].token = AsmBuffer[start + i]->token;
        new[i].value = AsmBuffer[start + i]->value;
        new[i].extra_value = AsmBuffer[start + i]->extra_value;
        if( AsmBuffer[start+i]->string_ptr == NULL ) {
            new[i].string_ptr = NULL;
        } else {
            new[i].string_ptr = AsmAlloc( strlen( AsmBuffer[start+i]->string_ptr ) + 1 );
            strcpy( new[i].string_ptr, AsmBuffer[start+i]->string_ptr );
        }
    }
    if( dir->e.constinfo->data != NULL ) {
        FreeInfo( dir );
        dir->e.constinfo = AsmAlloc( sizeof( const_info ) );
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = expand_early;
    }
    dir->e.constinfo->count = count;
    dir->e.constinfo->data = new;

    dir->sym.state = SYM_CONST;
    return( NOT_ERROR );
}

int ExpandTheWorld( int start_pos, bool early_only )
/**************************************************/
{
    int         val;

    if( ExpandAllConsts( start_pos, early_only ) == ERROR ) return( ERROR );
    if( !early_only ) {
        val = EvalExpr( Token_Count, start_pos, Token_Count );
        if( val == ERROR ) val = 0;
        Token_Count = val;
    }
    return( NOT_ERROR );

}

int ExpandAllConsts( int start_pos, bool early_only )
/***************************************************/
{
    int i;

    if( AsmBuffer[start_pos+1]->token == T_DIRECTIVE ) {
        switch( AsmBuffer[start_pos+1]->value ) {
        case T_EQU:
        case T_EQU2:
        case T_TEXTEQU:
            start_pos+=2;
        }
    }
    Globals.expand_count = 0;
    for( i=start_pos; AsmBuffer[i]->token != T_FINAL; i++ ) {
        if( AsmBuffer[i]->token != T_ID ) continue;
        switch( ExpandSymbol( i, early_only ) ) {
        case ERROR:
            return( ERROR );
        case STRING_EXPANDED:
            i--; // in case the new symbol also needs to be expanded
            continue;
        }
    }
    return( NOT_ERROR );
}

