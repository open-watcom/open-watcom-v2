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


#define     PLACEHOLDER_SIZE 3      /* for #dd - number sign, digit, digit */

// fixme some of these include files are not needed
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
#include "asmerr.h"

#include "womp.h"
#include "pcobj.h"
#include "objrec.h"
#include "myassert.h"
#include "fixup.h"
#include "expand.h"

#include "directiv.h"
// #undef _DIRECT_H_
// #define _DIRECT_FIX_
// #include "directiv.h"

extern char             *ReadTextLine( char * );
extern void             FlushCurrSeg( void );
extern void             AsmError( int );
extern int              InputQueueFile( char * );
extern int              AsmScan( char *, char * );
extern struct fixup     *CreateFixupRec( int );
extern void             InputQueueLine( char * );
extern void             PushLineQueue(void);
extern void             AsmTakeOut( char * );
extern int              EvalExpr( int, int, int );
extern dir_node         *dir_insert( char *name, int tab );
extern void             wipe_space( char *token );
extern char             *get_curr_filename( void );
extern void             PushMacro( char *, bool );

extern  const struct asm_ins    ASMFAR AsmOpTable[];
extern  uint            LineNumber;
extern  struct asm_tok  *AsmBuffer[];   // buffer to store token
extern  struct AsmCodeName AsmOpcode[];
extern  char            StringBuf[];
extern  char            Parse_Pass;     // phase of parsing
extern  module_info     ModuleInfo;
extern  int_8           DefineProc;     // TRUE if the definition of procedure
                                        // has not ended
extern dir_node         *CurrProc;
extern int_8            Use32;          // if 32-bit code is use
extern File_Info        AsmFiles;
extern char             *CurrString;    // Current Input Line
extern char             EndDirectiveFound;
extern char             AsmChars[];
extern int              Token_Count;    // number of tokens on line
extern int              MacroLocalVarCounter;


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

    /* get a pointer to the last next ptr ( or Head if empty ) */
    for( ptr = head; *ptr; ptr = &((*ptr)->next) );

    entry = AsmAlloc( sizeof( asmlines ) );
    entry->next = NULL;
    entry->line = AsmAlloc( strlen( data ) + 1 );
    entry->parmcount = 0;
    strcpy( entry->line, data );
    *ptr = entry;
    return( entry );
}

