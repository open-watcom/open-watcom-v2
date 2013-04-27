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
* Description:  Command line parsing for LIB clone tool.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "watcom.h"
#include "lib.h"
#include "cmdline.h"
#include "cmdscan.h"
#include "context.h"
#include "error.h"
#include "file.h"
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
/************************************/
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
 * Checks file for '.' and wildcards and then adds it to the list.
 */
static void VerifyAddFile( char *filename )
/*****************************************/
{
    DIR *               dirp;
    struct dirent *     direntp;
    char *              tempfilename;
    char *              newfilename;
    char                fullname[_MAX_PATH];
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char *              pattern;

    if ((strchr(filename,'?')!=NULL) || (strchr(filename,'*')!=NULL)){
        /*** Strip quotes from filename ***/
        newfilename = DupStrMem(filename);
        if( *filename == '"' ) {
            tempfilename = newfilename + 1;                     /* skip leading " */
            tempfilename[ strlen(tempfilename)-1 ] = '\0';      /* smite trailing " */
        } else {
            tempfilename = newfilename;
        }

        pattern = DupStrMem( tempfilename );
        dirp = opendir( pattern );
        if (dirp == NULL) {
            FatalError("can't find following files: %s",filename);
        } else {
            FreeMem( filename );
            FreeMem( newfilename );
            for (;;) {
                direntp = readdir (dirp);
                if ( direntp == NULL ) break;
                _splitpath( pattern, drive, dir, NULL, NULL );
                _makepath( fullname, drive, dir, "", "" );
                strcat( fullname, direntp->d_name );
                filename = fullname;
                filename = VerifyDot(filename);
                AddFile( TYPE_DEFAULT_FILE, filename );
                FreeMem( filename );
            }
            closedir( dirp );
        }
        FreeMem( pattern );
    } else {
        filename = VerifyDot(filename);
        AddFile( TYPE_DEFAULT_FILE, filename );
        FreeMem( filename );
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
        } else if( ch == '@' ) {                /* command file */
            filename = CmdScanFileNameWithoutQuotes();
            PushContext();
            if( OpenFileContext( filename ) ) {
                FatalError( "Cannot open '%s'.", filename );
            }
            FreeMem( filename );
            CmdStringParse( cmdOpts, itemsParsed );
            PopContext();
        } else {                                /* input file */
            UngetCharContext();
            filename = CmdScanFileName();
            VerifyAddFile(filename);
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
        for( ;; ) {
            if( str[0] == '"'  && str[len-1] == '"'  ) break;
            if( str[0] == '\'' && str[len-1] == '\'' ) break;
            len += 2;
            add_quote = 1;
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

    if( !CmdScanRecogChar( ':' ) ) {
        FatalError( "/%s requires an argument", optName );
        return( 0 );
    }
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/%s requires an argument", optName );
        return( 0 );
    }
    if( onlyOne )  OPT_CLEAN_STRING( p );
    add_string( p, str, quote );
    return( 1 );
}


/*
 * Parse the /DEBUGTYPE option.
 */
static int parse_debugtype( OPT_STRING **p )
/******************************************/
{
    return( do_string_parse( p, "DEBUGTYPE", 0, 0 ) );
}


/*
 * Parse the /DEF option.
 */
static int parse_def( OPT_STRING **p )
/************************************/
{
    return( do_string_parse( p, "DEF", 1, 0 ) );
}



/*
 * Parse the /EXPORT option, which is of the form
 *      /EXPORT:entryname[=internalname][,@ordinal[,NONAME]][,DATA]
 */
static int parse_export( OPT_STRING **optStr )
/********************************************/
{
    char *              strStart;
    char *              str;
    char *              p;
    char *              entryName = NULL;
    char *              internalName = NULL;
    char *              ordinal = NULL;
    size_t              len;
    int                 retcode = 1;

    /*** Extract the export string ***/
    if( !CmdScanRecogChar( ':' ) ) {
        FatalError( "/EXPORT requires an argument" );
        return( 0 );
    }
    str = CmdScanString();
    strStart = str;
    if( str == NULL ) {
        FatalError( "/EXPORT requires an argument" );
        return( 0 );
    }

    /*** Extract the entryName ***/
    p = str;
    while( *p != '\0'  &&  *p != '='  &&  *p != ',' )  p++;
    if( p == str ) {
        retcode = 0;
    } else {
        len = p - str + 1;
        if( str[0] != '\'' || str[len-2] != '\'' ) {
            len += 2;
            entryName = AllocMem( len );
            entryName[0] = '\'';
            memcpy( &(entryName[1]), str, len-3 );
            entryName[len-2] = '\'';
        } else {
            entryName = AllocMem( len );
            memcpy( entryName, str, len-1 );
        }
        entryName[len-1] = '\0';
        str = p;
    }

    /*** Extract the internalName ***/
    if( *str == '='  &&  retcode != 0 ) {
        str++;
        p = str;
        while( *p != '\0'  &&  *p != ',' )  p++;
        if( p == str ) {
            retcode = 0;
        } else {
            len = p - str + 1;
            if( str[0] != '\'' || str[len-2] != '\'' ) {
                len += 2;
                internalName = AllocMem( len );
                internalName[0] = '\'';
                memcpy( &(internalName[1]), str, len-3 );
                internalName[len-2] = '\'';
            } else {
                internalName = AllocMem( len );
                memcpy( internalName, str, len-1 );
            }
            internalName[len-1] = '\0';
            str = p;
        }
    }

    /*** Extract the ordinal ***/
    if( *str == ','  &&  retcode != 0 ) {
        if( *(++str) != '@' ) {
            retcode = 0;
        } else {
            str++;
            p = str;
            while( *p != '\0'  &&  isdigit( *p ) )  p++;
            if( p == str ) {
                retcode = 0;
            } else {
                len = p - str + 1;
                ordinal = AllocMem( len );
                memcpy( ordinal, str, len-1 );
                ordinal[len-1] = '\0';
            }
        }
    }

    /*** Abort on error ***/
    if( retcode == 0 ) {
        if( entryName != NULL )  FreeMem( entryName );
        if( internalName != NULL )  FreeMem( internalName );
        if( ordinal != NULL )  FreeMem( ordinal );
        FreeMem( strStart );
        return( 0 );
    }

    /*** Merge together Watcom-style:  entryName[.ordinal][=internalName] ***/
    FreeMem( strStart );
    len = strlen( entryName );
    if( ordinal != NULL ) {
        len++;                          /* for '.' */
        len += strlen( ordinal );
    }
    if( internalName != NULL ) {
        len++;                          /* for '=' */
        len += strlen( internalName );
    }
    str = AllocMem( len );
    *str = '\0';
    strcat( str, entryName );
    if( ordinal != NULL ) {
        strcat( str, "." );
        strcat( str, ordinal );
    }
    if( internalName != NULL ) {
        strcat( str, "=" );
        strcat( str, internalName );
    }
    add_string( optStr, str, 0 );
    FreeMem( entryName );
    if( internalName != NULL )  FreeMem( internalName );
    if( ordinal != NULL )  FreeMem( ordinal );

    return( 1 );
}


/*
 * Parse the /EXTRACT option.
 */
static int parse_extract( OPT_STRING **p )
/*************************************/
{
    return( do_string_parse( p, "EXTRACT", 1, 0 ) );
}


/*
 * Parse the /IMPORT option.
 */
static int parse_import( OPT_STRING **p )
/*************************************/
{
    return( do_string_parse( p, "IMPORT", 1, 0 ) );
}



/*
 * Parse the /INCLUDE option.
 */
static int parse_include( OPT_STRING **p )
/****************************************/
{
    return( do_string_parse( p, "INCLUDE", 0, '\'' ) );
}


/*
 * Parse the /LIST option.
 */
static int parse_list( OPT_STRING **p )
/************************************/
{
    char *              str;

    if( !CmdScanRecogChar( ':' ) )  return( 1 );
    str = CmdScanString();
    if( str != NULL ) {
        OPT_CLEAN_STRING( p );
        add_string( p, str, 0 );
    }
    return( 1 );
}


/*
 * Parse the /MACHINE option.
 */
static int parse_machine( OPT_STRING **p )
/****************************************/
{
    return( do_string_parse( p, "MACHINE", 0, 0 ) );
}


/*
 * Parse the /MAC option.
 */
static int parse_mac( OPT_STRING **p )
/****************************************/
{
    return( do_string_parse( p, "MAC", 0, 0 ) );
}



/*
 * Parse the /NAME option.
 */
static int parse_name( OPT_STRING **p )
/************************************/
{

    return( do_string_parse( p, "NAME", 1, 0 ) );
}



/*
 * Parse the /NODEFAULTIB option.
 */
static int parse_nodefaultlib( OPT_STRING **p )
/************************************/
{
    char *              str;

    if( !CmdScanRecogChar( ':' ) )  return( 1 );
    str = CmdScanString();
    if( str != NULL ) {
        OPT_CLEAN_STRING( p );
        add_string( p, str, 0 );
    }
    return( 1 );
}



/*
 * Parse the /OUT option.
 */
static int parse_out( OPT_STRING **p )
/************************************/
{
    return( do_string_parse( p, "OUT", 1, 0 ) );
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
 * Parse the /REMOVE option.
 */
static int parse_remove( OPT_STRING **p )
/*************************************/
{
    return( do_string_parse( p, "REMOVE", 0, 0 ) );
}


/*
 * Parse the /SUBSYSTEM option.
 */
static int parse_subsystem( OPT_STRING **p )
/******************************************/
{
    return( do_string_parse( p, "SUBSYSTEM", 1, 0 ) );
}


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


/* Include after all static functions were declared */
#include "optparsc.gh"
