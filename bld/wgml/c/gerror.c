/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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

#include <stdarg.h>
#include "wgml.h"

#include "clibext.h"


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
//    flushall();                         // TBD
//    fcloseall();                        // TBD
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

    switch( errornum ) {
    case ERR_STR_NOT_FOUND:
        /* this message means the error strings cannot be obtained from
         * the exe so its text is hard coded */
        sprintf( err_buf, "%s %d: %nResource strings not found", prefix,
                    errornum, &len );
        break;
    case ERR_DUMMY:
        /* dont print anything */
        return;
    default:
        get_msg( errornum, err_buf, sizeof( err_buf ) );
        vsprintf( str_buf, err_buf, arglist );
        if( *prefix == '\0' ) {
            // no prefix and errornumber
            sprintf( err_buf, "%n%s", &len, str_buf );
        } else {
            sprintf( err_buf, "%s %d: %n%s", prefix, errornum, &len, str_buf );
        }
        break;
    }

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
            while( !isspace( *end ) && end > start ) end--;
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
            while( isspace( *save ) ) save++;   // step over any spaces
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
/*  informational about line position in macro/file                        */
/***************************************************************************/

void g_info_inp_pos( void )
{
    char        linestr[MAX_L_AS_STR];

    if( input_cbs->fmflags & II_tag_mac ) {
        ultoa( input_cbs->s.m->lineno, linestr, 10 );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        ultoa( input_cbs->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
}

