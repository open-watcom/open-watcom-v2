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


/************************************************************************************
*     ErrCov Error Coverage Analysis tool:
*   Main.cpp : mainline of coverage tool
*
*************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ErrTable.hpp"
#include "TestFile.hpp"
#define ERRCODESIZE 128

int main(int argc, char *argv[]) {
    char        Usage[] = "Usage: \n  errcov [/i] <.gml file> <output file> [...]\n\
/i : output unused :INFO tags; requires data generated w/out -ew option.\n";
    ErrorTable  *errTable;
    TestFile    *inFile;
    int         fileCount = 2;
    char        ErrCode[ ERRCODESIZE ];
    int         errCount = 0;
    int         argIndex = 1;
    int         showInf = 0;

    // Process command line:
    if(argc < 3)  {
        printf( "%s", Usage );
        exit(0);
    }

    if( !strcmp( argv[argIndex], "/i" ) ) {
        showInf = 1;
        argIndex++;
    }

#if APP_TYPE == 1
    errTable = new CErrorTable(argv[argIndex]);
#elif APP_TYPE == 2
    errTable = new JavaErrorTable(argv[argIndex]);
#else
    printf("Compilation error: unsure which type of Coverage Tester to generate.\n");
    exit(0);
#endif
    errTable->ReadInTable();

    for(argIndex++ ; argIndex < argc; argIndex++) {
        inFile = new TestFile( argv[argIndex] );
        while( ! inFile->EndOfFile() ) {

            inFile->GetNextError(ErrCode);
            errCount++;
//          printf(" Marking Error %i: %s \n",errCount,ErrCode);
            if ( ! errTable->MarkError(ErrCode) ) {
                printf("Note: attempted to mark nonexistent error %s.\n",ErrCode);
            }
        }
        delete(inFile);
    }

    errTable->PrintUnmarked(stdout, showInf);

    delete(errTable);
    return(0);
}



