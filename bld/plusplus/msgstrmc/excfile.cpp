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


// ExcFile -- implementation
//
// 92/12/30 -- J.W.Welch        -- defined

#include "excfile.h"
#include "iostream.h"


ExcFile::ExcFile                // CONSTRUCTOR
    ( const char *name          // - file name
    , const char *text )        // - message
    : filename( name )
    , msg( text )
{
}


void ExcFile::display_msg_contents( // DISPLAY MESSAGE CONTENTS
    void ) const
{
    cerr << msg << ": " << filename;
}



// ExcFile... :  CTOR's supply message text for ExcFile base

#define ExcFileCtor( clname, msg )  \
clname::clname                      \
    ( const char *filename )        \
    : ExcFile( filename, msg )      \
{                                   \
}

ExcFileCtor( ExcFileOpenInput,  "cannot open input file" );
ExcFileCtor( ExcFileOpenOutput, "cannot open output file" );
ExcFileCtor( ExcFileRead,       "reading input file" );
ExcFileCtor( ExcFileWrite,      "writing input file" );
