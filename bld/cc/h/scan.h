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


/*     character classes    */

typedef enum charset_flags {
    C_BC = 0,       /* illegal source input character */
    C_AL = 0x01,    /* alphabetic characters and '_'  */
    C_DI = 0x02,    /* digit 0-9                      */
    C_HX = 0x04,    /* upper or lower case hex character (A-F,a-f) */
    C_EX = 0x08,    /* must be examined by GetNextChar */
    C_D1 = 0x10,    /* one byte delimiter             */
    C_D2 = 0x20,    /* two byte delimiters            */
    C_WS = 0x40,    /* white space                    */
    C_DB = 0x80     /* first byte of double-byte char */
} charset_flags;

#define EQ      0x80    /* delimiter can have = following */
#define DUP     0x40    /* delimiter can be duplicated    */
#define C_MASK  0x3F

/*
    After we know a character is a C_HX, we can extract the value
    as follows:

        (( c | HEX_MASK ) - HEX_BASE ) + 10
*/
#if defined( __370__ )
/* EBCDIC */
#define HEX_MASK        0x40
#define HEX_BASE        'A'
#else
/* ASCII */
#define HEX_MASK        0x20
#define HEX_BASE        'a'
#endif

extern unsigned JIS2Unicode( unsigned );

extern charset_flags CharSet[];
