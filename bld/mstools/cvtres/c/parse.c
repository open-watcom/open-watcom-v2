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
* Description:  Command line parsing for CVTRES clone tool.
*
****************************************************************************/


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "cmdscan.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "memory.h"
#include "message.h"
#include "parse.h"
#include "pathconv.h"


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
    int                 ch;
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
                if( OPT_PROCESS( cmdOpts ) != 0 ) {
                    cmd_line_error();
                }
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
 * Parse the /MACHINE option.
 */
static int parse_machine( OPT_STRING **p )
/****************************************/
{
    char *              str;

    p = p;
    if( !CmdScanRecogChar( ':' ) ) {
        FatalError( "/MACHINE requires an argument" );
        return( 0 );
    }
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/MACHINE requires an argument" );
        return( 0 );
    }
    Warning( "Ignoring option /MACHINE:%s", str );
    return( 1 );
}


/*
 * Destroy an OPT_STRING.
 */
static void OPT_CLEAN_STRING( OPT_STRING **p )
/********************************************/
{
    OPT_STRING *        s;

    while( *p != NULL ) {
        s = *p;
        *p = s->next;
        FreeMem( s );
    }
}


/*
 * Add another string to an OPT_STRING.
 */
static void add_string( OPT_STRING **p, char *str )
/*************************************************/
{
    OPT_STRING *        buf;
    OPT_STRING *        curElem;

    /*** Make a new list item ***/
    buf = AllocMem( sizeof(OPT_STRING) + strlen(str) );
    strcpy( buf->data, str );
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
 * For the /optName option, read in a string and store the string into the
 * given OPT_STRING.  If onlyOne is non-zero, any previous string in p will
 * be deleted.
 */
static int do_string_parse( OPT_STRING **p, char *optName, int onlyOne )
/**********************************************************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/%s requires an argument", optName );
        return( 0 );
    }
    if( onlyOne )  OPT_CLEAN_STRING( p );
    add_string( p, str );
    return( 1 );
}


/*
 * Parse the /O option.
 */
static int parse_o( OPT_STRING **p )
/**********************************/
{
    int                 retcode;
    char *              newstr;

    if( !CmdScanRecogChar( ' ' )  &&  !CmdScanRecogChar( '\t' ) ) {
        FatalError( "Whitespace required after /o" );
        return( 0 );
    }
    retcode = do_string_parse( p, "o", 1 );
    if( retcode ) {
        newstr = PathConvert( (*p)->data, '"' );
        OPT_CLEAN_STRING( p );
        add_string( p, newstr );
    }
    return( retcode );
}


/*
 * Parse the /OUT option.
 */
static int parse_out( OPT_STRING **p )
/************************************/
{
    int                 retcode;
    char *              newstr;

    if( !CmdScanRecogChar( ':' ) ) {
        FatalError( "/OUT requires an argument" );
        return( 0 );
    }
    retcode = do_string_parse( p, "OUT", 1 );
    if( retcode ) {
        newstr = PathConvert( (*p)->data, '"' );
        OPT_CLEAN_STRING( p );
        add_string( p, newstr );
    }
    return( retcode );
}


/*
 * Suppress warning messages.
 */
static void handle_nowwarn( OPT_STORAGE *cmdOpts, int x )
/*******************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    DisableWarnings( 1 );
}


/*
 * Handle the /ALPHA option.
 */
static void handle_alpha( OPT_STORAGE *cmdOpts, int x )
/*****************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /ALPHA" );
}


/*
 * Handle the /I386 option.
 */
static void handle_i386( OPT_STORAGE *cmdOpts, int x )
/****************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /I386" );
}


/*
 * Handle the /MIPS option.
 */
static void handle_mips( OPT_STORAGE *cmdOpts, int x )
/****************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /MIPS" );
}


/*
 * Handle the /PPC option.
 */
static void handle_ppc( OPT_STORAGE *cmdOpts, int x )
/***************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /PPC" );
}


/*
 * Handle the /R option.
 */
static void handle_r( OPT_STORAGE *cmdOpts, int x )
/*************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /R" );
}


/*
 * Handle the /READONLY option.
 */
static void handle_readonly( OPT_STORAGE *cmdOpts, int x )
/********************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /READONLY" );
}


/*
 * Handle the /V option.
 */
static void handle_v( OPT_STORAGE *cmdOpts, int x )
/*************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /V" );
}


/*
 * Handle the /VERBOSE option.
 */
static void handle_verbose( OPT_STORAGE *cmdOpts, int x )
/*******************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    Warning( "Ignoring unsupported option: /VERBOSE" );
}


/*
 * Return the next character (forced to lowercase since options are not
 * case-sensitive) and advance to the next one.
 */
static int OPT_GET_LOWER( void )
/******************************/
{
    return( tolower( GetCharContext() ) );
}


/*
 * If the next character is ch (in either uppercase or lowercase form), it
 * is consumed and a non-zero value is returned; otherwise, it is not
 * consumed and zero is returned.
 */
static int OPT_RECOG_LOWER( int ch )
/**********************************/
{
    return( CmdScanRecogChar( ch ) );
}


/*
 * Back up one character.
 */
static void OPT_UNGET( void )
/***************************/
{
    UngetCharContext();
}


/* Include after all static functions were declared */
#include "optparse.gc"
