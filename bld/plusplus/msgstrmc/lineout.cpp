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


// LineOut -- implementation
//
// 92/12/30 -- J.W.Welch        -- defined

#include <stdarg.h>

#include "lineout.h"
#include "excfile.h"


LineOut::LineOut(               // Constructor
    const char *file_name )     // - file name
    : file_name( file_name )
{
    file.clear();
    file.open( file_name, ( ios::out | ios::trunc ) );
    if( ! file ) {
        throw ExcFileOpenOutput( file_name );
    }
}


LineOut::~LineOut(              // Destructor
    void )
{
    file.close();
}


void LineOut::write(            // WRITE A LINE
    char *line )                // - line to be written
{
    file << line << endl;
    if( file.fail() ) {
        throw ExcFileWrite( file_name );
    }
}


void LineOut::write_strings(    // WRITE STRINGS
    char *str, ... )            // - NULL-terminated list of strings
{
    va_list args;               // - arguments
    char* arg;                  // - next argument

    va_start( args, str );
    for( arg = str; arg != 0;arg = va_arg( args, char* ) ) {
        file << arg;
        if( file.fail() ) {
            throw ExcFileWrite( file_name );
        }
    }
    file << endl;
    if( file.fail() ) {
        throw ExcFileWrite( file_name );
    }
    va_end( args );
}
