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
* Description:  Characters scan classes related definition pick file
*
****************************************************************************/


pick( SCAN_NAME,       ScanName           )  // identifier
pick( SCAN_WIDE,       ScanWide           )  // L"abc" or L'a' or Lname
pick( SCAN_NUM,        ScanNum            )  // number that starts with a digit
pick( SCAN_QUESTION,   ScanQuestionMark   )  // '?'
pick( SCAN_SLASH,      ScanSlash          )  // '/'
pick( SCAN_MINUS,      ScanMinus          )  // '-'
pick( SCAN_EQUAL,      ScanEqual          )  // '='
pick( SCAN_COLON,      ScanColon          )  // ':'
pick( SCAN_STAR,       ScanStar           )  // '*'
pick( SCAN_DELIM1,     ScanDelim1         )  // single character delimiter
pick( SCAN_DELIM2,     ScanDelim2         )  // one, two, or three byte delimiter
pick( SCAN_DOT,        ScanDot            )  // .
pick( SCAN_STRING,     ScanString         )  // "string"
pick( SCAN_CHARCONST,  ScanCharConst      )  // 'a'
pick( SCAN_CR,         ScanCarriageReturn )  // '\r'
pick( SCAN_NEWLINE,    ScanNewline        )  // '\n'
pick( SCAN_WHITESPACE, ScanWhiteSpace     )  // all whitespace
pick( SCAN_INVALID,    ScanInvalid        )  // all other characters
pick( SCAN_MACRO,      ScanMacroToken     )  // get next token from macro
pick( SCAN_EOF,        ScanEof            )  // end-of-file
