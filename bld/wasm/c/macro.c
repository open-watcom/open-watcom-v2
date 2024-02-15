/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#include "macro.h"

#include "clibext.h"


/* quick explanation:
 *
 * there are 2 entry points into this module:
 * MacroDef & ExpandMacro
 * MacroDef is called by asmins.c to define a macro
 * ExpandMacro is called by asmline to check if something is a macro, and
 * expand it if that is the case
 */

extern bool             DefineProc;     // true if the definition of procedure has not ended
extern int              MacroLocalVarCounter;

macro_state             MacroIntState = MACSTATE_NONE;

static asmline *asmline_append( asmlines *lines, char *line )
/***********************************************************/
{
    asmline *entry;

    /*
     * add macro line to the end of macro lines list
     */
    entry = AsmAlloc( sizeof( asmline ) );
    entry->parmcount = 0;
    entry->line = AsmStrDup( line );
    entry->next = NULL;
    if( lines->head == NULL ) {
        lines->head = entry;
    } else {
        lines->tail->next = entry;
    }
    lines->tail = entry;
    return( entry );
}

static char *replace_parm( parm_list *parm, char *start, size_t len, asmline *linestruct )
/*****************************************************************************************
 * search through parm list for word pointed at by start,
 * if you find it, set up the line string
 * this is similar to a printf format string
 * the placeholders are of the form #dd ( #, digit, digit )
 * this allows up to 100 parameters - lots :)
 * fixme - this max. should be docmented & checked for.
 */
{
    char            buffer[10];
    char            *new_line;
    char            *old_line;
    size_t          before;             // length of text before placeholder
    char            count = 0;

    old_line = linestruct->line;
    for( ; parm != NULL; parm = parm->next ) {
        if( ( parm->label != NULL )
          && ( strlen( parm->label ) == len )
          && ( strncmp( start, parm->label, len ) == 0 ) ) {
            /*
             * hey! it matches!
             */
            new_line = AsmAlloc( strlen( old_line ) - len + PLACEHOLDER_SIZE + 1 );
            before = start - old_line;
            if( before > 0 ) {
                if( *(start - 1) == '&' )
                    before--;
                strncpy( new_line, old_line, before );
            }
            *(new_line + before) = '\0';
            if( sprintf( buffer, "#%-2.2d", count ) != 3 ) {
                myassert( 0 );
            }
            strcat( new_line, buffer );
            if( *(start + len) == '&' )
                len++;
            strcat( new_line, start + len );
            linestruct->line = new_line;
            linestruct->parmcount++;

            AsmFree( old_line );
            /*
             * ptr to char after #dd
             */
            return( new_line + before + PLACEHOLDER_SIZE );
        }
        count++;
    }
    return( start + len );
}

static char *replace_label( local_label *locallabel, char *start, size_t len, asmline *linestruct )
/**************************************************************************************************
 * search through local label list for word pointed at by start,
 * if you find it, replace it by real label in the line string
 * this allows up to 100 parameters - lots :)
 * fixme - this max. should be docmented & checked for.
 */
{
    char            *new_line;
    char            *old_line;
    size_t          before;             // length of text before word

    old_line = linestruct->line;
    for( ; locallabel != NULL; locallabel = locallabel->next ) {
        if( ( locallabel->local_len == len )
          && ( strncmp( start, locallabel->local, len ) == 0 ) ) {
            /*
             * hey! it matches!
             *
             * check internal symbol
             */
            if( locallabel->label_len == 0 ) {
                /*
                 * if the internal symbol does not exist, it is created
                 * and saved for further use
                 */
                char    label[10];

                sprintf( label, "??%04d", MacroLocalVarCounter++ );
                locallabel->label = AsmStrDup( label );
                locallabel->label_len = strlen( locallabel->label );
            }
            new_line = AsmAlloc( strlen( old_line ) - len + locallabel->label_len + 1 );
            before = start - old_line;
            if( before > 0 ) {
                if( *(start - 1) == '&' )
                    before--;
                strncpy( new_line, old_line, before );
            }
            strcpy( new_line + before, locallabel->label );
            if( *(start + len) == '&' )
                len++;
            strcpy( new_line + before + locallabel->label_len, start + len );
            linestruct->line = new_line;

            AsmFree( old_line );
            /*
             * ptr to char after new label
             */
            return( new_line + before + locallabel->label_len );
        }
    }
    return( start + len );
}

