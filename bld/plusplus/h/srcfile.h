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


#ifndef _SRCFILE_H

#include "toknlocn.h"

#define PRODUCTION_BUFFER_SIZE  (1024*8)

typedef enum {
    SFO_SOURCE_FILE,
    SFO_READ_TEXT,
    SFO_READ_BINARY,
    SFO_WRITE_TEXT,
    SFO_WRITE_BINARY
} src_file_open;

typedef struct dir_list DIR_LIST;

// PROTOTYPES:

void SrcFileScanName(           // SCAN AN IDENTIFIER
    int c )                     // - first character to check
;
void SrcFileScanWhiteSpace(     // SCAN WHITESPACE
    int expanding )             // - expanding a macro
;
void SrcFileScanCppComment(      // SCAN C++ COMMENT
    void )
;
bool IsSrcFileCmdLine(          // DETERMINE IF SOURCE FILE IS FOR CMD-LINE
    SRCFILE sf )                // - a source file
;
bool IsSrcFileLibrary(          // DETERMINE IF SOURCE FILE IS #include <file.h>
    SRCFILE sf )                // - a source file
;
bool IsSrcFilePrimary(          // DETERMINE IF PRIMARY SOURCE FILE
    SRCFILE sf )                // - a source file
;
SRCFILE SrcFileEnclosingPrimary(// FIND ENCLOSING PRIMARY SOURCE FILE
    SRCFILE src )               // - a source file
;
void SrcFileAlias(              // SET UP ALIAS FOR SOURCE FILE
    char *name,                 // - alias name
    LINE_NO line,               // - line no. (used in #line)
    int adjust )                // - amount to adjust line no. before setting
;
bool SrcFileClose(              // CLOSE A SOURCE FILE
    bool shutdown )             // - shutdown in progress
;
void SrcFileCmdLnDummyClose(    // CLOSE DUMMY FILE FOR COMMAND LINE
    void )
;
void SrcFileCmdLnDummyOpen(     // OPEN DUMMY FILE FOR COMMAND LINE
    void )
;
int SrcFileCmdLnGetChar(        // GET NEXT CHARACTER FOR CMD-LINE FILE
    void )
;
void SrcFileCommand(            // MARK CURRENT SOURCE FILE AS A COMMAND FILE
    void )
;
SRCFILE SrcFileCurrent(         // GET CURRENT SRCFILE
    void )
;
void SrcFileSetErrLoc(          // SET TEMPORARY ERROR LOCATION TO CURR CHAR POSITION
    void )
;
void SrcFileCurrentLocation(    // SET LOCATION FOR CURRENT SOURCE FILE
    void )
;
void SrcFileGetTokenLocn(       // FILL IN TOKEN_LOCN FROM CURRENT TOKEN LOCATION
    TOKEN_LOCN *tgt )           // - to be filled in
;
void SrcFileResetTokenLocn(     // RESET TOKEN_LOCN
    TOKEN_LOCN *tgt )           // - from SrcFileGetTokenLocn
;
bool SrcFileAreTLSameLine(      // CHECK WHETHER TOKEN_LOCNs ARE THE SAME LINE
    TOKEN_LOCN *l1,             // - location one
    TOKEN_LOCN *l2 )            // - location two
;
unsigned SrcFileIndex(          // GET INDEX OF THIS SRCFILE
    SRCFILE sf )                // - the source file
;
void SrcFileLibrary(            // MARK CURRENT SOURCE FILE AS A LIBRARY FILE
    void )
;
LINE_NO SrcFileLine(            // GET CURRENT SOURCE LINE
    void )
;
char *SrcFileName(              // GET NAME OF SOURCE FILE
    SRCFILE sf )                // - source file
;
char *SrcFileFullName(          // GET FULL PATH NAME OF SOURCE FILE
    SRCFILE sf )                // - source file
;
char *SrcFileNameCurrent(       // GET NAME OF CURRENT SOURCE FILE
    void )
;
SRCFILE SrcFileNotReadOnly(     // GET NEXT NON-READ-ONLY SOURCE FILE
    SRCFILE curr )              // - current source file
;
SRCFILE SrcFileOpen(            // OPEN NEW SOURCE FILE
    void *fp,                   // - system file control
    char *name )                // - file name
