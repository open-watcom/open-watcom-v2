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


/****************************************************************************
*       ErrCov Error coverage tool:
*  TestFile.cpp; implements the testfile class and its derivatives
*  that were defined in TestFile.hpp
*
****************************************************************************/
#include "string.h"
#include "TestFile.hpp"


TestFile::TestFile(char *filename) {
    _testFileName=filename; // note that this isn't a local copy.
    _inFile = fopen( _testFileName, "rt" );
    if(_inFile==NULL) {
        fprintf( stderr, "Error opening file %s", _testFileName );
//      exit(1);
    }
    // Initialize _nextToken:
    _getNextToken();
}

void TestFile::GetNextError(char *ErrCode)  {
    strcpy(ErrCode, _nextToken);
    _getNextToken();
}



void TestFile::_getNextToken() {
    char Line[MAXLINELENGTH];
    char *token;
    int found=0;

    while( ( !found ) && ( !feof(_inFile) ) ) {
        fgets(Line,MAXLINELENGTH,_inFile);
        token=strtok(Line," :!");
        token=strtok(NULL," :!");
        // check if the token equals either "Error" or "Warning"
        if( (token!=NULL) && ( (!strcmp(token,"Warning")) || (!strcmp(token,"Error")) )  ) {
            token=strtok(NULL," :!");
            strcpy(_nextToken, token);
            found=1;
            return;
        }
    }
    strcpy(_nextToken,"\0");
}

int TestFile::EndOfFile()  {
    return( feof( _inFile ) );
}