static char *find_replacement_items( char **pline, bool *pquote )
{
    char    *start;
    char    *line;
    bool    quote;
    int     c;

    start = line = *pline;
    quote = *pquote;
    /*
     * scan across the string for space, &, " - to start a word
     */
    while( (c = *(unsigned char *)line) != '\0' ) {
        if( IS_VALID_ID_CHAR( c ) ) {
            if( line == start ) {
                /*
                 * ok to start at beginning of line
                 */
                break;
            }
        } else {
            if( isspace( c ) ) {
                /*
                 * find 1st non blank char
                 */
                while( isspace( *line ) ) {
                    line++;
                }
            } else {
                if( c == '"' ) {
                    /*
                     * toggle the quote flag
                     */
                    quote = !quote;
                }
                line++;
            }
            break;
        }
        line++;
    }
    start = line;
    /*
     * scan across the string for space, &, " - to end the word
     */
    while( (c = *(unsigned char *)line) != '\0' ) {
        if( !IS_VALID_ID_CHAR( c ) ) {
            if( c == '"' ) {
                /*
                 * toggle the quote flag
                 */
                quote = !quote;
            }
            break;
        }
        line++;
    }
    *pline = line;
    *pquote = quote;
    /*
     * look for this word in the replacement data
     * and replace it if it is found
     *
     * !!! IMPORTANT !!!
     * this would change line - it will have to be reallocated
     */
    if( !quote
      || *start == '&'
      || *(start - 1) == '&'
      || *(line + 1) == '&' ) {
        if( *start != '\0'
          && line != start ) {
            return( start );
        }
    }
    return( NULL );
}

static bool lineis( char *str, char *substr )
/*******************************************/
{
    size_t  len;

    len = strlen( substr );
    wipe_space( str );
    if( strnicmp( str, substr, len ) ) {
        return( false );
    }
    if( str[len] != '\0'
      && !isspace( str[len] ) ) {
        return( false );
    }
    return( true );
}

static bool is_repeat_block( char *ptr )
/**************************************/
{
    return( lineis( ptr, "for" )
          || lineis( ptr, "forc" )
          || lineis( ptr, "irp" )
          || lineis( ptr, "irpc" )
          || lineis( ptr, "rept" )
          || lineis( ptr, "repeat" ) );
}

static bool process_local( token_buffer *tokbuf, macro_info *info )
{
    asm_tok     *tok;

    tok = tokbuf->tokens + 1;
    while( tok->class == TC_ID ) {
        local_label *locallabel;

        locallabel = AsmAlloc( sizeof( local_label ) );
        locallabel->local = AsmStrDup( tok->string_ptr );
        locallabel->local_len = strlen( locallabel->local );
        locallabel->label = NULL;
        locallabel->label_len = 0;
        /*
         * add to the begining of list
         */
        locallabel->next = info->labels.head;
        if( info->labels.head == NULL ) {
            info->labels.tail = locallabel;
        }
        info->labels.head = locallabel;

        tok++;
        if( tok->class != TC_COMMA ) {
            if( tok->class != TC_FINAL ) {
                AsmError( EXPECTING_COMMA );
            }
            break;
        }
        tok++;
    }
    if( tok->class != TC_FINAL ) {
        AsmError( OPERAND_EXPECTED );
        return( RC_ERROR );
    }
    return( RC_OK );
}

