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


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "bool.h"
#include "error.h"

static void     do_nothing( void );


static void             (*bannerFunc)( void ) = do_nothing;
static bool             displayWarnings = true;


/*
 * If format==NULL, a default error message is displayed using perror().
 * Otherwise, format is treated as the format string, and passed to
 * vfprintf() with any arguments specified after it.  This function
 * terminates the program, returning exit status EXIT_FAILURE.
 */
void FatalError( const char *format, ... )
/****************************************/
{
    va_list             args;

    (*bannerFunc)();
    if( format == NULL ) {
        perror( "Fatal error" );
    } else {
        va_start( args, format );
        fprintf( stderr, "Fatal error: " );
        vfprintf( stderr, format, args );
        fprintf( stderr, "\n" );
        va_end( args );
    }

    exit( EXIT_FAILURE );
}


/*
 * Point out the offending location and exit with status EXIT_FAILURE.
 */
void InternalError( int line, const char *file )
/**********************************************/
{
    (*bannerFunc)();
    fprintf( stderr, "Internal error on line %d of %s. Please contact the Open Watcom maintainers at http://www.openwatcom.com\n",
             line, file );
    exit( EXIT_FAILURE );
}


/*
 * Print a warning message.
 */
void Warning( const char *format, ... )
/*************************************/
{
    va_list             args;

    if( displayWarnings ) {
        (*bannerFunc)();
        va_start( args, format );
        fprintf( stderr, "Warning: " );
        vfprintf( stderr, format, args );
        fprintf( stderr, "\n" );
        va_end( args );
    }
}


/*
 * Print an information message.  Obeys DisableWarnings.
 */
void Information( const char *format, ... )
/*****************************************/
{
    va_list             args;

    if( displayWarnings ) {
        (*bannerFunc)();
        va_start( args, format );
        fprintf( stderr, "Info: " );
        vfprintf( stderr, format, args );
        fprintf( stderr, "\n" );
        va_end( args );
    }
}


/*
 * Enables warnings if yesno is zero, and disables them if non-zero.
 */
void DisableWarnings( bool yesno )
/********************************/
{
    displayWarnings = !yesno;
}


/*
 * Set the function used to output the banner.
 */
void SetBannerFuncError( void (*func)( void ) )
/*********************************************/
{
    bannerFunc = func;
}


/*
 * Default banner-drawing function, which does nothing.
 */
static void do_nothing( void )
/****************************/
{
}
