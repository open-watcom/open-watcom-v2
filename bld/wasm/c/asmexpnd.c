/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WASM symbols expansion.
*
****************************************************************************/

#include "asmglob.h"
#include "asmeval.h"
#include "asmexpnd.h"
#if defined( _STANDALONE_ )
#include "asmalloc.h"
#include "directiv.h"
#include "asmlabel.h"
#include "asminput.h"
#include "myassert.h"

#include "clibext.h"

#endif


#if defined( _STANDALONE_ )

#define    MAX_EQU_NESTING      20

static label_list *label_cmp( char *name, label_list *head )
{
    label_list  *curr;

    for( curr = head; curr != NULL; curr = curr->next ) {
        if( stricmp( curr->label, name ) == 0 ) {
            return( curr );
        }
    }
    return( NULL );
}


void AddTokens( token_buffer *tokbuf, token_idx start, token_idx count )
/**********************************************************************/
{
    token_idx   i;

    if( count > 0 ) {
        for( i = tokbuf->count; i >= start; i-- ) {
            tokbuf->tokens[i + count] = tokbuf->tokens[i];
        }
        tokbuf->count += count;
    }
}

bool ExpandSymbol( token_buffer *tokbuf, token_idx i, bool early_only, bool *expanded )
/*************************************************************************************/
{
    dir_node            *dir;
    token_idx           j;
    token_idx           count;

    *expanded = false;
    /* expand constant */
    dir = (dir_node *)AsmGetSymbol( tokbuf->tokens[i].string_ptr );
    if( dir != NULL && dir->sym.state == SYM_CONST ) {
        if( dir->e.constinfo->expand_early || !early_only ) {
            DebugMsg(( "Expand Constant: %s ->", dir->sym.name ));
            /* insert the pre-scanned data for this constant */
            count = dir->e.constinfo->count;
            AddTokens( tokbuf, i, count - 1 );
            for( j = 0; j < count; j++ ) {
                tokbuf->tokens[i + j] = dir->e.constinfo->tokens[j];
#ifdef DEBUG_OUT
                if( tokbuf->tokens[i + j].class == TC_NUM ) {
                    DebugMsg(( " %d", tokbuf->tokens[i + j].u.value ));
                } else {
                    DebugMsg(( " %s", tokbuf->tokens[i + j].string_ptr ));
                }
#endif
            }
            DebugMsg(( "\n" ));
            Globals.expand_count++;
            if( Globals.expand_count >= MAX_EQU_NESTING ) {
                AsmError( NESTING_LEVEL_TOO_DEEP );
                return( RC_ERROR );
            }
            *expanded = true;
        }
    }
    return( RC_OK );
}