static bool macro_exam( token_buffer *tokbuf, token_idx i )
/*********************************************************/
{
    macro_info          *info;
    char                *line;
    char                *name;
    parm_list           *paramnode;
    asmline             *linestruct;
    char                buffer[ MAX_LINE_LEN ];
    dir_node            *dir;
    uint                nesting_depth = 0;
    bool                store_data;
    char                *start;
    bool                quote;
    bool                required;

    if( Options.mode & MODE_IDEAL ) {
        name = tokbuf->tokens[i+1].string_ptr;
    } else {
        name = tokbuf->tokens[i].string_ptr;
    }
    dir = (dir_node *)AsmGetSymbol( name );
    info = dir->e.macroinfo;

    store_data = ( Parse_Pass == PASS_1 || info->lines.head == NULL );

    /*
     * go past "MACRO" and name
     */
    i += 2;

    if( store_data ) {
        for( ; i < tokbuf->count ; ) {
            /*
             * first get the parm. name
             */
            name = tokbuf->tokens[i].string_ptr;
            i++;
            /*
             * now see if it has a default value or is required
             */
            start = NULL;
            required = false;
            if( tokbuf->tokens[i].class == TC_COLON ) {
                i++;
                if( *tokbuf->tokens[i].string_ptr == '=' ) {
                    i++;
                    if( tokbuf->tokens[i].class != TC_STRING ) {
                        AsmError( SYNTAX_ERROR );
                        return( RC_ERROR );
                    }
                    start = tokbuf->tokens[i].string_ptr;
                    i++;
                } else if( CMPLIT( tokbuf->tokens[i].string_ptr, "REQ" ) == 0 ) {
                    /*
                     * required parameter
                     */
                    required = true;
                    i++;
                }
            }
            /*
             * add parameter to the end of list
             */
            paramnode = AsmAlloc( sizeof( parm_list ) );
            paramnode->def = NULL;
            paramnode->replace = NULL;
            paramnode->required = false;
            paramnode->label = AsmStrDup( name );
            paramnode->def = AsmStrDup( start );
            paramnode->required = required;
            /*
             * add to the end of list
             */
            paramnode->next = NULL;
            if( info->params.head == NULL ) {
                info->params.head = paramnode;
            } else {
                info->params.tail->next = paramnode;
            }
            info->params.tail = paramnode;

            if( i < tokbuf->count
              && tokbuf->tokens[i].class != TC_COMMA ) {
                AsmError( EXPECTING_COMMA );
                return( RC_ERROR );
            }
            /*
             * go past comma
             */
            i++;

        } /* looping through parameters */
    }
    /*
     * now read in all the contents of the macro, and store them
     *
     * first read all local directive data from top
     */
    for( ;; ) {
        line = ReadTextLine( buffer );
        if( line == NULL ) {
            AsmError( UNEXPECTED_END_OF_FILE );
            return( RC_ERROR );
        }
        if( AsmScan( tokbuf, line ) )
            break;
        if( tokbuf->count > 0 ) {
            if( tokbuf->tokens[0].class != TC_DIRECTIVE
              || tokbuf->tokens[0].u.token != T_LOCAL ) {
                break;
            }
            if( store_data ) {
                if( process_local( tokbuf, info ) ) {
                    return( RC_ERROR );
                }
            }
        }
    }
    /*
     * now read in all the contents of the macro, and store them
     */
    for( ;; ) {
        char *ptr;

        if( lineis( line, "endm" ) ) {
            if( nesting_depth == 0 ) {
                return( RC_OK );
            }
            nesting_depth--;
        }
        ptr = line;
        while( isspace( *ptr ) )
            ptr++;
        if( is_repeat_block( ptr ) ) {
            nesting_depth++;
        }
        while( *ptr != '\0' && !isspace( *ptr ) )
            ptr++; // skip 1st token
        while( isspace( *ptr ) )
            ptr++;
        if( is_repeat_block( ptr )
          || lineis( ptr, "macro" ) ) {
            nesting_depth++;
        }

        if( store_data ) {
            linestruct = asmline_append( &(info->lines), line );
            line = linestruct->line;
            quote = false;
            for( ; *line != '\0'; ) {
                start = find_replacement_items( &line, &quote );
                if( start != NULL ) {
                    line = replace_parm( info->params.head, start, line - start, linestruct );
                }
            }
        }
        line = ReadTextLine( buffer );
        if( line == NULL ) {
            AsmError( UNEXPECTED_END_OF_FILE );
            return( RC_ERROR );
        }
    }
}

