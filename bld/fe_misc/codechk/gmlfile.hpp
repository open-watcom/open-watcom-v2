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
*  File: GMLFile.hpp: contains the def'n of a class that reads in a
*         .gml file, and prints the code fragments it contains, one by
*         one.
*
*****************************************************************************/
#include <stdio.h>
#define MAX_CODE_FRAG_SIZE 16000
#define MAX_LINE_LEN 1024

class GMLFile {
    protected:
    FILE *_inFile;
    char _nextFrag[MAX_CODE_FRAG_SIZE]; //next code fragment.
    char _nextErrCode[MAX_LINE_LEN]; // error Code of next fragment.
    char _retErrCode[MAX_LINE_LEN];  // error code of fragment just returned.
    int  _errIndex;

    virtual void _loadNext();

    public:
    // constructor
    GMLFile();

    // LoadFile: this function initializes the class to the beginning of
    //   the specified file.
    void LoadFile(const char *fileName);

    // AtEOF; This function returns non-zero if there is no
    // more data to read.
    int AtEOF();

    // DumpNextCodeFrag; This function searches thru the .gml file
    //  for the next entry with a code fragment.
    // If it is a :baderr, it returns the error code of that code fragment, or
    // 'No Error' if it is a :gooderr fragment.
    char *DumpNextCodeFrag(const char *dumpFile);
};

class JavaGMLFile : public GMLFile {
    protected:
    char _group[3];
    void _loadNext();

    public:
    JavaGMLFile();
};




