/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Error and warning message output.
*
****************************************************************************/


#include "wgml.h"
#include <stdarg.h>

#define MAX_ERR_LEN     1020

static  char    err_buf[MAX_ERR_LEN + 2];   // +2 for \n and \0
static  char    str_buf[MAX_ERR_LEN + 2];


void g_suicide( void )
{
    out_msg( "\n\nWGML suicide\n\n" );
    if( GlobalFlags.research ) {        // TBD

        print_macro_dict( macro_dict, true );

        if( tag_dict != NULL ) {
            print_tag_dict( tag_dict );
        }
        print_single_funcs_research();
        print_multi_funcs_research();

        if( global_dict != NULL ) {
            print_sym_dict( global_dict );
        }
        print_sym_dict( sys_dict );
    }
    out_msg( "\n\nWGML suicide\n\n" );
    fflush( NULL );
    if( environment ) {
        longjmp( *environment, 1 );
    }
    my_exit( 16 );
}


void out_msg( const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    vprintf( msg, args );
    va_end( args );
}


/***************************************************************************/
/*  construct msg  inserting string variables optionally                   */
/***************************************************************************/

#define MAX_LINE_LEN            75
static void g_msg_var( msg_ids errornum, int sev, va_list arglist )
/*****************************************************************/
{
    bool                supp_line = false;
    int                 len;
    const char      *   prefix;
    char            *   save;
    char            *   start;
    char            *   end;

    if( errornum == ERR_DUMMY ) {
        /* dont print anything */
        return;
    }

    switch( sev ) {
#if 0
    case SEV_INFO:
        prefix = "Info:";
        break;
#endif
    case SEV_WARNING:
        prefix = "Warning!";
        msg_indent = 0;
        break;
    case SEV_ERROR:
        prefix = "Error!";
        msg_indent = 0;
        break;
    case SEV_FATAL_ERR:
        prefix = "Fatal Error!";
        msg_indent = 0;
        break;
    default:
        prefix = "";
        supp_line = true;
        break;
    }

    if( errornum == ERR_STR_NOT_FOUND ) {
        /* this message means the error strings cannot be obtained from
         * the exe so its text is hard coded */
        strcpy( err_buf, "Resource strings not found" );
    } else {
        get_msg( errornum, err_buf, sizeof( err_buf ) );
    }
    vsprintf( str_buf, err_buf, arglist );
    len = 0;
    err_buf[0] = '\0';
    if( *prefix != '\0' ) {
        len = sprintf( err_buf, "%s %d: ", prefix, errornum );
        if( len < 0 ) {
            len = 0;
        }
    }
    strcat( err_buf + len, str_buf );

    if( !supp_line ) {    // save points to the ":" or is NULL
        save = strchr( err_buf, ':' );
    }

    start = err_buf;
    if( supp_line ) {
        if( (msg_indent > 0) && (start[0] == '\t') ) {
            start++;    // skip initial tab in favor of msg_indent
        }
        out_msg( "%*s%s\n", msg_indent, "", start );
    } else {
        while( strlen( start ) > MAX_LINE_LEN - msg_indent ) {
            end = start + MAX_LINE_LEN - msg_indent;
            while( !my_isspace( *end ) && end > start )
                end--;
            if( end != start )  {
                *end = '\0';
            } else {
                break;
            }
            out_msg( "%*s%s\n", msg_indent, "", start );
            start = end + 1;
            msg_indent = len;
        }
        out_msg( "%*s%s\n", msg_indent, "", start );
        if( save != NULL ) {    // set msg_indent for follow-on line
            save++;             // step over the ":"
            while( my_isspace( *save ) )    // step over any spaces
                save++;
            msg_indent = save - err_buf;
        }
    }
}

/***************************************************************************/
/*  error msg                                                              */
/***************************************************************************/

void g_err( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_ERROR, args );
    va_end( args );
}

/***************************************************************************/
/*  warning msg                                                            */
/***************************************************************************/

void g_warn( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_WARNING, args );
    va_end( args );
}

/***************************************************************************/
/*  informational msg (general)                                            */
/***************************************************************************/

void g_info( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_INFO, args );
    va_end( args );
}

/***************************************************************************/
/*  informational msg forced to left margin                                */
/***************************************************************************/

void g_info_lm( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    msg_indent = 0;
    g_msg_var( num, SEV_INFO, args );
    va_end( args );
}

/***************************************************************************/
/*  these functions do output that is controlled by GlobalFlags.research   */
/***************************************************************************/

void g_info_research( const msg_ids num, ... )
{
    va_list args;

    if( GlobalFlags.research ) {
        va_start( args, num );
        msg_indent = 0;
        g_msg_var( num, SEV_INFO, args );
        va_end( args );
    }
    return;
}


void out_msg_research( const char *msg, ... )
{
    va_list args;

    if( GlobalFlags.research ) {
        va_start( args, msg );
        vprintf( msg, args );
        va_end( args );
    }
    return;
}
