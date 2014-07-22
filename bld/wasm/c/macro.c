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
* Description:  maro processing routines
*
****************************************************************************/


#define     PLACEHOLDER_SIZE 3      /* for #dd - number sign, digit, digit */

#include "asmglob.h"
#include <ctype.h>

#include "asmalloc.h"
#include "asmeval.h"
#include "asmexpnd.h"
#include "directiv.h"
#include "asminput.h"

#include "myassert.h"

extern bool             GetQueueMacroHidden( void );

extern bool             DefineProc;     // TRUE if the definition of procedure
                                        // has not ended
extern int              MacroLocalVarCounter;

int                     MacroExitState = 0;

/* quick explanation:
 *
 * there are 2 entry points into this module:
 * MacroDef & ExpandMacro
 * MacroDef is called by asmins.c to define a macro
 * ExpandMacro is called by asmline to check if something is a macro, and
 * expand it if that is the case
 */

static asmlines *asmline_insert( asmlines **head, void *data )
/************************************************************/
{
    asmlines *entry;
    asmlines **ptr;
    size_t   len;

    /* get a pointer to the last next ptr ( or Head if empty ) */
    for( ptr = head; *ptr; ptr = &((*ptr)->next) );

    entry = AsmAlloc( sizeof( asmlines ) );
    entry->next = NULL;
    entry->parmcount = 0;
    len = strlen( data ) + 1;
    entry->line = AsmAlloc( len );
    memcpy( entry->line, data, len );
    *ptr = entry;
    return( entry );
}

static char *replace_parm( parm_list *parms, char *start, size_t len, asmlines *lstruct )
/***************************************************************************************/
{
    /* search through parm list for word pointed at by start,
     * if you find it, set up the line string
     * this is similar to a printf format string
     * the placeholders are of the form #dd ( #, digit, digit )
     * this allows up to 100 parameters - lots :)
     * fixme - this max. should be docmented & checked for.
     */
    char            buffer[10];
    parm_list       *p;
    char            *new_line;
    char            *old_line;
    size_t          before;             // length of text before placeholder
    char            count = 0;

    old_line = lstruct->line;
    for( p = parms; p != NULL; p = p->next ) {
        if( ( p->label != NULL ) && ( strlen( p->label ) == len ) &&
            ( strncmp( start, p->label, len ) == 0 ) ) {
            /* hey! it matches! */

            new_line = AsmAlloc( strlen( old_line ) - len + PLACEHOLDER_SIZE + 1 );
            before = start - old_line;
            if( *(start - 1) == '&' )
                before--;
            strncpy( new_line, old_line, before );
            *(new_line + before) = '\0';
            strcat( new_line, "#" );
            if( sprintf( buffer, "%2d", count ) != 2 ) {
                myassert( 0 );
            }
            if( buffer[0] == ' ' )
                buffer[0] = '0'; /* no spaces */
            strcat( new_line, buffer );
            if( *(start + len) == '&' )
                len++;
            strcat( new_line, start + len );
            lstruct->line = new_line;
            lstruct->parmcount++;

            AsmFree( old_line );

            return( new_line + before + PLACEHOLDER_SIZE );  /* ptr to char after #dd */
        }
        count++;
    }
    return( start + len );
}

#define is_valid_id_char( ch ) \
    ( isalpha(ch) || isdigit(ch) || ch=='_' || ch=='@' || ch=='$' || ch=='?' )

static void put_parm_placeholders_in_line( asmlines *linestruct, parm_list *parms )
/*********************************************************************************/
{
    char *line;
    char *tmp;
    char *start;
    char quote = FALSE;
    size_t len;

    /* handle the substitution operator ( & ) */
    line = linestruct->line;
    for( tmp = line; *tmp != '\0'; ) {
        /* scan across the string for space, &, " - to start a word */
        line = tmp;
        for( ; *tmp != '\0'; tmp++ ) {
            if( is_valid_id_char( *tmp ) ) {
                if( tmp == line ) break; /* ok to start at beginning of line */
                continue;
            } else if( isspace( *tmp ) ) {
                /* find 1st non blank char */
                while( isspace( *tmp ) )
                    tmp++;
                break;
            } else if( *tmp == '"' ) {
                /* toggle the quote flag */
                quote = ( quote + 1 ) %2;
                tmp++;
                break;
            } else {
                /* some other garbage */
                tmp++;
                break;
            }
        }
        start = tmp;
        /* scan across the string for space, &, " - to end the word */
        for( ; *tmp != '\0'; tmp++ ) {
            if( is_valid_id_char( *tmp ) ) {
                continue;
            } else if( isspace( *tmp ) ) {
                break;
            } else if( *tmp == '"' ) {
                /* toggle the quote flag */
                quote = ( quote + 1 ) %2;
                break;
            } else {
                break;
            }
        }
        len = tmp - start;
        /* look for this word in the macro parms, and replace it if it is */
        /* this would change line - it will have to be reallocated */
        if( !quote || *start =='&' || *(start-1)=='&' || *(start + len + 1)=='&' ) {
            if( *start != '\0' && len > 0 ) {
                tmp = replace_parm( parms, start, len, linestruct );
            }
        }
    }
}

