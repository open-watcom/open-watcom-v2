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
*  ErrTable.hpp: defines the ErrorTable class and its two
*  derivatives, JavaErrorTable and CErrorTable
*
****************************************************************************/
#include <stdio.h>
#include <string.h>
#define  ERR_HASH_SIZE 1000
#define  MAX_LINE_LEN 1024

class Error {
    public:
    int mark; // 0 when created, 1 if marked.
    char errorSymbol[80];
    char errorCode[10];
    Error *next;

    Error() {
        mark = 0;
        strcpy( errorSymbol, "uninitialized\0" );
        strcpy( errorCode, "xxxxxxxxx\0" );
        next = NULL;
    }
};

class ErrorTable {
    protected:
    char        _GMLFileName[ MAX_LINE_LEN];
    FILE        *_inFile;
    class Error *_errTable[ ERR_HASH_SIZE ];
    int         _getHash(char *ErrCode);


    public:

    // Constructor: accepts the name of a file to read Error Codes in from.
    //  Usually ends in '.gml'.
    ErrorTable();
    ErrorTable(char *fileName);

    // ReadinTable is called just after the object is initialized.
    virtual void ReadInTable();

    // MarkError: Accepts an error code. Returns 1 if the code was found and
    // marked, or 0 if it was unable to find the code.
    int MarkError(char *ErrCode);

    // AddError: Adds another error to the error table.
    void AddError(char *ErrCode, char *ErrSymbol);

    // ErrorSymbol: Accepts an error code, and returns a string containing
    // the symbol for that error.
    const char *ErrorSymbol( char *ErrCode);

    // Print Unmarked: Prints, on the supplied stream, all unmarked errors.
    void PrintUnmarked( FILE *out, int showInf );

};

class JavaErrorTable: public ErrorTable {
    private:
    void ReadInTable();

    public:
    JavaErrorTable(char *fileName);
};

class CErrorTable: public ErrorTable {
    private:
    void ReadInTable();

    public:
    CErrorTable(char *fileName);
};



