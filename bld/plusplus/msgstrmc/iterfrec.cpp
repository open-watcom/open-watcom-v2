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


// ITERFREC -- implementation of file-record iterations
//
// 92/12/30 -- J.W.Welch        -- defined

#include "iterfrec.h"
#include "excfile.h"

//---------------------------------------------------------------------------
// IterFileRec -- iterator for file of records
//---------------------------------------------------------------------------

IterFileRec::IterFileRec(       // Constructor
    const char* fname,          // - file name
    ifstream &file,             // - file structure
    Boolean &retn,              // - TRUE ==> opened
    Boolean reqd )              // - TRUE ==> file must exist
    : filename( fname )
    , file( file )
{
    file.open( fname, ( ios::in | ios::nocreate ) );
    if( ! file ) {
        if( reqd ) {
            throw ExcFileOpenInput( fname );
        } else {
            retn = FALSE;
        }
    } else {
        retn = TRUE;
    }
}


IterFileRec::~IterFileRec(      // Destructor
    void )
{
    file.close();
}


Boolean IterFileRec::next(      // GET NEXT RECORD
    void )
{
    Boolean retn;               // - TRUE ==> have data

    file.getline( buffer, sizeof( buffer ) - 1 );
    if( file.eof() ) {
        retn = FALSE;
    } else if( file.fail() ) {
        throw ExcFileRead( filename );
    } else {
        buffer[ strlen( buffer ) - 1 ] = '\0';
        retn = TRUE;
    }
    return retn;
}


void *IterFileRec::element(     // POINT AT ELEMENT
    void ) const
{
    return buffer;
}
