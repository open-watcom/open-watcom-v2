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
* Description:  WASM conditional processing routines
*
****************************************************************************/


#include "asmglob.h"
#include <ctype.h>
#include "directiv.h"
#include "asmexpnd.h"
#include "condasm.h"
#include "myassert.h"
#include "clibext.h"

#define    MAX_NESTING  20

typedef enum if_state {
    ACTIVE,                 /* current IF cond is true */
    LOOKING_FOR_TRUE_COND,  /* current IF cond is false, looking for elseif */
    DONE                    /* done true section of current if, just nuke
                               everything until we see an endif */
} if_state;

extern const asm_ins    ASMFAR *get_instruction( char *string );

extern int          MacroExitState;

static if_state     CurState = ACTIVE;
static int_8        NestLevel = 0;
static int_8        falseblocknestlevel = 0;


// fixme char *IfSymbol;        /* save symbols in IFDEF's so they don't get expanded */

void IfCondInit( void )
/*********************/
{
    CurState = ACTIVE;
    NestLevel = 0;
    falseblocknestlevel = 0;
}

void IfCondFini( void )
/*********************/
{
    if( NestLevel > 0 ) {
        AsmErr( UNCLOSED_CONDITIONALS, NestLevel );
    }
}

void prep_line_for_conditional_assembly( char *line )
/***************************************************/
{
    char            *ptr;
    char            *end;
    char            fix;
    const asm_ins   ASMFAR *ins;

    if( Comment( QUERY_COMMENT, 0, NULL ) ) {
        if( Comment( QUERY_COMMENT_DELIM, 0, line ) ) {
            Comment( END_COMMENT, 0, NULL );
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

    ins = get_instruction( ptr );
    *end = fix;
    if( ins == NULL ) {
        /* if it is not in the table */
        if( CurState == LOOKING_FOR_TRUE_COND || CurState == DONE || MacroExitState ) {
            *line = '\0';
        }
        return;
    }

    /* otherwise, see if it is a conditional assembly directive */

    switch( ins->token) {
    case T_ELSE:
    case T_ELSEIF:
    case T_ELSEIF1:
    case T_ELSEIF2:
    case T_ELSEIFB:
    case T_ELSEIFDEF:
    case T_ELSEIFE:
    case T_ELSEIFDIF:
    case T_ELSEIFDIFI:
    case T_ELSEIFIDN:
    case T_ELSEIFIDNI:
    case T_ELSEIFNB:
    case T_ELSEIFNDEF:
    case T_ENDIF:
    case T_IF:
    case T_IF1:
    case T_IF2:
    case T_IFB:
    case T_IFDEF:
    case T_IFE:
    case T_IFDIF:
    case T_IFDIFI:
    case T_IFIDN:
    case T_IFIDNI:
    case T_IFNB:
    case T_IFNDEF:
        break;
    default:
        if( CurState == LOOKING_FOR_TRUE_COND || CurState == DONE || MacroExitState ) {
            *line = '\0';
        }
    }
    return;
}

static bool check_defd( char *string )
/************************************/
{
    char                *ptr;
    char                *end;

    /* isolate 1st word */
    ptr = string + strspn( string, " \t" );
    end = ptr + strcspn( ptr, " \t" );
    *end = '\0';
    return( AsmGetSymbol( ptr ) != NULL );
}

static bool check_blank( char *string )
/*************************************/
{
    return( strlen( string ) == 0 );
}

static bool check_dif( bool sensitive, char *string, char *string2 )
/******************************************************************/
{
    if( sensitive ) {
        return( strcmp( string, string2 ) != 0 );
    } else {
        return( stricmp( string, string2 ) != 0 );
    }
}

bool conditional_error_directive( token_idx i )
/*********************************************/
{
    asm_token   direct;

    direct = AsmBuffer[i].u.token;

    /* expand any constants if necessary */
    switch( direct ) {
    case T_DOT_ERRE:
    case T_DOT_ERRNZ:
    case T_DOT_ERRDIF:
    case T_DOT_ERRDIFI:
    case T_DOT_ERRIDN:
    case T_DOT_ERRIDNI:
    case T_ERRIFE:
    case T_ERRIFDIF:
    case T_ERRIFDIFI:
    case T_ERRIFIDN:
    case T_ERRIFIDNI:
        ExpandTheWorld( i + 1, false, true );
    }

    switch( direct ) {
    case T_DOT_ERR:
    case T_ERR:
        AsmErr( FORCED );
        return( RC_ERROR );
    case T_DOT_ERRNZ:
        if( AsmBuffer[i+1].class == TC_NUM && AsmBuffer[i+1].u.value ) {
            AsmErr( FORCED_NOT_ZERO, AsmBuffer[i+1].u.value );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRE:
    case T_ERRIFE:
        if( AsmBuffer[i+1].class == TC_NUM && !AsmBuffer[i+1].u.value ) {
            AsmErr( FORCED_EQUAL, AsmBuffer[i+1].u.value );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRDEF:
    case T_ERRIFDEF:
        if( check_defd( AsmBuffer[i+1].string_ptr ) ) {
            AsmErr( FORCED_DEF, AsmBuffer[i+1].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRNDEF:
    case T_ERRIFNDEF:
        if( !check_defd( AsmBuffer[i+1].string_ptr ) ) {
            AsmErr( FORCED_NOT_DEF, AsmBuffer[i+1].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRB:
    case T_ERRIFB:
        if( AsmBuffer[i+1].class == TC_STRING &&
            check_blank( AsmBuffer[i+1].string_ptr ) ) {
            AsmErr( FORCED_BLANK, AsmBuffer[i+1].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRNB:
    case T_ERRIFNB:
        if( AsmBuffer[i+1].class != TC_STRING ||
            !check_blank( AsmBuffer[i+1].string_ptr ) ) {
            AsmErr( FORCED_NOT_BLANK, AsmBuffer[i+1].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRDIF:
    case T_ERRIFDIF:
        if( check_dif( true, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ) {
            AsmErr( FORCED_DIF, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRDIFI:
    case T_ERRIFDIFI:
        if( check_dif( false, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ) {
            AsmErr( FORCED_DIF, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRIDN:
    case T_ERRIFIDN:
        if( !check_dif( true, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ) {
            AsmErr( FORCED_IDN, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr );
            return( RC_ERROR );
        }
        break;
    case T_DOT_ERRIDNI:
    case T_ERRIFIDNI:
        if( !check_dif( false, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ) {
            AsmErr( FORCED_IDN, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr );
            return( RC_ERROR );
        }
        break;
    }
    return( RC_OK );
}

static if_state get_cond_state( token_idx i )
{
    asm_token   direct;
    if_state    cond_state;

    direct = AsmBuffer[i].u.token;

    /* expand any constants if necessary */
    switch( direct ) {
    case T_IF:
    case T_IFDIF:
    case T_IFDIFI:
    case T_IFE:
    case T_IFIDN:
    case T_IFIDNI:
    case T_ELSEIF:
    case T_ELSEIFDIF:
    case T_ELSEIFDIFI:
    case T_ELSEIFE:
    case T_ELSEIFIDN:
    case T_ELSEIFIDNI:
        ExpandTheWorld( i + 1, false, true );
    }

    switch( direct ) {
    case T_IF:
    case T_ELSEIF:
        cond_state = ( AsmBuffer[i+1].class == TC_NUM && AsmBuffer[i+1].u.value )
                   ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IF1:
    case T_ELSEIF1:
        cond_state = Parse_Pass == PASS_1 ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IF2:
    case T_ELSEIF2:
        cond_state = Parse_Pass == PASS_1 ? LOOKING_FOR_TRUE_COND : ACTIVE;
        break;
    case T_IFB:
    case T_ELSEIFB:
        cond_state = check_blank( AsmBuffer[i+1].string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFDEF:
    case T_ELSEIFDEF:
        cond_state = check_defd( AsmBuffer[i+1].string_ptr )  ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFE:
    case T_ELSEIFE:
        cond_state = ( AsmBuffer[i+1].class == TC_NUM && !AsmBuffer[i+1].u.value )
                   ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFDIF:
    case T_ELSEIFDIF:
        cond_state = check_dif( true, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFDIFI:
    case T_ELSEIFDIFI:
        cond_state = check_dif( false, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFIDN:
    case T_ELSEIFIDN:
        cond_state = !check_dif( true, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFIDNI:
    case T_ELSEIFIDNI:
        cond_state = !check_dif( false, AsmBuffer[i+1].string_ptr, AsmBuffer[i+3].string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFNB:
    case T_ELSEIFNB:
        cond_state = !check_blank( AsmBuffer[i+1].string_ptr ) ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    case T_IFNDEF:
    case T_ELSEIFNDEF:
        cond_state = !check_defd( AsmBuffer[i+1].string_ptr )  ? ACTIVE : LOOKING_FOR_TRUE_COND;
        break;
    default:
        cond_state = DONE;
        break;
    }
    return( cond_state );
}

bool conditional_assembly_directive( token_idx i )
/************************************************/
{
    asm_token   direct;

    direct = AsmBuffer[i].u.token;

    switch( direct ) {
    case T_IF:
    case T_IF1:
    case T_IF2:
    case T_IFB:
    case T_IFDEF:
    case T_IFDIF:
    case T_IFDIFI:
    case T_IFE:
    case T_IFIDN:
    case T_IFIDNI:
    case T_IFNB:
    case T_IFNDEF:
        if( CurState == ACTIVE ) {
            NestLevel++;
            if( NestLevel > MAX_NESTING ) {
                /*fixme */
                AsmError( NESTING_LEVEL_TOO_DEEP );
                return( RC_ERROR );
            }
            CurState = get_cond_state( i );
        } else {
            falseblocknestlevel++;
        }
        break;
    case T_ELSEIF:
    case T_ELSEIF1:
    case T_ELSEIF2:
    case T_ELSEIFB:
    case T_ELSEIFDEF:
    case T_ELSEIFDIF:
    case T_ELSEIFDIFI:
    case T_ELSEIFE:
    case T_ELSEIFIDN:
    case T_ELSEIFIDNI:
    case T_ELSEIFNB:
    case T_ELSEIFNDEF:
        if( CurState == ACTIVE ) {
            CurState = DONE;
        } else if( CurState == LOOKING_FOR_TRUE_COND ) {
            if( falseblocknestlevel == 0 ) {
                CurState = get_cond_state( i );
            }
        }
        break;
    case T_ELSE:
        if( CurState == ACTIVE ) {
            CurState = DONE;
        } else if( CurState == LOOKING_FOR_TRUE_COND ) {
            if( falseblocknestlevel == 0 ) {
                CurState = ACTIVE;
            }
        }
        break;
    case T_ENDIF:
        if( CurState == ACTIVE || falseblocknestlevel == 0 ) {
            NestLevel--;
            if( NestLevel < 0 ) {
                NestLevel = 0;
                AsmError( BLOCK_NESTING_ERROR );
                return( RC_ERROR );
            }
            CurState = ACTIVE;
        } else {
            falseblocknestlevel--;
        }
        break;
    default:
        /**/myassert( 0 );
        return( RC_ERROR );
    }
    return( RC_OK );
}