bool ExpandProcString( token_buffer *tokbuf, token_idx index, bool *expanded )
/****************************************************************************/
{
    token_idx           i;
    int                 cnt;
    int                 count = 0; /* number of words in the name string */
    int                 offset;
    token_idx           left_bracket = 0;
    token_idx           right_bracket = 0;
    char                *string;
    char                *word;
    char                *replace = NULL;
    char                buffer[MAX_LINE_LEN];
    label_list          *label = NULL;
    proc_info           *info = CurrProc->e.procinfo;
    size_t              len;
    char                *p;

    *expanded = false;
    strcpy( buffer, tokbuf->tokens[index].string_ptr );
    wipe_space( buffer );
    for( word = strtok( buffer, " \t" ); word != NULL; word = strtok( NULL, " \t" ) ) {
        count++;

        /**/myassert( CurrProc != NULL );
        label = label_cmp( word, info->paralist );
        if( label == NULL ) {
            label = label_cmp( word, info->locallist );
        }
        if( label != NULL ) {
            if( label->replace != NULL ) {
                replace = label->replace;
            }
        }

        if( replace != NULL ) {
            if( *replace == '\0' ) {
                replace++;  /* point to first register string */
                if( ( tokbuf->tokens[index + 1].class == TC_PLUS ) &&
                    ( tokbuf->tokens[index + 2].class == TC_NUM ) ) {
                    offset = tokbuf->tokens[index + 2].u.value;
                    if( ( ( Use32 ) && ( offset != 4 ) ) ||
                        ( ( !Use32 ) && ( offset != 2 ) ) ) {
                        AsmErr( CANT_ACCESS_MULTI_REG_PARMS, replace );
                        return( RC_ERROR );
                    }
                    /* Point to second register string */
                    if( Use32 ) {
                        replace += 4;
                    } else {
                        replace += 3;
                    }
                }
            }
            if( label->is_register && !info->is_vararg ) {
                left_bracket = right_bracket = 0;   /* reset bracket indexes */
                for( i = 0; i < index; i++ ) {
                    if( tokbuf->tokens[i].class == TC_OP_SQ_BRACKET ) {
                        break;
                    }
                }
                if( i < index ) {
                    left_bracket = i;
                    for( i = index + 1; i < tokbuf->count; i++ ) {
                        if( tokbuf->tokens[i].class == TC_CL_SQ_BRACKET ) {
                            right_bracket = i;
                            break;
                        }
                    }
                    if( right_bracket < left_bracket ) {
                        AsmError( SYNTAX_ERROR );
                        return( RC_ERROR );
                    }
                }
                if( ( (Options.mode & MODE_IDEAL) == 0 ) &&
                    ( left_bracket == 0 ) &&
                    ( right_bracket == 0 ) ) {
                    left_bracket = index;
                    right_bracket = index;
                }
            }
            if( index > 0 && tokbuf->tokens[index - 1].class == TC_DIRECTIVE ) {
                switch( tokbuf->tokens[index - 1].u.token ) {
                case T_IFDEF:
                case T_IFNDEF:
                case T_ELSEIFDEF:
                case T_ELSEIFNDEF:
                    /* do NOT expand strings in IFDEF,IFNDEF,ELSEIFDEF and
                     * ELSEIFNDEF directive.
                     * We want to know if they are defined, NOT their value
                     */
                    return( RC_OK );
                }
            }
            if( tokbuf->tokens[index + 1].class == TC_DIRECTIVE ) {
                /* this will never happen with multiple words in a string */
                switch( tokbuf->tokens[index + 1].u.token ) {
                case T_EQU:
                case T_EQU2:
                case T_TEXTEQU:
                    return( RC_OK );
                }
            }
            break;
        }
    }
    if( replace == NULL )
        return( RC_OK );

    DebugMsg(( "ExpandString: %s -> %s \n", word, replace ));

    /* now we need to build the new line string to pass through the scanner */
    p = buffer;
    /* NOTE: if we have a TC_DIRECTIVE, tokbuf->count is always set to 1 !??! */
    for( i = 0; i < tokbuf->count; i++ ) {
        if( i != index ) {
            /* register parameter ? */
            if( label->is_register && !info->is_vararg ) {
                /* token within brackets ? */
                if( ( i >= left_bracket ) && ( i <= right_bracket ) ) {
                    continue;   /*yes, skip it */
                }
            }
            len = strlen( tokbuf->tokens[i].string_ptr );
            if( tokbuf->tokens[i].class == TC_STRING )
                *p++ = '<';
            p = CATSTR( p, tokbuf->tokens[i].string_ptr, len );
            if( tokbuf->tokens[i].class == TC_STRING ) {
                *p++ = '>';
            }
        } else {
            if( tokbuf->tokens[i].class == TC_PERCENT ) {
                /* don't save the % */
                i++;
            }

            /* copy the string in ... 1 word at a time */
            len = strlen( tokbuf->tokens[index].string_ptr ) + 1;
            string = AsmTmpAlloc( len );
            memcpy( string, tokbuf->tokens[index].string_ptr, len );
            wipe_space( string );
            word = strtok( string, " \t" );
            for( cnt = 1; cnt < count; cnt++ ) {
                len = strlen( word );
                p = CATSTR( p, word, len );
                *p++ = ' ';
                word = strtok( NULL, " \t" );
            }
            len = strlen( replace );
            p = CATSTR( p, replace, len );
            for( word = strtok( NULL, " \t" ); word != NULL; word = strtok( NULL, " \t" ) ) {
                *p++ = ' ';
                len = strlen( word );
                p = CATSTR( p, word, len );
            }
        }
        *p++ = ' ';
    }
    *p = '\0';
    /* make sure this line goes at the front of the queue */
    PushLineQueue();
    InputQueueLine( buffer );
    tokbuf->tokens[0].class = 0;
    tokbuf->tokens[0].string_ptr = NULL;
    tokbuf->tokens[0].u.value = 0;
    *expanded = true;
    return( RC_OK );
}

static void FreeConstData( const_info *constinfo )
{
    if( constinfo->tokens != NULL ) {

        int i;

        for( i = 0; i < constinfo->count; i++ ) {
#ifdef DEBUG_OUT
            if( constinfo->tokens[i].class == TC_NUM ) {
                DebugMsg(( "%d ", constinfo->tokens[i].u.value ));
            } else {
                DebugMsg(( "%s ", constinfo->tokens[i].string_ptr ));
            }
#endif
            AsmFree( constinfo->tokens[i].string_ptr );
        }
        DebugMsg(( "\n" ));
        AsmFree( constinfo->tokens );
    }
}

