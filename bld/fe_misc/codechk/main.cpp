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
*  File: main.cpp: contains the program mainline.
*
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GMLFile.hpp"
#define BUFF_SIZE 1024


int CheckForError(char const *fileName, char const *errCode) {
    FILE *in;
    char buffer[BUFF_SIZE];
    char *token;
    int found = 0;

    in = fopen(fileName, "rt");
    if( in == NULL ) return( 0 );

    while(1) {
        if( 0 == fgets(buffer, BUFF_SIZE, in) ) {
            break; // we didn't find it.
        } else {
            token=strtok(buffer," :!");
            token=strtok(NULL," :!");
            // check if the token equals either "Error" or "Warning"
            if( (token!=NULL) && ( (!strcmp(token,"Warning")) || (!strcmp(token,"Error")) )  ) {
                token=strtok(NULL," :!");
                if( ! strcmp(token, errCode) ) {
                    found=1;
                    break;
                }
            }
        }
    }
    fclose(in);
    return(found);
}



void main(int argc, char *argv[]) {
    int retVal;
    char *errCode;
    char sysCommand[BUFF_SIZE];
    GMLFile *codeFrags;


#if APP_TYPE == 1
    codeFrags = new GMLFile;
    char compileStr[BUFF_SIZE] = "wpp386 %s -w9 > d\0";
    char targetName[BUFF_SIZE] = "codeout.cpp\0";
#elif APP_TYPE == 2
    codeFrags = new JavaGMLFile;
    char compileStr[BUFF_SIZE] = "wjd -w=9 %s > d\0";
    char targetName[BUFF_SIZE] = "codeout.java\0";
#else
    printf(" Compile-time error: no app type specified. \n");
    exit(0);
#endif

    if( argc < 2 ) {
        printf("Usage: codechk <.gml filename> [ <errCode> ]\n");
        printf(" tests each chunk of example code to ensure that \n");
        printf(" it generates the indicated error. Specify an optional\n");
        printf(" errCode to produce the code and errors for that chunk. \n");
    } else {
        codeFrags->LoadFile(argv[1]);
        if(argc == 3) {
//            errCode=codeFrags->DumpNextCodeFrag(targetName);
            while( ! codeFrags->AtEOF() ) {
                errCode=codeFrags->DumpNextCodeFrag(targetName);
                if( ! strcmp(errCode, argv[2]) ) {
                    sprintf(sysCommand,
                            compileStr,
                            targetName);
//                  printf(" running command \" %s \" \n",sysCommand);
                    system(sysCommand);
                    break;
                }
            };
        } else {
            while( ! codeFrags->AtEOF() ) {
                errCode=codeFrags->DumpNextCodeFrag(targetName);
                printf("Testing Error %s.\n",errCode);
                sprintf(sysCommand,
                        compileStr,
                        targetName);
//              printf(" running command \" %s \" \n",sysCommand);
                system(sysCommand);
                if( CheckForError("codeout.err", errCode) ) {
                    printf(" Verified error %s.\n",errCode);
                } else {
                    printf("** unverified error: %s.**\n",errCode);
                }
            };
            system("del /Q d");
            system("del /Q codeout.*");
        }
    }




}