static bool lineis( char *str, char *substr )
/*******************************************/
{
    size_t  len;

    len = strlen( substr );
    wipe_space( str );
    if( strnicmp( str, substr, len ) ) {
        return( FALSE );
    }
    if( str[len] != '\0' && !isspace( str[len] ) ) {
        return( FALSE );
    }
    return( TRUE );
}

static bool macro_local( void )
/*****************************/
{
    /* take a line that looks like  LOCAL varname [, varname * ] */

    token_idx   i = 0;
    char        buffer[MAX_LINE_LEN];
    char        *p;
    size_t      len;

    if( AsmBuffer[i].u.token != T_LOCAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }

    PushLineQueue();
    for( i++; AsmBuffer[i].class != TC_FINAL; i++ ) {
        /* define an equ to expand the specified variable to a temp. name */
        if( AsmBuffer[i].class != TC_ID ) {
            AsmError( OPERAND_EXPECTED );
            return( RC_ERROR );
        }
        len = strlen( AsmBuffer[i].string_ptr );
        p = CATSTR( buffer, AsmBuffer[i].string_ptr, len );
        p = CATLIT( p, " TEXTEQU " );
        sprintf( p, "??%04d", MacroLocalVarCounter );
        MacroLocalVarCounter++;
        InputQueueLine( buffer );
        i++;
        if( AsmBuffer[i].class == TC_FINAL ) break;
        /* now skip the comma */
        if( AsmBuffer[i].class != TC_COMMA ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}


static bool macro_exam( token_idx i )
/***********************************/
{
    macro_info          *info;
    char                *string;
    char                *token;
    char                *name;
    parm_list           *paranode;
    parm_list           *paracurr;
    asmlines            *linestruct;
    char                buffer[ MAX_LINE_LEN ];
    dir_node            *dir;
    uint                nesting_depth = 0;
    bool                store_data;
    size_t              len;

    if( Options.mode & MODE_IDEAL ) {
        name = AsmBuffer[i+1].string_ptr;
    } else {
        name = AsmBuffer[i].string_ptr;
    }
    dir = (dir_node *)AsmGetSymbol( name );
    info = dir->e.macroinfo;

    store_data = Parse_Pass == PASS_1 || info->data == NULL;

    /* go past "MACRO" and name*/
    i += 2;

    if( store_data ) {
        for( ; i < Token_Count ; ) {
            token = AsmBuffer[i].string_ptr;
            paranode = AsmAlloc( sizeof( parm_list ) );
            paranode->def = NULL;
            paranode->replace = NULL;
            paranode->required = FALSE;

            /* first get the parm. name */
            len = strlen( token ) + 1;
            paranode->label = AsmAlloc( len );
            memcpy( paranode->label, token, len );
            i++;

            /* now see if it has a default value or is required */
            if( AsmBuffer[i].class == TC_COLON ) {
                i++;
                if( *AsmBuffer[i].string_ptr == '=' ) {
                    i++;
                    if( AsmBuffer[i].class != TC_STRING ) {
                        AsmError( SYNTAX_ERROR );
                        return( RC_ERROR );
                    }
                    token = AsmBuffer[i].string_ptr;
                    len = strlen( token ) + 1;
                    paranode->def = AsmAlloc( len );
                    memcpy( paranode->def, token, len );
                    i++;
                } else if( CMPLIT( AsmBuffer[i].string_ptr, "REQ" ) == 0 ) {
                    /* required parameter */
                    paranode->required = TRUE;
                    i++;
                }
            }
            if( i< Token_Count && AsmBuffer[i].class != TC_COMMA ) {
                AsmError( EXPECTING_COMMA );
                return( RC_ERROR );
            }
            /* go past comma */
            i++;

            /* add this parm node to the list */
            // fixme
            paranode->next = NULL;
            if( info->parmlist == NULL ) {
                info->parmlist = paranode;
            } else {
                for( paracurr = info->parmlist;; paracurr = paracurr->next ) {
                    if( paracurr->next == NULL ) break;
                }
                paracurr->next = paranode;
            }

        } /* looping through parameters */
    }

    /* now read in all the contents of the macro, and store them */
    for( ; ; ) {
        char *ptr;

        string = ReadTextLine( buffer );
        if( string == NULL ) {
            AsmError( UNEXPECTED_END_OF_FILE );
            return( RC_ERROR );
        } else if( lineis( string, "endm" ) ) {
            if( nesting_depth ) {
                nesting_depth--;
            } else {
                return( RC_OK );
            }
        }
        ptr = string;
        while( isspace( *ptr ) )
            ptr++;
        if( lineis( ptr, "for" )
         || lineis( ptr, "forc" )
         || lineis( ptr, "irp" )
         || lineis( ptr, "irpc" )
         || lineis( ptr, "rept" )
         || lineis( ptr, "repeat" ) ) {
            nesting_depth++;
        }
        while( *ptr != '\0' && !isspace( *ptr ) )
            ptr++; // skip 1st token
        while( isspace( *ptr ) )
            ptr++;
        if( lineis( ptr, "macro" )
         || lineis( ptr, "for" )
         || lineis( ptr, "forc" )
         || lineis( ptr, "irp" )
         || lineis( ptr, "irpc" )
         || lineis( ptr, "rept" )
         || lineis( ptr, "repeat" ) ) {
            nesting_depth++;
        }

        if( store_data ) {
            linestruct = asmline_insert( &(info->data), string );
            /* make info->data point at the LAST line in the struct */
            put_parm_placeholders_in_line( linestruct, info->parmlist );
        }
    }
}

static size_t my_sprintf( char *dest, char *format, int argc, char *argv[] )
/**************************************************************************/
{
    /* just like sprintf, except take argv & argc for parms */
    /* so far it only handles string parms */

    char buffer[3];
    char *start;
    char *end;
    int  parmno = 0;

    argc = argc;
    *dest = '\0';
    start = format;
    for( end = start ;*end != '\0'; start = end + PLACEHOLDER_SIZE ) {
        /* scan till we hit a placeholdr ( #dd ) or the end of the string */
        for( end = start;
             !( ( *end == '#' ) && isdigit( *(end+1) ) && isdigit( *(end+2) ) )
             && ( *end != '\0' ); end++ );

        if( *end == '\0' ) {
            strncat( dest, start, end-start );
            return( strlen( dest ) );
        }
        /* we have a placeholder ( #dd ) */
        buffer[0] = *(end+1);
        buffer[1] = *(end+2);
        buffer[2] = '\0';
        parmno = atoi( buffer );
        strncat( dest, start, (end-start) );
        /**/myassert( parmno <= argc );
        if( argv[parmno] != NULL ) {
            strcat( dest, argv[parmno++] );
        }
    }
    return( strlen( dest ) );
}

static char *fill_in_parms( asmlines *lnode, parm_list *parmlist )
/****************************************************************/
{
    char                buffer[MAX_LINE_LEN];
    parm_list           *parm;
    char                *new_line;
    char                **parm_array; /* array of ptrs to parm replace str's */
    int                 count = 0;
    size_t              len;

    for( parm = parmlist; parm != NULL; parm = parm->next ) {
        count ++;
    }
    parm_array = AsmTmpAlloc( count * sizeof( char * ) );
    count = 0;
    for( parm = parmlist; parm != NULL; parm = parm->next ) {
        parm_array[count] = parm->replace;
        count++;
    }

    my_sprintf( buffer, lnode->line, count-1, parm_array );
    len = strlen( buffer ) + 1;
    new_line = AsmAlloc( len );
    memcpy( new_line, buffer, len );
    return( new_line );
}

static void free_parmlist( parm_list *head )
/******************************************/
{
    parm_list *parm;

    for( parm = head; parm != NULL; parm = parm->next ) {
        AsmFree( parm->replace );
        parm->replace = NULL;
    }
    return;
}

token_idx ExpandMacro( token_idx tok_count )
/******************************************/
{
    char        buffer[MAX_LINE_LEN];
    dir_node    *dir;
    asm_sym     *sym = NULL;
    macro_info  *info;
    parm_list   *parm;
    asmlines    *lnode;
    char        *line;
    token_idx   i;
    token_idx   macro_name_loc;
    char        expansion_flag = FALSE;
    token_idx   exp_start = 0;
    int         nesting_depth;
    char        *p;
    size_t      len;

    if( AsmBuffer[0].class == TC_FINAL )
        return( tok_count );

    /* first, find out if it is a macro */
    for( macro_name_loc = 0; macro_name_loc < tok_count; macro_name_loc++ ) {
        if( AsmBuffer[macro_name_loc].class == TC_ID ) {
            sym = AsmGetSymbol( AsmBuffer[macro_name_loc].string_ptr );
        }
        if( sym != NULL && sym->state == SYM_MACRO ) {
            break;
        }
    }
    if( sym == NULL || sym->state != SYM_MACRO ) {
        return( tok_count );
    }
    if( macro_name_loc > 0 || (Options.mode & MODE_IDEAL) == 0 ) {
        if( Options.mode & MODE_IDEAL ) {
            i = macro_name_loc - 1;
        } else {
            i = macro_name_loc + 1;
        }
        if( AsmBuffer[i].class == TC_DIRECTIVE &&
            AsmBuffer[i].u.token == T_MACRO ) {
            /* this is a macro DEFINITION! */
            return( tok_count );
        }
    }
    if( macro_name_loc != 0 ) {
        /* save the rest of the line from before the macro */
        PushLineQueue();
        p = buffer;
        for( i = 0; i < macro_name_loc; i++ ) {
            len = strlen( AsmBuffer[i].string_ptr );
            p = CATSTR( p, AsmBuffer[i].string_ptr, len );
            *p++ = ' ';;
        }
        *p = '\0';
        InputQueueLine( buffer );
    }
    i = macro_name_loc + 1;

    /* now get all the parameters from asmbuffer
     * they should alternate: parm /  comma
     */

    DebugMsg(( "Macro expansion:  %s \n", AsmBuffer[i - 1].string_ptr ));
    dir = (dir_node *)sym;
    info = dir->e.macroinfo;

    for( parm = info->parmlist; parm != NULL; parm = parm->next ) {
        p = buffer;
        if( i < tok_count ) {
            if( AsmBuffer[i].class == TC_COMMA ||
                ( AsmBuffer[i].class == TC_STRING &&
                  strlen( AsmBuffer[i].string_ptr ) == 0 ) ) {
                /* blank parm */

                if( parm->required ) {
                    AsmError( PARM_REQUIRED );
                    return( INVALID_IDX );
                }
                if( parm->def ) {
                    /* fill in the default value */
                    len = strlen( parm->def ) + 1;
                    parm->replace = AsmAlloc( len );
                    memcpy( parm->replace, parm->def, len );
                }
                if( AsmBuffer[i].class != TC_COMMA ) {
                    i++;
                    if( i < tok_count &&
                        AsmBuffer[i].class != TC_COMMA ) {
                        AsmError( EXPECTING_COMMA );
                        return( INVALID_IDX );
                    }
                }
                /* go past comma */
                i++;
            } else {
                /* we have a parm! :) */
                for( ; ; ) {
                    if( AsmBuffer[i].class == TC_FINAL )
                        break;
                    if( *(AsmBuffer[i].string_ptr) == '%' ) {
                        *(AsmBuffer[i].string_ptr) = ' ';
                        expansion_flag = TRUE;
                        exp_start = i;
                    }

                    if( !expansion_flag ) {
                        if( AsmBuffer[i].class == TC_COMMA ||
                            AsmBuffer[i].string_ptr == NULL ||
                            i == tok_count ) {
                            break;
                        }
                        if( AsmBuffer[i].class == TC_NUM ) {
                            if( *AsmBuffer[i].string_ptr == 0 ) {
                                p += sprintf( p, "%lu", AsmBuffer[i].u.value );
                            } else {
                                len = strlen( AsmBuffer[i].string_ptr );
                                p = CATSTR( p, AsmBuffer[i].string_ptr, len );
                            }
                        } else if( AsmBuffer[i].class == TC_STRING ) {
                            char        *src;

                            src = AsmBuffer[i].string_ptr;
                            while( *src != '\0' ) {
                                if( *src == '\'' ) {
                                    *p++ = '\''; /* have to escape delim */
                                }
                                *p++ = *src++;
                            }
                        } else {
                            len = strlen( AsmBuffer[i].string_ptr );
                            p = CATSTR( p, AsmBuffer[i].string_ptr, len );
                        }
                    } else {
                        bool expanded;
                        if( ExpandSymbol( i, FALSE, &expanded ) ) {
                            free_parmlist( info->parmlist );
                            return( INVALID_IDX );
                        }
                        if( expanded ) {
                            tok_count = Token_Count;
                            continue;
                        }

                        if( AsmBuffer[i].class == TC_COMMA ||
                            AsmBuffer[i].string_ptr == NULL ||
                            AsmBuffer[i + 1].class == TC_FINAL ) {
                            if( AsmBuffer[i + 1].class == TC_FINAL )
                                i++;
                            tok_count = EvalExpr( tok_count, exp_start, i - 1, TRUE );
                            expansion_flag = FALSE;
                            Token_Count = tok_count;
                            i = exp_start;
                        }
                    }
                    i++;
                }
                i++; /* go past the comma */
                *p = '\0';
                len = p + 1 - buffer;
                parm->replace = AsmAlloc( len );
                memcpy( parm->replace, buffer, len );
            }
        } else {
            if( parm->required ) {
                AsmError( PARM_REQUIRED );
                return( INVALID_IDX );
            }
        }
    }

    /* now actually fill in the parms */
    PushLineQueue();
    nesting_depth = 0;
    for( lnode = info->data; lnode != NULL; lnode = lnode->next ) {
        line = fill_in_parms( lnode, info->parmlist );
        if( lineis( line, "endm" ) ) {
            if( nesting_depth ) {
                nesting_depth--;
            }
        } else if( lineis( line, "local" ) ) {
            if( nesting_depth == 0 ) {
                AsmScan( line );
                if( macro_local() )
                    return( INVALID_IDX );
                AsmFree( line );
                continue;
            }
        } else {
            p = line;
            while( *p != '\0' && !isspace( *p ) )
                p++; // skip 1st token
            while( isspace( *p ) )
                p++; // skip all spaces
            if( lineis( p, "macro" ) ) {
                nesting_depth++;
            }
        }
        InputQueueLine( line );
        AsmFree( line );
    }

    /* put this macro into the file stack */
    PushMacro( sym->name, info->hidden );

    /* now free the parm replace strings */
    free_parmlist( info->parmlist );

    return( 0 );
}

bool MacroDef( token_idx i, bool hidden )
/***************************************/
{
    char        *name;
    token_idx   n;
    dir_node    *currproc;

    if( Options.mode & MODE_IDEAL ) {
        n = i + 1;
    } else if( i > 0 ) {
        n = --i;
    } else {
        n = INVALID_IDX;
    }
    if( ( Parse_Pass == PASS_1 ) &&
        ( ( n == INVALID_IDX ) || ( AsmBuffer[n].class != TC_ID ) ) ) {
        AsmError( PROC_MUST_HAVE_A_NAME );
        return( RC_ERROR );
    }
    name = AsmBuffer[n].string_ptr;
    currproc = (dir_node *)AsmGetSymbol( name );
    if( currproc == NULL ) {
        currproc = dir_insert( name, TAB_MACRO );
        currproc->e.macroinfo->srcfile = get_curr_srcfile();
        currproc->e.macroinfo->hidden = hidden;
    } else if( Parse_Pass == PASS_1 ) {
        AsmError( PROC_ALREADY_DEFINED );
        return( RC_ERROR );
    }
    return( macro_exam( i ) );
}

bool MacroEnd( bool exit_flag )
/*****************************/
{
    if( exit_flag ) {
        MacroExitState = 2;
    } else if( MacroExitState > 0 ) {
        if( GetQueueMacroHidden() ) {
            MacroExitState--;
        } else {
            MacroExitState = 0;
        }
    } else {
        MacroExitState = 0;
    }
    return( RC_OK );
}
