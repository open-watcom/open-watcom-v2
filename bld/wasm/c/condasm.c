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
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include "asmglob.h"
#include "asmops1.h"
#include "asmins1.h"
#include "directiv.h"
#include "asmerr.h"
#include "myassert.h"
#include "asmalloc.h"
#include "expand.h"
#include "asmdefs.h"

extern struct asm_tok   *AsmBuffer[];
extern struct AsmCodeName AsmOpcode[];
extern const struct asm_ins ASMFAR AsmOpTable[];

extern void             AsmError( int );

extern int              get_instruction_position( char *string );

enum if_state CurState = ACTIVE;
// fixme char *IfSymbol;        /* save symbols in IFDEF's so they don't get expanded */
#define    MAX_NESTING  20

void prep_line_for_conditional_assembly( char *line )
/***************************************************/
{
    char        *ptr;
    char        *end;
    int         count;
    char        delim;
    char        fix;

    if( Comment( QUERY_COMMENT, 0 ) ) {
        delim = (char)Comment( QUERY_COMMENT_DELIM, 0 );
        if( strchr( line, delim ) != NULL ) {
            Comment( END_COMMENT, 0 );
        }
        *line = '\0';
        return;
    }

    if( *line == '\0' ) return;

    /* fixme */
    /* get rid of # signs */
    ptr = line + strspn( line, " \t" );
    for( end = ptr; isalnum( *end ) || *end == '#' ; ++end )
        /* nothing to do */ ;
    fix = *end;
    *end = '\0';

    if( *ptr == '#' ) {
        /* treat #if as if, remove # sign */
        *ptr =' ';
        ptr++;
    }

    count = get_instruction_position( ptr );
    *end = fix;
    if( count == EMPTY ) {
        /* if it is not in the table */
        if( CurState == LOOKING_FOR_TRUE_COND || CurState == DONE ) {
            *line = '\0';
        }
        return;
    }

    /* otherwise, see if it is a conditional assembly directive */

    switch( AsmOpTable[count].token) {
    case T_IFDEF:
    case T_IFNDEF:
    case T_IFB:
    case T_IFNB:
    case T_IF:
    case T_IF1:
    case T_IF2:
    case T_IFE:
    case T_ELSEIF:
    case T_ELSE:
    case T_ENDIF:
    case T_IFDIF:
    case T_IFDIFI:
    case T_IFIDN:
    case T_IFIDNI:
        break;
    default:
        if( CurState == LOOKING_FOR_TRUE_COND || CurState == DONE ) {
            *line = '\0';
        }
    }
    return;
}

static char check_defd( char *string )
/************************************/
{
    char        *ptr;
    char        *end;
    struct asm_sym      **sym_ptr;

    /* isolate 1st word */
    ptr = string + strspn( string, " \t" );
    end = ptr + strcspn( ptr, " \t" );
    *end = '\0';


    sym_ptr = AsmFind( ptr );

    if( *sym_ptr != NULL ) {
        *end = '\0';
        return( TRUE );
    } else {
        *end = '\0';
        return( FALSE );
    }
}

static char check_blank( char *string )
/************************************/
{
    #if 0
    char        *ptr;
    char        *end;

    /* isolate word inside < > */
    /* error condtions? */
    for( ptr = string; *ptr !='\0' && *ptr != '<'; ptr++ );
    ptr++;
    for( end = ptr; *end !='\0' && *end != '>'; end++ );
    *end = '\0';
    return( strlen( ptr ) == 0 ? TRUE : FALSE );
    #endif

    return( strlen( string ) == 0 ? TRUE : FALSE );
}

static char check_dif( bool sensitive, char *string, char *string2 )
/******************************************************************/
{
    if( sensitive ) {
        return( strcmp( string, string2 ) != 0 );
    } else {
        return( stricmp( string, string2 ) != 0 );
    }
}

