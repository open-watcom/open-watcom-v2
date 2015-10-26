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


#include <limits.h>

typedef enum {
    C_AL,       // alphabetic characters and '$'
    C_EX,       // exponent ( 'E' or 'D' )
    C_SG,       // sign ( '+' or '-' )
    C_DP,       // decimal point
    C_DI,       // digit ( 0..9 )
    C_HL,       // hollerith 'H'
    C_AP,       // apostrophe ( ' )
    C_OP,       // illegal operators, delimiters
    C_SP,       // blank
    C_TC,       // TAB character
    C_BC,       // illegal source input character
    C_EL,       // end-of-line
    C_CM,       // end-of-line comment character
    C_OL,       // octal constant indicator
    C_HX,       // hexadecimal constant indicator
    C_CS,       // C string constant indicator
    C_DB,       // 1st byte of a double-byte character
    C_MAX = UCHAR_MAX   // force enum to be unsigned
} char_class;

#define C_EXT   0x80    //  extension to standard defined character set.
#define C_LOW   0x40    //  lower case character
#define C_MASK  ~( C_EXT | C_LOW )

#define XC_AL   (C_AL | C_EXT)
#define XC_EX   (C_EX | C_EXT)
#define XC_SG   (C_SG | C_EXT)
#define XC_DP   (C_DP | C_EXT)
#define XC_DI   (C_DI | C_EXT)
#define XC_HL   (C_HL | C_EXT)
#define XC_AP   (C_AP | C_EXT)
#define XC_OP   (C_OP | C_EXT)
#define XC_SP   (C_SP | C_EXT)
#define XC_TC   (C_TC | C_EXT)
#define XC_BC   (C_BC | C_EXT)
#define XC_EL   (C_EL | C_EXT)
#define XC_CM   (C_CM | C_EXT)
#define XC_OL   (C_OL | C_EXT)
#define XC_HX   (C_HX | C_EXT)
#define XC_CS   (C_CS | C_EXT)
#define XC_DB   (C_DB | C_EXT)

#define LC_AL   (XC_AL | C_LOW)
#define LC_EX   (XC_EX | C_LOW)
#define LC_HL   (XC_HL | C_LOW)
#define LC_OL   (XC_OL | C_LOW)
#define LC_HX   (XC_HX | C_LOW)
#define LC_CS   (XC_CS | C_LOW)

typedef enum {

//  state           description                  example

    SNS,        // no state determined
    SNM,        // number                       123
    SAN,        // alpha-numeric                V1Q2
    SSG,        // sign                         +  or  -
    SFT,        // floating point number        134.5
    SLX,        // logical or exponent of float 134.e
    SEX,        // exponent of float collected  134.5e
    SEN,        // exp number being collected   134.5e6
    SLG,        // logical being collected      .tru    or .junk
    SML,        // may be logical               134.
    SIQ,        // in quotes                    'joh    or 'jane''
    SAP,        // apostrophe                   'jane'
    SIH,        // in hollerith                 35h the cow jum
    SFM,        // in FORMAT statement          FORMAT( 1X
    SOL,        // octal constant               '777'o
    SHX,        // hexadecimal constant         'fff'x
    SCS,        // C string (null terminated)   'abc'c
    SLL,        // likely logical               x.
//----------------- end if states in table
    SFL,        // finished logical             .true.
    SOP,        // collected an operator        ,
    SHL,        // starting hollerith           35h
    SFQ,        // first quote                  '
    SSO,        // scanning is over             134.5e6+
    SSP,        // space character
    STC,        // TAB character
    SBC,        // bad character
    SNR,        // new record
    SCM,        // end-of-line comment
    SDB         // scanned 1st byte of a double-byte character
} token_state;
