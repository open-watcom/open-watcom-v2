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


#ifndef __FILECTL_H__
#define __FILECTL_H__

// FileCtl -- simple file control

#include <stdio.h>

#include "Str.h"

class FileCtl                   // FILE CONTROL: BASE
{
protected:
    Str _filename;              // - file name
    FILE* _file;                // - file control block

public:

    FileCtl                     // CONSTRUCTOR
        ( void )
    ;
    ~FileCtl                    // DESTRUCTOR
        ( void )
    ;
    void close                  // CLOSE FILE
        ( void )
    ;
    inline char const *filename // GET FILE NAME
        ( void ) const
    ;
    void io_err                 // THROW I/O ERROR
        ( char const * )
    ;
    void make_filename          // MAKE A FILE NAME
        ( char const *          // - file name
        , char const * )        // - default extension
    ;
    void open                   // OPEN FILE
        ( char const *          // - file name
        , char const *          // - default extension
        , char const * )        // - open mode
    ;

};

char const* FileCtl::filename   // GET FILE NAME
    ( void ) const
{
    return _filename;
}


class FileCtlOutput             // FILE CONTROL: OUTPUT
    : public FileCtl
{
public:

    inline
    FileCtlOutput               // CONSTRUCTOR
        ( void )
    ;
    void open                   // OPEN FILE
        ( char const *          // - file name
        , char const * )        // - default extension
    ;
    void write                  // WRITE BUFFER
        ( char const * )        // - buffer
    ;
};


FileCtlOutput::FileCtlOutput    // CONSTRUCTOR
    ( void )
    : FileCtl()
{
}


class FileCtlInput              // FILE CONTROL: input
    : public FileCtl
{
public:

    inline
    FileCtlInput                // CONSTRUCTOR
        ( void )
    ;
    void open                   // OPEN FILE
        ( char const *          // - file name
        , char const * )        // - default extension
    ;
    int getline                 // GET A LINE
        ( char*                 // - buffer
        , unsigned )            // - size
    ;
};


FileCtlInput::FileCtlInput      // CONSTRUCTOR
    ( void )
    : FileCtl()
{
}

#endif