bool StoreConstantNumber( const char *name, long value, bool redefine )
{
    asm_tok         *new;
    dir_node        *dir;
    struct asm_sym  *sym;

    sym = AsmGetSymbol( name );

    /* if we've never seen it before, put it in */
    if( sym == NULL ) {
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( RC_ERROR );
        }
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = false;
    } else {
        /* check if it can be redefined */
        dir = (dir_node *)sym;
        if( sym->state == SYM_UNDEFINED ) {
            dir_change( dir, TAB_CONST );
            dir->e.constinfo->redefine = redefine;
            dir->e.constinfo->expand_early = false;
        } else if( ( sym->state != SYM_CONST ) ||
                   ( !dir->e.constinfo->redefine &&
                   ( Parse_Pass == PASS_1 ) ) ) {
            /* error */
            AsmErr( LABEL_ALREADY_DEFINED, sym->name );
            return( RC_ERROR );
        }
    }
    new = AsmAlloc( sizeof( asm_tok ) );
    memset( new[0].u.bytes, 0, sizeof( new[0].u.bytes ) );
    new[0].class = TC_NUM;
    new[0].u.value = value;
    new[0].string_ptr = NULL;
    FreeConstData( dir->e.constinfo );
    dir->e.constinfo->count = 1;
    dir->e.constinfo->tokens = new;
    return( RC_OK );
}

static bool createconstant( const char *name, bool value, token_buffer *tokbuf, token_idx start, bool redefine, bool expand_early )
/*********************************************************************************************************************************/
{
    asm_tok             *new;
    dir_node            *dir;
    token_idx           i;
    token_idx           count;
    token_idx           counta;
    bool                can_be_redefine;
    bool                new_constant;
    size_t              len;

    new_constant = false;
    dir = (dir_node *)AsmGetSymbol( name );

    /* if we've never seen it before, put it in */
    if( dir == NULL ) {
        new_constant = true;
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( RC_ERROR );
        }
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = expand_early;
    } else if( dir->sym.state == SYM_UNDEFINED ) {
        dir_change( dir, TAB_CONST );
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = expand_early;
    } else if(( dir->sym.state != SYM_CONST )
        || ( !dir->e.constinfo->redefine && ( Parse_Pass == PASS_1 ))) {
        /* error */
        AsmErr( LABEL_ALREADY_DEFINED, dir->sym.name );
        return( RC_ERROR );
    }

    if( value ) {
        if( dir->e.constinfo->count == 1 && dir->e.constinfo->tokens[0].class == TC_NUM ) {
            dir->e.constinfo->tokens[0].u.value = 1;
        } else {
            /* just define it to be 1 and get out */
            new = AsmAlloc( sizeof( asm_tok ) );
            memset( new[0].u.bytes, 0, sizeof( new[0].u.bytes ) );
            new[0].class = TC_NUM;
            new[0].u.value = 1;
            new[0].string_ptr = NULL;
            FreeConstData( dir->e.constinfo );
            dir->e.constinfo->count = 1;
            dir->e.constinfo->tokens = new;
        }
        return( RC_OK );
    }

    if( tokbuf->tokens[start].class != TC_FINAL
      && tokbuf->tokens[start].class != TC_ID
      && tokbuf->tokens[start + 1].class == TC_FINAL ) {
        counta = 1;
        count = 1;
    } else {
        /*
         * expand any constants
         */
        if( ExpandTheWorld( tokbuf, start, false, true ) )
            return( RC_ERROR );

        for( counta = 0, i = start; tokbuf->tokens[i].class != TC_FINAL; i++ ) {
            if( ( tokbuf->tokens[i].class != TC_STRING ) || ( tokbuf->tokens[i].u.value != 0 ) ) {
                counta++;
            }
        }
        count = i - start;
    }

    if( counta == 0 ) {
        new = NULL;
        count = 0;
        can_be_redefine = true;
    } else {
        new = AsmAlloc( counta * sizeof( asm_tok ) );
        can_be_redefine = ( counta > 1 );
    }
    for( i = 0; i < count; i++ ) {
        switch( tokbuf->tokens[start + i].class ) {
        case TC_STRING:
            if( count != 1 && tokbuf->tokens[start + i].u.value == 0 ) {
                i--;
                count--;
                start++;
                continue;
            }
            can_be_redefine = true;
            break;
        case TC_NUM:
            break;
        case TC_ID:
            if( IS_SYM_COUNTER( tokbuf->tokens[start + i].string_ptr ) ) {
                char            buff[40];

                /*
                 * We want a '$' symbol to have the value at it's
                 * point of definition, not point of expansion.
                 */
                sprintf( buff, ".$%p/%lx", GetCurrSeg(), (unsigned long)GetCurrAddr() );
                tokbuf->tokens[start + i].string_ptr = buff;
                if( AsmGetSymbol( buff ) == NULL ) {
                    new_constant = true;
                    MakeLabel( tokbuf, start + i, MT_NEAR );
                }
                break;
            }
        default:
            can_be_redefine = true;
            break;
        }
        new[i].class = tokbuf->tokens[start + i].class;
        memcpy( new[i].u.bytes, tokbuf->tokens[start + i].u.bytes, sizeof( new[i].u.bytes ) );
        if( tokbuf->tokens[start + i].string_ptr == NULL ) {
            new[i].string_ptr = NULL;
        } else {
            len = strlen( tokbuf->tokens[start + i].string_ptr ) + 1;
            new[i].string_ptr = AsmAlloc( len );
            memcpy( new[i].string_ptr, tokbuf->tokens[start + i].string_ptr, len );
        }
    }
    if( new_constant && can_be_redefine )
        dir->e.constinfo->redefine = true;
    FreeConstData( dir->e.constinfo );
    dir->e.constinfo->count = count;
    dir->e.constinfo->tokens = new;
    return( RC_OK );
}

