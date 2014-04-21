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
* Description:  Command line parsing for NMAKE clone tool.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "bool.h"
#include "nmake.h"
#include "cmdline.h"
#include "cmdscan.h"
#include "context.h"
#include "error.h"
#include "memory.h"
#include "message.h"
#include "parse.h"


/*
 * Initialize the OPT_STORAGE structure.
 */
void InitParse( OPT_STORAGE *cmdOpts )
/************************************/
{
    OPT_INIT( cmdOpts );
}


/*
 * Destroy the OPT_STORAGE structure.
 */
void FiniParse( OPT_STORAGE *cmdOpts )
/************************************/
{
    OPT_FINI( cmdOpts );
}


/*
 * Add another string to an OPT_STRING.
 */
static void add_string( OPT_STRING **p, char *str, char quote )
/*************************************************************/
{
    OPT_STRING *        buf;
    OPT_STRING *        curElem;
    size_t              len;
    bool                add_quote = FALSE;

    len = strlen(str);
    if( quote != '\0' ) {
        for( ;; ) {
            if( str[0] == '"'  && str[len-1] == '"'  ) break;
            if( str[0] == '\'' && str[len-1] == '\'' ) break;
            len += 2;
            add_quote = TRUE;
        }
    }
    /*** Make a new list item ***/
    buf = AllocMem( sizeof(OPT_STRING) + len );
    if( add_quote ) {
        buf->data[0] = quote;
        strcpy( &(buf->data[1]), str );
        buf->data[len-1] = quote;
        buf->data[len] = '\0';
    } else {
        strcpy( buf->data, str );
    }
    buf->next = NULL;

    /*** Put it at the end of the list ***/
    if( *p == NULL ) {
        *p = buf;
    } else {
        curElem = *p;
        while( curElem->next != NULL )  curElem = curElem->next;
        curElem->next = buf;
    }
}


/*
 * Gripe about a command line error.
 */
static void cmd_line_error( void )
/********************************/
{
    char *              str;

    GoToMarkContext();
    str = CmdScanString();
    Warning( "Ignoring invalid option '%s'", str );
}


/*
 * Parse the command string contained in the current context.
 */
void CmdStringParse( OPT_STORAGE *cmdOpts, int *itemsParsed )
/***********************************************************/
{
    char                ch;
    char *              filename;
    char *              str;

    for( ;; ) {
        /*** Find the start of the next item ***/
        CmdScanWhitespace();
        ch = GetCharContext();
        if( ch == '\0' )  break;
        MarkPosContext();               /* mark start of switch */

        /*** Handle switches, command files, and input files ***/
        if( ch == '-'  ||  ch == '/' ) {        /* switch */
            if( OPT_PROCESS( cmdOpts ) ) {
                /*
                 * Switch didn't match, if user entered empty switch,
                 * just be silent like MS's nmake does.
                 */

                ch = GetCharContext();
                if( ch != '\0' && !isspace( ch ) ) {
                    cmd_line_error();
                }
            }
        } else if( ch == '@' ) {                /* command file */
            filename = CmdScanFileNameWithoutQuotes();
            PushContext();
            if( OpenFileContext( filename ) ) {
                FatalError( "Cannot open '%s'.", filename );
            }
            FreeMem( filename );
            CmdStringParse( cmdOpts, itemsParsed );
            PopContext();
        } else {                                /* targets and macros */
            UngetCharContext();
            str = CmdScanString();
            add_string( &(cmdOpts->t010101010101_value), str, '\0' );
            cmdOpts->t010101010101 = 1;
        }
        (*itemsParsed)++;
    }
    CloseContext();
}


/*
 * Suppress warning messages.
 */
static void handle_nowwarn( OPT_STORAGE *cmdOpts, int x )
/*******************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    DisableWarnings( TRUE );
}



/*
 * Takes care of the t010101010101 option.
 */
static int parse_t010101010101( OPT_STRING **p )
/**********************************************/
{
    p = p;
    return( 1 );
}


/*
 * Destroy an OPT_STRING.
 */
void OPT_CLEAN_STRING( OPT_STRING **p )
/*************************************/
{
    OPT_STRING *        s;

    while( (s = *p) != NULL ) {
        *p = s->next;
        FreeMem( s );
    }
}


/*
 * For the /optName option, read in :string and store the string into the
 * given OPT_STRING.  If onlyOne is non-zero, any previous string in p will
 * be deleted.  If quote is non-zero, make sure the string is quoted.
 * Use quote if there aren't any quotes already.
 */
