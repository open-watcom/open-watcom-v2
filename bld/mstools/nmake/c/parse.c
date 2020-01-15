/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "cmdlnprs.h"


#include "parseext.c"

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
            cmdOpts->t010101010101 = true;
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
    /* unused parammeters */ (void)cmdOpts; (void)x;

    DisableWarnings( true );
}



/*
 * Takes care of the t010101010101 option.
 */
static bool parse_t010101010101( OPT_STRING **p )
/***********************************************/
{
    p = p;
    return( true );
}


/*
 * For the /optName option, read in :string and store the string into the
 * given OPT_STRING.  If onlyOne is non-zero, any previous string in p will
 * be deleted.  If quote is non-zero, make sure the string is quoted.
 * Use quote if there aren't any quotes already.
 */
static bool do_string_parse( OPT_STRING **p, char *optName, bool onlyOne,
                            char quote )
/***********************************************************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/%s option requires a filename", optName );
        return( false );
    }
    if( onlyOne )
        OPT_CLEAN_STRING( p );
    add_string( p, str, quote );
    return( true );
}


/*
 * Parse the /F option.
 */
static bool parse_F( OPT_STRING **p )
/***********************************/
{
    return( do_string_parse( p, "F", true, '\0' ) );
}

/*
 * Parse the /X option.
 */
static bool parse_X( OPT_STRING **p )
/***********************************/
{
    return( do_string_parse( p, "X", true, '\0' ) );
}

/*
 * Parse combining parameters
 */
static bool parse_combining( OPT_STORAGE *cmdOpts, int x )
/********************************************************/
{
    char    ch;

    /* unused parammeters */ (void)x;

    /*
     * Make sure -L is translated correctly to -NOLOGO if it is
     * a first (or the only) parameter
     */
    if( cmdOpts->l ) {
        cmdOpts->nologo = true;
    }

    /* scan for combined options */
    do {
        /* get next character */
        ch = (char)toupper( (unsigned char)GetCharContext() );

        switch( ch ) {
        case 'A':  cmdOpts->a = true; break;      /* gml-option: A */
        case 'B':  cmdOpts->b = true; break;      /* gml-option: B */
        case 'C':  cmdOpts->c = true; break;      /* gml-option: C */
        case 'D':  cmdOpts->d = true; break;      /* gml-option: D */
        case 'E':  cmdOpts->e = true; break;      /* gml-option: E */
        case 'I':  cmdOpts->i = true; break;      /* gml-option: I */
        case 'K':  cmdOpts->k = true; break;      /* gml-option: K */
        case 'L':  cmdOpts->nologo = true; break; /* gml-option: L */
        case 'N':  cmdOpts->n = true; break;      /* gml-option: N */
        case 'P':  cmdOpts->p = true; break;      /* gml-option: P */
        case 'Q':  cmdOpts->q = true; break;      /* gml-option: Q */
        case 'R':  cmdOpts->r = true; break;      /* gml-option: R */
        case 'S':  cmdOpts->s = true; break;      /* gml-option: S */
        case 'T':  cmdOpts->t = true; break;      /* gml-option: T */
        case 'U':  cmdOpts->u = true; break;      /* gml-option: U */
        case 'Y':  cmdOpts->y = true; break;      /* gml-option: Y */
        case '\0': break;
        default:
            /* if character is space, return without an error */
            if( isspace( ch ) )
                return( true );

            return( false );
        }
    } while( ch != '\0' );

    /* all went nicely, return success */
    return( true );
}

/*
 * Parse the /passwopts option.
 */
static bool parse_passwopts( OPT_STRING **p )
/*******************************************/
{
    char *str;
    char *src;
    char *dst;

    if( !CmdScanRecogChar( ':' ) )
    {
        FatalError("/passwopts:{argument} requires an argument");
        return( false );
    }

    str = CmdScanString();
    if (str == NULL)
    {
        FatalError("/passwopts requires an argument");
        return( false );
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
            return( false );
        }

        *dst = 0x00;
    }

    add_string( p, str, '\0' );
    return( true );
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
