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


#ifndef __CMDLINE_H_
#define __CMDLINE_H_

// CMDLINE.H -- C++ definitions for command-line processing
//
// 91/06/06 -- J.W.Welch        -- defined
// 92/12/29 -- B.J. Stecher     -- QNX support

#include "vbuf.h"
#include "cmdscan.h"
#include "cmdlnctx.h"

// PROTOTYPES -- exposed

void GenCOptions(               // PROCESS ALL OPTIONS
    char **argv );              // - command line vector


// PROTOTYPES -- internal to command-line processing

void AddUndefName(              // SAVE A #UNDEF NAME
    void );

void BadCmdLine(                // SIGNAL CMD-LINE ERROR
    int error_code );           // - error code

void CmdLnBatchAbort(           // ABANDON BATCH-FILE PROCESSING
    void )
;
void CmdLnBatchOpen(            // OPEN CMD-LN BATCH FILE
    char const * fname )        // - file name
;
void CmdLnBatchFreeRecord(      // FREE A CMD-LN RECORD
    VBUF *buf )                 // - virtual buffer
;
boolean CmdLnBatchRead(         // READ NEXT LINE IN COMMAND BUFFER
    VBUF *buf )                 // - virtual buffer
;
void CmdLnSwNotImplemented(     // ISSUE WARNING FOR UN-IMPLEMENTED SWITCH
    void )
;
void CmdLnWarn(                 // ISSUE WARNING FOR A SWITCH
    unsigned message )          // - message code
;
int CmdPeekChar(                // PEEK AT NEXT CHARACTER, IN LOWER CASE
    void )
;
char *copy_eq(                  // COPY OPTION, UP TO '=' OR DELIMITER
    char *dest,                 // - destination
    char *src )                 // - source
;
void Define_Extensions(         // PREDEFINE EXTENSIONS AS MACROS
    void );


void InitModInfo(               // INITIALIZE MODULE INFORMATION
    void );

unsigned VerifyPackAmount(      // forces PackAmount to 1,2,4,8
    unsigned );

void MiscMacroDefs(             // PREDEFINE MISCELLANEOUS MACROS
    void );

MEPTR DefineCmdLineMacro(       // DEFINE A MACRO FROM THE COMMAND LINE
    boolean many_tokes )        // - TRUE ==> scan multiple tokens
;
void PreDefineStringMacro(      // PREDEFINE A MACRO
    char *str );                // - name to define

void SetDBChar(                 // SET ASIAN CHARACTERS
    int character_set );        // - character set

extern void CBanner();          // DEFINED IN WATCOM.C



#endif
