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
* Description:  Command line parsing for asaxp clone tool.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "asaxp.h"
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
 * Appends '.' to a filename without extension ie. 'my_file' becomes 'my_file.'
 */
static char *VerifyDot( char *filename )
/**************************************/
{
    char *              newfilename;
    char *              tempfilename;
    int                 quotes_found=0;

    if (strchr(filename,'.')==NULL) {
        /*** Strip quotes from filename ***/
        newfilename = ReallocMem( filename, strlen(filename)+2 );
        if( *newfilename == '"' ) {
            tempfilename = newfilename + 1;                     /* skip leading " */
            tempfilename[ strlen(tempfilename)-1 ] = '\0';      /* smite trailing " */
            quotes_found=1;
        } else {
            tempfilename = newfilename;
        }
        /*** Append '.' at the end of filename and add quotes if needed ***/
        if (quotes_found) {
            filename = DupQuoteStrMem(strcat(tempfilename,"."),'"');
        } else {
            filename = DupStrMem(strcat(tempfilename,"."));
        }
        FreeMem( newfilename );
    }
    return filename;
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
 * Add another string to an OPT_STRING.
 */
static void add_string( OPT_STRING **p, char *str, char quote )
/*************************************************************/
{
    OPT_STRING *        buf;
    OPT_STRING *        curElem;
    int                 len;
    int                 add_quote = 0;

    len = strlen(str);
    if( quote != 0 ) {
        do {
            if( str[0] == '"'  && str[len-1] == '"'  ) break;
            if( str[0] == '\'' && str[len-1] == '\'' ) break;
            len += 2;
            add_quote = 1;
        } while( 0 );
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
            if( OPT_PROCESS( cmdOpts ) != 0 ) {
                cmd_line_error();
            }
        } else {                                /* filename */
            UngetCharContext();
            filename = CmdScanFileName();
            filename = VerifyDot(filename);
            add_string( &(cmdOpts->t010101010101_value), filename, 0 );
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
    DisableWarnings( 1 );
}


/*
 * Takes care of the t010101010101 option.
 */
static int parse_t010101010101( OPT_STRING **p )
/*********************************************/
{
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
 * For the /optName option, read in :string and store the string into the
 * given OPT_STRING.  If onlyOne is non-zero, any previous string in p will
 * be deleted.  If quote is non-zero, make sure the string is quoted.
 * Use quote if there aren't any quotes already.
 */
static int do_string_parse( OPT_STRING **p, char *optName, int onlyOne,
/*********************************************************************/
                            int quote )
{
    char *              str;

    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/%s option requires an argument", optName );
        return( 0 );
    }
    if( onlyOne )  OPT_CLEAN_STRING( p );
    add_string( p, str, quote );
    return( 1 );
}


/*
 * Parse the /D option.
 */
static int parse_D( OPT_STRING **p )
/******************************************/
{
    return( do_string_parse( p, "D", 0, 0 ) );
}


static void handle_Fo( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    char *              filename;

    x = x;
    OPT_CLEAN_STRING( &cmdOpts->fo_value );
    CmdScanWhitespace();
    filename = CmdScanFileName();
    if( filename != NULL ) {
        filename = VerifyDot(filename);
        add_string( &cmdOpts->fo_value, filename, 0 );
    } else {
        cmdOpts->fo=0;
        cmdOpts->o=0;
    }
}


/*
 * Parse the /Fo and /o option. Does nothing because handle_Fo does the job.
 */
static int parse_Fo( OPT_STRING **p )
/***********************************/
{
    return( 1 );
}


/*
 * Parse the /I option.
 */
static int parse_I( OPT_STRING **p )
/******************************************/
{
    char *              filename;

    CmdScanWhitespace();
    filename = CmdScanFileName();
    if( filename != NULL ) {
        add_string( p, filename, 0 );
    } else {
        OPT_CLEAN_STRING( p );
    }
    return( 1 );

}


/*
 * Parse the /U option.
 */
static int parse_U( OPT_STRING **p )
/******************************************/
{
    return( do_string_parse( p, "U", 0, 0 ) );
}


/*
 * Parse the /passwopts option.
 */
static int parse_passwopts( OPT_STRING **p )
{
    char *str;
    char *src;
    char *dst;

    if (!CmdScanRecogChar(':'))
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

    add_string(p, str, 0);
    return 1;
} /* parse_passwopts() */


/*
 * Return the next character (forced to lowercase since LINK's options are
 * not case-sensitive) and advance to the next one.
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


/*
 * Scan a number. Leading whitespace is permitted.
 */
static int OPT_GET_NUMBER( unsigned *p )
/**************************************/
{
    unsigned            value;

    CmdScanWhitespace();
    if( CmdScanNumber( &value ) ) {
        *p = value;
        return( 1 );
    } else {
        return( 0 );
    }
}


/* Include after all static functions were declared */
#include "optparsc.gh"
