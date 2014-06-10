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
* Description:  Generates makefile style auto dependancy files.  Default
*     output name will be based from source file name with the extension
*     '.d'.  Target name may be overridden.  
*     AdClose() has an option to erase when closing (supposed to delete 
*     when there was an error which aborted compilation...
*
****************************************************************************/

#include "plusplus.h"

#include "memmgr.h"
#include "iosupp.h"
#include "srcfile.h"
#include "cgdata.h"
#include "iopath.h"
//#include "hfile.h"

static FILE *AutoDepFile;


char *DoForceSlash( char *name, char slash )
{
    char *save = name;
    if( !slash || !save )
        return name;
    while( name[0] ) {
        if( IS_DIR_SEP( name[0] ) )
            name[0] = slash;
        name++;
    }
    return( save );
}

void AdDump( void )
{
    SRCFILE retn;
    char    *name;

    if( AutoDepFile ) {
        fprintf( AutoDepFile, "%s : ", DoForceSlash( IoSuppOutFileName( OFT_TRG ), ForceSlash ) );
        fprintf( AutoDepFile, "%s", DoForceSlash( IoSuppOutFileName( OFT_SRCDEP ), ForceSlash ) );
        for( retn = SrcFileNotReadOnly( SrcFileWalkInit() );
             retn != NULL && !IsSrcFilePrimary( retn );
             retn = SrcFileNotReadOnly( SrcFileWalkNext( retn ) ) )
        {
            name = SrcFileName(retn);
            if( HAS_PATH( name ) || DependHeaderPath == NULL ) {
               fprintf( AutoDepFile, " %s", DoForceSlash( name, ForceSlash ) );
            } else {
               fprintf( AutoDepFile, " %s%s", DependHeaderPath, DoForceSlash( name, ForceSlash ) );
            }
        }
    }
}

void AdOpen( void )
{
    AutoDepFile = fopen( IoSuppOutFileName( OFT_DEP ), "wt" );
}

void AdClose( bool and_erase )
{
    // for now fake out doing anything....
    // and_erase = TRUE;
    if( AutoDepFile != NULL ) {
        fclose( AutoDepFile );
        AutoDepFile = NULL;
    }
    if( and_erase ) {
        remove( IoSuppOutFileName( OFT_DEP ) );
    }
}
