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


/*****************************************************************************
*   Sample Code Checker:
*  Written by:  Mike Neame,Jan 19, 1998
*  File: GMLFile.cpp: Implements the class defined in GMLFile.hpp
*
*****************************************************************************/
#include <string.h>
#include "GMLFile.hpp"


GMLFile::GMLFile() {
    _errIndex=0;
    _nextFrag[0] = NULL;
    _nextErrCode[0] = NULL;
    _retErrCode[0] = NULL;
    _inFile = NULL;
}


void GMLFile::_loadNext() {
    char        buffer[ MAX_LINE_LEN ];
    while(1) {
        if( 0 == fgets(buffer, MAX_LINE_LEN, _inFile) ) {
            // no more output.
            _nextFrag[0] = NULL;
            _nextErrCode[0] = NULL;
            break;
        } else {
            if( ! strncmp(buffer, ":MSGSYM.", 8) ) {

                // now, construct the errCode:
                if( !strncmp( &buffer[9], "ERR_", 4) ) {
                    sprintf(_nextErrCode,"E%3.3i\0",_errIndex);
                    _errIndex++;
                } else if( !strncmp(&buffer[9] , "WARN_", 5) ) {
                    sprintf(_nextErrCode,"W%3.3i\0",_errIndex);
                    _errIndex++;
                } else if( !strncmp( &buffer[9], "ANSI", 4) ) {
                    sprintf(_nextErrCode,"W%3.3i\0",_errIndex);
                    _errIndex++;
                } else if( !strncmp( &buffer[9], "INF_", 4) ) {
                    sprintf(_nextErrCode,"N%3.3i\0",_errIndex);
                    _errIndex++;
                } else {
                    fprintf( stdout, "Scanner error: unable to classify symbol %s\n as \
error or warning. \n", &buffer[9] );
                }
            } else if(! strncmp(buffer, ":errbad.", 8) ) {
                _nextFrag[0] = NULL;
                fgets(buffer, MAX_LINE_LEN, _inFile);
                while( strncmp(buffer, ":eerrbad.", 9) ) {
                    strncat( _nextFrag,
                             buffer,
                             MAX_CODE_FRAG_SIZE-strlen(_nextFrag));
                    fgets(buffer, MAX_LINE_LEN, _inFile);
                };
                break; // out of endless while loop
            }
        }
    }

}


void GMLFile::LoadFile(const char *fileName) {
    if( _inFile != NULL ) {
        fclose(_inFile);
    }
    _inFile = fopen(fileName, "rt");
    if( _inFile == NULL ) {
        printf(" Error opening file %s.\n",fileName);
    } else {
        // load first frag.
        _errIndex=0;
        _loadNext();
    }
}

int GMLFile::AtEOF() {
    if(( _inFile == NULL ) || ( feof(_inFile) )) {
        return( 1 );
    } else {
        return( 0 );
    }
}

char *GMLFile::DumpNextCodeFrag(const char *dumpFile) {
    FILE *out;
    out = fopen(dumpFile, "wt" );
    if( out != NULL ) {
        fprintf(out, "\n//Sample code fragment: Should generate error %s.\n",_nextErrCode);
        fprintf(out, "%s\n",_nextFrag);
        fclose(out);

        strncpy(_retErrCode, _nextErrCode, MAX_LINE_LEN);
        _loadNext();

        return(_retErrCode);
    } else {
        return(NULL);
    }
}

/***************************************************************************************/



JavaGMLFile::JavaGMLFile() : GMLFile() {
    _group[0]=NULL;
}


void JavaGMLFile::_loadNext() {
    char        buffer[ MAX_LINE_LEN ];
    while(1) {
        if( 0 == fgets(buffer, MAX_LINE_LEN, _inFile) ) {
            // no more output.
            _nextFrag[0] = NULL;
            _nextErrCode[0] = NULL;
            break;
        } else {
            if( ! strncmp(buffer, ":MSGSYM.", 8) ) {

                // now, construct the errCode:
                sprintf(_nextErrCode,"%s-%3.3i\0",_group,_errIndex);
                _errIndex++;

            } else if(! strncmp(buffer, ":MSGGRP.", 8) ) {
                strncpy(_group, &buffer[9], 2);
                _group[2] = NULL;
                _errIndex = 0;

            } else if(! strncmp(buffer, ":errbad.", 8) ) {
                _nextFrag[0] = NULL;
                fgets(buffer, MAX_LINE_LEN, _inFile);
                while( strncmp(buffer, ":eerrbad.", 9) ) {
                    strncat( _nextFrag,
                             buffer,
                             MAX_CODE_FRAG_SIZE-strlen(_nextFrag));
                    fgets(buffer, MAX_LINE_LEN, _inFile);
                };
                break; // out of endless while loop
            }
        }
    }






};