static int do_string_parse( OPT_STRING **p, char *optName, bool onlyOne,
                            char quote )
/**********************************************************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/%s option requires a filename", optName );
        return( 0 );
    }
    if( onlyOne ) OPT_CLEAN_STRING( p );
    add_string( p, str, quote );
    return( 1 );
}


/*
 * Parse the /F option.
 */
static int parse_F( OPT_STRING **p )
/******************************************/
{
    return( do_string_parse( p, "F", TRUE, '\0' ) );
}

/*
 * Parse the /X option.
 */
static int parse_X( OPT_STRING **p )
/******************************************/
{
    return( do_string_parse( p, "X", TRUE, '\0' ) );
}

/*
 * Parse combining parameters
 */
static int parse_combining( OPT_STORAGE *cmdOpts, int x )
/******************************************/
{
    char    ch;

    x = x;

    /*
     * Make sure -L is translated correctly to -NOLOGO if it is
     * a first (or the only) parameter
     */
    if( cmdOpts->l ) {
        cmdOpts->nologo = 1;
    }

    /* scan for combined options */
    do {
        /* get next character */
        ch = (char)toupper( (unsigned char)GetCharContext() );

        switch( ch ) {
        case 'A':  cmdOpts->a = 1; break;      /* gml-option: A */
        case 'B':  cmdOpts->b = 1; break;      /* gml-option: B */
        case 'C':  cmdOpts->c = 1; break;      /* gml-option: C */
        case 'D':  cmdOpts->d = 1; break;      /* gml-option: D */
        case 'E':  cmdOpts->e = 1; break;      /* gml-option: E */
        case 'I':  cmdOpts->i = 1; break;      /* gml-option: I */
        case 'K':  cmdOpts->k = 1; break;      /* gml-option: K */
        case 'L':  cmdOpts->nologo = 1; break; /* gml-option: L */
        case 'N':  cmdOpts->n = 1; break;      /* gml-option: N */
        case 'P':  cmdOpts->p = 1; break;      /* gml-option: P */
        case 'Q':  cmdOpts->q = 1; break;      /* gml-option: Q */
        case 'R':  cmdOpts->r = 1; break;      /* gml-option: R */
        case 'S':  cmdOpts->s = 1; break;      /* gml-option: S */
        case 'T':  cmdOpts->t = 1; break;      /* gml-option: T */
        case 'U':  cmdOpts->u = 1; break;      /* gml-option: U */
        case 'Y':  cmdOpts->y = 1; break;      /* gml-option: Y */
        case '\0': break;
        default:
            /* if character is space, return without an error */
            if( isspace( ch ) )
                return 1;

            return 0;
        }
    } while( ch != '\0' );

    /* all went nicely, return success */
    return 1;
}

/*
 * Parse the /passwopts option.
 */
static int parse_passwopts( OPT_STRING **p )
{
    char *str;
    char *src;
    char *dst;

    if( !CmdScanRecogChar( ':' ) )
    {
        FatalError("/passwopts:{argument} requires an argument");
        return 0;
    }

    str = CmdScanString();
    if (str == NULL)
    {
        FatalError("/passwopts requires an argument");
        return 0;
    }

    /*
     * If quoted, stip out the quote characters.
     */
    if (*str == '\"')
    {
        for (dst = str, src = str + 1; *src && (*src != '\"'); )
        {
            *dst++ = *src++;
        }

        if (*src != '\"')
        {
            FatalError("/passwopts argument is missing closing quote");
            return 0;
        }

        *dst = 0x00;
    }

    add_string(p, str, '\0');
    return 1;
} /* parse_passwopts() */


/*
 * Return the next character (forced to lowercase since LINK's options are
 * not case-sensitive) and advance to the next one.
 */
int OPT_GET_LOWER( void )
/***********************/
{
    return( tolower( (unsigned char)GetCharContext() ) );
}


/*
 * If the next character is ch (in either uppercase or lowercase form), it
 * is consumed and a non-zero value is returned; otherwise, it is not
 * consumed and zero is returned.
 */
bool OPT_RECOG_LOWER( int ch )
/****************************/
{
    return( CmdScanRecogLowerChar( ch ) );
}


/*
 * Back up one character.
 */
void OPT_UNGET( void )
/********************/
{
    UngetCharContext();
}


/* Include after all static functions were declared */
#include "cmdlnprs.gc"
