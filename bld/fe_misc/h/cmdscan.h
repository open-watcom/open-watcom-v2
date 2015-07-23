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


#ifndef __CMDSCAN_H__
#define __CMDSCAN_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _SWITCH_CHAR1   '-'
#define _SWITCH_CHAR2   '/'

// PROTOTYPES

bool CmdDelimitChar(            // TEST IF SWITCH-DELIMITING CHARACTER
    void )
;
bool CmdRecogLowerChar(         // RECOGNIZE A LOWER CASE CHARACTER
    int recog )                 // - character to be recognized
;
bool CmdRecogChar(              // RECOGNIZE A CHARACTER
    int recog )                 // - character to be recognized
;
size_t CmdScanNumber(           // SCAN A NUMBER
    unsigned *pvalue )          // - addr( return value )
;
bool CmdRecogEquals(            // RECOGNIZE AN '=' or synonym
    void )
;
bool CmdPathDelim(              // SKIP EQUALCHAR # or ' ' IN COMMAND LINE
    void )
;
char const *CmdScanAddr(        // RETURN COMMAND-LINE SCAN ADDRESS
    void )
;
int CmdScanChar(                // SCAN THE NEXT CHARACTER
    void )
;
size_t CmdScanFilename(         // SCAN A FILE NAME
    char const **option )       // - addr( option pointer )
;
size_t CmdScanId(               // SCAN AN IDENTIFIER
    char const **option )       // - addr( option pointer )
;
void CmdScanInit(               // INITIALIZE FOR COMMAND SCANNING
    char const *cmd_line )      // - command line
;
int CmdScanLowerChar(           // SCAN THE NEXT CHARACTER, IN LOWER CASE
    void )
;
int CmdPeekChar(                // PEEK AT NEXT CHARACTER, IN LOWER CASE
    void )
;
size_t CmdScanOption(           // SCAN AN OPTION
    char const **option )       // - addr( option pointer )
;
bool CmdScanBufferEnd(          // TEST IF END OF BUFFER
    void )
;
bool CmdScanSwEnd(              // TEST IF END OF SWITCH
    void )
;
void CmdScanSwitchBackup(       // BACK UP SCANNER TO START OF SWITCH
    void )
;
void CmdScanSwitchBegin(        // REMEMBER START OF SWITCH
    void )
;
char const *CmdScanUngetChar(   // UNGET THE LAST CMD SCAN CHARACTER
    void )
;
int CmdScanWhiteSpace(          // SCAN OVER WHITE SPACE
    void )
;

#ifdef __cplusplus
};
#endif

#endif  // __CMDSCAN_H