static char *replace_parm( parm_list *parms, char *start, char len, asmlines *lstruct )
/*************************************************************************************/
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
    char            before;             // length of text before placeholder
    char            count = 0;

    old_line = lstruct->line;
    for( p = parms; p != NULL; p = p->next ) {
        if( ( p->label != NULL ) && ( strlen( p->label ) == len ) &&
            ( strncmp( start, p->label, len ) == 0 ) ) {
            /* hey! it matches! */

            new_line = AsmAlloc( strlen(old_line) - len + PLACEHOLDER_SIZE +1 );
            before = start - old_line;
            if( *(start-1) == '&' ) before--;
            strncpy( new_line, old_line, before );
            *(new_line+before) = '\0';
            strcat( new_line, "#" );
            if( sprintf(buffer,"%2d", count ) != 2 ) {
                myassert( 0 );
            }
            if( buffer[0] == ' ' ) buffer[0]='0'; /* no spaces */
            strcat( new_line, buffer );
            if( *(start+len) == '&' ) len++;
            strcat( new_line, start+len );
            lstruct->line = new_line;
            lstruct->parmcount++;

            AsmFree( old_line );

            return( new_line + PLACEHOLDER_SIZE );  /* ptr to char after #dd */
        }
        count++;
    }
    return( start+len );
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
    char len;

    /* handle the substitution operator ( & ) */
    line = linestruct->line;
    for( tmp = line; *tmp != '\0'; ) {
        /* scan across the string for space, &, " - to start a word */
        for( ; *tmp != '\0'; tmp++ ) {
            if( is_valid_id_char( *tmp ) ) {
                if( tmp == line ) break; /* ok to start at beginning of line */
                continue;
            } else if( isspace( *tmp ) ) {
                /* find 1st non blank char */
                while( isspace( *tmp ) ) tmp++;
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
        if( !quote || *start =='&' || *(start-1)=='&' || *(start+len+1)=='&' ) {
            if( *start != '\0' && len > 0 ) {
                tmp = replace_parm( parms, start, len, linestruct );
            }
        }
    }
}

static int_8 lineis( char *str, char *substr )
/********************************************/
{
    int len;

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

static int macro_local( void )
/****************************/
{
    /* take a line that looks like  LOCAL varname [, varname * ] */

    int i = 0;
    char buffer[MAX_LINE_LEN];

    if( AsmBuffer[i]->value != T_LOCAL ) {
        AsmError( SYNTAX_ERROR );
        return( ERROR );
    }

    PushLineQueue();
    for( i++; AsmBuffer[i]->token != T_FINAL; i++ ) {
        /* define an equ to expand the specified variable to a temp. name */
        if( AsmBuffer[i]->token != T_ID ) {
            AsmError( OPERAND_EXPECTED );
            return( ERROR );
        }
        strcpy( buffer, AsmBuffer[i]->string_ptr );
        strcat( buffer, " TEXTEQU " );
        sprintf( buffer + strlen( buffer ), "??%#04d", MacroLocalVarCounter );
        MacroLocalVarCounter++;
        InputQueueLine( buffer );
        i++;
        if( AsmBuffer[i]->token == T_FINAL ) break;
        /* now skip the comma */
        if( AsmBuffer[i]->token != T_COMMA ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }
    }
    return( NOT_ERROR );
}


static int macro_exam( int i )
/****************************/
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

    name = AsmBuffer[i++]->string_ptr;
    dir = (dir_node *)AsmGetSymbol( name );
    info = dir->e.macroinfo;

    store_data = Parse_Pass == PASS_1 || info->data == NULL;

    /* go past "MACRO" */
    i++;

    if( store_data ) {
        for( ; i < Token_Count ; ) {
            token = AsmBuffer[i]->string_ptr;
            paranode = AsmAlloc( sizeof( parm_list ) );
            paranode->def = NULL;
            paranode->replace = NULL;
            paranode->required = FALSE;

            /* first get the parm. name */
            paranode->label = AsmAlloc( strlen( token ) + 1 );
            strcpy( paranode->label, token );
            i++;

            /* now see if it has a default value or is required */
            if( AsmBuffer[i]->token == T_COLON ) {
                i++;
                if( *AsmBuffer[i]->string_ptr == '=' ) {
                    i++;
                    if( AsmBuffer[i]->token != T_STRING ) {
                        AsmError( SYNTAX_ERROR );
                        return( ERROR );
                    }
                    token = AsmBuffer[i]->string_ptr;
                    paranode->def = AsmAlloc( strlen( token ) + 1 );
                    strcpy( paranode->def, token );
                    i++;
                } else if( strcmp( AsmBuffer[i]->string_ptr, "REQ" ) == 0 ) {
                    /* required parameter */
                    paranode->required = TRUE;
                    i++;
                }
            }
            if( i< Token_Count && AsmBuffer[i]->token != T_COMMA ) {
                AsmError( EXPECTING_COMMA );
                return( ERROR );
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
            return( ERROR );
        } else if( lineis( string, "endm" ) ) {
            if( nesting_depth ) {
                nesting_depth--;
            } else {
                return( NOT_ERROR );
            }
        }
        ptr = string;
        while( isspace( *ptr ) ) ptr++;
        if( lineis( ptr, "for" )
         || lineis( ptr, "forc" )
         || lineis( ptr, "irp" ) ) {
            nesting_depth++;
        }
        while( *ptr != '\0' && !isspace( *ptr ) ) ptr++; // skip 1st token
        while( isspace( *ptr ) ) ptr++;
        if( lineis( ptr, "macro" )
         || lineis( ptr, "for" )
         || lineis( ptr, "forc" )
         || lineis( ptr, "irp" ) ) {
            nesting_depth++;
        }

        if( store_data ) {
            linestruct = asmline_insert( &(info->data), string );
            /* make info->data point at the LAST line in the struct */
            put_parm_placeholders_in_line( linestruct, info->parmlist );
        }
    }
}

static int my_sprintf( char *dest, char *format, int argc, char *argv[] )
/***********************************************************************/
{
    /* just like sprintf, except take argv & argc for parms */
    /* so far it only handles string parms */

    char buffer[3];
    char *start;
    char *end;
    char parmno = 0;

    *dest = NULL;
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
    char                count = 0;

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
    new_line = AsmAlloc( strlen( (char*)buffer ) + 1 );
    strcpy( new_line, buffer );
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

int ExpandMacro( int tok_count)
/*****************************/
{
    char        buffer[MAX_LINE_LEN];
    char        next_char[2];
    dir_node    *dir;
    asm_sym     *sym = NULL;
    macro_info  *info;
    parm_list   *parm;
    asmlines    *lnode;
    char        *line;
    int         count = 0;
    int         macro_name_loc;
    char        expansion_flag = FALSE;
    int         exp_start;

    if( AsmBuffer[count]->token == T_FINAL ) return( tok_count );

    next_char[1]='\0';

    /* first, find out if it is a macro */
    for( ; count < tok_count; count++ ) {
        if( AsmBuffer[count]->token == T_ID ) {
            sym = AsmGetSymbol( AsmBuffer[count]->string_ptr );
        }
        if( sym != NULL && sym->state == SYM_MACRO ) break;
    }
    if( sym == NULL || sym->state != SYM_MACRO ) {
        return( tok_count );
    }
    macro_name_loc = count;
    if( AsmBuffer[count+1]->token == T_DIRECTIVE &&
        AsmBuffer[count+1]->value == T_MACRO ) {
        /* this is a macro DEFINITION! */
        return( tok_count );
    }

    if( macro_name_loc != 0 ) {
        /* save the rest of the line from before the macro */
        PushLineQueue();
        buffer[0]='\0';
        for( count=0; count < macro_name_loc; count++ ) {
            strcat( buffer, AsmBuffer[count]->string_ptr );
        }
        InputQueueLine( buffer );
    }
    count = macro_name_loc + 1;

    /* now get all the parameters from asmbuffer
     * they should alternate: parm /  comma
     */

    DebugMsg(( "Macro expansion:  %s \n", AsmBuffer[count-1]->string_ptr ));
    dir = (dir_node *)sym;
    info = dir->e.macroinfo;

    for( parm = info->parmlist; parm != NULL; parm = parm->next ) {
        buffer[0]='\0';
        if( count < tok_count ) {
            if( AsmBuffer[count]->token == T_COMMA ||
                ( AsmBuffer[count]->token == T_STRING &&
                  strlen( AsmBuffer[count]->string_ptr ) == 0 ) ) {
                /* blank parm */

                if( parm->required ) {
                    AsmError( PARM_REQUIRED );
                    return( ERROR );
                }
                if( parm->def ) {
                    /* fill in the default value */
                    parm->replace = AsmAlloc( strlen( parm->def )+1 );
                    strcpy( parm->replace, parm->def );
                }
                if( AsmBuffer[count]->token != T_COMMA ) {
                    count++;
                    if( count < tok_count &&
                        AsmBuffer[count]->token != T_COMMA ) {
                        AsmError( EXPECTING_COMMA );
                        return( ERROR );
                    }
                }
                /* go past comma */
                count++;
            } else {
                /* we have a parm! :) */
                for( ; ; ) {
                    if( AsmBuffer[count]->token == T_FINAL ) break;
                    if( *(AsmBuffer[count]->string_ptr) == '%' ) {
                        *(AsmBuffer[count]->string_ptr) = ' ';
                        expansion_flag = TRUE;
                        exp_start = count;
                    }

                    if( !expansion_flag ) {
                        if( AsmBuffer[count]->token == T_COMMA ||
                            AsmBuffer[count]->string_ptr == NULL ||
                            count == tok_count ) {
                            break;
                        }
                        if( ( AsmBuffer[count]->token > T_ID_IN_BACKQUOTES ) ) {
                            next_char[0] = *( AsmBuffer[count]->string_ptr );
                            strcat( buffer, next_char );
                        } else if( AsmBuffer[count]->token == T_NUM ) {
                            itoa( AsmBuffer[count]->value, buffer+strlen( buffer ), 10 );
                        } else if( AsmBuffer[count]->token == T_STRING ) {
                            char        *src;
                            char        *dst;

                            dst = &buffer[strlen(buffer)];
                            *dst++ = '\'';

                            src = AsmBuffer[count]->string_ptr;
                            while( *src != '\0' ) {
                                if( *src == '\'' ) {
                                    *dst++ = '\''; /* have to escape delim */
                                }
                                *dst++ = *src++;
                            }
                            *dst++ = '\'';
                            *dst = '\0';
                        } else {
                            strcat( buffer, AsmBuffer[count]->string_ptr );
                        }
                    } else {
                        switch( ExpandSymbol( count, FALSE ) ) {
                        case ERROR:
                            free_parmlist( info->parmlist );
                            return( ERROR );
                        case STRING_EXPANDED:
                            continue;
                        }

                        if( AsmBuffer[count]->token == T_COMMA ||
                            AsmBuffer[count]->string_ptr == NULL ||
                            AsmBuffer[count+1]->token == T_FINAL ) {
                            if( AsmBuffer[count+1]->token == T_FINAL ) count++;
                            tok_count = EvalExpr( tok_count, exp_start, count-1 );
                            expansion_flag = FALSE;
                            Token_Count = tok_count;
                            count = exp_start;
                        }
                    }
                    count++;
                }
                count++; /* go past the comma */
                parm->replace = AsmAlloc( strlen( buffer )+1 );
                strcpy( parm->replace, buffer );
            }
        } else {
            if( parm->required ) {
                AsmError( PARM_REQUIRED );
                return( ERROR );
            }
        }
    }

    /* now actually fill in the parms */
    PushLineQueue();
    for( lnode = info->data; lnode != NULL; lnode = lnode->next ) {
        line = fill_in_parms( lnode, info->parmlist );
        if( lineis( line, "local" ) ) {
            AsmScan( line, StringBuf );
            if( macro_local() == ERROR ) return( ERROR );
            AsmFree( line );
            continue;
        }
        InputQueueLine( line );
        AsmFree( line );
    }

    /* put this macro into the file stack */
    PushMacro( sym->name, info->hidden );

    /* now free the parm replace strings */
    free_parmlist( info->parmlist );

//    tok_count = macro_name_loc - 1;
    return( 0 );
}

int MacroDef( int i, bool hidden )
/********************************/
{
    struct asm_sym      *sym;
    char                *name;
    dir_node            *currproc;

    if( Parse_Pass != PASS_1 ) {
        /**/myassert( i >= 0 );
        //  currproc = DirLookup( AsmBuffer[i]->string_ptr, TAB_MACRO );
        currproc = (dir_node *)AsmGetSymbol( AsmBuffer[i]->string_ptr );
        if( currproc != NULL ) {
            return( macro_exam( i ) );
        }
    }
    // error checking for nested macro defs here?

    if( i < 0 ) {
        AsmError( PROC_MUST_HAVE_A_NAME );
        return( ERROR );
    }
    name = AsmBuffer[i]->string_ptr;
    sym = AsmGetSymbol( name );
    if( sym == NULL ) {
         currproc = dir_insert( name, TAB_MACRO );
         name = get_curr_filename();
         currproc->e.macroinfo->filename = AsmAlloc( strlen( name ) + 1 );
         strcpy( currproc->e.macroinfo->filename, name );
         currproc->e.macroinfo->hidden = hidden;
         sym = (asm_sym *)currproc;
    } else {
        AsmError( PROC_ALREADY_DEFINED );
        return( ERROR );
    }
    sym->state = SYM_MACRO;
    return( macro_exam( i ) ) ;
}
