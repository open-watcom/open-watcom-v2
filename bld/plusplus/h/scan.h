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


#ifndef __SCAN_H__
#define __SCAN_H__

/* special "logical" characters that represent special states */

enum {
    LCHR_EOF            = 0x0100,       /* last char in program source */
    LCHR_CONTINUE_STRING,               /* string concatenation in progress */
    LCHR_CONTINUE_LSTRING,              /* wide string concatenation in progress */
    LCHR_MAX
};

/*     character classes    */

#define C_AL    0x01    /* alphabetic characters and '_'  */
#define C_DI    0x02    /* digit 0-9                      */
#define C_HX    0x04    /* upper or lower case hex character (A-F,a-f) */
#define C_EX    0x08    /* must be examined by GetNextChar */
#define C_WS    0x40    /* white space                    */
#define C_DB    0x80    /* first byte of double-byte char */
#define C_BC    0       /* illegal source input character */

#define C___    0       /* no flags set for character */

/*
    After we know a character is a C_HX, we can extract the value
    as follows:

        (( c | HEX_MASK ) - HEX_BASE ) + 10
*/
#ifdef _CHARSET
#if _CHARSET == _EBCDIC
/* EBCDIC */
#define HEX_MASK        0x40
#define HEX_BASE        'A'
#define ONE_CASE( c )   ((c)|'\x40')
#endif
#else
/* ASCII */
#define HEX_MASK        0x20
#define HEX_BASE        'a'
#define ONE_CASE( c )   ((c)|'\x20')
#endif

extern unsigned char    CharSet[];  // character characterizations
extern unsigned         JIS2Unicode( unsigned );
extern unsigned char    TokValue[];

#endif // __SCAN_H__