static size_t my_sprintf( char *dest, char *format, int argc, char *argv[] )
/***************************************************************************
 * just like sprintf, except take argv & argc for parms
 * so far it only handles string parms
 */
{
    char buffer[3];
    char *start;
    char *end;
    int  parmno;

    /* unused parameters */ (void)argc;

    *dest = '\0';
    start = format;
    for( end = start ; *end != '\0'; start = end + PLACEHOLDER_SIZE ) {
        /*
         * scan till we hit a placeholdr ( #dd ) or the end of the string
         */
        for( end = start; *end != '\0'; end++ ) {
            if( end[0] == '#'
              && isdigit( end[1] )
              && isdigit( end[2] ) ) {
                break;
            }
        }
        if( *end == '\0' ) {
            strncat( dest, start, end - start );
            break;
        }
        /*
         * we have a placeholder ( #dd )
         */
        buffer[0] = end[1];
        buffer[1] = end[2];
        buffer[2] = '\0';
        parmno = atoi( buffer );
        strncat( dest, start, end - start );
        /**/myassert( parmno < argc );
        if( argv[parmno] != NULL ) {
            strcat( dest, argv[parmno] );
        }
    }
    return( strlen( dest ) );
}

static char *fill_in_parms_and_labels( char *line, macro_info *info )
/*******************************************************************/
{
    char                buffer[MAX_LINE_LEN];
    parm_list           *parm;
    char                *new_line;
    char                **parm_array; /* array of ptrs to parm replace str's */
    int                 count;
    asmline             linestruct;
    bool                quote;
    char                *start;

    if( info->labels.head != NULL ) {
        /*
         * replace macro local labels by internal symbols
         */
        linestruct.line = line = AsmStrDup( line );
        quote = false;
        for( ; *line != '\0'; ) {
            start = find_replacement_items( &line, &quote );
            if( start != NULL ) {
                line = replace_label( info->labels.head, start, line - start, &linestruct );
            }
        }
        line = linestruct.line;
    }
    count = 0;
    for( parm = info->params.head; parm != NULL; parm = parm->next ) {
        count++;
    }
    if( count > 0 ) {
        parm_array = AsmAlloc( count * sizeof( char * ) );
        count = 0;
        for( parm = info->params.head; parm != NULL; parm = parm->next ) {
            parm_array[count] = parm->replace;
            count++;
        }
        /*
         * replace parameters by actual values
         */
        my_sprintf( buffer, line, count, parm_array );
        new_line = AsmStrDup( buffer );
        AsmFree( parm_array );
    } else {
        new_line = AsmStrDup( line );
    }
    if( info->labels.head != NULL ) {
        AsmFree( linestruct.line );
    }
    return( new_line );
}

static void reset_parmlist( parm_list *head )
/*******************************************/
{
    parm_list *parm;

    for( parm = head; parm != NULL; parm = parm->next ) {
        AsmFree( parm->replace );
        parm->replace = NULL;
    }
}