;
void SrcFileNotAFile(           // LABEL SRCFILE AS A DEVICE
    SRCFILE sf )                // - the device source file
;
void SrcFilePoint(              // SET CURRENT SRCFILE
    SRCFILE srcfile )           // - source file to be set as current
;
void SetSrcFilePrimary(         // MARK CURRENT SOURCE FILE AS THE PRIMARY FILE
    void )
;
SRCFILE SrcFileGetPrimary(      // GET PRIMARY SOURCE FILE
    void )
;
bool SrcFilesOpen(              // DETERMINE IF ANY SOURCE FILES OPEN
    void )
;
void SrcFileReadOnlyFile(       // SPECIFY FILE AS READ-ONLY
    char const *file )          // - the file
;
void SrcFileReadOnlyDir(        // SPECIFY DIRECTORY AS READ-ONLY
    char const *dir )           // - the directory
;
time_t SrcFileTimeStamp(        // GET TIME STAMP FOR FILE
    SRCFILE srcfile )           // - source file
;
void SrcFileTraceBack(          // INDICATE SRCFILE USED IN TRACE-BACK
    SRCFILE sf )                // - source-file in message
;
SRCFILE SrcFileTraceBackFile(   // GET SRCFILE TRACED BACK
    void )
;
bool SrcFileTraceBackReqd(      // DETERMINE IF MSG TRACE-BACK REQ'D
    SRCFILE sf )                // - source-file in message
;
SRCFILE SrcFileWalkInit(        // START WALK OF SOURCE FILES
    void )
;
SRCFILE SrcFileWalkNext(        // NEXT FILE IN WALK OF SOURCE FILES
    SRCFILE curr )              // - previous file
;
SRCFILE SrcFileIncluded(        // FILE THAT INCLUDES THIS FILE
    SRCFILE curr,               // - current file
    LINE_NO *line )             // - line that contains #include
;

// PROTOTYPES: source-file guarding

#define GUARD_IMPL              // indicate source file guarding implemented

bool SrcFileGuardedIf(          // SKIP REST OF GUARDED FILE, IF POSSIBLE
    bool value )                // - <value> in #if <value>
;
bool SrcFileProcessOnce(        // CHECK WHETHER WE HAVE TO OPEN THE FILE
    char *name )
;
void SrcFileGuardPpElse(        // #ELSE DETECTED IN SOURCE FILE
    void )
;
void SrcFileGuardPpEndif(       // #ENDIF DETECTED IN SOURCE FILE
    void )
;
void SrcFileGuardPpIf(          // #IF DETECTED IN SOURCE FILE
    void )
;
void SrcFileGuardPpIfndef(      // SUPPLY #IFNDEF NAME
    char *name,                 // - macro name
    unsigned len )              // - length of name
;
void SrcFileGuardStateSig(      // SIGNAL SIGNIFICANCE (TOKEN, ETC) IN FILE
    void )
;
bool SrcFileSame(               // ARE THESE SRC FILES THE SAME FILE?
    SRCFILE f1,                 // - src-file 1
    SRCFILE f2 )                // - src-file 2
;

FILE *SrcFileFOpen(             // FOPEN A FILE WITH HANDLE CACHEING
    char *name,                 // - name of file to open
    src_file_open kind )        // - how to open file
;

int SrcFileFClose(              // FCLOSE A FILE
    FILE *fp )                  // - file pointer to close
;
void SrcFileSetCreatePCHeader(  // MARK SRCFILE TO CREATE PCHDR WHEN #include FINISHES
    void )
;

void SrcFileSetTab(             // Set number of spaces in a tab stop
    unsigned tab )              // - tab stop spacing
;
void SrcFileOnceOnly(           // CURRENT SRCFILE CAN BE SKIPPED IF #INCLUDE AGAIN
    void )
;
void SrcFileSetSwEnd(           // SET CURRENT SRCFILE IGNORE CMDLINE SW END STATUS
    bool val )                  // - value to use to set status
;

#define DEF_TAB_WIDTH 8

SRCFILE SrcFileGetIndex( SRCFILE );
SRCFILE SrcFileMapIndex( SRCFILE );

#define _SRCFILE_H
#endif
