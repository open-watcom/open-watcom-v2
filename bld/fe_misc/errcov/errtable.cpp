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
*  ErrTable.cpp: implements the ErrorTable class and its two
*  derivatives, JavaErrorTable and CErrorTable
*
****************************************************************************/
#include <stdlib.h>
#include "ErrTable.hpp"


ErrorTable::ErrorTable(char *fileName)  {
    int index;
    strncpy(_GMLFileName, fileName, MAX_LINE_LEN);
    for( index=0; index < ERR_HASH_SIZE; index++) {
        _errTable[index] = NULL;
    }
}

ErrorTable::ErrorTable()  {
    int index;
    strcpy(_GMLFileName, "c_messag.gml\0");
    for( index=0; index < ERR_HASH_SIZE; index++) {
        _errTable[index] = NULL;
    }
}

void ErrorTable::ReadInTable() {
    printf("Internal Error: Instance of ErrorTable used to read file. Try \n\
using a CErrorTable or JavaErrorTable instead.\n");
    exit(0);

}

int ErrorTable::_getHash(char *ErrCode) {
    int hash=0;
    int length=strlen(ErrCode);
    int index;
    for(index=0; index<length; index++) {
        hash = (hash * 10) + abs(ErrCode[index] - 30);
    }
    hash %= ERR_HASH_SIZE;
    return(hash);
}

int ErrorTable::MarkError(char *ErrCode) {
    int hash = _getHash( ErrCode );
    Error *curr;

    curr = _errTable[hash];
    while (curr != NULL ) {
        if( ! strcmp(ErrCode, curr->errorCode) ) {
            curr->mark = 1;
            return(1);
        }
        curr = curr->next;
    }
    return(0); // if we fell thru this far, it's not there.
}

void ErrorTable::AddError(char *ErrCode, char *ErrSymbol)  {
    int hash = _getHash( ErrCode );
    Error *curr;
    Error *add = new Error;
    strcpy( add->errorSymbol, ErrSymbol );
    strcpy( add->errorCode, ErrCode);

    curr = _errTable[hash];
    if( curr == NULL) {
        _errTable[hash]=add;
    } else {
        while( (curr->next != NULL) && ( strcmp(curr->errorCode,ErrCode) )) {
            curr = curr->next;
        }
        if( strcmp(curr->errorCode,ErrCode) ) {
            curr->next = add;
        }
        // else it's already in the list.
    }
}

const char *ErrorTable::ErrorSymbol( char *ErrCode)  {
    int hash = _getHash( ErrCode );
    Error *curr;

    curr = _errTable[hash];
    while (curr != NULL ) {
        if( ! strcmp(ErrCode, curr->errorCode) ) {
            return(curr->errorSymbol);
        }
        curr = curr->next;
    }
    return("Symbol not found.\0"); // if we fell thru this far, it's not there.
}

void ErrorTable::PrintUnmarked( FILE *out, int showInf )  {
    Error *curr;
    int index;
    for(index = 0; index < ERR_HASH_SIZE; index++) {
        curr = _errTable[index];
        while( curr != NULL ) {
            if( curr->mark == 0 ) {
                if( ( showInf ) || ( strncmp(curr->errorSymbol,"INF",3) ) ) {
                    fprintf(out," Unused Error: %s, %s\n",curr->errorCode,curr->errorSymbol);
                }
            }
            curr = curr->next;
        }
    }
}

/**********************************************************************************/

JavaErrorTable::JavaErrorTable(char *fileName): ErrorTable(fileName) { }

void JavaErrorTable::ReadInTable() {
    // read the java input file.
    // scans for a ":MSGSYM" or "MSGGRP:" flag, and acts accordingly.
    char buffer[ MAX_LINE_LEN ];
    char group[ MAX_LINE_LEN ] = "XX\0";
    char symbol[ MAX_LINE_LEN ];
    char code[ MAX_LINE_LEN ];
    int grpIndex = 0;
    int found = 0;

    printf("Reading in Java message file.\n");

    _inFile = fopen( _GMLFileName, "rt" );
    if(_inFile==NULL) {
        fprintf( stderr, "Error opening file %s \n", _GMLFileName );
        exit(0);
    }

    while( ! feof( _inFile )  ) {
        // scan for next :MSGSYM symbol or :MSGGRP symbol.
        fgets(buffer, MAX_LINE_LEN, _inFile);
        if( ! strncmp( buffer, ":MSGSYM.\0", 8) ) {
            // otherwise, we've found a new symbol.
            strncpy( symbol, &buffer[9], MAX_LINE_LEN );
            // now, construct the errCode:
            sprintf(code,"%s-%3.3i\0",group,grpIndex);
            AddError(code, symbol);
            grpIndex++;
//         printf("Adding error code %s, %s\n",code, symbol);
        } else if( ! strncmp( buffer, ":MSGGRP.\0", 8) ) {
            strncpy( group, &buffer[9], MAX_LINE_LEN);
            group[2]=NULL; // eliminate the newline character.
            grpIndex=0;

        }

    }
    fclose( _inFile );


}

/**********************************************************************************/
CErrorTable::CErrorTable(char *fileName): ErrorTable(fileName)  { }

void CErrorTable::ReadInTable() {
    /*/ read the C input file.
    // scans for a ":MSGSYM" tag, reads in the data it needs, then repeats until EOF */
    char buffer[ MAX_LINE_LEN ];
    char symbol[ MAX_LINE_LEN ];
    char code[ MAX_LINE_LEN ];
    int errIndex = 0;
    int found = 0;

    printf("Reading in C message file.\n");

    _inFile = fopen( _GMLFileName, "rt" );
    if(_inFile==NULL) {
        fprintf( stderr, "Error opening file %s", _GMLFileName );
    }

    while( ! feof( _inFile )  ) {
        // scan for next :MSGSYM symbol
        while(( ! found ) && ( ! feof( _inFile ) )) {
            fgets(buffer, MAX_LINE_LEN, _inFile);
            if( ! strncmp( buffer, ":MSGSYM.\0", 8) ) {
                found = 1;
            }
        }
        found =0;
        if( feof( _inFile ) ) break;

        // otherwise, we've found a new symbol.
        strncpy( symbol, &buffer[9], MAX_LINE_LEN );


        // now, construct the errCode:
        if( !strncmp( symbol, "ERR_", 4) ) {
            sprintf(code,"E%3.3i\0",errIndex);
            AddError(code, symbol);
            errIndex++;
        } else if( !strncmp( symbol, "WARN_", 5) ) {
            sprintf(code,"W%3.3i\0",errIndex);
            AddError(code, symbol);
            errIndex++;
        } else if( !strncmp( symbol, "ANSI", 4) ) {
            sprintf(code,"W%3.3i\0",errIndex);
            AddError(code, symbol);
            errIndex++;
        } else if( !strncmp( symbol, "INF_", 4) ) {
            sprintf(code,"N%3.3i\0",errIndex);
            AddError(code, symbol);
            errIndex++;
        } else {
            fprintf( stdout, "Scanner error: unable to classify symbol %s\n as \
error or warning. \n", symbol );
        }
//        printf("Read new code: %s, %s\n",code, symbol);
    }
    fclose( _inFile );
}



