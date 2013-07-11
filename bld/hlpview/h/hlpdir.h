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


//
//  hlpdir.h    --Classes to create the directory of a .HLP file.
//                For the Watcom .HLP compiler.
//
#ifndef HLPDIR_H
#define HLPDIR_H

#include <watcom.h> // for uint_8, etc
#include <string.h>
#include "myfile.h" // for class Dumpable, OutFile
#include "btrees.h" // for the Btree* classes.


//
//  HFSDirectory    --Class to write the directory file,
//            and dump the other files.
//

class HFSDirectory
{
    OutFile     _output;
    Btree       _files;
    static char const   _dirMagic[];

public:
    HFSDirectory( char const filename[] );

    void    dump();
    Dumpable    *addFile( Dumpable *newfile, char const name[] );
};

#endif
