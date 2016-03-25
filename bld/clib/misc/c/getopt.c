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
* Description:  Implementation of traditional getopt().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

_WCRTDATA char      *optarg;            // pointer to option argument
_WCRTDATA int       optind = 1;         // current argv[] index
_WCRTDATA int       optopt;             // currently processed chracter
_WCRTDATA int       opterr = 1;         // error output control flag

#ifdef __UNIX__
char                __altoptchar = '-';
#else
char                __altoptchar = '/'; // alternate option character
#endif
char                __optchar;          // matched option char ('-' or altoptchar)

static int          opt_offset = 0;     // position in currently parsed argument

// Error messages suggested by Single UNIX Specification
#define NO_ARG_MSG      "%s: option requires an argument -- %c\n"
#define BAD_OPT_MSG     "%s: illegal option -- %c\n"

_WCRTLINK int getopt( int argc, char * const argv[], const char *optstring )
/**************************************************************************/
{
    char        *ptr;
    char        *curr_arg;

    optarg = NULL;
    curr_arg = argv[optind];
    if( curr_arg == NULL ) {
        return( -1 );
    }
    for( ;; ) {
        optopt = (unsigned char)curr_arg[opt_offset];
        if( isspace( optopt ) ) {
            opt_offset++;
            continue;
        }
        break;
    }
    if( opt_offset > 1 || optopt == '-' || optopt == __altoptchar ) {
        if( opt_offset > 1 ) {
            optopt = curr_arg[opt_offset];
            if( optopt == '-' || optopt == __altoptchar ) {
                __optchar = optopt;
                opt_offset++;
                optopt = curr_arg[opt_offset];
            }
        } else {
            __optchar = optopt;
            opt_offset++;
            optopt = curr_arg[opt_offset];
        }
        if( optopt == '\0' ) {  // option char by itself should be
            return( -1 );       // left alone
        }
        if( optopt == '-' && curr_arg[opt_offset + 1] == '\0' ) {
            opt_offset = 0;
            ++optind;
            return( -1 );   // "--" POSIX end of options delimiter
        }
        ptr = strchr( optstring, optopt );
        if( ptr == NULL ) {
            if( opterr && *optstring != ':' ) {
                fprintf( stderr, BAD_OPT_MSG, argv[0], optopt );
            }
            return( '?' );  // unrecognized option
        }
        if( *(ptr + 1) == ':' ) {   // check if option requires argument
            if( curr_arg[opt_offset + 1] == '\0' ) {
                if( argv[optind + 1] == NULL ) {
                    if( *optstring == ':' ) {
                        return( ':' );
                    } else {
                        if( opterr ) {
                            fprintf( stderr, NO_ARG_MSG, argv[0], optopt );
                        }
                        return( '?' );
                    }
                }
                optarg = argv[optind + 1];
                ++optind;
            } else {
                optarg = &curr_arg[opt_offset + 1];
            }
            opt_offset = 0;
            ++optind;
        } else {
            opt_offset++;
            if( curr_arg[opt_offset] == '\0' ) {    // last char in argv element
                opt_offset = 0;
                ++optind;
            }
        }
        return( optopt );   // return recognized option char
    } else {
        return( -1 );       // no more options
    }
}
