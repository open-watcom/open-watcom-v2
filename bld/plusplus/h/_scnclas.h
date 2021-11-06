/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Characters scan classes related definition pick file
*
****************************************************************************/


pick( SCAN_NAME,          scanName            ) // identifier
pick( SCAN_WIDE,          scanWide            ) // L"abc" or L'a' or Lname
pick( SCAN_NUM,           scanNum             ) // number that starts with a digit
pick( SCAN_DELIM1,        scanDelim1          ) // single character delimiter only
pick( SCAN_DELIM2,        scanDelim2          ) // 1-4 characters delimiter
pick( SCAN_SLASH,         scanSlash           ) // /, /=, // comment, or /* comment */
pick( SCAN_DOT,           scanDot             ) // .
pick( SCAN_STRING,        scanString          ) // "string"
pick( SCAN_CHARCONST,     scanCharConst       ) // 'a'
pick( SCAN_CR,            scanCarriageReturn  ) // '\r'
pick( SCAN_NEWLINE,       scanNewline         ) // '\n'
pick( SCAN_WHITESPACE,    scanWhiteSpace      ) // all whitespace
pick( SCAN_INVALID,       scanInvalid         ) // all other characters
pick( SCAN_EOF,           scanEof             ) // end-of-file