bool ExpandMacro( token_buffer *tokbuf )
/**************************************/
{
    char        buffer[MAX_LINE_LEN];
    dir_node    *dir;
    asm_sym     *sym = NULL;
    macro_info  *info;
    parm_list   *parm;
    asmline     *linestruct;
    char        *line;
    token_idx   i;
    token_idx   macro_name_loc;
    bool        expansion_flag = false;
    token_idx   expr_start = 0;
    char        *p;
    size_t      len;
    local_label *locallabel;

    if( tokbuf->tokens[0].class == TC_FINAL )
        return( RC_OK );
    /*
     * first, find out if it is a macro
     */
    for( macro_name_loc = 0; macro_name_loc < tokbuf->count; macro_name_loc++ ) {
        if( tokbuf->tokens[macro_name_loc].class == TC_ID ) {
            sym = AsmGetSymbol( tokbuf->tokens[macro_name_loc].string_ptr );
        }
        if( sym != NULL
          && sym->state == SYM_MACRO ) {
            break;
        }
    }
    if( sym == NULL
      || sym->state != SYM_MACRO ) {
        /*
         * not a macro, continue regular processing
         */
        return( RC_OK );
    }
    if( macro_name_loc > 0
      || (Options.mode & MODE_IDEAL) == 0 ) {
        if( Options.mode & MODE_IDEAL ) {
            i = macro_name_loc - 1;
        } else {
            i = macro_name_loc + 1;
        }
        if( tokbuf->tokens[i].class == TC_DIRECTIVE
          && tokbuf->tokens[i].u.token == T_MACRO ) {
            /*
             * this is a macro DEFINITION!
             * continue regular processing
             */
            return( RC_OK );
        }
    }
    if( macro_name_loc != 0 ) {
        /*
         * save the rest of the line from before the macro
         */
        PushLineQueue();
        p = buffer;
        for( i = 0; i < macro_name_loc; i++ ) {
            len = strlen( tokbuf->tokens[i].string_ptr );
            p = CATSTR( p, tokbuf->tokens[i].string_ptr, len );
            *p++ = ' ';;
        }
        *p = '\0';
        InputQueueLine( buffer );
    }
    i = macro_name_loc + 1;
    /*
     * now get all the parameters from asmbuffer
     * they should alternate: parm /  comma
     */
    DebugMsg(( "Macro expansion:  %s \n", tokbuf->tokens[i - 1].string_ptr ));
    dir = (dir_node *)sym;
    info = dir->e.macroinfo;

    for( parm = info->params.head; parm != NULL; parm = parm->next ) {
        p = buffer;
        if( i < tokbuf->count ) {
            if( tokbuf->tokens[i].class == TC_COMMA
              || ( tokbuf->tokens[i].class == TC_STRING
              && strlen( tokbuf->tokens[i].string_ptr ) == 0 ) ) {
                /*
                 * blank parm
                 */
                if( parm->required ) {
                    AsmError( PARM_REQUIRED );
                    return( RC_ERROR );
                }
                if( parm->def != NULL ) {
                    /*
                     * fill in the default value
                     */
                    parm->replace = AsmStrDup( parm->def );
                }
                if( tokbuf->tokens[i].class != TC_COMMA ) {
                    i++;
                    if( i < tokbuf->count
                      && tokbuf->tokens[i].class != TC_COMMA ) {
                        AsmError( EXPECTING_COMMA );
                        return( RC_ERROR );
                    }
                }
                /*
                 * go past the comma
                 */
                i++;
            } else {
                /*
                 * we have a parm! :)
                 */
                for( ;; ) {
                    if( tokbuf->tokens[i].class == TC_FINAL )
                        break;
                    if( *(tokbuf->tokens[i].string_ptr) == '%' ) {
                        *(tokbuf->tokens[i].string_ptr) = ' ';
                        expansion_flag = true;
                        expr_start = i;
                    }

                    if( !expansion_flag ) {
                        if( tokbuf->tokens[i].class == TC_COMMA
                          || tokbuf->tokens[i].string_ptr == NULL
                          || i == tokbuf->count ) {
                            break;
                        }
                        if( tokbuf->tokens[i].class == TC_NUM ) {
                            if( *tokbuf->tokens[i].string_ptr == 0 ) {
                                p += sprintf( p, "%lu", tokbuf->tokens[i].u.value );
                            } else {
                                len = strlen( tokbuf->tokens[i].string_ptr );
                                p = CATSTR( p, tokbuf->tokens[i].string_ptr, len );
                            }
                        } else if( tokbuf->tokens[i].class == TC_STRING ) {
                            char        *src;

                            src = tokbuf->tokens[i].string_ptr;
                            while( *src != '\0' ) {
                                if( *src == '\'' ) {
                                    /* have to escape delim */
                                    *p++ = '\'';
                                }
                                *p++ = *src++;
                            }
                        } else {
                            len = strlen( tokbuf->tokens[i].string_ptr );
                            p = CATSTR( p, tokbuf->tokens[i].string_ptr, len );
                        }
                    } else {
                        bool expanded;
                        if( ExpandSymbol( tokbuf, i, false, &expanded ) ) {
                            reset_parmlist( info->params.head );
                            return( RC_ERROR );
                        }
                        if( expanded ) {
                            continue;
                        }

                        if( tokbuf->tokens[i].class == TC_COMMA
                          || tokbuf->tokens[i].string_ptr == NULL
                          || tokbuf->tokens[i + 1].class == TC_FINAL ) {
                            if( tokbuf->tokens[i + 1].class == TC_FINAL )
                                i++;
                            tokbuf->count = EvalExpr( tokbuf, expr_start, i - 1, true );
                            expansion_flag = false;
                            i = expr_start;
                        }
                    }
                    i++;
                }
                *p = '\0';
                parm->replace = AsmStrDup( buffer );
                /*
                 * go past the comma
                 */
                i++;
            }
        } else {
            if( parm->required ) {
                AsmError( PARM_REQUIRED );
                return( RC_ERROR );
            }
        }
    }
    /*
     * now actually fill in the actual parms
     */
    PushLineQueue();
    for( linestruct = info->lines.head; linestruct != NULL; linestruct = linestruct->next ) {
        line = fill_in_parms_and_labels( linestruct->line, info );
        InputQueueLine( line );
        AsmFree( line );
    }
    /*
     * put this macro into the file stack
     */
    PushMacro( sym->name, info->hidden );
    /*
     * reset the parm replace strings
     */
    reset_parmlist( info->params.head );
    /*
     * reset the local label replace strings, if exists
     */
    for( locallabel = info->labels.head; locallabel != NULL; locallabel = locallabel->next ) {
        if( locallabel->label_len > 0 ) {
            locallabel->label_len = 0;
            AsmFree( locallabel->label );
            locallabel->label = NULL;
        }
    }
    /*
     * free the scanner token array
     * macro is expanded in source queue and original scanner tokens are not necessary
     */
    tokbuf->count = 0;
    tokbuf->tokens[0].class = TC_FINAL;
    tokbuf->tokens[0].string_ptr = tokbuf->stringbuf;

    return( RC_OK );
}

