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


// MSGENCOD.H -- header for MSGENCOD program
//
// 91/10/03 -- J.W.Welch        -- defined

#include "stdlib.h"
#include "string.h"
#include "watlib.h"

#define ENC_BIT   0x80
#define LARGE_BIT 0x40

// PROTOTYPES:


int close_c_file(               // CLOSE C FILE
    void )
;
int close_h_file(               // CLOSE H FILE
    void )
;
int open_c_file(                // OPEN C FILE
    const char *fname )         // - file name
;
int open_h_file(                // OPEN H FILE
    const char *fname )         // - file name
;
int phrase_optimize(            // OPTIMIZE PHRASES
    void )
;
int print_encode_info(          // WRITE ENCODED INFORMATION
    void )
;
int process_group(              // PROCESS GROUP DEFINITION
    const char *scan )          // - text
;
int process_text(               // PROCESS TEXT OF MESSAGE
    const char *scan,           // - text
    unsigned number )           // - message number within group
;
int write_c_chr(                // WRITE EOR ON C FILE
    char chr )                  // - character
;
int write_c_comma(              // WRITE ", " ON C FILE
    void )
;
int write_c_delim(              // WRITE DELIMITER ( '{' OR ',' ) TO C FILE
    void )
;
int write_c_eol(                // WRITE EOR ON C FILE
    void )
;
int write_c_line(               // WRITE STRING, EOR TO C FILE
    const char *str )           // - string
;
int write_c_str(                // WRITE STRING TO C FILE
    const char *str )           // - string
;
int write_c_strs(               // WRITE VARIABLE LIST OF STRINGS
    char *str, ... )            // - strings (NULL delimited)
;
int write_c_strs_eol(           // WRITE VARIABLE LIST OF STRINGS
    char *str, ... )            // - strings (NULL delimited)
;
int write_h_eol(                // WRITE EOR ON H FILE
    void )
;
int write_h_line(               // WRITE STRING, EOR TO H FILE
    const char *str )           // - string
;
int write_h_str(                // WRITE STRING TO H FILE
    const char *str )           // - string
;
