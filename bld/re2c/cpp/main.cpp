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


#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "globals.h"
#include "parser.h"
#include "dfa.h"

char *fileName;
bool sFlag = false;
bool bFlag = false;

int main(int argc, char *argv[]){
    fileName = NULL;
    if(argc == 1)
        goto usage;
    while(--argc > 1){
        char *p = *++argv;
        while(*++p != '\0'){
            switch(*p){
            case 'e':
                xlat = asc2ebc;
                talx = ebc2asc;
                break;
            case 's':
                sFlag = true;
                break;
            case 'b':
                sFlag = true;
                bFlag = true;
                break;
            default:
                goto usage;
            }
        }
    }
    fileName = *++argv;
    int fd;
    if(fileName[0] == '-' && fileName[1] == '\0'){
        fileName = "<stdin>";
        fd = 0;
    } else {
        if((fd = open(fileName, O_RDONLY)) < 0){
            std::cerr << "can't open " << fileName << "\n";
            return 1;
        }
    }
    parse(fd, std::cout);
    return 0;
usage:
    std::cerr << "usage: re2c [-esb] name\n";
    return 2;
}