int conditional_error_directive( int i )
/**************************************/
{
    uint_16         direct;

    direct = AsmBuffer[i]->value;

    /* expand any constants if necessary */
    switch( direct ) {
    case T_ERRE:
    case T_ERRNZ:
    case T_ERRDIF:
    case T_ERRDIFI:
    case T_ERRIDN:
    case T_ERRIDNI:
        ExpandTheWorld( i+1, FALSE );
    }

    switch( direct ) {
    case T_ERR:
        AsmErr( FORCED );
        return( ERROR );
    case T_ERRNZ:
        if( AsmBuffer[i+1]->token == T_NUM && AsmBuffer[i+1]->value ) {
            AsmErr( FORCED_NOT_ZERO, AsmBuffer[i+1]->value );
            return( ERROR );
        }
        break;
    case T_ERRE:
        if( AsmBuffer[i+1]->token == T_NUM && !AsmBuffer[i+1]->value ) {
            AsmErr( FORCED_EQUAL, AsmBuffer[i+1]->value );
            return( ERROR );
        }
        break;
    case T_ERRDEF:
        if( check_defd( AsmBuffer[i+1]->string_ptr ) ) {
            AsmErr( FORCED_DEF, AsmBuffer[i+1]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRNDEF:
        if( !check_defd( AsmBuffer[i+1]->string_ptr ) ) {
            AsmErr( FORCED_NOT_DEF, AsmBuffer[i+1]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRB:
        if( AsmBuffer[i+1]->token == T_STRING &&
            check_blank( AsmBuffer[i+1]->string_ptr ) ) {
            AsmErr( FORCED_BLANK, AsmBuffer[i+1]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRNB:
        if( AsmBuffer[i+1]->token != T_STRING ||
            !check_blank( AsmBuffer[i+1]->string_ptr ) ) {
            AsmErr( FORCED_NOT_BLANK, AsmBuffer[i+1]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRDIF:
        if( check_dif( TRUE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ) {
            AsmErr( FORCED_DIF, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRDIFI:
        if( check_dif( FALSE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ) {
            AsmErr( FORCED_DIF, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRIDN:
        if( !check_dif( TRUE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ) {
            AsmErr( FORCED_IDN, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr );
            return( ERROR );
        }
        break;
    case T_ERRIDNI:
        if( !check_dif( FALSE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ) {
            AsmErr( FORCED_IDN, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr );
            return( ERROR );
        }
        break;
    }
    return( NOT_ERROR );
}

static int_8    NestLevel = 0;

int conditional_assembly_directive( int i )
/*****************************************/
{
    uint_16         direct;
    static int_8    falseblocknestlevel = 0;

    direct = AsmBuffer[i]->value;

    switch( CurState ) {
    case ACTIVE:
        switch( direct ) {
        case T_IF:
        case T_IF1:
        case T_IF2:
        case T_IFE:
        case T_IFDEF:
        case T_IFNDEF:
        case T_IFB:
        case T_IFNB:
        case T_IFDIF:
        case T_IFDIFI:
        case T_IFIDN:
        case T_IFIDNI:
            NestLevel++;
            if( NestLevel > MAX_NESTING ) {
                /*fixme */
                AsmError( NESTING_LEVEL_TOO_DEEP );
                return( ERROR );
            }
        }
        break;
    case LOOKING_FOR_TRUE_COND:
        if( direct == T_ELSE || direct == T_ELSEIF ) {
            if( falseblocknestlevel == 0 ) {
                break;
            }
        }
        /* fall through for other cases */
    case DONE:
        switch( direct ) {
        case T_IF:
        case T_IF1:
        case T_IF2:
        case T_IFE:
        case T_IFDEF:
        case T_IFNDEF:
        case T_IFB:
        case T_IFNB:
        case T_IFDIF:
        case T_IFDIFI:
        case T_IFIDN:
        case T_IFIDNI:
            falseblocknestlevel++;
            return( NOT_ERROR );
            break;
        case T_ENDIF:
            if( falseblocknestlevel > 0 ) {
                falseblocknestlevel--;
                return( NOT_ERROR );
            }
            break;
        case T_ELSE:
        case T_ELSEIF:
            return( NOT_ERROR );
        default:
            /**/myassert( 0 );
            return( ERROR );
        }
    }

    /* expand any constants if necessary */
    switch( direct ) {
    case T_IF:
    case T_IFE:
    case T_ELSEIF:
    case T_IFDIF:
    case T_IFDIFI:
    case T_IFIDN:
    case T_IFIDNI:
        ExpandTheWorld( i+1, FALSE );
    }

    switch( direct ) {
    case T_IF1:
        CurState = Parse_Pass == PASS_1 ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IF2:
        CurState = Parse_Pass == PASS_1 ? LOOKING_FOR_TRUE_COND : ACTIVE;
        break;
    case T_IF:
        CurState = ( AsmBuffer[i+1]->token == T_NUM && AsmBuffer[i+1]->value )
                   ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFE:
        CurState = ( AsmBuffer[i+1]->token == T_NUM && !AsmBuffer[i+1]->value )
                   ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFDEF:
        CurState = check_defd( AsmBuffer[i+1]->string_ptr )  ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFNDEF:
        CurState = !check_defd( AsmBuffer[i+1]->string_ptr )  ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFB:
        CurState = check_blank( AsmBuffer[i+1]->string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFNB:
        CurState = !check_blank( AsmBuffer[i+1]->string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFDIF:
        CurState = check_dif( TRUE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFDIFI:
        CurState = check_dif( FALSE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFIDN:
        CurState = !check_dif( TRUE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFIDNI:
        CurState = !check_dif( FALSE, AsmBuffer[i+1]->string_ptr, AsmBuffer[i+3]->string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_ELSE:
    case T_ELSEIF:
        switch( CurState ) {
        case ACTIVE:
            CurState = DONE;
            break;
        case LOOKING_FOR_TRUE_COND:
            if( direct == T_ELSE ) {
                CurState = ACTIVE;
            } else {
                CurState = ( AsmBuffer[i+1]->token == T_NUM &&
                             AsmBuffer[i+1]->value )
                           ? ACTIVE : LOOKING_FOR_TRUE_COND;
            }
            break;
        }
        break;
    case T_ENDIF:
        NestLevel--;
        if( NestLevel < 0 ) {
            NestLevel = 0;
            AsmError( BLOCK_NESTING_ERROR );
            return( ERROR );
        }
        CurState = ACTIVE;
    }
    return( NOT_ERROR );
}

void CheckForOpenConditionals()
{
    if( NestLevel > 0 ) {
        AsmErr( UNCLOSED_CONDITIONALS, NestLevel );
    }
}
