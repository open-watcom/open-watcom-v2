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
* Description:  PE Dump Utility mainline.
*
****************************************************************************/


#include "param.h"
#include "read.h"
#include "format.h"
#include "strings2.h"

void main( int argc, char *argv[] ) {
    Parameters          param;
    ExeFile             exeFile;
    bool                ret;

    ret = true;
    defaultParam( &param );
    exeFile.file = NULL;
    exeFile.tabEnt = NULL;

    printBanner();
    if( argc > 1 ) {
        ret = loadParam( &param, argc, argv );
    } else {
        printHelp();
        ret = false;
    }

    if( ret )   ret = openExeFile( &exeFile, &param );
    if( ret )   ret = readExeHeaders( &exeFile );
    if( ret )   {
                printDosHeader( &exeFile, &param );
                printPeHeader( &exeFile, &param );
                ret = findResourceObject( &exeFile );
    }
    if( ret )   ret = loadAllResources( &exeFile );
    if( ret )   {
                printResObject( &exeFile, &param );
                printTableContents( exeFile.tabEnt, &exeFile,
                                    &param, exeFile.resObj.physical_offset,
                                    0 );
    }
    freeAllResources( &exeFile );
}

