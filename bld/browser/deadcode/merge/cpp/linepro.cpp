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


// System includes --------------------------------------------------------

#include <stdio.h>

// Project includes -------------------------------------------------------

#include "linepro.h"

LinePrologue::LinePrologue()
/**************************/
    : opcode_lengths( NULL )
{
}

LinePrologue::~LinePrologue()
/***************************/
{
    if( opcode_lengths != NULL ) {
        delete [] opcode_lengths;
    }

    #if DEBUG_LIST
        printf( "LinePrologue::~LinePrologue() -- directories contains %d elements\n", directories.entries() );
        printf( "LinePrologue::~LinePrologue() -- filenames contains %d elements\n", filenames.entries() );
    #endif

#if 0   // ITB -- why is this not included?
    int i;

    for( i = 0; i < directories.entries(); i += 1 ) {
        if( directories[ i ] != NULL ) {
            delete directories[ i ];
        }
    }

    for( i = 0; i < filenames.count(); i += 1 ) {
        if( filenames[ i ] != NULL ) {
            delete filenames[ i ];
        }
    }
#endif
}

bool LinePrologue::operator==( const LinePrologue & pro )
/************************************************/
{
    if(         pro.min_instr == min_instr
             && pro.default_is_stmt == default_is_stmt
             && pro.line_base == line_base
             && pro.line_range == line_range
             && pro.opcode_base == opcode_base ) {

        int i;

        /*
         * It's opcode_base - 1 because of the DWARF spec...
         */
        for( i = 0; i < opcode_base - 1; i += 1 ) {
            if( pro.opcode_lengths[ i ] != opcode_lengths[ i ]) {
                return 0;
            }
        }
    } else {
        return 0;
    }

    return 1;

}

bool LinePrologue::operator!=( const LinePrologue & pro )
/*******************************************************/
{
    return !operator==( pro );
}

