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


//
// Format Codes
//

enum {
    A_FORMAT,   // A
    BN_FORMAT,  // BN
    BZ_FORMAT,  // BZ
    D_FORMAT,   // Dw.d
    E_FORMAT,   // Ew.d or Ew.dEe
    ED_FORMAT,  // Ew.dDe
    F_FORMAT,   // Fw.d
    G_FORMAT,   // Gw.d or Gw.dEe
    H_FORMAT,   // nH and literals 'xxx'
    I_FORMAT,   // Iw or Iw.m
    L_FORMAT,   // Lw
    P_FORMAT,   // kP
    S_FORMAT,   // S
    SP_FORMAT,  // SP
    SS_FORMAT,  // SS
    T_FORMAT,   // T
    TL_FORMAT,  // TL
    TR_FORMAT,  // TR
    X_FORMAT,   // nX
    C_FORMAT,   // :
    LP_FORMAT,  // (
    RP_FORMAT,  // )
    SL_FORMAT,  // /
    M_FORMAT,   // \ or $
    Z_FORMAT,   // Zw (extension)
    END_FORMAT, // end of format statement indicator
    REP_FORMAT, // repeat specification indicator
    EQ_FORMAT,  // Ew.dQe | we place these formats here to maintain object
    Q_FORMAT    // Qw.d   | file compatability with previous releases
};

// The following flag is used to mark the code to revert to, if necessary

#define REV_CODE        0x80    // used to mark code to revert to if needed
#define EXTEND_FORMAT   0x40    // extend format code

// Delimiter Codes

#define NO_DELIM        0       // code has not been delimited
#define YES_DELIM       1       // code has been delimited by a ',' or '/'
#define P_DELIM         2       // code has been delimited by a kP code