bool DefineConstant( token_buffer *tokbuf, token_idx i, bool redefine, bool expand_early )
/*****************************************************************************************
 * if expand_early is true, expand before doing any parsing
 */
{
    char                *name;

    if( i != 0 ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    /*
     * get the name
     */
    name = tokbuf->tokens[i++].string_ptr;
    /*
     * skip directive EQU, TEXTEQU, ...
     */
    i++;
    return( createconstant( name, false, tokbuf, i, redefine, expand_early ) );
}

bool StoreConstant( const char *name, const char *value, bool redefine )
/**********************************************************************/
{
    token_buffer    tokbuf;

    AsmScan( &tokbuf, value );
    return( createconstant( name, false, &tokbuf, 0, redefine, false ) );
}

void MakeConstantUnderscored( const char *name )
/**********************************************/
{
    char    buffer[MAX_LINE_LEN];

    /* define a macro */
    strcpy( buffer, "__" );
    strcpy( buffer + 2, name );
    strcat( buffer, "__" );
    createconstant( buffer, true, NULL, 0, true, false );
}

bool ExpandAllConsts( token_buffer *tokbuf, token_idx start_pos, bool early_only )
/********************************************************************************/
{
    token_idx   i;
    bool        expanded;

    if( tokbuf->tokens[start_pos + 1].class == TC_DIRECTIVE ) {
        switch( tokbuf->tokens[start_pos + 1].u.token ) {
        case T_EQU:
        case T_EQU2:
        case T_TEXTEQU:
            start_pos += 2;
        }
    }
    Globals.expand_count = 0;
    for( i = start_pos; tokbuf->tokens[i].class != TC_FINAL; i++ ) {
        if( tokbuf->tokens[i].class != TC_ID )
            continue;
        if( ExpandSymbol( tokbuf, i, early_only, &expanded ) )
            return( RC_ERROR );
        if( expanded ) {
            i--; // in case the new symbol also needs to be expanded
            continue;
        }
    }
    return( RC_OK );
}

bool ExpandTheWorld( token_buffer *tokbuf, token_idx start_pos, bool early_only, bool flag_msg )
/**********************************************************************************************/
{
    token_idx   val;

    if( ExpandAllConsts( tokbuf, start_pos, early_only ) )
        return( RC_ERROR );
    if( !early_only ) {
        val = EvalExpr( tokbuf, start_pos, tokbuf->count, flag_msg );
        if( ISINVALID_IDX( val ) )
            return( RC_ERROR );
        tokbuf->count = val;
    }
    return( RC_OK );
}

bool ExpandTheConstant( token_buffer *tokbuf, token_idx start_pos, bool early_only, bool flag_msg )
/***************************************************************************/
{
    token_idx   val;

    if( ExpandAllConsts( tokbuf, start_pos, early_only ) )
        return( RC_ERROR );
    if( !early_only ) {
        val = EvalConstant( tokbuf, start_pos + 2, tokbuf->count, flag_msg );
        if( ISINVALID_IDX( val ) )
            return( RC_ERROR );
        tokbuf->count = val;
    }
    return( RC_OK );
}

#else

bool ExpandTheWorld( token_buffer *tokbuf, token_idx start_pos, bool early_only, bool flag_msg )
/**********************************************************************************************/
{
    token_idx   val;

    /* unused parameters */ (void)early_only;

    val = EvalExpr( tokbuf, start_pos, tokbuf->count, flag_msg );
    if( ISINVALID_IDX( val ) )
        return( RC_ERROR );
    tokbuf->count = val;
    return( RC_OK );
}

#endif