bool MacroDef( token_buffer *tokbuf, token_idx i, bool hidden )
/*************************************************************/
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
    if( ISINVALID_IDX( n )
      || ( tokbuf->tokens[n].class != TC_ID ) ) {
        if( Parse_Pass == PASS_1 )
            AsmError( PROC_MUST_HAVE_A_NAME );
        return( RC_ERROR );
    }
    name = tokbuf->tokens[n].string_ptr;
    currproc = (dir_node *)AsmGetSymbol( name );
    if( currproc == NULL ) {
        currproc = dir_insert( name, TAB_MACRO );
        currproc->e.macroinfo->srcfile = get_curr_srcfile();
        currproc->e.macroinfo->hidden = hidden;
    } else {
        if( Parse_Pass == PASS_1 ) {
            AsmError( PROC_ALREADY_DEFINED );
            return( RC_ERROR );
        }
    }
    return( macro_exam( tokbuf, i ) );
}

bool MacroEnd( bool exit_flag )
/*****************************/
{
    if( exit_flag ) {
        MacroIntState = 2;
    } else if( MacroIntState > 0 ) {
        if( GetQueueMacroHidden() ) {
            MacroIntState--;
        } else {
            MacroIntState = 0;
        }
    } else {
        MacroIntState = 0;
    }
    return( RC_OK );
}
