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
* Description:  Error and informational message functions for whc.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include "wio.h"
#include "hcerrors.h"
#include "errstrs.h"
//#include "clibext.h"

typedef const char *ErrString;


//  ErrorPrint  --"printf" function to print error messages.

void ErrorPrint( FILE *fp, ErrString format, va_list values )
{
    const char  *letter;
    ErrString   string;
    int     value;

    for( letter = format; *letter != '\0'; letter++ ) {
        if( *letter != '%' ) {
            fputc( *letter, fp );
        } else {
            // Currently, only %s (strings) and %d (integers) are supported.
            switch( *++letter ) {
            case 'S':
            case 's':
                string = va_arg( values, ErrString );
                fprintf( fp, "%s", string );
                break;

            case 'D':
            case 'd':
                value = va_arg( values, int );
                fprintf( fp, "%d", value );
                break;

            case '%':
                fputc( '%', fp );
            }
        }
    }
    return;
}


// The "Fatal Error" function.

void HCError( int err_num, ... )
{
    ErrString   string = err_strings[err_num];
    va_list values;

    fputc( '\n', stderr );
    va_start( values, err_num );
    ErrorPrint( stderr, string, values );
    va_end( values );

    // Throw an exception to jump back to main().
    throw HCException();
}


// The "Non-fatal Error" function.

void HCWarning( int err_num, ... )
{
    ErrString   string = err_strings[err_num];
    va_list values;

    fputc( '\n', stderr );
    va_start( values, err_num );
    ErrorPrint( stderr, string, values );
    va_end( values );

    return;
}


// The next seven functions are to reassure the user that something is
// happening at any given time.

static int printOutput = 1;

void SetQuiet( int be_quiet )
{
    // Shut up if output is redirected
    if( isatty( STDERR_FILENO ) )
        printOutput = !be_quiet;
    else
        printOutput = 0;
    return;
}

void HCStartFile( char const name[] )
{
    if( printOutput ) {
        fprintf( stderr, "\nReading %s  ", name );
    }
}

void HCTick()
{
    static const char wheel[]="\\|/-";
    static unsigned i=0;
    if( printOutput ) {
        fputc( '\b', stderr );
        fputc( wheel[i++ % 4], stderr );
    }
}

void HCDoneTick()
{
    if( printOutput ) {
        fprintf( stderr, "\nFinished.\n" );
    }
}

void HCStartOutput()
{
    if( printOutput ) {
        fprintf( stderr, "\nWriting .HLP file.  " );
    }
}

void HCStartPhrase()
{
    if( printOutput ) {
        fprintf( stderr, "\nConstructing phrase table:         " );
    }
}

void HCPhraseLoop( int pass )
{
    if( printOutput ) {
        fprintf( stderr, "\b\b\b\b\b\b\bPass %d.", pass );
    }
}


// A call-back function for the memory tracker, if it is in use.

void HCMemerr( void )
{
    HCError( MEM_ERR );
}
