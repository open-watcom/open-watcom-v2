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

#define    MAX_EQU_NESTING      20

static int              createconstant( char *, bool, int, bool, bool );

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


void AddTokens( asm_tok **buffer, int start, int count )
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

int ExpandSymbol( int i, bool early_only )
/***************************************/
{
    dir_node            *dir;
    int                 j;

    /* expand constant */
    dir = (dir_node *)AsmGetSymbol( AsmBuffer[i]->string_ptr );
    if( dir == NULL )
        return( NOT_ERROR );
    switch( dir->sym.state ) {
    case SYM_CONST:
        if(( dir->e.constinfo->expand_early == FALSE )
            && ( early_only == TRUE )) return( NOT_ERROR );
        DebugMsg(( "Expand Constant: %s ->", dir->sym.name ));
        /* insert the pre-scanned data for this constant */
        AddTokens( AsmBuffer, i, dir->e.constinfo->count - 1 );
        for( j = 0; j < dir->e.constinfo->count; j++ ) {
            AsmBuffer[i+j]->class = dir->e.constinfo->data[j].class;
            AsmBuffer[i+j]->u.value = dir->e.constinfo->data[j].u.value;
            AsmBuffer[i+j]->string_ptr = dir->e.constinfo->data[j].string_ptr;
            #ifdef DEBUG_OUT
            if( AsmBuffer[i+j]->class == TC_NUM ) {
                DebugMsg(( " %d", AsmBuffer[i+j]->u.value ));
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
    int                 offset;
    int                 left_bracket = 0;
    int                 right_bracket = 0;
    char                *string;
    char                *word;
    char                *replace = NULL;
    char                buffer[MAX_LINE_LEN];
    label_list          *label;
    proc_info           *info = CurrProc->e.procinfo;

    string = AsmTmpAlloc( strlen( AsmBuffer[index]->string_ptr ) + 1 );
    strcpy( string, AsmBuffer[index]->string_ptr );
    wipe_space( string );
    for( word = strtok( string, " \t" ); word != NULL; word = strtok( NULL, " \t" ) ) {
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
                if( ( AsmBuffer[index + 1]->class == TC_PLUS ) &&
                    ( AsmBuffer[index + 2]->class == TC_NUM ) ) {
                    offset = AsmBuffer[index + 2]->u.value;
                    if( ( ( Use32 ) && ( offset != 4 ) ) ||
                        ( ( !Use32 ) && ( offset != 2 ) ) ) {
                        AsmErr( CANT_ACCESS_MULTI_REG_PARMS, replace );
                        return( ERROR );
                    }
                    /* Point to second register string */
                    if( Use32 )
                        replace += 4;
                    else
                        replace += 3;
                }
            }
            if( ( label->is_register ) && ( info->is_vararg == FALSE ) ) {
                left_bracket = right_bracket = 0;   /* reset bracket indexes */
                for( i = 0; i < index; i++ ) {
                    if( AsmBuffer[i]->class == TC_OP_SQ_BRACKET )
                        break;
                }
                if( i < index ) {
                    left_bracket = i;
                    for( i = index + 1; i < Token_Count; i++ ) {
                        if( AsmBuffer[i]->class == TC_CL_SQ_BRACKET ) {
                            right_bracket = i;
                            break;
                        }
                    }
                    if( right_bracket < left_bracket ) {
                        AsmError( SYNTAX_ERROR );
                        return( ERROR );
                    }
                }
                if( ( (Options.mode & MODE_IDEAL) == 0 ) &&
                    ( left_bracket == 0 ) &&
                    ( right_bracket == 0 ) ) {
                    left_bracket = index;
                    right_bracket = index;
                }
            }
            if( index > 0 && AsmBuffer[index-1]->class == TC_DIRECTIVE ) {
                switch( AsmBuffer[index-1]->u.token ) {
                case T_IFDEF:
                case T_IFNDEF:
                case T_ELSEIFDEF:
                case T_ELSEIFNDEF:
                    /* do NOT expand strings in IFDEF,IFNDEF,ELSEIFDEF and
                     * ELSEIFNDEF directive.
                     * We want to know if they are defined, NOT their value
                     */
                    return( NOT_ERROR );
                }
            }
            if( AsmBuffer[index+1]->class == TC_DIRECTIVE ) {
                /* this will never happen with multiple words in a string */
                switch( AsmBuffer[index+1]->u.token ) {
                case T_EQU:
                case T_EQU2:
                case T_TEXTEQU:
                    return( NOT_ERROR );
                }
            }
            break;
        }
    }
    if( replace == NULL ) return( NOT_ERROR );

    DebugMsg(( "ExpandString: %s -> %s \n", word, replace ));

    /* now we need to build the new line string to pass through the scanner */
    buffer[0] = '\0';
    /* NOTE: if we have a TC_DIRECTIVE, token_count is always set to 1 !??! */
    for( i = 0; i < Token_Count; i++ ) {
        if( i != index ) {
            /* register parameter ? */
            if( ( label->is_register ) && ( info->is_vararg == FALSE ) ) {
                /* token within brackets ? */
                if( ( i >= left_bracket ) && ( i <= right_bracket ) )
                    continue;   /*yes, skip it */
            }
            // if( expand_directive_string( buffer, i ) == ERROR ) return( ERROR );
            if( AsmBuffer[i]->class == TC_STRING ) {
                strcat( buffer, "<" );
                strcat( buffer, AsmBuffer[i]->string_ptr );
                strcat( buffer, ">" );
            } else {
                strcat( buffer, AsmBuffer[i]->string_ptr );
            }
        } else {
            if( AsmBuffer[i]->class == TC_PERCENT ) {
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
            for( word = strtok( NULL, " \t" ); word != NULL; word = strtok( NULL, " \t" ) ) {
                strcat( buffer, " " );
                strcat( buffer, word );
            }
        }
        strcat( buffer, " " );
    }
    /* make sure this line goes at the front of the queue */
    PushLineQueue();
    InputQueueLine( buffer );
    AsmBuffer[0]->class = 0;
    AsmBuffer[0]->string_ptr = NULL;
    AsmBuffer[0]->u.value = 0;
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

int StoreConstant( char *name, char *value, bool redefine )
/**********************************************************/
{
    AsmScan( value );
    return( createconstant( name, FALSE, 0, redefine, FALSE ) );
}

void MakeConstantUnderscored( char const *name )
/**********************************************/
{
    char    buffer[MAX_LINE_LEN];

    /* define a macro */
    strcpy( buffer, "__" );
    strcpy( buffer + 2, name );
    strcat( buffer, "__" );
    strupr( buffer );
    createconstant( buffer, TRUE, 0, TRUE, FALSE );
}

static void FreeConstData( const_info *constinfo )
{
    if( constinfo->data != NULL ) {

        int i;

        for( i=0; i < constinfo->count; i++ ) {
#ifdef DEBUG_OUT
            if( constinfo->data[i].class == TC_NUM ) {
                DebugMsg(( "%d ", constinfo->data[i].u.value ));
            } else {
                DebugMsg(( "%s ", constinfo->data[i].string_ptr ));
            }
#endif
            AsmFree( constinfo->data[i].string_ptr );
        }
        DebugMsg(( "\n" ));
        AsmFree( constinfo->data );
    }
}

int StoreConstantNumber( char *name, long value, bool redefine )
{
    struct asm_tok  *new;
    dir_node        *dir;
    struct asm_sym  *sym;

    sym = AsmGetSymbol( name );

    /* if we've never seen it before, put it in */
    if( sym == NULL ) {
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( ERROR );
        }
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = FALSE;
    } else {
        /* check if it can be redefined */
        dir = (dir_node *)sym;
        if( sym->state == SYM_UNDEFINED ) {
            dir_change( dir, TAB_CONST );
            dir->e.constinfo->redefine = redefine;
            dir->e.constinfo->expand_early = FALSE;
        } else if( ( sym->state != SYM_CONST ) ||
                   ( ( dir->e.constinfo->redefine == FALSE ) &&
                   ( Parse_Pass == PASS_1 ) ) ) {
            /* error */
            AsmError( LABEL_ALREADY_DEFINED );
            return( ERROR );
        }
    }
    new = AsmAlloc( sizeof( struct asm_tok ) );
    memset( new[0].u.bytes, 0, sizeof( new[0].u.bytes ) );
    new[0].class = TC_NUM;
    new[0].u.value = value;
    new[0].string_ptr = NULL;
    FreeConstData( dir->e.constinfo );
    dir->e.constinfo->count = 1;
    dir->e.constinfo->data = new;
    return( NOT_ERROR );
}

static int createconstant( char *name, bool value, int start, bool redefine, bool expand_early )
/**********************************************************************************************/
{
    asm_tok             *new;
    dir_node            *dir;
    int                 i;
    int                 count;
    int                 counta;
    bool                can_be_redefine;
    bool                new_constant;

    new_constant = FALSE;
    dir = (dir_node *)AsmGetSymbol( name );

    /* if we've never seen it before, put it in */
    if( dir == NULL ) {
        new_constant = TRUE;
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( ERROR );
        }
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = expand_early;
    } else if( dir->sym.state == SYM_UNDEFINED ) {
        dir_change( dir, TAB_CONST );
        dir->e.constinfo->redefine = redefine;
        dir->e.constinfo->expand_early = expand_early;
    } else if(( dir->sym.state != SYM_CONST )
        || (( dir->e.constinfo->redefine == FALSE ) && ( Parse_Pass == PASS_1 ))) {
        /* error */
        AsmError( LABEL_ALREADY_DEFINED );
        return( ERROR );
    }

    if( value ) {
        /* just define it to be 1 and get out */
        new = AsmAlloc( sizeof( asm_tok ) );
        memset( new[0].u.bytes, 0, sizeof( new[0].u.bytes ) );
        new[0].class = TC_NUM;
        new[0].u.value = 1;
        new[0].string_ptr = NULL;
        FreeConstData( dir->e.constinfo );
        dir->e.constinfo->count = 1;
        dir->e.constinfo->data = new;
        return( NOT_ERROR );
    }

    /* expand any constants */
    if( ExpandTheWorld( start, FALSE, TRUE ) == ERROR )
        return( ERROR );

    for( counta = 0, i = start; AsmBuffer[i]->class != TC_FINAL; i++ ) {
        if( ( AsmBuffer[i]->class != TC_STRING )
            || ( AsmBuffer[i]->u.value != 0 ) ) {
            counta++;
        }
    }
    count = i - start;

    if( counta == 0 ) {
        new = NULL;
        count = 0;
        can_be_redefine = TRUE;
    } else {
        new = AsmAlloc( counta * sizeof( asm_tok ) );
        can_be_redefine = ( counta > 1 ) ? TRUE : FALSE;
    }
    for( i=0; i < count; i++ ) {
        switch( AsmBuffer[start+i]->class ) {
        case TC_STRING:
            if( AsmBuffer[start+i]->u.value == 0 ) {
                i--;
                count--;
                start++;
                continue;
            }
            can_be_redefine = TRUE;
            break;
        case TC_NUM:
            break;
        case TC_ID:
            if( IS_SYM_COUNTER( AsmBuffer[start+i]->string_ptr ) ) {
                char            buff[40];
                /*
                    We want a '$' symbol to have the value at it's
                    point of definition, not point of expansion.
                */
                sprintf( buff, ".$%lx/%lx", (unsigned long)GetCurrSeg(), (unsigned long)GetCurrAddr() );
                AsmBuffer[start+i]->string_ptr = buff;
                if( AsmGetSymbol( buff ) == NULL ) {
                    new_constant = TRUE;
                    MakeLabel( buff, MT_NEAR );
                }
                break;
            }
        default:
            can_be_redefine = TRUE;
            break;
        }
        new[i].class = AsmBuffer[start + i]->class;
        memcpy( new[i].u.bytes, AsmBuffer[start + i]->u.bytes, sizeof( new[i].u.bytes ) );
        if( AsmBuffer[start+i]->string_ptr == NULL ) {
            new[i].string_ptr = NULL;
        } else {
            new[i].string_ptr = AsmAlloc( strlen( AsmBuffer[start+i]->string_ptr ) + 1 );
            strcpy( new[i].string_ptr, AsmBuffer[start+i]->string_ptr );
        }
    }
    if( new_constant && can_be_redefine )
        dir->e.constinfo->redefine = TRUE;
    FreeConstData( dir->e.constinfo );
    dir->e.constinfo->count = count;
    dir->e.constinfo->data = new;
    return( NOT_ERROR );
}

int ExpandAllConsts( int start_pos, bool early_only )
/***************************************************/
{
    int i;

    if( AsmBuffer[start_pos+1]->class == TC_DIRECTIVE ) {
        switch( AsmBuffer[start_pos+1]->u.token ) {
        case T_EQU:
        case T_EQU2:
        case T_TEXTEQU:
            start_pos+=2;
        }
    }
    Globals.expand_count = 0;
    for( i=start_pos; AsmBuffer[i]->class != TC_FINAL; i++ ) {
        if( AsmBuffer[i]->class != TC_ID ) continue;
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

int ExpandTheWorld( int start_pos, bool early_only, bool flag_msg )
/******************************************************************/
{
    if( ExpandAllConsts( start_pos, early_only ) == ERROR ) return( ERROR );
    if( early_only == FALSE ) {
        int    val;

        val = EvalExpr( Token_Count, start_pos, Token_Count, flag_msg );
        if( val == ERROR )
            return( ERROR );
        Token_Count = val;
    }
    return( NOT_ERROR );
}

int ExpandTheConstant( int start_pos, bool early_only, bool flag_msg )
/******************************************************************/
{
    if( ExpandAllConsts( start_pos, early_only ) == ERROR ) return( ERROR );
    if( early_only == FALSE ) {
        int    val;

        val = EvalConstant( Token_Count, start_pos + 2, Token_Count, flag_msg );
        if( val == ERROR )
            return( ERROR );
        Token_Count = val;
    }
    return( NOT_ERROR );
}

#else

int ExpandTheWorld( int start_pos, bool early_only, bool flag_msg )
/******************************************************************/
{
    int    val;

    val = EvalExpr( Token_Count, start_pos, Token_Count, flag_msg );
    if( val == ERROR )
        return( ERROR );
    Token_Count = val;
    return( NOT_ERROR );
}

#endif
