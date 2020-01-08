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
* Description:  Command line parsing for RC clone tool.
*
****************************************************************************/


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "cmdline.h"
#include "cmdscan.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "memory.h"
#include "message.h"
#include "pathconv.h"
#include "rc.h"
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

    for( ;; ) {
        /*** Find the start of the next item ***/
        CmdScanWhitespace();
        ch = GetCharContext();
        if( ch == '\0' )  break;
        MarkPosContext();               /* mark start of switch */

        /*** Handle switches, command files, and input files ***/
        if( ch == '-'  ||  ch == '/' ) {        /* switch */
            for( ;; ) {
                ch = GetCharContext();
                if( isspace( ch )  ||  ch == '\0' ) {
                    break;
                } else {
                    UngetCharContext();
                }
                if( OPT_PROCESS( cmdOpts ) ) {
                    cmd_line_error();
                }
                if( cmdOpts->gotlongoption ) {
                    cmdOpts->gotlongoption = false;
                    break;
                }
            }
        } else if( ch == '"' ) {                /* quoted option or file name */
            ch = GetCharContext();
            if( ch == '-' ) {
                Quoted = true;
                if( OPT_PROCESS( cmdOpts ) ) {
                    cmd_line_error();
                }
            } else {
                UngetCharContext();
                UngetCharContext();
                filename = CmdScanFileName();
                AddFile( TYPE_DEFAULT_FILE, filename );
                FreeMem( filename );
            }
        } else {                                /* input file */
            UngetCharContext();
            filename = CmdScanFileName();
            AddFile( TYPE_DEFAULT_FILE, filename );
            FreeMem( filename );
        }
        (*itemsParsed)++;
    }
    CloseContext();
}


/*
 * For the /optName option, read in a string and store the string into the
 * given OPT_STRING.  If onlyOne is non-zero, any previous string in p will
 * be deleted.
 */
static bool do_string_parse( OPT_STRING **p, char *optName, bool onlyOne )
/************************************************************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/%s requires an argument", optName );
        return( false );
    }
    if( onlyOne )
        OPT_CLEAN_STRING( p );
    add_string( p, str, '\0' );
    return( true );
}


/*
 * Parse the /c option.
 */
static bool parse_c( OPT_STRING **p )
/***********************************/
{
    return( do_string_parse( p, "c", true ) );
}


/*
 * Parse the /d option.
 */
static bool parse_d( OPT_STRING **p )
/***********************************/
{
    return( do_string_parse( p, "d", false ) );
}


/*
 * Parse the /fo option.
 */
static bool parse_fo( OPT_STRING **p )
/************************************/
{
    bool                retcode;
    char *              newstr;

    retcode = do_string_parse( p, "fo", true );
    if( retcode ) {
        newstr = PathConvert( (*p)->data, '"' );
        OPT_CLEAN_STRING( p );
        add_string( p, newstr, '\0' );
    }
    return( retcode );
}


/*
 * Parse the /i option.
 */
static bool parse_i( OPT_STRING **p )
/***********************************/
{
    return( do_string_parse( p, "i", false ) );
}


/*
 * Parse the /l option.
 */
static bool parse_l( OPT_STRING **p )
/***********************************/
{
    return( do_string_parse( p, "l", true ) );
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
 * Signal that an option longer than one character was parsed.  This info
 * is used for chaining of options.
 */
static void handle_long_option( OPT_STORAGE *cmdOpts, int x )
/***********************************************************/
{
    /* unused parammeters */ (void)x;

    cmdOpts->gotlongoption = true;
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
 * Return the next character (forced to lowercase since RC's options are
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
