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


#include "wpch.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include "..\hpp\brmerge.hpp"

MHandle handle;

void __stdcall DefnIter(SourceLocn *locn, DeclRec const *, void *)
{
    printf("Defn: %s (%d,%d)\n",
           locn->filePath, locn->line, locn->col );
}

void __stdcall FnIter(SourceLocn *locn, DeclRec const *sym, void *)
{
    printf("\t%s\n", locn->name);
}


void __stdcall SymIter(SourceLocn *locn, DeclRec const * sym, void *)
{
    printf("Class: %s\n", locn->name);
    MemberFnsIter( handle, sym, &::FnIter, NULL );
}


static char const       BRMExt[] = ".BRM";


void Test( void )
{
    DIR         *dirp;
    DIR         *file;
    char        path[_MAX_PATH];
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];

    dirp = opendir( "." );
    if( dirp != NULL ){
        handle = NewMerger();
        for( ;; ){
            file = readdir( dirp );
            if( file == NULL ){
                break;
            }
            _fullpath( path, file->d_name, _MAX_PATH );
            _splitpath( path, drive, dir, fname, ext );
            if( stricmp( ext, BRMExt ) == 0 ){
                AddFile( handle, file->d_name );
            }
        }
        FileClassesIter(handle, "c:\\helpcomp\\cpp\\topic.cpp", &::SymIter, NULL );
        DestroyMerger(handle);
    }
    closedir( dirp );
}
